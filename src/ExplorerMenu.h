/**
 *	@file	ExplorerMenu.h
 *	@brief	エクスプローラ・メニュー
 */
#pragma once

#include <vector>
#include <algorithm>

#include "DonutDefine.h"
#include "FavoriteOrder.h"
#include "MtlCtrlw.h"
#include "resource.h"
#include "nsIStyleSheetService.h"
#define WM_MENURBUTTONUP			0x0122

#ifndef TPM_RECURSE
#define TPM_RECURSE 				0x0001L
#endif


// for debug
#ifdef _DEBUG
	const bool _Mtl_ExplorerMenu_traceOn = false;
	#define epmTRACE	if (_Mtl_ExplorerMenu_traceOn) ATLTRACE
#else
	#define epmTRACE
#endif



// be care, if no element, &array[0] is invalid.
template <class _Key, class _Val>
class CCrasyMap : public CSimpleArray< std::pair<_Key, _Val> > {
public:
	bool FindKey(const _Key &__key)
	{
		if (GetSize() == 0)
			return false;

		return end() != std::find_if( begin(), end(), _Finder(__key) );
	}


	void Add(const _Key &__key, const _Val &__val)
	{
		if (GetSize() == 0) {
			CSimpleArray< std::pair<_Key, _Val> >::Add( std::make_pair(__key, __val) );
			return;
		}

		std::pair<_Key, _Val>*p = std::find_if( begin(), end(), _Finder(__key) );

		if ( p == end() )
			CSimpleArray< std::pair<_Key, _Val> >::Add( std::make_pair(__key, __val) );
		else
			(*p).second = __val;
	}


	_Val Lookup(const _Key &__key)
	{
		if (GetSize() == 0)
			return _Val();

		std::pair<_Key, _Val>*p = std::find_if( begin(), end(), _Finder(__key) );

		if ( p != end() )
			return p->second;
		else
			return _Val();
	}


	void Sort()
	{
		if (GetSize() == 0)
			return;

		std::sort( begin(), end(), _Compare() );
	}


	std::pair<_Key, _Val>*begin()
	{
		return &(*this)[0];
	}


	std::pair<_Key, _Val>*end()
	{
		return &(*this)[0] + GetSize();
	}


	//	template <class _Key, class _Val>
	struct _Finder {
		_Key __aKey;
		_Finder(_Key __key) : __aKey(__key) { }
		bool operator ()(const std::pair<_Key, _Val> &src)
		{
			return (__aKey == src.first);
		}
	};

	//	template <class _Key, class _Val>
	struct _Compare {
		bool operator ()(const std::pair<_Key, _Val> &x, const std::pair<_Key, _Val> &y)
		{
			return x.second < y.second;
		}
	};
};



#define MESSAGE_HANDLER_WND(msg, func)						   \
	if (uMsg == msg) {										   \
		bHandled = TRUE;									   \
		lResult  = func(hWnd, uMsg, wParam, lParam, bHandled); \
		if (bHandled)										   \
			return TRUE;									   \
	}

// Extended styles

enum EExplorerMenuStyle {
	EMS_ADDITIONALMENUITEM		= 0x00000001L,
	EMS_ADDITIONALMENUITEMNOSEP = 0x00000002L,
	EMS_DRAW_ICON				= 0x00000004L,		// UH
	EMS_SPACE_MIN				= 0x00000008L,		// UH
	EMS_DEL_NOASK				= 0x00000010L,		// UH
	EMS_ADDITIONAL_SAVEFAV_TO	= 0x00000020L,		// minit
	EMS_CUSTOMIZE_ORDER 		= 0x00000040L,
	EMS_ADDITIONAL_RESETORDER	= 0x00000080L,
	EMS_USER					= 0x00010000L,
};



enum EAdditional_Type {
	ADDITIONAL_TYPE_OPENALL 	= 0x00000001L,
	ADDITIONAL_TYPE_SAVEFAV 	= 0x00000002L,
	ADDITIONAL_TYPE_RESETORDER	= 0x00000004L,
};



#define PASS_MSG_MAP_MENUOWNER_TO_EXPMENU(x) \
	if ( x.m_menu.IsMenu() ) {				 \
		CHAIN_MSG_MAP_MEMBER(x) 			 \
	}

#define PASS_MSG_MAP_MENUOWNER_TO_EXPMENU_DEFAULT(x) \
	if (x.m_menu.IsMenu()  && m_bShow == FALSE) {	 \
		CHAIN_MSG_MAP_MEMBER(x) 					 \
	}

#define HANDLE_MENU_MESSAGE_CONTEXTMENU_EX(x)										 \
	if (uMsg == WM_INITMENUPOPUP || uMsg == WM_DRAWITEM || uMsg == WM_MEASUREITEM) { \
		if (x != NULL) {															 \
			bHandled = TRUE;														 \
			OnMsgHandle(uMsg, wParam, lParam);										 \
			lResult  = x->HandleMenuMsg(uMsg, wParam, lParam);						 \
			if (lResult != NOERROR) bHandled = FALSE;								 \
		} else {																	 \
			bHandled = FALSE;														 \
		}																			 \
		if (bHandled)																 \
			return TRUE;															 \
	}



class CExplorerContextMenuWindow : public CWindowImpl<CExplorerContextMenuWindow> {
private:
	CString 				m_strFileName;
	LPVOID					m_pVoid;
	int 					m_nCount;

	typedef void   (*SHOWCONTEXTMENUONPOPUP)(CString &strFileName, HWND hWnd, LPVOID lpParam);
	SHOWCONTEXTMENUONPOPUP	m_fpnShowContext;

public:
	CExplorerContextMenuWindow() : m_strFileName(), m_pVoid(0), m_nCount(0), m_fpnShowContext(0) {;}		//+++

	BEGIN_MSG_MAP(CExplorerContextMenuWindow)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

private:

public:
	void SetData(SHOWCONTEXTMENUONPOPUP pFunc, CString strFileName, LPVOID lpParam)
	{
		m_fpnShowContext = pFunc;
		m_strFileName	 = strFileName;
		m_pVoid 		 = lpParam;
		m_nCount		 = 0;
	}


private:
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		m_nCount++;
		//if(m_nCount < 5) return 0;
		::KillTimer(m_hWnd, (UINT) wParam);
		m_fpnShowContext(m_strFileName, m_hWnd, m_pVoid);
		DestroyWindow();
		return 0;
	}
};




class CMenuItem {
public:
	CString 	m_strText;
	CString 	m_strPath;
	BOOL		m_bDir;
	BOOL		m_bSelected;
	HWND		m_hItemWnd;
	BOOL		m_bIcon;		//minit
	HMENU		m_hMenu;		//minit

public:
	CMenuItem()
	{
		m_strText	= _T("");
		m_strPath	= _T("");
		m_bDir		= FALSE;
		m_bSelected = FALSE;
		m_hItemWnd	= NULL;
		m_bIcon 	= FALSE;
		m_hMenu 	= NULL; 	//+++
	}
};



template <class T>
class ATL_NO_VTABLE CExplorerMenuImpl /* : public CExplorerMenuImplBase */ {
	// Declarations

	// Constants

	// Data members
protected:
	DWORD						 m_dwStyle;

public:
	CMenuHandle 				 m_menu;

private:
	//	CMenu m_menuOriginal;
	int 						 m_nOriginalMenuItemCountExceptInsertPointMenuItem;
	int 						 m_nOriginalInsertPoint;

public:
	int 						 m_nMaxMenuBreakCount;
	int 						 m_nMaxMenuItemTextLength;

private:
	CString 					 m_strRootDirectoryPath;
	CString 					 m_strNoEntries;
	CString 					 m_strAdditional;

	CSimpleArray<HMENU> 		 m_arrMenuHandle;
	int 						 m_nCurrentMenuID;
	CCrasyMap<int, CString> 	 m_mapID;		// id and file path
	CCrasyMap<HMENU, CString>	 m_mapSubMenu;	// menu handle and file path

	int 						 m_nInsertPointMenuItemID;
	int 						 m_nMinID;
	int 						 m_nMaxID;

	enum { s_nIDLast = 0x0100 , s_nIDExProp = 1 };

	CSimpleArray<CString>		 m_arrIgnoredPath;

	CSimpleArray<CMenuItem *>	 m_arrMenuItem;
	HIMAGELIST					 m_hImgList;
	CImageList					 m_img;
	CFont						 m_fontMenu;
	int 						 m_nImgDir;
	int 						 m_nImgUrl;

public:
	int 						 m_nSelIndex;
private:
	CCrasyMap<CString, int> 	 m_mapIcon; 	// id and file path
	CItemIDList 				 m_idlHtm;		// used to draw .url icon faster

	BOOL						 m_bResetIDCount;
public:
	HWND						 m_hWnd;
private:
	CComPtr<IShellFolder>		 m_pSHDesktopFolder;
	CComPtr<IContextMenu2>		 m_pContMenu2;

	HMENU						 m_hLastPopupMenu;
	int 						 m_nLastID;

	CExplorerContextMenuWindow	 m_wndMenu;

	int 						 m_nScrollCount;


public:
	// Constructor
	CExplorerMenuImpl(
			int 			nInsertPointMenuItemID,
			const CString&	strNoEntries			= _T("(empty)"),
			int 			nMinID					= 0x7000,
			int 			nMaxID					= 0x7FFF,
			const CString&	strAdditional			= _T("Open This Directory"),
			int 			nMaxMenuItemTextLength	= 55
	)	: m_strNoEntries(strNoEntries)
		, m_strAdditional(strAdditional)
		, m_nInsertPointMenuItemID(nInsertPointMenuItemID)
		, m_nMinID(nMinID)
		, m_nMaxID(nMaxID)
		, m_dwStyle(0)
		, m_nMaxMenuBreakCount(5000)
		, m_nMaxMenuItemTextLength(55)
		, m_nScrollCount(0)
	  #if 1 //+++
		, m_nOriginalMenuItemCountExceptInsertPointMenuItem(0)
		, m_nOriginalInsertPoint(0)
		, m_nLastID(0)
		, m_idlHtm( MtlGetHtmlFileIDList() )
	   #if 1 //+++ 書く必要ないが抜けチェック対策で.
		, m_menu()
		, m_strRootDirectoryPath()
		, m_arrMenuHandle()
		, m_mapID()
		, m_mapSubMenu()
		, m_arrIgnoredPath()
		, m_arrMenuItem()
		, m_img()
		, m_fontMenu()
		, m_mapIcon()
		, m_pSHDesktopFolder()
		, m_pContMenu2()
		, m_wndMenu()
	   #endif
	  #endif
	{
		m_nCurrentMenuID = m_nMinID;

		SHFILEINFO		sfi;
		m_hImgList		 = (HIMAGELIST) ::SHGetFileInfo(_T("C:\\"), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		m_img.Attach(m_hImgList);

		m_nImgDir		 = -1;
		m_nImgUrl		 = -1;
		GetSystemSettings();
		m_nSelIndex 	 = -1;

		m_bResetIDCount  = FALSE;
		m_hWnd			 = NULL;

		m_hLastPopupMenu = NULL;
	}


	~CExplorerMenuImpl()
	{
		//		m_img.Destroy();
	}


	void RefreshMenu()
	{
	}


private:
	void GetSystemSettings()
	{
		// refresh our font
		NONCLIENTMETRICS info = {0};

		info.cbSize = sizeof (info);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof (info), &info, 0);
		LOGFONT 		 logfont;
		memset( &logfont, 0, sizeof (LOGFONT) );

		if (m_fontMenu.m_hFont != NULL)
			m_fontMenu.GetLogFont(logfont);

		if (  logfont.lfHeight			!= info.lfMenuFont.lfHeight
		   || logfont.lfWidth			!= info.lfMenuFont.lfWidth
		   || logfont.lfEscapement		!= info.lfMenuFont.lfEscapement
		   || logfont.lfOrientation 	!= info.lfMenuFont.lfOrientation
		   || logfont.lfWeight			!= info.lfMenuFont.lfWeight
		   || logfont.lfItalic			!= info.lfMenuFont.lfItalic
		   || logfont.lfUnderline		!= info.lfMenuFont.lfUnderline
		   || logfont.lfStrikeOut		!= info.lfMenuFont.lfStrikeOut
		   || logfont.lfCharSet 		!= info.lfMenuFont.lfCharSet
		   || logfont.lfOutPrecision	!= info.lfMenuFont.lfOutPrecision
		   || logfont.lfClipPrecision	!= info.lfMenuFont.lfClipPrecision
		   || logfont.lfQuality 		!= info.lfMenuFont.lfQuality
		   || logfont.lfPitchAndFamily	!= info.lfMenuFont.lfPitchAndFamily
		   || lstrcmp(logfont.lfFaceName, info.lfMenuFont.lfFaceName) != 0)
		{
			HFONT hFontMenu = ::CreateFontIndirect(&info.lfMenuFont);
			ATLASSERT(hFontMenu != NULL);

			if (hFontMenu != NULL) {
				if (m_fontMenu.m_hFont != NULL)
					m_fontMenu.DeleteObject();

				m_fontMenu.Attach(hFontMenu);
			}
		}
	}


public:
	BOOL InstallExplorerMenu(HMENU hMenu)
	{
		SetMenuHandle(hMenu);

		if (!m_pSHDesktopFolder) {
			HRESULT hr = ::SHGetDesktopFolder(&m_pSHDesktopFolder);

			if ( FAILED(hr) ) return FALSE;
		}

		return TRUE;
	}


	// Overridables
	void OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos)
	{
		//CDebugWindow::OutPutString("Menu Sorting - ascending method");
		std::sort( _begin(infos), _end(infos), _DefaultMenuItemCompare(*this) );
		//CDebugWindow::OutPutString("Menu Sorting - ascending method End");
	}


private:
	void OnExecute(const CString &strFilePath)
	{
	}


	CString OnGetMenuItemText(const CString &strPath)
	{
		CString str = MtlGetDisplayTextFromPath(strPath);

		str = MtlCompactString(str, m_nMaxMenuItemTextLength);
		str.Replace( _T("&"), _T("&&") );

		return str;
	}


	void OnMenuPrepareContext(CItemIDList &idlFolder)
	{
	}


	// Attributes
	void SetMenuHandle(HMENU hMenu)
	{
		ATLASSERT( ::IsMenu(hMenu) );

		// save original menu infos
		ATLASSERT(_FindInsertPoint(hMenu) != -1); // need InsertPointMenuItem
		m_nOriginalMenuItemCountExceptInsertPointMenuItem = ::GetMenuItemCount(hMenu) - 1;
		m_nOriginalInsertPoint							  = _FindInsertPoint(hMenu);

		m_menu											  = hMenu;
		T *pT = static_cast<T *>(this);
		pT->OnSetMenuHandle(hMenu);
	}


	void OnSetMenuHandle(HMENU hMenu)
	{
	}


	CString GetRootDirectoryPath() const
	{
		return m_strRootDirectoryPath;
	}


public:
	void SetRootDirectoryPath(const CString &strDirectoryPath)
	{
		//ATLASSERT(strDirectoryPath.IsEmpty() == FALSE);
		m_strRootDirectoryPath = strDirectoryPath;
	}


private:
	BOOL LoadAdditionalMenu(_U_STRINGorID menu)
	{
		return m_menuAdditional.LoadMenu(menu);
	}


public:
	void SetStyle(DWORD dwStyle)
	{
		m_dwStyle = dwStyle;
	}


	DWORD GetStyle() const
	{
		return m_dwStyle;
	}


	void AddIgnoredPath(CString &strPath)
	{
		m_arrIgnoredPath.Add(strPath);
	}


	void ResetIgnoredPath()
	{
		m_arrIgnoredPath.RemoveAll();
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CExplorerMenu)

		//if(CMainOption::s_dwErrorBlock < 5){
			HANDLE_MENU_MESSAGE_CONTEXTMENU_EX(m_pContMenu2)
		//}
		MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)

		MESSAGE_HANDLER_WND(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER_WND(WM_MENURBUTTONUP, OnRButtonUp)

		MESSAGE_HANDLER_WND(SC_VSCROLL,OnVScroll)

		COMMAND_RANGE_HANDLER_EX(m_nMinID, m_nMaxID, OnCommandRange)
	END_MSG_MAP()


private:
	LRESULT OnVScroll(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		DefWindowProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}


public:
	LRESULT OnRButtonUp(HWND hWnd, UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CMenuItem * pMenuItemSel = NULL;
		WORD		wID 		 = -1;
		CMenuHandle menu( (HMENU) lParam );
		HMENU		hMyMenu 	 = m_menu.m_hMenu;

		int 		nID 		 = (int) menu.GetMenuItemID( int (wParam) );

		if (nID < m_nMinID || m_nMaxID < nID) {
			bHandled = FALSE;
			return 0;
		}

		for (int ii = 0; ii < menu.GetMenuItemCount(); ii++) {
			CMenuItemInfo mii;
			mii.fMask	 = MIIM_DATA | MIIM_ID | MIIM_STATE;
			menu.GetMenuItemInfo(ii, TRUE, &mii);

			CMenuItem *   pMenuItem = (CMenuItem *) mii.dwItemData;

			if (pMenuItem == NULL) continue;

			//if (pMenuItem->m_bSelected==FALSE) continue;
			if (pMenuItem->m_hMenu != hMyMenu) {
				bHandled = FALSE;
				return 0;
			}

			if (mii.fState != MFS_HILITE) continue; 	//minit

			pMenuItemSel = pMenuItem;
			wID 		 = mii.wID;
			break;
		}

		if (pMenuItemSel == NULL) return 0;

		if ( _CheckOsVersion_2000Later() ) {
			//2000 XP 2003Server ...later
			//さらに右クリックメニューを表示させる
			if ( !::IsWindow(hWnd) )	//!::IsWindow(m_hWnd)) //メニュー表示にハンドルが必要
				return 1;

			CString strFileName;

			if (pMenuItemSel->m_bDir)
				strFileName = pMenuItemSel->m_strPath.Left(pMenuItemSel->m_strPath.GetLength() - 1) + _T('\0');
			else
				strFileName = pMenuItemSel->m_strPath + _T('\0');

			//if(CMainOption::s_dwErrorBlock < 5)
			ShowContextMenuOnPopup(strFileName, hWnd, this);
		} else if ( _CheckOsVersion_98Later() ) {
			//95 98 ME ...etc
			//右クリックメニューが表示できない()ので拡張プロパティ画面を出す

			if ( !::IsWindow(hWnd) )	//!::IsWindow(m_hWnd)) //メニュー表示にハンドルが必要
				return 1;

			CString strFileName;

			if (pMenuItemSel->m_bDir)
				strFileName = pMenuItemSel->m_strPath.Left(pMenuItemSel->m_strPath.GetLength() - 1) + _T('\0');
			else
				strFileName = pMenuItemSel->m_strPath + _T('\0');

			HWND	hWndPopup;

			while (1) {
				hWndPopup = ::FindWindow(_T("#32768"), NULL);

				if ( ::IsWindow(hWndPopup) && ::IsWindowVisible(hWndPopup) ) {
					::SendMessage(hWndPopup, WM_KEYDOWN, VK_ESCAPE, 0);
				} else
					break;
			}

			RECT	rc = { 0, 0, 1, 1 };
			m_wndMenu.Create(hWnd, rc, NULL, WS_POPUP);
			m_wndMenu.SetData(ShowContextMenuOnPopup, strFileName, this);
			::SetTimer(m_wndMenu.m_hWnd, 0, 1, NULL);

		} else {
			if ( !::IsWindow(hWnd) )	//!::IsWindow(m_hWnd)) //メニュー表示にハンドルが必要
				return 1;

			CString strFileName;

			if (pMenuItemSel->m_bDir)
				strFileName = pMenuItemSel->m_strPath.Left(pMenuItemSel->m_strPath.GetLength() - 1) + _T('\0');
			else
				strFileName = pMenuItemSel->m_strPath + _T('\0');

			::SendMessage(CWindow(hWnd).GetTopLevelParent(), WM_SET_EXPROPERTY, (WPARAM) (LPCTSTR) strFileName, 0);
		}

		bHandled = TRUE;
		return 1;
	}


private:
	static void ShowContextMenuOnPopup(CString &strFileName, HWND hWnd, LPVOID lpParam)
	{
		CExplorerMenuImpl<T> *pThis 		 = (CExplorerMenuImpl<T>*)lpParam;

		HRESULT 			  hr;
		CString 			  strFolder;
		CComPtr<IShellFolder> pSHFolder;
		CComPtr<IContextMenu> pContMenu;
		CItemIDList 		  idlFile;
		CItemIDList 		  idlFolder;
		CPoint				  pos;
		CMenu				  menuC;
		int 				  iCmd;
		int 				  flags;
		HWND				  hWndPopup;
		CMINVOKECOMMANDINFO   InvokeInfo	 = { sizeof (CMINVOKECOMMANDINFO) };

		strFolder = strFileName.Left( strFileName.ReverseFind('\\') );
		idlFolder = strFolder;
		idlFile   = strFileName;
		idlFile  -= idlFolder;

		if ( idlFile.IsNull() ) return ;

		CString 			  strCheckFile	 = idlFile.GetPath();
		CString 			  strCheckFolder = idlFolder.GetPath();

		::GetCursorPos(&pos);
		menuC.CreatePopupMenu();

		if (menuC.m_hMenu == NULL) return ;

		hr		  = pThis->m_pSHDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &pSHFolder);

		if (FAILED(hr) || pSHFolder == NULL) return ;

		LPCITEMIDLIST		  pIidList		 = (LPCITEMIDLIST) (LPITEMIDLIST) idlFile;
		hr		  = pSHFolder->GetUIObjectOf(pThis->m_hWnd, 1, &pIidList, IID_IContextMenu, NULL, (void **) &pContMenu);

		if (hr != NOERROR || pContMenu == NULL) return ;

		BOOL				  bMenuEx		 = TRUE;
		pThis->m_pContMenu2.Release();
		hr		  = pContMenu->QueryInterface(IID_IContextMenu2, (void **) &pThis->m_pContMenu2);

		if (FAILED(hr) || pThis->m_pContMenu2 == NULL) {
			bMenuEx = FALSE;
			hr		= pSHFolder->GetUIObjectOf(pThis->m_hWnd, 1, &pIidList, IID_IContextMenu, 0, (void **) &pThis->m_pContMenu2);

			if ( FAILED(hr) )
				return;
		}

		int 				  nPos			 = 0;
		menuC.InsertMenu( 0, MF_BYPOSITION | MF_ENABLED, s_nIDExProp, _T("拡張プロパティ(&E)") );
		menuC.InsertMenu( 1, MF_BYPOSITION | MF_SEPARATOR, 0);
		nPos	 += 2;

		hr		  = pThis->m_pContMenu2->QueryContextMenu(menuC, nPos, s_nIDLast, 0x7FFF, CMF_NORMAL);

		if (hr == 0) goto CleanUp;

		//m_bMessageStop = TRUE;
		flags	  = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | 0x00000001; //TPM_RECURSE
		iCmd	  = menuC.TrackPopupMenu(flags | TPM_RETURNCMD, pos.x, pos.y, hWnd, NULL);
		//m_bMessageStop = FALSE;

		if ( _CheckOsVersion_2000Later() ) {
			while (1) {
				hWndPopup = ::FindWindow(_T("#32768"), NULL);

				if ( ::IsWindow(hWndPopup) && ::IsWindowVisible(hWndPopup) ) {
					::SendMessage(hWndPopup, WM_KEYDOWN, VK_ESCAPE, 0);
				} else
					break;
			}
		}

		if (iCmd == 0) goto CleanUp;

		if (iCmd == s_nIDExProp) {
			::SendMessage(CWindow(hWnd).GetTopLevelParent(), WM_SET_EXPROPERTY, (WPARAM) (LPCTSTR) strFileName, 0);
			goto CleanUp;
		}

		//if(CMainOption::s_dwErrorBlock < 4){
		InvokeInfo.fMask	= 0;
		InvokeInfo.hwnd 	= pThis->m_hWnd;
		InvokeInfo.lpVerb	= (LPCSTR) MAKEINTRESOURCE(iCmd - s_nIDLast);		//+++ //*メモ UNICODE対策いる?いらない?
		InvokeInfo.nShow	= SW_SHOWNORMAL;
		hr					= pThis->m_pContMenu2->InvokeCommand(&InvokeInfo);

	  CleanUp:
		pThis->m_pContMenu2.Release();
		pThis->m_pContMenu2 = NULL;
	}


	struct _Function_MenuClose {
		bool operator ()(HWND hWnd)
		{
		  #if 1 	//+++
			if (::IsWindow(hWnd) && ::IsWindowVisible(hWnd) ) {
				TCHAR	buf[ 32 ];
				buf[0] = 0;
				::GetClassName(hWnd, buf, 10);
				CString 	str = buf;
				if ( str == _T("#32768") )
					::SendMessage(hWnd, WM_KEYDOWN, VK_ESCAPE, 0);
			}
			return	true;
		  #else
			CString 	str;
			::GetClassName(hWnd, str.GetBuffer(11), 10);
			str.ReleaseBuffer();
			if ( str == _T("#32768") && ::IsWindow(hWnd) && ::IsWindowVisible(hWnd) )
				::SendMessage(hWnd, WM_KEYDOWN, VK_ESCAPE, 0);

			return	true;
		  #endif
		}
	};


	enum _CmdBarDrawConstants {
		s_kcxGap		  = 1,
		s_kcxTextMargin   = 2,
		s_kcxButtonMargin = 3,
		s_kcyButtonMargin = 3
	};


public:
	enum {
		_nMaxMenuItemTextLength = 100,
	};


private:
	enum {
		_chChevronShortcut		= _T('/')
	};


	LRESULT OnMsgHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_INITMENUPOPUP)
			ATLTRACE("##### WM_INITMENUPOPUP %x %x\n", wParam, lParam);
		else if (uMsg == WM_MEASUREITEM)
			ATLTRACE("##### WM_MEASUREITEM   %x %x\n", wParam, lParam);
		else
			ATLTRACE("##### WM_DRAWITEM      %x %x\n", wParam, lParam);

		return 0;
	}


	LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		LPMEASUREITEMSTRUCT lpMeasureItemStruct = (LPMEASUREITEMSTRUCT) lParam;

		ATLASSERT( (int) wParam == 0 );

		//まずメニュー項目のIDをチェック
		if (_CheckID(lpMeasureItemStruct->itemID) == FALSE) {
			bHandled = FALSE;
			return TRUE;
		}

		if (lpMeasureItemStruct->CtlType == ODT_MENU && lpMeasureItemStruct->itemData != NULL
		   && ( (CMenuItem *) lpMeasureItemStruct->itemData )->m_bIcon)
		{
			CMenuItem *pMenuData = (CMenuItem *) lpMeasureItemStruct->itemData;
			int 	   nImgCx	 = 0, nImgCy = 0;
			m_img.GetIconSize(nImgCx, nImgCy);

			// compute size of text - use DrawText with DT_CALCRECT
			CWindowDC  dc(NULL);
			HFONT	   hOldFont;

			// need bold version of font
			LOGFONT    lf;
			m_fontMenu.GetLogFont(lf);
			lf.lfWeight 				  += 200;
			CFont	   fontBold;
			fontBold.CreateFontIndirect(&lf);
			ATLASSERT(fontBold.m_hFont != NULL);
			hOldFont					   = dc.SelectFont(fontBold);

			RECT	   rcText	 = { 0, 0, 0, 0 };
			dc.DrawText(pMenuData->m_strText, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
			int 	   cx		 = rcText.right - rcText.left;
			dc.SelectFont(hOldFont);

			// height of item is the bigger of these two
			if (CFavoritesMenuOption::GetStyle() & EMS_SPACE_MIN)
				lpMeasureItemStruct->itemHeight = nImgCy;
			else
				lpMeasureItemStruct->itemHeight = (int) (nImgCy * 1.3);

			// width is width of text plus a bunch of stuff
			cx		+= 2 * s_kcxTextMargin; 	// L/R margin for readability
			cx		+= s_kcxGap;				// space between button and menu text
			cx		+= 2 * nImgCx;				// button width (L=button; R=empty margin)

			// Windows adds 1 to returned value
			cx							  -= ::GetSystemMetrics(SM_CXMENUCHECK) - 2;
			lpMeasureItemStruct->itemWidth = (int) (cx * 0.80); 		// done deal

		} else {
			bHandled = FALSE;
		}

		return (LRESULT) TRUE;
	}


	LRESULT OnDrawItem(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT) lParam;

		if (lpDrawItemStruct->CtlType != ODT_MENU) {
			bHandled = FALSE;
			return TRUE;
		}

		if (lpDrawItemStruct->itemData == 0) {
			bHandled = FALSE;
			return TRUE;
		}

		if (_CheckID(lpDrawItemStruct->itemID) == FALSE) {
			bHandled = FALSE;
			return TRUE;
		}

		CDCHandle		 dc 			  = lpDrawItemStruct->hDC;
		const RECT &	 rcItem 		  = lpDrawItemStruct->rcItem;

		CMenuHandle 	 menu			  = (HMENU) lpDrawItemStruct->hwndItem;

		CString 		 strCmd;
		CMenuItem * 	 pMenuData		  = (CMenuItem *) lpDrawItemStruct->itemData;

		int 			 nIndex 		  = 0;

		if (pMenuData && pMenuData->m_bIcon) {
			if (pMenuData->m_bDir) {
				if (m_nImgDir == -1) {
					CItemIDList idlURL(pMenuData->m_strPath);
					//					m_nImgDir = MtlGetSystemIconIndex(idlURL);
					m_nImgDir = MtlGetNormalIconIndex(idlURL);
				}

				nIndex = m_nImgDir;
			} else {
				CString strExt;
				strExt = pMenuData->m_strPath.Mid(pMenuData->m_strPath.ReverseFind('.') + 1);
				strExt.MakeUpper();

				if ( strExt == _T("EXE") || strExt == _T("LNK") ) {
					CItemIDList idlURL(pMenuData->m_strPath);
					//					nIndex = MtlGetSystemIconIndex(idlURL);
					nIndex = MtlGetNormalIconIndex(idlURL);
				} else {
					int nIcon = m_mapIcon.Lookup(strExt);

					if (nIcon == 0) {
						CItemIDList idlURL(pMenuData->m_strPath);
						//						nIcon = MtlGetSystemIconIndex(idlURL);
						nIcon = MtlGetNormalIconIndex(idlURL, m_idlHtm);
						m_mapIcon.Add(strExt, nIcon);
					}

					nIndex = nIcon;
				}
			}
		}

		BOOL		bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
		if (bSelected || lpDrawItemStruct->itemAction == ODA_SELECT) {
			RECT	rcBG	= rcItem;
			BOOL	bRet	= dc.FillRect( &rcBG, (HBRUSH) LongToPtr( bSelected ? (COLOR_HIGHLIGHT + 1) : (COLOR_MENU + 1) ) );
		}

		pMenuData->m_bSelected = bSelected;
		pMenuData->m_hItemWnd  = lpDrawItemStruct->hwndItem;

		// draw pushed-in or popped-out edge
		::ImageList_Draw(m_hImgList, nIndex, dc, rcItem.left + 1, rcItem.top + 1, ILD_TRANSPARENT);

		RECT	rcText	= rcItem;
		rcText.left 	+= 18;

		COLORREF		 colorText = ::GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT);

		dc.SetBkMode(TRANSPARENT);

		if (pMenuData)
			strCmd = pMenuData->m_strText;

		DrawMenuText(dc, rcText, strCmd, colorText);			// finally!


		return (LRESULT) TRUE;
	}


	void DrawMenuText(CDCHandle& dc, RECT& rc, LPCTSTR lpstrText, COLORREF color)
	{
		int nTab = -1;
		int len  = lstrlen(lpstrText);
		for (int i = 0; i < len; i++)
		{
			if(lpstrText[i] == '\t')
			{
				nTab = i;
				break;
			}
		}
		dc.SetTextColor(color);
		dc.DrawText(lpstrText, nTab, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		if(nTab != -1)
			dc.DrawText(&lpstrText[nTab + 1], -1, &rc, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	}


	void OnInitMenuPopup(HMENU hmenuPopup, UINT uPos, BOOL fSystemMenu)
	{
		SetMsgHandled(FALSE);

		if ( m_strRootDirectoryPath.IsEmpty() )
			return;

		m_nSelIndex = -1;
		epmTRACE(_T("CExplorerMenu::OnInitMenuPopup : %d\n"), m_nCurrentMenuID);

		//		Somehow, fSystemMenu is sometimes wrong value, why?
		//		if (fSystemMenu) {// window menu, do nothing
		//			epmTRACE(_T(" It's system menu, return");
		//			return;
		//		}

		if (m_menu.m_hMenu == hmenuPopup) { 					// root menu
			epmTRACE( _T(" It's root menu\n") );
			// delayed clean up if menu not clicked
			m_mapID.RemoveAll();
			// at first, add it to the list
			m_arrMenuHandle.Add(hmenuPopup);

			// restore original menu (it's need cuz WM_MENUSELECT is not sent if not toplevel menu. for example. chevron menu)
			_RestoreOriginalMenu();

			// delete insert point menu item
			_DeleteInsertPointMenuItem();

			if (m_bResetIDCount)
				m_nCurrentMenuID = m_nMinID;

			if (m_hLastPopupMenu && m_hLastPopupMenu == hmenuPopup) {
				m_nCurrentMenuID = m_nLastID;
			}

			m_nLastID		 = m_nCurrentMenuID;
			m_hLastPopupMenu = hmenuPopup;

			int nInsertPoint = m_nOriginalInsertPoint;
			_Explore(m_menu, nInsertPoint, m_strRootDirectoryPath);

		} else if (m_arrMenuHandle.Find(hmenuPopup) != -1) {	// it's my menu
			//*+++  あとで削除
			CString strMsg;
			strMsg.Format( _T("WM_INITPOPUPMENU - %s"), _LoadDirectoryPath(hmenuPopup) );
			//CDebugWindow::OutPutString(strMsg);

			// first, clean up all the menu items
			_DeleteAllMenuItems(hmenuPopup);
			_Explore( hmenuPopup, 0, _LoadDirectoryPath(hmenuPopup) );
			SetMsgHandled(TRUE);								// eat it!!
		}
	}


	void OnCommandRange(UINT, int nID, HWND hWndCtrl)
	{
		if (hWndCtrl) {
			SetMsgHandled(FALSE);
			return;
		}

		epmTRACE(_T("CExplorerMenu::OnCommandRange : %d\n"), nID);
		T*	pT = static_cast<T*>(this);
		if (CString() != m_mapID.Lookup(nID))
			pT->OnExecute(m_mapID.Lookup(nID));
		m_mapID.RemoveAll();// delayed clean up.
	}




	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UINT  uItem   = LOWORD(wParam);

		epmTRACE(_T("CExplorerMenu::OnMenuSelect uItem = %d\n"), uItem);

		UINT  fuFlags = HIWORD(wParam);
		HMENU hmenu   = (HMENU) lParam;

		SetMsgHandled(FALSE);
		bHandled = FALSE;										// must be, oh my god!!!!!!

		// Check if a menu is closing, do a cleanup
		if (fuFlags == 0xFFFF && hmenu == NULL) {				// menu closing
			// NOTE: this message sent only if top level menu!!!
			epmTRACE( _T(" Cleanup\n") );
			_CleanUpExceptIDMap();
		}
		//else if (m_arrMenuHandle.Find(hmenu) != -1){// it's my menu
		//}

		return 0;
	}


	void _CleanUpExceptIDMap()
	{
		epmTRACE( _T("CExplorereMenu::_CleanUpExceptIDMap\n") );

		// restore original menu
		_RestoreOriginalMenu();

		m_arrMenuHandle.RemoveAll();
		m_mapSubMenu.RemoveAll();

		for (int ii = 0; ii < m_arrMenuItem.GetSize(); ii++) {
			CMenuItem *pMenuData = m_arrMenuItem[ii];
			delete pMenuData;
			m_arrMenuItem[ii] = NULL;	//+++ delete後は強制クリアしておく.
		}

		m_arrMenuItem.RemoveAll();

		m_nCurrentMenuID = m_nMinID;
	}


	void _AddNoneMenuItem(CMenuHandle menu, int &nInsertPoint)
	{
		CMenuItemInfo mii;

		mii.fMask	   = MIIM_STATE | MIIM_TYPE;
		mii.fType	   = MFT_STRING;
		mii.fState	   = MFS_GRAYED;
		mii.dwTypeData = (LPTSTR) (LPCTSTR) m_strNoEntries;
		menu.InsertMenuItem(nInsertPoint++, TRUE, &mii);
	}


	struct _DefaultMenuItemCompare : public std::binary_function<const CMenuItemInfo&, const CMenuItemInfo&, bool> {
		CExplorerMenuImpl<T>* _expmenu;

		_DefaultMenuItemCompare(CExplorerMenuImpl<T>& m) : _expmenu(&m) { }

		bool operator()(const CMenuItemInfo& x, const CMenuItemInfo& y)
		{
			CString 	strPathA = _expmenu->_GetFilePath(x);
			CString 	strPathB = _expmenu->_GetFilePath(y);

			bool	bDirA = MtlIsDirectoryPath(strPathA);
			bool	bDirB = MtlIsDirectoryPath(strPathB);

			if (bDirA == bDirB) {
				//Old WTL Use lstrcmp on Compare() , but WTL 7 Use _tcscmp.
				int 	nRet = ::lstrcmp(strPathA,strPathB);
				return (nRet <= 0) /*? true : false*/;
				//return strPathA < strPathB;
			} else {
				if (bDirA)
					return true;
				else
					return false;
			}
		}
	};


	struct _SetupMenuItemInfoToArray : public std::unary_function<const CString &, void> {
		CSimpleArray<CMenuItemInfo>*	array;
		CExplorerMenuImpl<T>*			expmenu;

		_SetupMenuItemInfoToArray(CSimpleArray<CMenuItemInfo> &a, CExplorerMenuImpl<T> &m)
			: array(&a) , expmenu(&m)
		{
		}


		void operator ()(const CString &strPath, bool bDir)
		{
			if (bDir) {
				CString 	  strDirectoryPath2 = strPath;

				if (expmenu->m_arrIgnoredPath.Find(strDirectoryPath2) != -1)
					return;

				CMenuHandle   menuSub;
				menuSub.CreatePopupMenu();

				// add to map and array of menus
				expmenu->_SaveDirectoryPath(menuSub.m_hMenu, strPath);
				expmenu->m_arrMenuHandle.Add(menuSub.m_hMenu);

				LPTSTR		  lpstrText 		= new TCHAR[strPath.GetLength() + 1];
				::lstrcpy(lpstrText, strPath);

				// UH
				CMenuItem *   pMenuData 		= NULL;
				//if (expmenu->GetStyle()&EMS_DRAW_ICON)  //commented out by minit
				{
					pMenuData		   = new CMenuItem;
					pMenuData->m_strText.Format(_T("%s"), lpstrText);
					pMenuData->m_strPath.Format(_T("%s"), strPath);
					pMenuData->m_bDir  = TRUE;

					if (expmenu->GetStyle() & EMS_DRAW_ICON)
						pMenuData->m_bIcon = TRUE;

					pMenuData->m_hMenu = expmenu->m_menu.m_hMenu;
					expmenu->m_arrMenuItem.Add(pMenuData);
				}
				//^^^

				CMenuItemInfo mii;
				mii.fMask	   = MIIM_SUBMENU | MIIM_TYPE | MIIM_DATA;

				mii.fType	   = MFT_STRING;

				if (expmenu->GetStyle() & EMS_DRAW_ICON)
					mii.fType |= MFT_OWNERDRAW;

				mii.hSubMenu   = menuSub.m_hMenu;
				mii.dwTypeData = lpstrText;
				mii.dwItemData = (DWORD_PTR) pMenuData;

				array->Add(mii);
			} else {
				CString 	  strFilePath2 = strPath;

				if (expmenu->m_arrIgnoredPath.Find(strFilePath2) != -1)
					return;

				LPTSTR		  lpstrText    = new TCHAR[strPath.GetLength() + 1];
				::lstrcpy(lpstrText, strPath);

				// UH
				CMenuItem *   pMenuData    = NULL;
				//if (expmenu->GetStyle()&EMS_DRAW_ICON)  //commented out by minit
				{
					pMenuData		   = new CMenuItem;
					pMenuData->m_strText.Format(_T("%s"), lpstrText);
					pMenuData->m_strPath.Format(_T("%s"), strPath);

					if (expmenu->GetStyle() & EMS_DRAW_ICON)
						pMenuData->m_bIcon = TRUE;

					pMenuData->m_hMenu = expmenu->m_menu.m_hMenu;
					expmenu->m_arrMenuItem.Add(pMenuData);
				}
				//^^^

				CMenuItemInfo mii;
				mii.fMask	   = MIIM_ID | MIIM_TYPE | MIIM_DATA;

				mii.fType	   = MFT_STRING;

				if (expmenu->GetStyle() & EMS_DRAW_ICON)
					mii.fType |= MFT_OWNERDRAW;

				mii.wID 	   = ++expmenu->m_nCurrentMenuID;
				mii.dwTypeData = lpstrText;
				mii.dwItemData = (DWORD_PTR) pMenuData;

				if (expmenu->m_nCurrentMenuID < expmenu->m_nMaxID) {
					// add to command map
					expmenu->m_mapID.Add(expmenu->m_nCurrentMenuID, strPath);
					array->Add(mii);
				}
			}
		}
	};


	void _Explore(CMenuHandle menuPopup, int nInsertPoint, const CString &strDirectoryPath)
	{
		int 						nBreakCount = nInsertPoint;

		//「これらを全て開く」を追加
		if ( (m_dwStyle & EMS_ADDITIONALMENUITEM) && MtlIsFileExist(strDirectoryPath) )
			_AddAdditionalMenuItem(menuPopup, nInsertPoint, nBreakCount, strDirectoryPath, ADDITIONAL_TYPE_OPENALL);

		//「ここにお気に入り追加」を追加
		if ( (m_dwStyle & EMS_ADDITIONAL_SAVEFAV_TO) )
			_AddAdditionalMenuItem(menuPopup, nInsertPoint, nBreakCount, strDirectoryPath, ADDITIONAL_TYPE_SAVEFAV);

		if ( (m_dwStyle & EMS_ADDITIONAL_RESETORDER) && MtlIsFileExist(strDirectoryPath) )
			_AddAdditionalMenuItem(menuPopup, nInsertPoint, nBreakCount, strDirectoryPath, ADDITIONAL_TYPE_RESETORDER);

		CSimpleArray<CMenuItemInfo> infos;
		MtlForEachObject_OldShell( strDirectoryPath, _SetupMenuItemInfoToArray(infos, *this) );

		// setup menu item info
		T * 						pT			= static_cast<T *>(this);
		pT->OnMenuItemInitialUpdate(strDirectoryPath, infos);

		int 						i;

		// shorten text size
		for (i = 0; i < infos.GetSize(); ++i) {
			CMenuItemInfo &info 	 = infos[i];
			LPTSTR		   lpsz 	 = info.dwTypeData;
			CString 	   str		 = _GetMenuItemText(lpsz);
			delete[] lpsz;
			lpsz			= new TCHAR[str.GetLength() + 1];
			::lstrcpy(lpsz, str);
			info.dwTypeData = lpsz;

			CMenuItem *    pMenuItem = (CMenuItem *) info.dwItemData;

			if (pMenuItem && pMenuItem->m_bIcon)
				pMenuItem->m_strText.Format(_T("%s"), lpsz);
		}

		// insert menu items
		std::for_each( _begin(infos), _end(infos), _MenuItemInserter(menuPopup, nInsertPoint, nBreakCount, m_nMaxMenuBreakCount) );

		// add "none" menu item
		if (infos.GetSize() == 0)
			_AddNoneMenuItem(menuPopup, nInsertPoint);

		// clean up texts
		for (i = 0; i < infos.GetSize(); ++i) {
			delete[] infos[i].dwTypeData;
			infos[i].dwTypeData = NULL; 	//*+++ 不要だけれど、念のためクリア.
		}

		//CDebugWindow::OutPutString(_T("Function \"_Explorer\" returned"));
	}


	void _AddAdditionalMenuItem(CMenuHandle menuDest,
								int &nInsertPoint,
								int &nBreakCount,
								const CString &strDirectory,
								int nType)
	{
		CString 	  strDirectoryPath = strDirectory;

		MtlMakeSureTrailingBackSlash(strDirectoryPath);

		CString 	  strCaption	   = m_strAdditional;
		CString 	  strUserData	   = strDirectoryPath;

		if (nType == ADDITIONAL_TYPE_OPENALL) {
			strUserData.Insert( 0, _T("a ") );
		} else if (nType == ADDITIONAL_TYPE_SAVEFAV) {
			strUserData.Insert( 0, _T("s ") );
			strCaption = _T("この位置にお気に入りを追加");
		} else if (nType == ADDITIONAL_TYPE_RESETORDER) {
			strUserData.Insert( 0, _T("r ") );
			strCaption = _T("並び順を初期化する");
		}

		CMenuItemInfo mii;
		mii.fMask	   = MIIM_ID | MIIM_TYPE;
		mii.fType	   = MFT_STRING;
		mii.wID 	   = ++m_nCurrentMenuID;
		mii.dwTypeData = (LPTSTR) (LPCTSTR) strCaption;
		menuDest.InsertMenuItem(nInsertPoint++, TRUE, &mii);
		++nBreakCount;

		if (m_nCurrentMenuID < m_nMaxID) {
			m_mapID.Add(m_nCurrentMenuID, strUserData);
		} else {
			ATLASSERT(FALSE);
			// ::MessageBeep(MB_ICONEXCLAMATION);
		}

		if ( !(m_dwStyle & EMS_ADDITIONALMENUITEMNOSEP) )
			_InsertSeparator(menuDest, nInsertPoint);
	}


	void _InsertSeparator(CMenuHandle menuDest, int &nInsertPoint)
	{
		CMenuItemInfo	mii;
		mii.fMask = MIIM_TYPE;
		mii.fType = MF_SEPARATOR;
		menuDest.InsertMenuItem(nInsertPoint++, TRUE, &mii);
	}


	struct _MenuItemInserter {
		CMenuHandle 	_menu;
		int &			_nInsertPoint;
		int &			_nBreakCount;
		int 			_nMaxMenuBreakCount;


		_MenuItemInserter(CMenuHandle menu, int &nInsertPoint, int &nBreakCount, int nMaxMenuBreakCount)
			: _menu(menu)
			, _nInsertPoint(nInsertPoint)
			, _nBreakCount(nBreakCount)
			, _nMaxMenuBreakCount(nMaxMenuBreakCount)
		{
		}


		void operator ()(CMenuItemInfo &mii)
		{
			if (_nBreakCount + 1 > _nMaxMenuBreakCount) {	// what a difficult algorithm...
				_nBreakCount = 0;							// reset
				mii.fType	|= MFT_MENUBREAK;
			}
			++_nBreakCount;
			epmTRACE(_T("InsertMenuItem(%s, %d)\n"), mii.dwTypeData, _nInsertPoint);
			_menu.InsertMenuItem(_nInsertPoint++, TRUE, &mii);
		}
	};


	static void _DeleteAllMenuItems(CMenuHandle menu)
	{
		for (int i = 0; i < menu.GetMenuItemCount(); ++i) {
			CMenuHandle menuSub = menu.GetSubMenu(i);

			if (menuSub.m_hMenu) {
				_DeleteAllMenuItems(menuSub.m_hMenu);
				MTLVERIFY( menuSub.DestroyMenu() );
			}
		}

		while ( menu.DeleteMenu(0, MF_BYPOSITION) )
			;
	}


	void _SaveDirectoryPath(CMenuHandle menu, const CString &strPath)
	{
		m_mapSubMenu.Add(menu.m_hMenu, strPath);
		// m_mapSubMenu[menu.m_hMenu] = strPath;
	}


	CString _LoadDirectoryPath(CMenuHandle menu)
	{
		// return m_mapSubMenu[menu.m_hMenu];
		return m_mapSubMenu.Lookup(menu.m_hMenu);
	}


	int _FindInsertPoint(CMenuHandle menu)
	{
		int 	nItems 			= menu.GetMenuItemCount();
		int 	nInsertPoint;

		for (nInsertPoint = 0; nInsertPoint < nItems; ++nInsertPoint) {
			CMenuItemInfo mii;
			mii.fMask = MIIM_ID;
			menu.GetMenuItemInfo(nInsertPoint, TRUE, &mii);

			if (mii.wID == m_nInsertPointMenuItemID)
				break;
		}

		if (nInsertPoint >= nItems)  // not found
			return -1;

		ATLASSERT(nInsertPoint < nItems && "You need a menu item with an ID = m_nInsertPointMenuItemID");
		return nInsertPoint;
	}


	void _DeleteInsertPointMenuItem()
	{
		// epmTRACE(_T("_DeleteInsertPointMenuItem(%d) : %s\n"), m_nInsertPointMenuItemID, m_strTestText);
		m_menu.DeleteMenu(m_nInsertPointMenuItemID, MF_BYCOMMAND);
	}


	CString _GetMenuItemText(const CString &strPath)
	{
		T *pT = static_cast<T *>(this);

		return pT->OnGetMenuItemText(strPath);
	}


	void _RestoreOriginalMenu()
	{
		epmTRACE(_T("_RestoreOriginalMenu\n"));

		while (m_menu.GetMenuItemCount() > m_nOriginalMenuItemCountExceptInsertPointMenuItem) {
			CMenuHandle menuSub = m_menu.GetSubMenu(m_nOriginalInsertPoint);
			if (menuSub.m_hMenu) {
				epmTRACE(_T(" DestroyMenu\n"));
				_DeleteAllMenuItems(menuSub.m_hMenu);
				MTLVERIFY(menuSub.DestroyMenu());
			}
			m_menu.DeleteMenu(m_nOriginalInsertPoint, MF_BYPOSITION);
		}

		m_menu.InsertMenu(m_nOriginalInsertPoint, MF_BYPOSITION, m_nInsertPointMenuItemID, _T("Insert Point"));
	}


public:
	CString _GetFilePath(const CMenuItemInfo& mii)
	{
		CString str = m_mapID.Lookup(mii.wID);
		if (str.IsEmpty()) {		// dir path
			str = m_mapSubMenu.Lookup(mii.hSubMenu);
		}

		ATLASSERT(!str.IsEmpty());
		return str;
	}


	void ResetIDCount(BOOL bSts)
	{
		m_bResetIDCount=bSts;
	}


	void SetTargetWindow(HWND hWnd)
	{
		m_hWnd = hWnd;
	}


private:
	BOOL _CheckID(int nID)
	{
		if (nID == 0xFFFFFFFF || nID == 0) {
			return TRUE;
		} else if (m_nMinID <= nID && nID <= m_nMaxID) {
			return TRUE;
		//}else if(COMMAND_RANGE_START <= nID && nID <= COMMAND_RANGE_END) {
		//	return TRUE;
		}

		return FALSE;
	}

};



class CExplorerMenu : public CExplorerMenuImpl<CExplorerMenu> {
public:
	//*+++ 適当にデフォルトコンストラクタを用意.
	CExplorerMenu() : CExplorerMenuImpl<CExplorerMenu>(0) {}

	void OnExecute(const CString& strFilePath)
	{
		::ShellExecute(NULL, _T("open"), strFilePath, NULL, NULL, SW_SHOWNORMAL);
	}
};



#ifndef NO_STYLESHEET
class CStyleSheetMenu : public CExplorerMenuImpl<CStyleSheetMenu> {
public:
	typedef CExplorerMenuImpl<CStyleSheetMenu>  baseClass;
	//*+++ 適当にデフォルトコンストラクタを用意.
	CStyleSheetMenu() : CExplorerMenuImpl<CStyleSheetMenu>(0) {;}

	void OnExecute(const CString& strFilePath)
	{
		//CString strStyleSheetPath(strFilePath);
		//::SendMessage(m_hWnd, WM_USER_CHANGE_CSS, (WPARAM)strStyleSheetPath.GetBuffer(0), 0);
		std::vector<char> cpath = Misc::tcs_to_sjis(strFilePath);
		nsCOMPtr<nsILocalFile> file;
        nsresult rv = NS_NewNativeLocalFile(nsEmbedCString(&cpath[0]), PR_FALSE,
                                   getter_AddRefs(file));
		if(NS_FAILED(rv)) return;
		nsCOMPtr<nsIIOService> ios = do_GetService("@mozilla.org/network/io-service;1");
		nsCOMPtr<nsIURI> uri;
		ios->NewFileURI(file, getter_AddRefs(uri));
		nsCOMPtr<nsIStyleSheetService> sss = do_GetService("@mozilla.org/content/style-sheet-service;1");
		PRBool r = PR_FALSE;
		sss->SheetRegistered(uri, nsIStyleSheetService::USER_SHEET, &r);

		if(!r)
			sss->LoadAndRegisterSheet(uri, nsIStyleSheetService::USER_SHEET);
		else
			sss->UnregisterSheet(uri, nsIStyleSheetService::USER_SHEET);
	}

	void OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos)
	{
		int i;
		for(i=0;i<infos.GetSize();i++){
			CString   strPath = baseClass::_GetFilePath(infos[i]);
			infos[i].fMask |= MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
			infos[i].fState = IsStyleSheetRegistered(strPath) ? MFS_CHECKED : 0;
		}
		baseClass::OnMenuItemInitialUpdate(strDirPath, infos);		
	}
    bool IsStyleSheetRegistered(const CString& strFilePath)
	{
		nsCOMPtr<nsILocalFile> file;
		std::vector<char> cpath = Misc::tcs_to_sjis(strFilePath);
        nsresult rv = NS_NewNativeLocalFile(nsEmbedCString(&cpath[0]), PR_FALSE,
                                   getter_AddRefs(file));
		if(NS_FAILED(rv)) return false;
		nsCOMPtr<nsIIOService> ios = do_GetService("@mozilla.org/network/io-service;1");
		nsCOMPtr<nsIURI> uri;
		ios->NewFileURI(file, getter_AddRefs(uri));
		nsCOMPtr<nsIStyleSheetService> sss = do_GetService("@mozilla.org/content/style-sheet-service;1");
		PRBool r = PR_FALSE;
		sss->SheetRegistered(uri, nsIStyleSheetService::USER_SHEET, &r);
		return (r == PR_TRUE);
	}
};
#endif



class CExplorerContextMenuDialog : public CDialogImpl<CExplorerContextMenuDialog> {
public:
	enum { IDD = IDD_DIALOG_CONTEXTMENU };

private:
	CCommandBarCtrl2	m_CmdBar;
	HMENU				m_hMenu;
	int 				m_nResult;
	HWND				m_hBaseWnd;

public:
	CExplorerContextMenuDialog(HMENU hMenu, HWND hWnd = NULL)
		: m_CmdBar()
		, m_hMenu(hMenu)
		, m_nResult(0)
		, m_hBaseWnd(hWnd)
	{
	}


	BEGIN_MSG_MAP(CExplorerContextMenuDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_BUTTON_CONTEXTMENU, OnBtnContextMenu)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}


	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDOK);
		return 0;
	}


	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}


	LRESULT OnBtnContextMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		POINT		pos;
		::GetCursorPos(&pos);
		CMenuHandle menu;
		menu.Attach(m_hMenu);

		int nCmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD|TPM_RECURSE,pos.x,pos.y,m_hBaseWnd,NULL);
		m_nResult = nCmd;
		return 0;
	}


	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		POINT		pos;
		::GetCursorPos(&pos);
		CMenuHandle menu;
		menu.Attach(m_hMenu);

		if(!::IsMenu(m_hMenu))
			return 0;

		int nCmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD|TPM_RECURSE,0,0,m_hWnd,NULL);
		m_nResult = nCmd;
		return 0;
	}


	int GetResult()
	{
		return m_nResult;
	}

};
