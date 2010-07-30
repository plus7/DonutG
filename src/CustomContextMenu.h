/**
 *	@file	CustomContextMenu.h
 *	@brief	atlhostEx.hから移動したカスタムコンテキストメニューの処理部分
 */
#pragma once

#include "MtlUser.h"
#include "ToolTipManager.h"
#include "DonutPFunc.h"

#if 1	//+++
#include <FlatComboBox.h>
#include "SearchBar.h"		//+++ GetSearchEngineMenuのため
#include "Donut.h"			//+++ Donut_GetActiveStatusStr()のため
#endif

/*
	CCustomContextMenu
	atlhostEx.hから移動したカスタムコンテキストメニューの処理部分

	正直全部書き直したい。なにやっているのか分からないし汚い。汚したのは自分ですが(minit)
 */

class CCustomContextMenu {
	HWND		m_hWndTopLevel;
  #if 1	//++
	int			m_nContextMenuMode;							//+++ CONTEXT_MENU_DEFAULT,CONTEXT_MENU_TEXTSELECT,CONTEXT_MENU_ANCHOR
	POINT		m_pt;										//+++
	CString		m_strUrl;									//+++
	static CCustomContextMenu*	s_pThis;					//+++
  #endif

public:
	CCustomContextMenu() : m_hWndTopLevel(0), m_nContextMenuMode(0) { s_pThis = this; } 	//+++

	static CCustomContextMenu* 	GetInstance() { return s_pThis; }							//+++
	int 						GetContextMenuMode() const { return m_nContextMenuMode; }	//+++
	const CString& 				GetAnchorUrl() const { return m_strUrl; }					//+++
	const POINT& 				GetPoint() 	const { return m_pt; }							//+++


	HRESULT Show(DWORD dwId, DWORD x, DWORD y, IUnknown *pCommandTarget)
	{
		//ウィンドウハンドルが欲しい。pCommandTargetから取れないだろうか
		CComQIPtr<IOleWindow>	pWindow = pCommandTarget;
		if (!pWindow) {
			ATLASSERT(FALSE);
			return S_FALSE;
		}

		HWND		hWnd;
		HRESULT 	hr	  = pWindow->GetWindow(&hWnd);
		if ( FAILED(hr) )
			ATLASSERT(FALSE);

		m_hWndTopLevel = CWindow(hWnd).GetTopLevelWindow();

		CPoint		pt(x, y);
		return ShowContextMenu(dwId, &pt, pCommandTarget, NULL);
	}


private:

	HRESULT ShowContextMenuEx(POINT *pptPosition)
	{
		//まずメニュー用のリソースが使用されていればそれを開放する
		::SendMessage(GetTopLevelWindow(), WM_RELEASE_CONTEXTMENU, 0, 0);

		CMenu			menu;
		menu.CreatePopupMenu();

		MENUITEMINFO	menuInfo;
		memset( &menuInfo, 0, sizeof (MENUITEMINFO) );
		menuInfo.cbSize = sizeof (MENUITEMINFO);
		menuInfo.fMask	= MIIM_ID | MIIM_TYPE;

		CSimpleArray<HMENU> aryDestroyMenu;

		CString 		strPath = _GetFilePath( _T("Menu.ini") );
		CIniFileI		pr(strPath, _T("MenuEx"));
		DWORD			dwCount = pr.GetValue(_T("Count"), 0);
		CString 		strKey;

		for (DWORD ii = 0; ii < dwCount; ++ii) {
			strKey.Format(_T("%02d"), ii);
			DWORD		dwCmd = pr.GetValue(strKey, 0);

			CString 	strCmd;
			CToolTipManager::LoadToolTipText(dwCmd, strCmd);

			if (strCmd.IsEmpty() == FALSE) {
				menuInfo.fType		= MFT_STRING;
				menuInfo.wID		= dwCmd;
			  #if 1	//+++
				menuInfo.dwTypeData = LPTSTR(LPCTSTR(strCmd));
			  #else
				menuInfo.dwTypeData = strCmd.GetBuffer(0);
			  #endif
				menuInfo.cch		= strCmd.GetLength();
			} else {
				menuInfo.fType = MFT_SEPARATOR;
			}

			::InsertMenuItem(menu.m_hMenu, ii, MF_BYPOSITION, &menuInfo);
			CstmContextMenuDropdown(menu.m_hMenu, dwCmd, aryDestroyMenu);

			//特殊メニューの初期化 minit
			_BeforeInitSpecialMenu(dwCmd);
		}
		pr.Close();

		// 編集しないメニューとする
		menuInfo.fType = MFT_SEPARATOR;
		::InsertMenuItem(menu.m_hMenu, 0, MF_BYPOSITION, &menuInfo);

		//表示中はメインメニューの方に送るメッセージを制限する minit
		::SendMessage(GetTopLevelWindow(), WM_MENU_RESTRICT_MESSAGE, (WPARAM) TRUE, 0);

		// Show shortcut menu
		CIniFileI		pr2(strPath, _T("Option"));
		DWORD	dwREqualL	= pr2.GetValue(_T("REqualL"), 0);
		pr2.Close();

		DWORD	dwMenuStyle = TPM_LEFTALIGN | TPM_RETURNCMD;
		if (dwREqualL)
			dwMenuStyle |= TPM_RIGHTBUTTON;

		int 	iSelection	= ::TrackPopupMenu(   menu.m_hMenu
												, dwMenuStyle
												, pptPosition->x
												, pptPosition->y
												, 0
												, GetTopLevelWindow()
												, (RECT *) NULL);

		for (int jj = 0; jj < aryDestroyMenu.GetSize(); jj++)
			::DestroyMenu(aryDestroyMenu[jj]);

		HWND						hWndTopLevelParent = GetTopLevelWindow();
		::SendMessage(hWndTopLevelParent, WM_COMMAND, iSelection, NULL);

		//メッセージの制限を解除する minit
		::PostMessage(hWndTopLevelParent, WM_MENU_RESTRICT_MESSAGE, (WPARAM) FALSE, 0);

		return S_OK;
	}


	// MSHTML requests to display its context menu
	STDMETHOD	(ShowContextMenu) (DWORD dwID, POINT * pptPosition, IUnknown * pCommandTarget, IDispatch * pDispatchObjectHit)
	{
		m_nContextMenuMode			= 0;	//+++
		//まずメニュー用のリソースが使用されていればそれを開放する
		::SendMessage(GetTopLevelWindow(), WM_RELEASE_CONTEXTMENU, 0, 0);

		if (::GetKeyState(VK_LBUTTON) < 0)	// || ::GetKeyState(VK_LBUTTON)<0) //なんじゃこりゃ
			return ShowContextMenuEx(pptPosition);

		if (   dwID != CONTEXT_MENU_DEFAULT
			&& dwID != CONTEXT_MENU_TEXTSELECT
			&& dwID != CONTEXT_MENU_ANCHOR)
		{
			return S_FALSE;
		}

	  #if 1	//+++
		m_nContextMenuMode			= dwID;	//+++
		if (pptPosition)					//+++
			m_pt	= *pptPosition;			//+++
		if (dwID == CONTEXT_MENU_ANCHOR)
			m_strUrl = Donut_GetActiveStatusStr();
	  #endif

		enum {
			IDR_BROWSE_CONTEXT_MENU 	= 24641,
			IDR_FORM_CONTEXT_MENU		= 24640,
			SHDVID_GETMIMECSETMENU		=	 27,
			SHDVID_ADDMENUEXTENSIONS	=	 53,
		};

		CComPtr<IOleCommandTarget>	spCT;
		CComPtr<IOleWindow> 		spWnd;
		CComVariant 				var;
		CComVariant					var1;
		CComVariant					var2;

		HRESULT hr	   	= pCommandTarget->QueryInterface(IID_IOleCommandTarget, (void **) &spCT );
		hr			   	= pCommandTarget->QueryInterface(IID_IOleWindow       , (void **) &spWnd);
		HWND	hwnd	= 0;
		hr			   	= spWnd->GetWindow(&hwnd);

		HINSTANCE	hinstSHDOCLC   = LoadLibrary( TEXT("SHDOCLC.DLL") );
		if (hinstSHDOCLC == NULL) {
			hinstSHDOCLC = LoadLibrary( TEXT("ieframe.dll") );		// for vista
			if (hinstSHDOCLC == NULL)
				return S_FALSE;
		}

		HMENU	hMenu		= LoadMenu( hinstSHDOCLC, MAKEINTRESOURCE(IDR_BROWSE_CONTEXT_MENU) );
		HMENU	hMenuSub	= GetSubMenu(hMenu, dwID);

		if (hMenu == 0) {				//+++
			FreeLibrary(hinstSHDOCLC);	//+++
			return S_FALSE;				//+++
		}

		// Get the language submenu
		hr			   = spCT->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, 0, NULL, &var);

		MENUITEMINFO	mii = { 0 };
		mii.cbSize	   = sizeof (mii);
		mii.fMask	   = MIIM_SUBMENU;
		mii.hSubMenu   = (HMENU) var.byref;

	  #ifndef IDM_LANGUAGE
		enum { IDM_LANGUAGE 	= 2292 };
	  #endif
		SetMenuItemInfo(hMenuSub, IDM_LANGUAGE, FALSE, &mii);

		// Insert Shortcut Menu Extensions from registry
		V_VT(&var1)    = VT_INT_PTR;
		V_BYREF(&var1) = hMenuSub;

		V_VT(&var2)    = VT_I4;
		V_I4(&var2)    = dwID;

		hr			   = spCT->Exec(&CGID_ShellDocView, SHDVID_ADDMENUEXTENSIONS, 0, &var1, &var2);

		CSimpleMap<DWORD, DWORD>	mapCmd;
		CSimpleArray<HMENU> 		aryDestroyMenu;
		CstmContextMenu(hMenuSub, dwID, mapCmd, aryDestroyMenu);

		//初期化前メッセージを送る
		for (int ii = 0; ii < mapCmd.GetSize(); ii++)
			_BeforeInitSpecialMenu( mapCmd.GetValueAt( ii ) );

		//表示中はメインメニューの方に送るメッセージを制限する minit
		::SendMessage(GetTopLevelWindow(), WM_MENU_RESTRICT_MESSAGE, (WPARAM) TRUE, 0);

		// Show shortcut menu
		CString 	strPath 	= _GetFilePath( _T("Menu.ini") );
		CIniFileI	pr(strPath, _T("Option"));
		DWORD		dwREqualL	= pr.GetValue(_T("REqualL"), 0 );
		pr.Close();
		int 		nCmdAddFav	= MtlGetCmdIDFromAccessKey( hMenuSub, _T("&F") );
		DWORD		dwMenuStyle = TPM_LEFTALIGN | TPM_RETURNCMD;

		if (dwREqualL)
			dwMenuStyle |= TPM_RIGHTBUTTON;

		int 	iSelection	= ::TrackPopupMenu(   hMenuSub
												, dwMenuStyle
												, pptPosition->x
												, pptPosition->y
												, 0
												, GetTopLevelWindow()
												, (RECT *) NULL );

		// Send selected shortcut menu item command to shell
		LRESULT  lRes	= S_OK;
		if (iSelection != 0)
			lRes = ::SendMessage(hwnd, WM_COMMAND, iSelection, NULL);

		BOOL	bSendFrm = FALSE;

		// コマンド範囲
		if (COMMAND_RANGE_START <= iSelection && iSelection <= COMMAND_RANGE_END)
			bSendFrm = TRUE;

		// お気に入り、グループ
		else if ( (FAVORITE_MENU_ID_MIN <= iSelection && iSelection <= FAVORITE_MENU_ID_MAX)
				||(FAVGROUP_MENU_ID_MIN <= iSelection && iSelection <= FAVGROUP_MENU_ID_MAX) )
			bSendFrm = TRUE;

		// スクリプト
		else if (ID_INSERTPOINT_SCRIPTMENU <= iSelection && iSelection <= ID_INSERTPOINT_SCRIPTMENU_END)
			bSendFrm = TRUE;

	  #if 1	//+++ 検索エンジン
		else if (ID_INSERTPOINT_SEARCHENGINE <= iSelection && iSelection <= ID_INSERTPOINT_SEARCHENGINE_END)
			bSendFrm = TRUE;
	  #endif

		if ( bSendFrm == FALSE && mapCmd.Lookup( (DWORD) iSelection ) )
			bSendFrm = TRUE;

		HWND	hWndTopLevelParent = GetTopLevelWindow();
		if (bSendFrm)
			lRes = ::SendMessage(hWndTopLevelParent, WM_COMMAND, iSelection, NULL);

		//この時点でウィンドウが消されてしまう可能性があるけれども・・
		if (iSelection == nCmdAddFav)
			::SendMessage(hWndTopLevelParent, WM_REFRESH_EXPBAR, 0, 0);

		//ここでお片づけ
		for (int ii = 0; ii < aryDestroyMenu.GetSize(); ii++)
			::DestroyMenu(aryDestroyMenu[ii]);

		for (int ii = 0; ii < mapCmd.GetSize(); ii++) {
			::RemoveMenu( hMenuSub, mapCmd.GetKeyAt( ii ), MF_BYCOMMAND );
			_RestoreSpecialMenu( mapCmd.GetValueAt( ii ) );
		}

		//メッセージの制限を解除する minit
		::PostMessage(hWndTopLevelParent, WM_MENU_RESTRICT_MESSAGE, (WPARAM) FALSE, 0);

		::DestroyMenu(hMenu);

		::FreeLibrary(hinstSHDOCLC);

		//m_nContextMenuMode = 0;	//+++

		return S_OK;
	}

	void _RestoreSpecialMenu(DWORD dwCmd)
	{
		if (dwCmd == ID_FAVORITES_DROPDOWN)
			::SendMessage(GetTopLevelWindow(), WM_MENU_REFRESH_FAV      , (WPARAM) FALSE, 0);
		else if (dwCmd == ID_FAVORITES_GROUP_DROPDOWN)
			::SendMessage(GetTopLevelWindow(), WM_MENU_REFRESH_FAV_GROUP, (WPARAM) FALSE, 0);
		else if (dwCmd == ID_SCRIPT)
			::SendMessage(GetTopLevelWindow(), WM_MENU_REFRESH_SCRIPT   , (WPARAM) FALSE, 0);
	}



	void _BeforeInitSpecialMenu(DWORD dwCmd)
	{
		if (dwCmd == ID_FAVORITES_DROPDOWN)
			::SendMessage(GetTopLevelWindow(), WM_MENU_REFRESH_FAV      , (WPARAM) TRUE, 0);
		else if (dwCmd == ID_FAVORITES_GROUP_DROPDOWN)
			::SendMessage(GetTopLevelWindow(), WM_MENU_REFRESH_FAV_GROUP, (WPARAM) TRUE, 0);
		else if (dwCmd == ID_SCRIPT)
			::SendMessage(GetTopLevelWindow(), WM_MENU_REFRESH_SCRIPT   , (WPARAM) TRUE, 0);
	}



	void CstmContextMenu(HMENU hMenu, DWORD dwID, CSimpleMap<DWORD, DWORD> &mapCmd, CSimpleArray<HMENU> &aryDestroyMenu)
	{
		//コンテキストメニューの「前に戻る」から「デスクトップ項目として設定」の次のセパレータまでを削除
		//for (int ii=0; ii<8 && dwID==CONTEXT_MENU_DEFAULT; ii++)
		for (int ii = 0; ii < 3 && dwID == CONTEXT_MENU_DEFAULT; ii++)	//前に戻ると次に進むだけ消す
			::DeleteMenu(hMenu, 0 , MF_BYPOSITION);

		MENUITEMINFO menuInfo;
		memset( &menuInfo, 0, sizeof (MENUITEMINFO) );
		menuInfo.cbSize = sizeof (MENUITEMINFO);
		menuInfo.fMask	= MIIM_ID | MIIM_TYPE;

		CString 	strSection;
		strSection.Format(_T("Type%d"), dwID);
		CString 	strPath 	= _GetFilePath( _T("Menu.ini") );
		CIniFileI	pr(strPath, strSection);
		DWORD		dwCount 	= pr.GetValue(_T("FrontCount"), 0 );

		//メインメニューからメニューの文字列を取得するため
		//CMenuHandle mh;
		//mh.LoadMenu(IDR_MAINFRAME);

		DWORD		 jj;
		for (jj = 0; jj < dwCount; jj++) {
			CString strKey;
			strKey.Format(_T("Front%02d"), jj);
			DWORD	dwCmd = pr.GetValue(strKey, 0);

			CString strCmd;
			CToolTipManager::LoadToolTipText(dwCmd, strCmd);			//StringTableから文字列を取得
			//mh.GetMenuString(dwCmd, strCmd, MF_BYCOMMAND);			// メインメニューから文字列を取得

			if (strCmd.IsEmpty() == FALSE) {
				menuInfo.fType		= MFT_STRING;
				menuInfo.wID		= dwCmd;
			  #if 1	//+++
				menuInfo.dwTypeData = LPTSTR(LPCTSTR(strCmd));
			  #else
				menuInfo.dwTypeData = strCmd.GetBuffer(0);
			  #endif
				menuInfo.cch		= strCmd.GetLength();

				// コマンドを覚える
				DWORD value = 1;

				if (dwCmd == ID_FAVORITES_DROPDOWN) {
					value = 2;
					//menuInfo.dwTypeData = _T("お気に入り(&A)"); 			//メインメニューから文字列を取得できないため
				} else if (dwCmd == ID_FAVORITES_GROUP_DROPDOWN) {
					value = 3;
					//menuInfo.dwTypeData = _T("お気に入りグループ(&G)"); 	//上に同じ
				} else if (dwCmd == ID_SCRIPT) {
					value = 4;
					//menuInfo.dwTypeData = _T("スクリプト(&S)"); 			//上に同じ
				}

				mapCmd.Add(dwCmd, value);
			} else {
				menuInfo.fType = MFT_SEPARATOR;
			}

			::InsertMenuItem(hMenu, jj, MF_BYPOSITION, &menuInfo);

			CstmContextMenuDropdown(hMenu, dwCmd, aryDestroyMenu);
		}

		dwCount = pr.GetValue(_T("BackCount"), 0);

		for (jj = 0; jj < dwCount; jj++) {
			CString 	strKey;
			strKey.Format(_T("Back%02d"), jj);
			DWORD	dwCmd = pr.GetValue(strKey, 0);

			CString 	strCmd;
			CToolTipManager::LoadToolTipText(dwCmd, strCmd);

			if (strCmd.IsEmpty() == FALSE) {
				menuInfo.fType		= MFT_STRING;
				menuInfo.wID		= dwCmd;
			  #if 1	//+++
				menuInfo.dwTypeData = LPTSTR(LPCTSTR(strCmd));
			  #else
				menuInfo.dwTypeData = strCmd.GetBuffer(0);
			  #endif
				menuInfo.cch		= strCmd.GetLength();

				// コマンドを覚える
				mapCmd.Add(dwCmd, 1);
			} else {
				menuInfo.fType = MFT_SEPARATOR;
			}

			::InsertMenuItem(hMenu, ::GetMenuItemCount(hMenu), MF_BYPOSITION, &menuInfo);

			CstmContextMenuDropdown(hMenu, dwCmd, aryDestroyMenu);
		}

		pr.Close();

		// 編集しないメニューとする
		menuInfo.fType = MFT_SEPARATOR;
		::InsertMenuItem(hMenu, 0, MF_BYPOSITION, &menuInfo);
	}


	void CstmContextMenuDropdown(HMENU hMenu, DWORD dwCmd, CSimpleArray<HMENU> &aryDestroyMenu)
	{
		MENUITEMINFO mii  = { 0 };

		mii.cbSize = sizeof (mii);
		mii.fMask  = MIIM_SUBMENU;

		CMenuHandle  menu;
		DWORD		 dwID = 0;

		switch (dwCmd) {
		case ID_FAVORITES_DROPDOWN:
			mii.hSubMenu = (HMENU) ::SendMessage(GetTopLevelWindow(), WM_MENU_GET_FAV	   , 0, 0);
			break;

		case ID_FAVORITES_GROUP_DROPDOWN:
			mii.hSubMenu = (HMENU) ::SendMessage(GetTopLevelWindow(), WM_MENU_GET_FAV_GROUP, 0, 0);
			break;

		case ID_SCRIPT:
			mii.hSubMenu = (HMENU) ::SendMessage(GetTopLevelWindow(), WM_MENU_GET_SCRIPT   , 0, 0);
			break;

		case ID_SEARCHENGINE_MENU:
			{
				CMenuHandle menu = CDonutSearchBar::GetInstance()->GetSearchEngineMenuHandle();
				mii.hSubMenu	 = menu.m_hMenu;
			}
			break;

		case ID_DLCTL_CHG_MULTI:	dwID = IDR_MULTIMEDIA;		break;
		case ID_DLCTL_CHG_SECU: 	dwID = IDR_SECURITY;		break;
		case ID_VIEW_FONT_SIZE: 	dwID = IDR_VIEW_FONT_SIZE;	break;
		case ID_COOKIE: 			dwID = IDR_COOKIE;			break;
		case ID_HTMLZOOM_MENU: 		dwID = IDR_ZOOM_MENU;		break;		//+++
		default:					return; 					break;
		}

		if (dwID != 0) {
			menu.LoadMenu(dwID);
			HMENU	hMenuSub = menu.GetSubMenu(0);
			mii.hSubMenu	 = hMenuSub;
			aryDestroyMenu.Add(hMenuSub);
		}

		SetMenuItemInfo(hMenu, dwCmd, FALSE, &mii);
	}


	HWND GetTopLevelWindow()
	{
		return m_hWndTopLevel;
	}

  #if 0	//+++ 失敗
	void getUrl()
	{
	  #if 0
		CComPtr<IDispatch>		  spDisp;
		HRESULT 				  hr	= m_spBrowser->get_Document(&spDisp);
		if ( FAILED(hr) )
			return;
		CComQIPtr<IHTMLDocument2> spDoc = spDisp;
		if (!spDoc) 								// document not initialized yet
			return;
	  #endif
	}
  #endif
};

__declspec(selectany) CCustomContextMenu*	CCustomContextMenu::s_pThis = 0;

