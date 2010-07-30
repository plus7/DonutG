/**
 *	@file	DonutFavoritesMenu.h
 *	@brief	お気に入りメニュー.
 */

#pragma once

#include "FavoritesMenu.h"
#include "IniFile.h"


template <class _MainFrame>
class CDonutFavoritesMenu : public CFavoritesMenu<CDonutFavoritesMenu< _MainFrame > > {
	_MainFrame *		__m_pMainFrame;

public:
	CDonutFavoritesMenu(_MainFrame *__pMainFrame, int nInsertPointMenuItemID)
		: CFavoritesMenu<CDonutFavoritesMenu>( nInsertPointMenuItemID, _T("(なし)"),
											  FAVORITE_MENU_ID_MIN, FAVORITE_MENU_ID_MAX, _T("これらをすべて開く") )
		, __m_pMainFrame(__pMainFrame)
	{
		RefreshMenu();
	}


	// Overrides
	void OnExecute(const CString &strFilePath)
	{
		_The_OpenAllFiles(strFilePath, __m_pMainFrame);
	}


	void RefreshMenu()
	{
		SetStyle( CFavoritesMenuOption::GetStyle() );
		m_nMaxMenuItemTextLength = CFavoritesMenuOption::GetMaxMenuItemTextLength();
		m_nMaxMenuBreakCount	 = CFavoritesMenuOption::GetMaxMenuBreakCount();

		SetRootDirectoryPath( DonutGetFavoritesFolder() );
	}


	//overrides
	void OnSetMenuHandle(HMENU hMenu)
	{
	}


public:
	BEGIN_MSG_MAP(CDonutFavoritesMenu)
		MESSAGE_HANDLER(WM_MENUCHAR, OnMenuChar)
		CHAIN_MSG_MAP(CFavoritesMenu<CDonutFavoritesMenu>)
	END_MSG_MAP()


private:
	void OnMenuPrepareContext(CItemIDList &idlFolder)
	{
		CString 	strFolder;
		MtlGetFavoritesFolder(strFolder);
		idlFolder = strFolder;
	}


private:
	LRESULT OnMenuChar(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
	  #ifdef _CMDBAR_EXTRA_TRACE
		ATLTRACE2(atlTraceUI, 0, "CmdBar - OnMenuChar\n");
	  #endif
		bHandled = TRUE;
		LRESULT lRet;

		if (LOWORD(wParam) != 0x0D)
			lRet = 0;
		else
			lRet = MAKELRESULT(1, 1);

		if (HIWORD(wParam) == MF_POPUP) {
			// Convert character to lower/uppercase and possibly Unicode, using current keyboard layout
			TCHAR		ch		  = (TCHAR) LOWORD(wParam);
			CMenuHandle menu	  = (HMENU) lParam;
			int 		nCount	  = ::GetMenuItemCount(menu);
			int 		nRetCode  = MNC_EXECUTE;
			BOOL		bRet;
			TCHAR		szString[_nMaxMenuItemTextLength];
			szString[0]			  = 0;	//+++
			WORD		wMnem	  = 0;
			bool		bFound	  = false;
			int 		nSelIndex = m_nSelIndex;
			int 		nSelFirst = -1;
			int 		nHitCnt   = 0;

			for (int i = 0; i < nCount; i++) {
				CMenuItemInfo 	mii;
				mii.cch 	   = _nMaxMenuItemTextLength;
				mii.fMask	   = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
				mii.dwTypeData = szString;
				bRet		   = menu.GetMenuItemInfo(i, TRUE, &mii);
				if ( !bRet || (mii.fType & MFT_SEPARATOR) )
					continue;

				CMenuItem *   pMenuItem = (CMenuItem *) mii.dwItemData;

				if (pMenuItem == NULL)
					continue;

				LPTSTR		  p 		= pMenuItem->m_strText.GetBuffer(0);

				if (p != NULL) {
					if (p != NULL && *p) {
						DWORD 	dwP = MAKELONG(*(p), 0);
						DWORD 	dwC = MAKELONG( ch , 0);

						if ( ::CharLower( (LPTSTR) ULongToPtr(dwP) ) == ::CharLower( (LPTSTR) ULongToPtr(dwC) ) ) {
							if (nSelFirst == -1)
								nSelFirst = i;

							nHitCnt++;

							if (!bFound && m_nSelIndex < i) {
								wMnem		= (WORD) i;
								PostMessage(__m_pMainFrame->m_hWnd, TB_SETHOTITEM, (WPARAM) -1, 0L);

								if (m_nSelIndex < i) {
									m_nSelIndex = i;
									bFound		= true;
									// nRetCode = MNC_SELECT;
									// break;
								}

								bFound		= true;
								m_nSelIndex = i;
								// GiveFocusBack();
							} else {
								if (bFound) {
									// nRetCode = MNC_SELECT;
									// break;
								}
							}
						}
					}
				}
			}

			if (nSelIndex == m_nSelIndex && nSelFirst != -1) {
				wMnem		= (WORD) nSelFirst;
				m_nSelIndex = nSelFirst;
				bFound		= true;
			}

			if (nHitCnt > 1)
				nRetCode = MNC_SELECT;

			if (bFound) {
				bHandled = TRUE;
				lRet	 = MAKELRESULT(wMnem, nRetCode);
			}
		}

		return lRet;
	}
};



class CFavoritesMenuOption {
public:
	static DWORD			  s_dwStyle;

private:
	static DWORD			  s_dwMaxMenuItemTextLength;
	static DWORD			  s_dwMaxMenuBreakCount;
	static CString *		  s_pUserDirectory;

	static CSimpleArray<HWND>*	s_pWnds;

public:
	static void Init()
	{
		ATLASSERT( s_pWnds == NULL );				//+++
		ATLASSERT( s_pUserDirectory == NULL );		//+++
		s_pWnds 		 = new CSimpleArray<HWND>();
		s_pUserDirectory = new CString();

		GetProfile();
	}


	static void Term()
	{
		WriteProfile();

		delete 	s_pWnds;
		delete 	s_pUserDirectory;
		s_pWnds 		 = NULL;	//+++
		s_pUserDirectory = NULL;	//+++
	}


	static void Add(HWND hWnd)
	{
		s_pWnds->Add(hWnd);
	}


	static void CallBack()
	{
		for (int i = 0; i < s_pWnds->GetSize(); ++i) {
			HWND hWnd = (*s_pWnds)[i];

			if ( ::IsWindow(hWnd) )
				MtlPostCommand(hWnd, ID_VIEW_REFRESH_FAVBAR);
		}
	}


	static CString GetUserDirectory()
	{
		return *s_pUserDirectory;
	}


	static void SetUserDirectory(const CString &strPath)
	{
		*s_pUserDirectory = strPath;
	}


	static DWORD GetStyle()
	{
		return s_dwStyle;
	}


	static void SetStyle(DWORD dwStyle)
	{
		s_dwStyle = dwStyle;
	}


	static void SetMaxMenuItemTextLength(DWORD n)
	{
		s_dwMaxMenuItemTextLength = n;
	}


	static DWORD GetMaxMenuItemTextLength()
	{
		return s_dwMaxMenuItemTextLength;
	}


	static void SetMaxMenuBreakCount(DWORD n)
	{
		s_dwMaxMenuBreakCount = n;
	}


	static DWORD GetMaxMenuBreakCount()
	{
		return s_dwMaxMenuBreakCount;
	}


	static void GetProfile()
	{
		CIniFileI	pr( g_szIniFileName, _T("FavoritesMenu") );

		pr.QueryValue( s_dwStyle				, _T("Style") );
		pr.QueryValue( s_dwMaxMenuItemTextLength, _T("Max_Text_Length") );
		pr.QueryValue( s_dwMaxMenuBreakCount	, _T("Max_Break_Count") );

		*s_pUserDirectory	= pr.GetStringUW(_T("UserFolder"));
	}


	static void WriteProfile()
	{
		CIniFileO	pr( g_szIniFileName, _T("FavoritesMenu") );

		pr.SetValue( s_dwStyle					, _T("Style")			);
		pr.SetStringUW( *s_pUserDirectory		, _T("UserFolder")		);
		pr.SetValue( s_dwMaxMenuItemTextLength	, _T("Max_Text_Length") );
		pr.SetValue( s_dwMaxMenuBreakCount		, _T("Max_Break_Count") );
	}
};



__declspec(selectany) DWORD CFavoritesMenuOption::s_dwStyle 				= EMS_DRAW_ICON | EMS_ORDER_FAVORITES | EMS_ADDITIONALMENUITEM | EMS_CUSTOMIZE_ORDER;
__declspec(selectany) DWORD CFavoritesMenuOption::s_dwMaxMenuItemTextLength = 55;
__declspec(selectany) DWORD CFavoritesMenuOption::s_dwMaxMenuBreakCount 	= 5000;
__declspec(selectany) CSimpleArray<HWND>*CFavoritesMenuOption::s_pWnds		= NULL;
__declspec(selectany) CString * CFavoritesMenuOption::s_pUserDirectory		= NULL;



inline CString DonutGetFavoritesFolder()
{
	if ( _check_flag( EMS_USER_DEFINED_FOLDER, CFavoritesMenuOption::GetStyle() ) ) {
		CString strDir = Misc::GetFullPath_ForExe( CFavoritesMenuOption::GetUserDirectory() );

		if ( !strDir.IsEmpty() ) {
			return strDir;
		}
	}

	CString 	strStdDir;
	MtlGetFavoritesFolder(strStdDir);

	return strStdDir;
}



inline CString DonutGetFavoriteGroupFolder()
{
	return Misc::GetExeDirectory() + _T("FavoriteGroup\\");
}
