/**
 *	@file	ExplorerToolBarCtrl.h
 *	@brief	linkバー用の処理
 */

#pragma once



//optional styles
enum ELnk {
	LNK_LARGEICON	= 0x00000001,
	LNK_NOICON		= 0x00000002,
};



// for debug
#ifdef _DEBUG
	const bool _Mtl_ExplorerToolBarCtrl_traceOn = false;
	#define etbTRACE	if (_Mtl_ExplorerToolBarCtrl_traceOn)  ATLTRACE
#else
	#define etbTRACE
#endif


template <class _ExpBar>
class CExpBarMenu : public CFavoritesMenu<CExpBarMenu<_ExpBar> > {
	_ExpBar *		__m_pExpBar;

public:
	CExpBarMenu(_ExpBar *__pExpBar, int nInsertPointMenuItemID)
		: CFavoritesMenu<CExpBarMenu>( nInsertPointMenuItemID, _T("(なし)"), 0x012c, 0x7530, _T("これらをすべて開く") )
		, __m_pExpBar(__pExpBar)
	{
		RefreshStyle();
	}


	// Overrides
	void OnExecute(const CString &strFilePath)
	{
		__m_pExpBar->OnExecuteFromExpMenu(strFilePath);
	}


	void RefreshStyle()
	{
		SetStyle( CFavoritesMenuOption::GetStyle() );
		m_nMaxMenuItemTextLength = CFavoritesMenuOption::GetMaxMenuItemTextLength();
		m_nMaxMenuBreakCount	 = CFavoritesMenuOption::GetMaxMenuBreakCount();
	}
};



template <class T, class TBase = CToolBarCtrl, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE 	CExplorerToolBarCtrlImpl
	: public CWindowImpl< T, TBase, TWinTraits>
	, public CFileNotification
{
public:
	// Declarations
	DECLARE_WND_SUPERCLASS( NULL, TBase::GetWndClassName() )

private:
	typedef CExplorerToolBarCtrlImpl<T, TBase, TWinTraits>	thisClass;

	// Constants
	enum { s_nIDLast = 100, s_nIDExProp = 1  };

	// Data members
	CFont									m_font;
	CContainedWindow						m_wndParent;
	CMenu									m_menu;

protected:
	CExpBarMenu<CExplorerToolBarCtrlImpl>	m_ExpMenu;

private:
	CItemIDList 							m_idlRootFolder;	// root idl
	CCrasyMap<int, CItemIDList> 			m_mapID;			// command id and idlist
	CSimpleArray<int>						m_arrFolderCmdID;	// folders
	CItemIDList 							m_idlHtml;			// for faster drawing

	int 									m_nMaxID;
	int 									m_nMinID;
	int 									m_nCurrentID;

	CComPtr<IContextMenu2>					m_spContextMenu2;
	DWORD									m_dwOptionalStyle;


public:
	// Constructor
	CExplorerToolBarCtrlImpl(int nInsertPointMenuItemID, int nMinID, int nMaxID)
		: m_wndParent(this, 1)
		, m_ExpMenu(this, nInsertPointMenuItemID)
		, m_nMaxID(nMaxID)
		, m_nMinID(nMinID)
		, m_dwOptionalStyle(0)
	{
		m_menu.CreatePopupMenu();
		m_menu.InsertMenu( 0, MF_BYPOSITION | MF_STRING, nInsertPointMenuItemID, _T("InsertPoint") );
		m_ExpMenu.InstallExplorerMenu(m_menu.m_hMenu);
		m_nCurrentID = m_nMinID;

		m_idlHtml	 = MtlGetHtmlFileIDList();
	}


	~CExplorerToolBarCtrlImpl()
	{
		if ( m_wndParent.IsWindow() )
			m_wndParent.UnsubclassWindow();
	}


	// Attributes
	void SetExplorerToolBarRootPath(const CString &strPath)
	{
		m_idlRootFolder = strPath;
	}


	CString GetExplorerToolBarRootPath()
	{
		return m_idlRootFolder.GetPath();
	}


	void SetOptionalStyle(DWORD dwStyle)
	{
		m_dwOptionalStyle = dwStyle;
	}


private:
	DWORD GetExtendedStyle()
	{
		return m_dwOptionalStyle;
	}


	// Overridables
	void OnInitExplorerToolBarCtrl()
	{
	}


	void OnTermExplorerToolBarCtrl()
	{
	}


	void OnExecute(const CString &strPath)
	{
	}


public:
	void OnInitialUpdateTBButtons(CSimpleArray<TBBUTTON> &btns, LPCITEMIDLIST pidl)
	{
		std::sort( _begin(btns), _end(btns), _DefaultTBBtnCompare<thisClass>(this) );
	}


private:
	bool OnRenameExplorerToolBarCtrl(CString &strName)
	{
		return false;
	}


	// Methods
	void SetRootDirectoryPath(const CString &strPath)
	{
		m_strRootPath = strPath;
	}


public:
	void OnExecuteFromExpMenu(const CString &strPath)
	{
		T *pT = static_cast<T *>(this);

		pT->OnExecute(strPath);
	}


	// Message map and handlers
	BEGIN_MSG_MAP(CExplorerToolBarCtrlImpl)
		MESSAGE_HANDLER(WM_CREATE , OnCreate )
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		HANDLE_MENU_MESSAGE_CONTEXTMENU(m_spContextMenu2)

		MESSAGE_HANDLER(WM_MENURBUTTONUP, OnMenuRButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONUP	, OnRButtonUp	 )
		MESSAGE_HANDLER(GET_REGISTERED_MESSAGE(Mtl_FileNotification), OnFileNotification)
		CHAIN_MSG_MAP_MEMBER(m_ExpMenu)

		//	REFLECTED_NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnDropDown)
	ALT_MSG_MAP(1)
		NOTIFY_CODE_HANDLER(RBN_CHEVRONPUSHED, OnChevronPushed)
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO  , OnToolTipText)
		COMMAND_RANGE_HANDLER_EX(m_nMinID	 , m_nMaxID, OnCommandRange)
	END_MSG_MAP()


private:
	LRESULT OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;

		pDispInfo->szText[0] = 0;
		bHandled			 = FALSE;

		if ( (idCtrl != 0) && !(pDispInfo->uFlags & TTF_IDISHWND)
		   && m_nMinID <= idCtrl && idCtrl <= m_nMaxID )
		{
			CItemIDList idlFolder = m_idlRootFolder;
			CItemIDList idl 	  = m_mapID.Lookup(idCtrl);
			CString 	strPath   = idl.GetPath();
			idl 	-= idlFolder;

			if ( idl.IsNull() )
				return 0;

			int 		nLen	  = _countof(pDispInfo->szText);

			CString 	strName   = MtlGetDisplayName(idl);
			CString 	strUrl	  = MtlGetInternetShortcutUrl(strPath);
			CString 	strTip	  = MtlMakeFavoriteToolTipText(strName, strUrl, nLen);

			::lstrcpyn(pDispInfo->szText, strTip, nLen);
			bHandled = TRUE;
		}

		return 0;
	}


	LRESULT OnFileNotification(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		_UpdateButtons(); // sorry
		CReBarCtrl rebar;
		rebar.Attach( GetParent() );
		MtlRefreshBandIdealSize(rebar, m_hWnd);
		rebar.Detach();
		return 0;
	}


	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		CPoint	 pt( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
		int 	 nIndex = HitTest(&pt);

		if (nIndex < 0) {
			::SendMessage(GetTopLevelParent(), WM_SHOW_TOOLBARMENU, 0, 0);
			return 0;
		}

		TBBUTTON tbb;
		GetButton(nIndex, &tbb);
		int 	 nCmdID = tbb.idCommand;
		MarkButton(nCmdID, TRUE);
		_PopupContextMenu(nCmdID, pt);
		MarkButton(nCmdID, FALSE);
		return 0;
	}


	LRESULT OnMenuRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		m_ExpMenu.OnRButtonUp(m_hWnd, uMsg, wParam, lParam, bHandled);
		return 0;
	}


	LRESULT OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMREBARCHEVRON lpnm	 = (LPNMREBARCHEVRON) pnmh;

		if ( lpnm->wID != GetDlgCtrlID() ) {
			bHandled = FALSE;
			return 1;
		}

		CWindow 		 wnd(m_hWnd);
		int 			 nCount  = (int) wnd.SendMessage(TB_BUTTONCOUNT);

		if (nCount <= 0)  // probably not a toolbar
			return 1;

		RECT	rcClient;
		int 	ret 	= wnd.GetClientRect(&rcClient);
		ATLASSERT(ret);

		for (int i = 0; i < nCount; i++) {
			TBBUTTON	tbb;
			ret = wnd.SendMessage(TB_GETBUTTON, i, (LPARAM) &tbb);
			ATLASSERT(ret);
			RECT	 rcButton;
			ret = wnd.SendMessage(TB_GETITEMRECT, i, (LPARAM) &rcButton);
			ATLASSERT(ret);

			if ( !(rcButton.right > rcClient.right) ) {
				CString strPath = _GetPathFromCmdID(tbb.idCommand);
				ATLASSERT( !strPath.IsEmpty() );
				m_ExpMenu.AddIgnoredPath(strPath);
			}
		}

		CWindow 		 wndFrom = lpnm->hdr.hwndFrom;
		RECT			 rect	 = lpnm->rc;
		wndFrom.ClientToScreen(&rect);
		TPMPARAMS		 TPMParams;
		TPMParams.cbSize	= sizeof (TPMPARAMS);
		TPMParams.rcExclude = rect;

		m_ExpMenu.SetRootDirectoryPath( m_idlRootFolder.GetPath() );
		DoTrackPopupMenu(m_menu.m_hMenu,
						 TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN | (!AtlIsOldWindows() ? (true ? TPM_VERPOSANIMATION : TPM_NOANIMATION) : 0),
						 rect.left, rect.bottom, &TPMParams);

		// eat next message if click is on the same button
		MtlEatNextLButtonDownOnChevron(GetTopLevelParent(), rect);

		m_ExpMenu.ResetIgnoredPath();
		return 0;
	}


	void OnCommandRange(UINT, int nID, HWND hWndCtrl)
	{
		etbTRACE(_T("CExplorerMenu::OnCommandRange : %d\n"), nID);
		DoPopupMenu(nID, true);
	}


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
	{
		// These styles are required
		ModifyStyle(0, TBSTYLE_LIST | TBSTYLE_FLAT);
		// Let the toolbar initialize itself
		LRESULT lRet			  = DefWindowProc(uMsg, wParam, lParam);
		// get and use system settings
		_GetSystemSettings();
		// set image list
		_SetSystemImageList();
		// Parent init
		CWindow wndParent		  = GetParent();
		CWindow wndTopLevelParent = wndParent.GetTopLevelParent();
		m_wndParent.SubclassWindow(wndTopLevelParent);
		// Toolbar Init
		SetButtonStructSize();

		_UpdateButtons();
		SetUpFileNotificationThread(m_hWnd, m_idlRootFolder.GetPath(), false);

		CFavoritesMenuOption::Add(m_hWnd);

		T * 	pT				  = static_cast<T *>(this);
		pT->OnInitExplorerToolBarCtrl();

		return lRet;
	}


	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		T *pT = static_cast<T *>(this);
		pT->OnTermExplorerToolBarCtrl();
		return 0;
	}


	LRESULT OnDropDown(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		// Check if this comes from us
		if (pnmh->hwndFrom != m_hWnd) {
			bHandled = FALSE;
			return 1;
		}

		LPNMTOOLBAR pNMToolBar = (LPNMTOOLBAR) pnmh;
		int 		nCmdID	   = pNMToolBar->iItem;
		DoPopupMenu(nCmdID, true);

		return TBDDRET_TREATPRESSED;
	}


	void _GetSystemSettings()
	{
		// refresh our font
		NONCLIENTMETRICS info = {0};

		info.cbSize = sizeof (info);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof (info), &info, 0);

		MTL::CLogFont	 logfont;
		logfont.InitDefault();

		if (m_font.m_hFont != NULL)
			m_font.GetLogFont(logfont);

		if (logfont != info.lfMenuFont) {
			HFONT hFontMenu = ::CreateFontIndirect(&info.lfMenuFont);
			ATLASSERT(hFontMenu != NULL);

			if (hFontMenu != NULL) {
				if (m_font.m_hFont != NULL)
					m_font.DeleteObject();

				m_font.Attach(hFontMenu);
				SetFont(m_font);
				AddStrings( _T("NS\0") );	// for proper item height
				AutoSize();
			}
		}
	}


	void _CleanUpButtons()
	{
		m_mapID.RemoveAll();
		m_arrFolderCmdID.RemoveAll();

		m_nCurrentID = m_nMinID;

		int  nCount = GetButtonCount();

		for (int i = 0; i < nCount; i++) {
			int ret = DeleteButton(0);
			ATLASSERT(ret);
		}
	}


public:
	void _UpdateButtons()
	{
		CLockRedraw 		   lock(m_hWnd);

		// cleanup
		_CleanUpButtons();

		if ( m_idlRootFolder.IsNull() )
			return;

		// Set up buttons
		CSimpleArray<TBBUTTON> btns;

		MtlForEachObject( m_idlRootFolder, _Function_SetUpTBBtn<thisClass>(this, btns, m_nCurrentID) );

		// do anything you want
		T * 				   pT = static_cast<T *>(this);
		pT->OnInitialUpdateTBButtons(btns, m_idlRootFolder);

		// insert items
		int 				   i;

		for (i = 0; i < btns.GetSize(); ++i) {
			MTLVERIFY( InsertButton(-1, &btns[i]) );
		}

		// Set up button text
		for (i = 0; i < btns.GetSize(); ++i) {
			TBBUTTON &				 btn	= btns[i];
			CItemIDList 			 idl	= _GetIDListFromCmdID(btn);
			CString 				 strBtn = MtlGetDisplayName(idl - m_idlRootFolder);
			strBtn	   = _GetButtonText(strBtn);
			CVersional<TBBUTTONINFO> bi;
			bi.dwMask  = TBIF_TEXT;
			bi.pszText = (LPTSTR) (LPCTSTR) strBtn;
			MTLVERIFY( SetButtonInfo(btn.idCommand, &bi) );
		}

		if (GetButtonCount() == 0)
			_AddNoneButton();
	}


private:
	template <class _This>
	struct _Function_SetUpTBBtn {
		_This * 					_pThis;
		CSimpleArray<TBBUTTON>& 	_btns;
		int &						_nCmdID;

	public:
		_Function_SetUpTBBtn(_This *pThis, CSimpleArray<TBBUTTON> &btns, int &nCmdID)
			: _pThis(pThis)
			, _btns(btns)
			, _nCmdID(nCmdID)
		{
		}

		void operator ()(IShellFolder *pFolder, const CItemIDList &idlFolder, const CItemIDList &idlFile, bool bFolder)
		{
			CItemIDList idlFull = idlFolder + idlFile;

			TBBUTTON	btn = {0};

			btn.iBitmap   = MtlGetNormalIconIndex(idlFull, _pThis->m_idlHtml);
			btn.idCommand = _nCmdID;
			btn.fsState   = TBSTATE_ENABLED;
			btn.fsStyle   = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;	// | TBSTYLE_DROPDOWN is not proper effect...
			btn.dwData	  = 0;
			btn.iString   = 0;

			_btns.Add(btn);
			_pThis->m_mapID.Add(_nCmdID, idlFull);				// id map set up

			if (bFolder)
				_pThis->m_arrFolderCmdID.Add(_nCmdID);

			++_nCmdID;
		}
	};


	void _AddNoneButton()
	{
		// NOTE: Command Bar currently supports only menu items
		//		 that are enabled and have a drop-down menu
		TBBUTTON	 btn = {0};
		btn.iBitmap   = NULL;
		btn.idCommand = 0;
		btn.fsState   = 0;
		btn.fsStyle   = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;		// | TBSTYLE_DROPDOWN;
		btn.dwData	  = 0;
		btn.iString   = 0;

		BOOL		 bRet = InsertButton(-1, &btn);
		ATLASSERT(bRet);

		TBBUTTONINFO	bi;
		memset( &bi, 0, sizeof (bi) );
		bi.cbSize	  = sizeof (TBBUTTONINFO);
		bi.dwMask	  = TBIF_TEXT;
		bi.pszText	  = _T("(なし)");

		bRet		  = SetButtonInfo(0, &bi);
		ATLASSERT(bRet);
	}


	static CString _GetButtonText(const CString &strBtn)
	{
		return MtlCompactString( strBtn, CFavoritesMenuOption::GetMaxMenuItemTextLength() );
	}


	void DoPopupMenu(int nCmdID, bool bAnimate)
	{
		// get popup menu and it's position
		RECT	  rect = {0};

		GetItemRect(CommandToIndex(nCmdID), &rect);
		ClientToScreen(&rect);
		TPMPARAMS TPMParams;
		TPMParams.cbSize	= sizeof (TPMPARAMS);
		TPMParams.rcExclude = rect;
		//		HMENU hMenuPopup = ::GetSubMenu(m_menu.m_hMenu, nIndex);

		// press button and display popup menu
		//		PressButton(nCmdID, TRUE);
		//		SetHotItem(nCmdID);

		CString   strPath = _GetPathFromCmdID(nCmdID);

		if ( strPath.IsEmpty() )
			return;

		if ( MtlIsDirectoryPath(strPath) ) {
			int MenuFlag = TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN;
			MenuFlag |= (!AtlIsOldWindows() ? (bAnimate ? TPM_VERPOSANIMATION : TPM_NOANIMATION) : 0);

			m_ExpMenu.SetRootDirectoryPath(strPath);
			DoTrackPopupMenu(m_menu.m_hMenu, MenuFlag, rect.left, rect.bottom, &TPMParams);
		} else {
			T *pT = static_cast<T *>(this);
			pT->OnExecute(strPath);
		}
		//	PressButton(nCmdID, FALSE);
		//	if(::GetFocus() != m_hWnd)
		//		SetHotItem(-1);
	}


	BOOL DoTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, LPTPMPARAMS lpParams = NULL)
	{
		CMenuHandle menuPopup = hMenu;

		BOOL		bTrackRet = menuPopup.TrackPopupMenuEx(uFlags, x, y, m_hWnd, lpParams); // owner is me

		return bTrackRet;
	}


	void _SetSystemImageList()
	{
		ATLASSERT( ::IsWindow(m_hWnd) );

		int 	   nFlags = SHGFI_SYSICONINDEX;

		if ( !(m_dwOptionalStyle & LNK_LARGEICON) )
			nFlags |= SHGFI_SMALLICON;

		HIMAGELIST	hImgs;

		if (m_dwOptionalStyle & LNK_NOICON) {
			hImgs = NULL;
		} else {
			SHFILEINFO sfi;
			hImgs = (HIMAGELIST) ::SHGetFileInfo(_T("C:\\"), 0, &sfi, sizeof (sfi), nFlags);
		}

		SetImageList(hImgs);
	}


	template <class _This>
	struct _DefaultTBBtnCompare : public std::binary_function<const TV_INSERTSTRUCT &, const TV_INSERTSTRUCT &, bool> {
		_This * 	_pThis;

		_DefaultTBBtnCompare(_This *pThis) : _pThis(pThis) { }

		bool operator ()(const TBBUTTON &x, const TBBUTTON &y)
		{
			CItemIDList idlFolder = _pThis->m_idlRootFolder;
			CItemIDList idlA	  = _pThis->_GetIDListFromCmdID(x);

			idlA -= idlFolder;
			CItemIDList idlB	  = _pThis->_GetIDListFromCmdID(y);
			idlB -= idlFolder;

			CString 	strA	  = MtlGetDisplayName(idlA);
			CString 	strB	  = MtlGetDisplayName(idlB);
			ATLASSERT( !strA.IsEmpty() && !strB.IsEmpty() );


			bool		bDirA	  = _pThis->_IsFolderCmdID(x.idCommand);
			bool		bDirB	  = _pThis->_IsFolderCmdID(y.idCommand);

			if (bDirA == bDirB) {
				int nRet = ::lstrcmp(strA, strB);
				return (nRet < 0) /*? true : false*/;
			} else {
				return bDirA;
			}
		}
	};


	CItemIDList _GetIDListFromCmdID(const TBBUTTON &btn)
	{
		return m_mapID.Lookup(btn.idCommand);
	}


public:
	CItemIDList _GetIDListFromCmdID(int nCmdID)
	{
		return m_mapID.Lookup(nCmdID);
	}


	CString _GetPathFromCmdID(int nCmdID)
	{
		CItemIDList idl 	= m_mapID.Lookup(nCmdID);

		if ( idl.IsNull() )
			return CString();

		CString 	strPath = idl.GetPath();

		if ( _IsFolderCmdID(nCmdID) )
			MtlMakeSureTrailingBackSlash(strPath);

		return strPath;
	}


private:
	CString _GetRootFolderPath()
	{
		CString strPath = m_idlRootFolder.GetPath();

		MtlMakeSureTrailingBackSlash(strPath);

		return strPath;
	}


	bool _PopupContextMenu(int nCmdID, CPoint pt)
	{
		CItemIDList 		  idl		 = _GetIDListFromCmdID(nCmdID);
		CItemIDList 		  idlFolder  = m_idlRootFolder;

		idl -= idlFolder; // get file name
		if ( idl.IsNull() )
			return false;

		// get Desktop folder
		CComPtr<IShellFolder> spDesktopFolder;
		HRESULT hr	= ::SHGetDesktopFolder(&spDesktopFolder);
		if ( FAILED(hr) )
			return false;

		// get IShellFolder
		CComPtr<IShellFolder> spFolder;
		hr	= spDesktopFolder->BindToObject(idlFolder, NULL, IID_IShellFolder, (void **) &spFolder);
		if ( FAILED(hr) )
			return false;

		LPITEMIDLIST	pidls[]  = { idl.m_pidl };

		CComPtr<IContextMenu> spContextMenu;
		hr	= spFolder->GetUIObjectOf(m_hWnd, 1, (const struct _ITEMIDLIST **) pidls, IID_IContextMenu, 0, (void **) &spContextMenu);
		if ( FAILED(hr) )
			return false;

		m_spContextMenu2.Release();

		hr	= spContextMenu->QueryInterface(IID_IContextMenu2, (void **) &m_spContextMenu2);
		if ( FAILED(hr) )
			return false;

		// setup menu
		CMenu	menu;
		menu.CreatePopupMenu();

		menu.InsertMenu( 0, MF_BYPOSITION | MF_ENABLED, s_nIDExProp, _T("拡張プロパティ(&E)") );
		menu.InsertMenu(1, MF_BYPOSITION | MF_SEPARATOR, 0);
		int 	nPos		 = 2;
		UINT	uFlags	 = CMF_EXPLORE | CMF_CANRENAME;

		hr	= m_spContextMenu2->QueryContextMenu(menu.m_hMenu, nPos, s_nIDLast, 0x7fff, uFlags);
		if ( FAILED(hr) ) {
			return false;
		}

		int 	nRenameID  = MtlGetCmdIDFromAccessKey( menu.m_hMenu, _T("&M") );

		ClientToScreen(&pt);
		UINT	uMenuFlags	= TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
		uMenuFlags			|= (_CheckOsVersion_98Later() ? TPM_RECURSE : 0); //minit
		int 	idCmd		= menu.TrackPopupMenu(uMenuFlags | TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);

		CMINVOKECOMMANDINFO   cmi;
		cmi.cbSize		 = sizeof (CMINVOKECOMMANDINFO);
		cmi.fMask		 = 0;
		cmi.hwnd		 = m_hWnd;
		// Fixed by NISHIZAWA, T2A(MAKEINTRESOURCE(idCmd - s_nIDLast)); is wrong. See MAKEINTRESOURCE macro defination.
		cmi.lpVerb		 =	(LPCSTR) MAKEINTRESOURCE(idCmd - s_nIDLast);		//+++ //*メモ UNICODE対策いる?いらない?
		cmi.lpParameters = NULL;
		cmi.lpDirectory  = NULL;
		cmi.nShow		 = SW_SHOWNORMAL;
		cmi.dwHotKey	 = 0;
		cmi.hIcon		 = NULL;

		if (idCmd == 0) {
			m_spContextMenu2.Release();
			return false;
		} else if (idCmd == nRenameID) {
			_RenameFile(nCmdID);
			m_spContextMenu2.Release();
			return true;
		} else if (idCmd == s_nIDExProp) {
			::SendMessage(GetTopLevelParent(), WM_SET_EXPROPERTY, (WPARAM) (LPCTSTR) _GetIDListFromCmdID(nCmdID).GetPath(), 0);
			return true;
		}

		// Execute command now
		hr				 = m_spContextMenu2->InvokeCommand(&cmi);

		m_spContextMenu2.Release();

		if ( FAILED(hr) )
			return false;

		return true;
	}


	void _RenameFile(int nCmdID)
	{
		CItemIDList 		  idlFolder = m_idlRootFolder;
		CItemIDList 		  idl		= _GetIDListFromCmdID(nCmdID);

		idl -= idlFolder;

		CComPtr<IShellFolder> spFolder;

		if ( !MtlGetShellFolder(idlFolder, &spFolder) )
			return;

		CString 	strName;
		if ( !MtlGetDisplayName(spFolder, idl, strName) )
			return;

		T * 		pT		= static_cast<T *>(this);
		if ( pT->OnRenameExplorerToolBarCtrl(strName) )
			MtlSetDisplayName(spFolder, idl, strName, m_hWnd);
	}


public:
	bool _IsFolderCmdID(int nCmdID)
	{
		//*
		if (m_arrFolderCmdID.Find(nCmdID) == -1)
			return false;
		else
			return true;
	}
};
