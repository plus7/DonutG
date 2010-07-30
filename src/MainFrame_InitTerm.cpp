/**
 *	@file	MainFrame_InitTerm.cpp
 *	@brief	CMainFrame の 初期化/終了処理
 *	@note
 *		+++ MainFrame.cpp から分離.
 */

#include "stdafx.h"
#include "MainFrame.h"
#include "Donut.h"
#include "DialogHook.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "nsICookieManager.h"
#include "nsIBrowserHistory.h"
#include "nsICacheService.h"

//void _PrivateTerm();


// ===========================================================================
// 初期化

CMainFrame::CMainFrame()
	: m_FavoriteMenu(this, ID_INSERTPOINT_FAVORITEMENU)
	, m_FavGroupMenu(this, ID_INSERTPOINT_GROUPMENU)
   #ifndef NO_STYLESHEET
	, m_styleSheetMenu(ID_INSERTPOINT_CSSMENU, _T("(empty)"), ID_INSERTPOINT_CSSMENU, ID_INSERTPOINT_CSSMENU_END)
   #endif
	, m_LinkBar(this, ID_INSERTPOINT_FAVORITEMENU)
	, m_nBackUpTimerID(0)
	, m_wndMDIClient(this, 1)
	, CDDEMessageHandler<CMainFrame>( _T("Donut") )
	, m_hWndFocus(NULL)
	, m_ExplorerBar(m_wndSplit, m_wndAdditionalSplit)
	, m_ScriptMenu(ID_INSERTPOINT_SCRIPTMENU, _T("(empty)"), ID_INSERTPOINT_SCRIPTMENU, ID_INSERTPOINT_SCRIPTMENU_END)
	, m_DropFavoriteMenu(this, ID_INSERTPOINT_FAVORITEMENU)
	, m_DropFavGroupMenu(this, ID_INSERTPOINT_GROUPMENU)
	, m_DropScriptMenu(ID_INSERTPOINT_SCRIPTMENU, _T("(empty)"), ID_INSERTPOINT_SCRIPTMENU, ID_INSERTPOINT_SCRIPTMENU_END)
	, m_bShow(FALSE)
	, m_hGroupThread(NULL)
	, m_OnUserOpenFile_nCmdShow(0)
	, m_nMenuBarStyle(-1)		//+++
	, m_nBgColor(-1)			//+++
	, m_bTray(0)				//+++
	, m_bFullScreen(0)			//+++
	, m_bOldMaximized(0)		//+++
	//, m_bMinimized(0)			//+++
	, m_strIcon()				//+++
	, m_strIconSm() 			//+++
{
	g_pMainWnd = this;			//+++ CreateEx()中にプラグイン初期化とかで参照されるので、呼び元でなく CMainFreameで設定するように変更.
}



//+++ 各初期化処理ごとに関数分離
/** メインフレーム作成. 各種初期化.
 */
LRESULT CMainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
{
	CFavoritesMenuOption::Add(m_hWnd);
 	init_font();									// フォント初期化
	m_wndDebug.Create();							// デバッグウィンドウの作成
	init_menus_infomation();						// メニュー情報の初期化

	HWND hWndCmdBar 	= init_commandBarWindow();	// メニューバーの初期化
	HWND hWndToolBar	= init_toolBar();			// ツールバーの初期化
	HWND hWndAddressBar	= init_addressBar();		// アドレスバーの初期化
	HWND hWndSearchBar	= init_searchBar();			// 検索バーの初期化
	HWND hWndMDITab		= init_tabCtrl();			// タブバーの初期化
	HWND hWndLinkBar	= init_linkBar();			// リンクバーの初期化

	init_rebar();									// リバーの初期化
	init_statusBar();								// ステータスバーの初期化
	init_progressBar();								// プログレスバーの初期化
	init_pluginManager();							// プラグインマネージャの初期化

	// 各種バーの配置
	init_band_position( hWndCmdBar,hWndToolBar,hWndAddressBar,hWndMDITab,hWndLinkBar,hWndSearchBar );

	init_loadStatusBarState();						// ステータスバーの状態設定
	init_mdiClientWindow();							//
	init_splitterWindow();							// 窓分割(エクスプローラバーと通常ページ)の初期化
	init_findBar();							    // ページ検索バーの初期化
	init_explorerBar();								// エクスプローラバーの初期化
	init_mdiClient_misc(hWndCmdBar, hWndToolBar);	// mdi-client関係の雑多な設定

	CmdUIAddToolBar(hWndToolBar);					// set up UI
	//CmdUIAddToolBar(m_SearchBar.m_wndToolBar);	// set up UI
	CmdUIAddToolBar(m_SearchBar.GetHWndToolBar());	// set up UI

	init_setUserAgent();							// ユーザーエージェントの設定
	init_message_loop();							// メッセージループの準備

	init_mdiTab();									// タブに関する設定
	//SetAutoBackUp();//OnBackUpOptionChanged(0,0,0);// OnCreate後の処理で別途呼び出すようにした.
	RegisterDragDrop();	//DragAcceptFiles();		// ドラッグ＆ドロップ準備

	CCriticalIdleTimer::InstallCriticalIdleTimer(m_hWnd, ID_VIEW_IDLE);

	init_sysMenu();									// システムメニューに追加

	::SetTimer(m_hWnd, ENT_READ_ACCEL, 200, NULL);	// アクセラキー読み込み. 遅延させるためにWM_TIMERで処理.

	InitSkin();										//スキンを初期化

	CDonutSimpleEventManager::RaiseEvent(EVENT_INITIALIZE_COMPLETE);	// イベント関係の準備
	CDialogHook::InstallHook(m_hWnd);				// ダイアログ関係の準備

	init_loadPlugins();								// プラグインを読み込む

	UpdateLayout();									// 画面更新

	return 0;	//return lRet;
}


/// フォント
void CMainFrame::init_font()
{
	CIniFileI		prFont( g_szIniFileName, _T("Main") );
	MTL::CLogFont	lf;
	lf.InitDefault();
	if ( lf.GetProfile(prFont) ) {
		CFontHandle 	font;
		MTLVERIFY( font.CreateFontIndirect(&lf) );
		if (font.m_hFont) {
			if (m_font.m_hFont != NULL)
				m_font.DeleteObject();
			m_font.Attach(font.m_hFont);
			SetFont(m_font);
		}
	}
}


//	//デバッグウィンドウの作成
//	m_wndDebug.Create();


/// initialize menus' infomation
void CMainFrame::init_menus_infomation()
{
	CMenuHandle 	   menu 		  = m_hMenu;
	CMenuHandle 	   menuFav		  = menu.GetSubMenu(_nPosFavoriteMenu);
	m_FavoriteMenu.InstallExplorerMenu(menuFav);
	m_FavoriteMenu.SetTargetWindow(m_hWnd);

	CMenuHandle 	   menuGroup	  = menuFav.GetSubMenu(_nPosFavGroupMenu);
	m_FavGroupMenu.InstallExplorerMenu(menuGroup);
	m_FavGroupMenu.SetTargetWindow(m_hWnd);

   #ifndef NO_STYLESHEET
	CMenuHandle 	   menuCss		  = menu.GetSubMenu(_nPosCssMenu);
	CMenuHandle 	   menuCssSub	  = menuCss.GetSubMenu(_nPosSubCssMenu);
	m_styleSheetMenu.SetRootDirectoryPath( Misc::GetExeDirectory() + _T("CSS") );
	m_styleSheetMenu.SetTargetWindow(m_hWnd);
	m_styleSheetMenu.ResetIDCount(TRUE);
	m_styleSheetMenu.InstallExplorerMenu(menuCssSub);
   #endif

	m_ScriptMenuMst.LoadMenu(IDR_SCRIPT);
	m_ScriptMenu.SetRootDirectoryPath( Misc::GetExeDirectory() + _T("Script") );
	m_ScriptMenu.SetTargetWindow(m_hWnd);
	m_ScriptMenu.ResetIDCount(TRUE);
	m_ScriptMenu.InstallExplorerMenu(m_ScriptMenuMst);

	OnMenuGetFav(); 	//initialize custom context dropdown
	OnMenuGetFavGroup();
	OnMenuGetScript();

	CMenuHandle 	   menuEncode	  = menu.GetSubMenu(_nPosEncodeMenu);
	m_MenuEncode.Init(menuEncode, m_hWnd, _nPosEncodeMenuSub);

	// MRU list
	CMainOption::SetMRUMenuHandle(menu, _nPosMRU);

	//MenuDropTaget
	m_wndMenuDropTarget.Create(m_hWnd, rcDefault, _T("MenuDropTargetWindow"), WS_POPUP, 0);
	m_wndMenuDropTarget.SetTargetMenu(menu);
}


/// create command bar window
HWND	CMainFrame::init_commandBarWindow()
{
	SetMenu(NULL);		// remove menu
	HWND	hWndCmdBar	  = m_CmdBar.Create(m_hWnd,rcDefault,NULL,MTL_SIMPLE_CMDBAR2_PANE_STYLE,0,ATL_IDW_COMMAND_BAR);
	if (m_font.m_hFont) {
		LOGFONT lf;
		m_font.GetLogFont(lf);
		m_CmdBar.SetMenuLogFont(lf);
	}

	m_CmdBar.AttachMenu(m_hMenu);
	m_CmdBar.EnableButton(_nPosWindowMenu, false);
	return hWndCmdBar;
}


/// create toolbar
HWND	CMainFrame::init_toolBar()
{
  #if 1 //+++ 関数分離の都合取得しなおし
	CMenuHandle 	   menu 		  = m_hMenu;
	CMenuHandle 	   menuFav		  = menu.GetSubMenu(_nPosFavoriteMenu);
	CMenuHandle 	   menuGroup	  = menuFav.GetSubMenu(_nPosFavGroupMenu);
	CMenuHandle 	   menuCss		  = menu.GetSubMenu(_nPosCssMenu);
	CMenuHandle 	   menuCssSub	  = menuCss.GetSubMenu(_nPosSubCssMenu);
  #endif

	HWND	hWndToolBar   = m_ToolBar.DonutToolBar_Create(m_hWnd);
	ATLASSERT( ::IsWindow(hWndToolBar) );

	m_ToolBar.DonutToolBar_SetFavoritesMenu(menuFav, menuGroup, menuCssSub);

	if (m_CmdBar.m_fontMenu.m_hFont)
		m_ToolBar.SetFont(m_CmdBar.m_fontMenu.m_hFont);

	return hWndToolBar;
}


/// create addressbar
HWND	CMainFrame::init_addressBar()
{
	HWND	hWndAddressBar = m_AddressBar.Create( m_hWnd, IDC_ADDRESSBAR, ID_VIEW_GO,
												/*+++ IDB_GOBUTTON, IDB_GOBUTTON_HOT,*/ 16, 16, RGB(255, 0, 255) );
	ATLASSERT( ::IsWindow(hWndAddressBar) );

	if (m_CmdBar.m_fontMenu.m_hFont)
		m_AddressBar.SetFont(m_CmdBar.m_fontMenu.m_hFont);

	m_AddressBar.m_comboFlat.SetDrawStyle(CSkinOption::s_nComboStyle);
	return hWndAddressBar;
}


/// create searchbar
HWND	CMainFrame::init_searchBar()
{
	HWND	hWndSearchBar  = m_SearchBar.Create(m_hWnd);

	if (m_CmdBar.m_fontMenu.m_hFont)
		m_SearchBar.SetFont(m_CmdBar.m_fontMenu.m_hFont);

	m_SearchBar.SetDlgCtrlID(IDC_SEARCHBAR);
	m_SearchBar.SetComboStyle(CSkinOption::s_nComboStyle);
	return	hWndSearchBar;
}


/// create findbar
HWND	CMainFrame::init_findBar()
{
	//HWND	hWndFindBar  = m_fayt.Create(m_hWnd);
	m_hWndFAYT = m_fayt.Create(m_hWndAdditionalSplit);//,rcDefault,_T("X"),WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|BS_CENTER|BS_VCENTER);
	if (m_CmdBar.m_fontMenu.m_hFont)
		m_fayt.SetFont(m_CmdBar.m_fontMenu.m_hFont);

	m_fayt.SetDlgCtrlID(IDC_SEARCHBAR);
	//m_fayt.SetComboStyle(CSkinOption::s_nComboStyle);
	return	m_hWndFAYT;//hWndFindBar;
}


/// create tabctrl
HWND	CMainFrame::init_tabCtrl()
{
	HWND	hWndMDITab	  = m_MDITab.Create(m_hWnd, CRect(0, 0, 200, 20), _T("Donut MDITab"),
												WS_CHILD | WS_VISIBLE, NULL, IDC_MDITAB);
	m_MDITab.LoadMenu(IDR_MENU_TAB);
	m_MDITab.LoadConnectingAndDownloadingImageList( IDB_MDITAB, 6, 6, RGB(255, 0, 255) );
	return hWndMDITab;
}


/// create link bar
HWND	CMainFrame::init_linkBar()
{
	CIniFileI prLnk( g_szIniFileName, _T("LinkBar") );
	DWORD		dwStyle   = prLnk.GetValue( _T("ExtendedStyle") );
	prLnk.Close();
	m_LinkBar.SetOptionalStyle(dwStyle);
	HWND	hWndLinkBar   = m_LinkBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE, 0, IDC_LINKBAR);
	return 	hWndLinkBar;
}


/// create rebar
void	CMainFrame::init_rebar()
{
	DWORD		dwRebarStyle   	= MTL_SIMPLE_REBAR_STYLE | RBS_DBLCLKTOGGLE;
	{
		CIniFileI	pr( g_szIniFileName, _T("ReBar") );
		DWORD		dwNoBoader 		= pr.GetValue( _T("NoBoader") );
		if (dwNoBoader)
			dwRebarStyle &= ~RBS_BANDBORDERS;
	}
	CreateSimpleReBar(dwRebarStyle);
	m_ReBar.SubclassWindow(m_hWndToolBar);

	//CreateSimpleReBar(MTL_SIMPLE_REBAR_STYLE | RBS_DBLCLKTOGGLE);

	if (m_CmdBar.m_fontMenu.m_hFont) {
		::SendMessage( m_hWndToolBar, WM_SETFONT, (WPARAM) m_CmdBar.m_fontMenu.m_hFont, MAKELPARAM(TRUE, 0) );
		m_LinkBar.SetFont(m_CmdBar.m_fontMenu.m_hFont);
	}
}


/// create statusbar
void	CMainFrame::init_statusBar()
{
	//CreateSimpleStatusBar();
	m_wndStatusBar.Create(m_hWnd, ATL_IDS_IDLEMESSAGE,
							 WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP | SBT_TOOLTIPS);

	m_hWndStatusBar = m_wndStatusBar.m_hWnd;
	//		int nPanes[] = { ID_DEFAULT_PANE, ID_PROGRESS_PANE, ID_COMBOBOX_PANE};
	static const int   nPanes[] = { ID_DEFAULT_PANE, ID_PROGRESS_PANE, ID_PRIVACY_PANE, ID_SECURE_PANE, ID_COMBOBOX_PANE };
	m_wndStatusBar.SetPanes( (int *) nPanes, 5, false );
	m_wndStatusBar.SetCommand( ID_PRIVACY_PANE, ID_PRIVACYREPORT );
	m_wndStatusBar.SetCommand( ID_SECURE_PANE, ID_SECURITYREPORT);
	m_wndStatusBar.SetIcon( ID_PRIVACY_PANE, 1 );				//minit
	m_wndStatusBar.SetIcon( ID_SECURE_PANE , 0 );				//minit
	m_wndStatusBar.SetOwnerDraw( m_wndStatusBar.IsValidBmp() );

	// UH
	InitStatusBar();
}


// UH
void CMainFrame::InitStatusBar()
{
	enum {	//+++ IDはデフォルトの名前になっているが、交換した場合にややこしいので、第一領域、第二領域として扱う.
		ID_PAIN_1	= ID_PROGRESS_PANE,
		ID_PAIN_2	= ID_COMBOBOX_PANE,
	};
	DWORD		dwSzPain1 = 125;
	DWORD		dwSzPain2 = 125;
	{
		DWORD		dwVal	  = 0;
		CIniFileI	pr( g_szIniFileName, _T("StatusBar") );

		if (pr.QueryValue( dwVal, _T("SizePain") ) == ERROR_SUCCESS) {
			dwSzPain1 = LOWORD(dwVal);
			dwSzPain2 = HIWORD(dwVal);
		}

		if (pr.QueryValue( dwVal, _T("SwapPain") ) == ERROR_SUCCESS)
			g_bSwapProxy = dwVal != 0;
	}

	//+++ 位置交換の修正.
	if (g_bSwapProxy == 0) {
		g_dwProgressPainWidth = dwSzPain1;					//+++ 手抜きでプログレスペインの横幅をグローバル変数に控える.
		m_wndStatusBar.SetPaneWidth(ID_PAIN_1, 0);			//dwSzPain1); //起動時はペインサイズが0
		if (m_cmbBox.IsUseIE())
			dwSzPain2 = 0;									// IEのProxyを使う場合はProxyペインサイズを0に
		m_wndStatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);
	} else {	// 交換しているとき.
		g_dwProgressPainWidth = dwSzPain2;					//+++ 手抜きでプログレスペインの横幅をグローバル変数に控える.
		m_wndStatusBar.SetPaneWidth(ID_PAIN_2, dwSzPain2);	//+++ 交換してるときは、最初からサイズ確保.
		if (m_cmbBox.IsUseIE())
			dwSzPain1 = 0;									// IEのProxyを使う場合はProxyペインサイズを0に
		m_wndStatusBar.SetPaneWidth(ID_PAIN_1, dwSzPain1);
	}

	m_wndStatusBar.SetPaneWidth(ID_SECURE_PANE	, 25);
	m_wndStatusBar.SetPaneWidth(ID_PRIVACY_PANE , 25);
}


/// create prgress bar
void CMainFrame::init_progressBar()
{
	RECT			   rect;
	m_wndProgress.Create(m_hWndStatusBar, NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | PBS_SMOOTH, 0, IDC_PROGRESS);
	m_wndProgress.ModifyStyleEx(WS_EX_STATICEDGE, 0);

	// Proxyコンボボックス
	// IEのProxyを使う場合、コンボボックスとステータスバーのProxyペインを削除
	m_cmbBox.Create(m_hWndStatusBar, rect, NULL,
					 WS_CHILD | WS_CLIPSIBLINGS | PBS_SMOOTH | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, IDC_COMBBOX);

	m_cmbBox.SetFont( m_wndStatusBar.GetFont() );
	m_cmbBox.SetDrawStyle(CSkinOption::s_nComboStyle);
	m_cmbBox.IsUseIE() ? m_cmbBox.ShowWindow(SW_HIDE)
					   : m_cmbBox.ShowWindow(SW_SHOWNORMAL);
	m_cmbBox.ResetProxyList();
}


/// Plugin Toolbar Load
void CMainFrame::init_pluginManager()
{
	CPluginManager::Init();
	CPluginManager::ReadPluginData(PLT_TOOLBAR, m_hWnd);
	CPluginManager::LoadAllPlugin(PLT_TOOLBAR, m_hWnd, true);	//ツールバープラグインを全部ロード

	{
		int nCount = CPluginManager::GetCount(PLT_TOOLBAR);
		for (int i = 0; i < nCount; i++) {
			HWND hWnd = CPluginManager::GetHWND(PLT_TOOLBAR, i);

			if ( ::IsWindow(hWnd) )
				::SetProp( hWnd, _T("Donut_Plugin_ID"), HANDLE( IDC_PLUGIN_TOOLBAR + i) );
		}
	}
	//LoadPluginToolbars();
}


/// load band position
void CMainFrame::init_band_position(
	HWND 	hWndCmdBar,
	HWND	hWndToolBar,
	HWND	hWndAddressBar,
	HWND	hWndMDITab,
	HWND	hWndLinkBar,
	HWND	hWndSearchBar  )
{
	CSimpleArray<HWND> aryHWnd;
	aryHWnd.Add( hWndCmdBar 	);		// メニューバー
	aryHWnd.Add( hWndToolBar	);		// ツールバー
	aryHWnd.Add( hWndAddressBar );		// アドレスバー
	aryHWnd.Add( hWndMDITab 	);		// タブバー
	aryHWnd.Add( hWndLinkBar	);		// リンクバー
	aryHWnd.Add( hWndSearchBar	);		// 検索バー

	int 			   nToolbarPluginCount = CPluginManager::GetCount(PLT_TOOLBAR);
	CReBarBandInfo *   pRbis			   = new CReBarBandInfo[STDBAR_COUNT + nToolbarPluginCount];

	int 			   nIndex;

	for (nIndex = 0; nIndex < aryHWnd.GetSize(); nIndex++) {
		pRbis[nIndex].nIndex	= nIndex;
		pRbis[nIndex].hWnd		= aryHWnd	  [ nIndex ];
		pRbis[nIndex].nID		= STDBAR_ID   [ nIndex ];
		pRbis[nIndex].fStyle	= STDBAR_STYLE[ nIndex ];
		pRbis[nIndex].lpText	= STDBAR_TEXT [ nIndex ];
		pRbis[nIndex].cx		= 0;
	}

	for (nIndex = 0; nIndex < nToolbarPluginCount; nIndex++) {
		pRbis[STDBAR_COUNT + nIndex].nIndex   = STDBAR_COUNT + nIndex;
		pRbis[STDBAR_COUNT + nIndex].hWnd	  = CPluginManager::GetHWND(PLT_TOOLBAR, nIndex);
		pRbis[STDBAR_COUNT + nIndex].nID	  = IDC_PLUGIN_TOOLBAR + nIndex;
		pRbis[STDBAR_COUNT + nIndex].fStyle   = RBBS_BREAK;
		pRbis[STDBAR_COUNT + nIndex].lpText   = NULL;
		pRbis[STDBAR_COUNT + nIndex].cx 	  = 0;
	}

	{
		CIniFileI pr( g_szIniFileName, _T("ReBar") );
		MtlGetProfileReBarBandsState( pRbis, pRbis + STDBAR_COUNT + nToolbarPluginCount, pr, *this);
		//x pr.Close(); 	//+++
	}

	delete[] pRbis; 											//memory leak bug Fix  minit

	m_CmdBar.RefreshBandIdealSize(m_hWndToolBar);
	m_AddressBar.InitReBarBandInfo(m_hWndToolBar);				// if you dislike a header, remove this.
	ShowLinkText( (m_AddressBar.m_dwAddressBarExtendedStyle & ABR_EX_TEXTVISIBLE) != 0 );
}


/// load status bar state
void CMainFrame::init_loadStatusBarState()
{
	CIniFileI pr( g_szIniFileName, _T("Main") );
	BOOL	bStatusBarVisible = TRUE;
	MtlGetProfileStatusBarState(pr, m_hWndStatusBar, bStatusBarVisible);
	m_bStatusBarVisibleUnlessFullScreen = bStatusBarVisible;
	//x pr.Close();
}


/// create mdi client window
void CMainFrame::init_mdiClientWindow()
{
	m_menuWindow	= ::GetSubMenu(m_hMenu, _nPosWindowMenu);
	CreateMDIClient(m_menuWindow.m_hMenu);
	m_wndMDIClient.SubclassWindow(m_hWndMDIClient);

	// NOTE: If WS_CLIPCHILDREN not set, MDI Client will try erase background over MDI child window,
	//		 but as OnEraseBkgnd does nothing, it goes well.
	if (CMainOption::s_bTabMode)
		m_wndMDIClient.ModifyStyle(WS_CLIPCHILDREN, 0); 		// to avoid flicker, but scary
}


/// splitter window
void CMainFrame::init_splitterWindow()
{
	m_hWndClient	= m_wndSplit.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_wndSplit.SetSplitterExtendedStyle(0);
	m_hWndAdditionalSplit = m_wndAdditionalSplit.Create(m_hWndClient, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	//m_wndAdditionalSplit.SetSplitterExtendedStyle(SPLIT_BOTTOMALIGNED);
	//m_wndAdditionalSplit.m_cxySplitBar = 0;
	///m_wndAdditionalSplit.SetSinglePaneMode(SPLIT_PANE_TOP);
}
/*
void CMainFrame::init_faytWindow(){
	m_hWndFAYT = m_fayt.Create(m_hWndAdditionalSplit,rcDefault,_T("X"),WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|BS_CENTER|BS_VCENTER);
}*/

/// pane container
void CMainFrame::init_explorerBar()
{
	m_ExplorerBar.Create(m_hWndClient);
	m_ExplorerBar.Init(m_hWndMDIClient, m_hWndFAYT);
}


/// MDIClient misc
void CMainFrame::init_mdiClient_misc(HWND hWndCmdBar, HWND hWndToolBar)
{
	m_mcCmdBar.InstallAsMDICmdBar(hWndCmdBar, m_hWndMDIClient, CMainOption::s_bTabMode);
	m_mcToolBar.InstallAsStandard(hWndToolBar, m_hWnd, true, ID_VIEW_FULLSCREEN);
	CIniFileI pr( _GetFilePath( _T("Menu.ini") ), _T("Option") );
	DWORD	dwNoButton = pr.GetValue( _T("NoButton") );
	pr.Close();
	bool	bNoButton  = dwNoButton != 0/*? true : false*/;
	m_mcCmdBar.ShowButton(!bNoButton);
	//m_mcToolBar.ShowButton(!bNoButton);
	m_MDITab.SetMDIClient(m_hWndMDIClient);
}


//	// set up UI
//	CmdUIAddToolBar(hWndToolBar);
//	CmdUIAddToolBar(m_SearchBar.m_wndToolBar);


void CMainFrame::init_setUserAgent()
{
	nsCOMPtr<nsIPrefBranch> pb = do_GetService("@mozilla.org/preferences-service;1");
	if(!pb) return;
	std::vector<char>	userAgent = Misc::tcs_to_sjis( CDLControlOption::userAgent() );
	pb->SetCharPref("general.useragent.override", &userAgent[0]);
}


void CMainFrame::init_message_loop()
{
	// message loop
	CMessageLoop *pLoop 	 = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
}


/// mdiタブに関する情報の読み込み
void CMainFrame::init_mdiTab()
{
	CIniFileI	pr( g_szIniFileName, _T("MDITab") );
	MtlGetProfileMDITab(pr, m_MDITab);
	pr.Close();

	// フォント
	if (m_font.m_hFont) {
		MTL::CLogFont 	 lf;
		m_font.GetLogFont(lf);
		m_MDITab.SetMDITabLogFont(lf);
	}
}


//	// for dragging files
//	RegisterDragDrop(); 										//DragAcceptFiles();
//	CCriticalIdleTimer::InstallCriticalIdleTimer(m_hWnd, ID_VIEW_IDLE);

// システムメニューに追加
void CMainFrame::init_sysMenu()
{
	// システムメニューに追加
	HMENU		hSysMenu = ::GetSystemMenu(m_hWnd, FALSE);
	//		::AppendMenu(hSysMenu, MF_ENABLED|MF_STRING, ID_VIEW_COMMANDBAR, _T("メニューを表示"));

	TCHAR		cText[]	 = _T("メニューを表示");
	MENUITEMINFO  menuInfo;
	memset( &menuInfo, 0, sizeof (MENUITEMINFO) );
	menuInfo.cbSize 	= sizeof (MENUITEMINFO);
	menuInfo.fMask		= MIIM_ID | MIIM_TYPE;
	menuInfo.fType		= MFT_STRING;
	menuInfo.wID		= ID_VIEW_COMMANDBAR;
	menuInfo.dwTypeData = cText;
	menuInfo.cch		= sizeof (cText);
	::InsertMenuItem(hSysMenu, 0, MF_BYPOSITION, &menuInfo);
}


//	::SetTimer(m_hWnd, ENT_READ_ACCEL, 200, NULL);
//	//スキンを初期化
//	InitSkin();

//	CDonutSimpleEventManager::RaiseEvent(EVENT_INITIALIZE_COMPLETE);
//	//Hook InputDialogMessage (javascript prompt())
//	CDialogHook::InstallHook(m_hWnd);
//	//CSearchBoxHook::InstallSearchHook(m_hWnd);


void CMainFrame::init_loadPlugins()
{
	//プラグインのアーリーローディング
	CPluginManager::LoadAllPlugin(PLT_EXPLORERBAR, m_ExplorerBar.m_PluginBar);

	//オペレーションプラグインのロード
	CPluginManager::ReadPluginData(PLT_OPERATION);

  #if 1	//+++ 無理やり DockingBarプラグインを、ExplorerBarとして扱ってみる...
	CPluginManager::LoadAllPlugin(PLT_DOCKINGBAR, m_ExplorerBar.m_PluginBar);
  #else
	CPluginManager::LoadAllPlugin(PLT_DOCKINGBAR, m_hWnd);
  #endif
}



// ===========================================================================
// 終了処理

CMainFrame::~CMainFrame()
{
	if ( m_wndMDIClient.IsWindow() )
		m_wndMDIClient.UnsubclassWindow();
}



void CMainFrame::OnEndSession(BOOL wParam, UINT lParam) 						//　ShutDown minit
{
	if (wParam == TRUE) {
		OnDestroy();
		_PrivateTerm();
	}
}


void CMainFrame::OnDestroy()
{
	SetMsgHandled(FALSE);
	MtlSendCommand(m_hWnd, ID_VIEW_STOP_ALL);									// added by DOGSTORE

	//CSearchBoxHook::UninstallSearchHook();
	CDialogHook::UninstallHook();

	//デバッグウィンドウ削除
	m_wndDebug.Destroy();

	//全プラグイン解放
	CPluginManager::Term();

	m_ReBar.UnsubclassWindow();

	//バックアップスレッドの開放
	if (m_hGroupThread) {
		DWORD dwResult = ::WaitForSingleObject(m_hGroupThread, DONUT_BACKUP_THREADWAIT);

		if (dwResult == WAIT_TIMEOUT) {
			ErrorLogPrintf(_T("MainFrame::OnDestroyにて、自動更新スレッドの終了が出来ない模様\n"));
			::TerminateThread(m_hGroupThread, 1);
		}

		CloseHandle(m_hGroupThread);
		m_hGroupThread = NULL;			//+++ 最後とはいえ、一応 NULLしとく.
	}

	// Note. The script error dialog makes the app hung up.
	//		 I can't find the reason, but I can avoid it by closing
	//		 windows explicitly.
	MtlCloseAllMDIChildren(m_hWndMDIClient);

	CCriticalIdleTimer::UninstallCriticalIdleTimer();

	// what can I trust?
	ATLASSERT( ::IsMenu(m_hMenu) );
	::DestroyMenu(m_hMenu);

	_RemoveTmpDirectory();
	DestroyCustomDropDownMenu();

	RevokeDragDrop();
}



void CMainFrame::DestroyCustomDropDownMenu()
{
	OnMenuRefreshFav(FALSE);
	OnMenuRefreshFavGroup(FALSE);
	OnMenuRefreshScript(FALSE);
}


struct CMainFrame::_Function_DeleteFile {
	int m_dummy;
	_Function_DeleteFile(int ndummy) : m_dummy(ndummy)
	{
	}


	void operator ()(const CString &strFile)
	{
		if ( MtlIsExt( strFile, _T(".tmp") ) )
			::DeleteFile(strFile);
	}
};


void CMainFrame::_RemoveTmpDirectory()
{
	CString 	strTempPath = Misc::GetExeDirectory() + _T("ShortcutTmp\\");
	if (::GetFileAttributes(strTempPath) != 0xFFFFFFFF) {
		MtlForEachFile( strTempPath, CMainFrame::_Function_DeleteFile(0) );
		::RemoveDirectory(strTempPath);
	}
}



void CMainFrame::OnClose()
{
	SetMsgHandled(FALSE);

	if ( !CDonutConfirmOption::OnDonutExit(m_hWnd) ) {
		SetMsgHandled(TRUE);
		if (IsWindowVisible() == FALSE) {
			SetHideTrayIcon();
			Sleep(100);
		}
		return;
	}

	// タイマーをとめる.
	if (m_nBackUpTimerID != 0) {
		KillTimer(m_nBackUpTimerID);
		m_nBackUpTimerID	= 0;			//+++ 一応 0クリアしとく.
	}

  #if 0 //+++ _WriteProfile()からバックアップの処理をこちらに移す(先に行う) ... やっぱりやめ
	_SaveGroupOption( CStartUpOption::GetDefaultDFGFilePath() );
  #endif

	CChildFrame::SetMainframeCloseFlag();	//+++ mainfrmがcloseすることをChildFrameに教える(ナビロックのclose不可をやめるため)


	if ( IsFullScreen() ) {
		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_FULLSCREEN;	// save style
		_ShowBandsFullScreen(FALSE);								// restore bands position
	} else {
		CMainOption::s_dwMainExtendedStyle &= ~MAIN_EX_FULLSCREEN;
	}

	m_mcCmdBar.Uninstall();
	m_mcToolBar.Uninstall();

	_WriteProfile();

	CMainOption::s_bAppClosing = true;

	DelCache();
	DelCookie();
	DelHistory();
}


void CMainFrame::DelCookie()
{
	bool bDelCookie	 = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_COOKIE) != 0;	//+++ ? TRUE : FALSE;
	if(!bDelCookie) return;

	nsCOMPtr<nsICookieManager> cm = do_GetService("@mozilla.org/cookiemanager;1");
	cm->RemoveAll();
}

void CMainFrame::DelCache()
{
	bool bDelCache	 = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_CASH  ) != 0;	//+++ ? TRUE : FALSE;
	if(!bDelCache) return;

	nsCOMPtr<nsICacheService> cs = do_GetService("@mozilla.org/network/cache-service;1");
	cs->EvictEntries(nsICache::STORE_ON_DISK);
	cs->EvictEntries(nsICache::STORE_IN_MEMORY);
}

void CMainFrame::DelHistory()
{
	bool bDelHistory  = (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_DEL_HISTORY) != 0;	//+++ ? TRUE : FALSE;
	if (bDelHistory == false) return;

	nsCOMPtr<nsIBrowserHistory> history = do_GetService("@mozilla.org/browser/nav-history-service;1");
	history->RemoveAllPages();

}


