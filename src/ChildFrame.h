/**
 *	@file	ChildFrame.h
 *	@brief	タブページ１つの処理.
 */

#pragma once

#include <vector>
#include "PluginManager.h"
#include "MtlBrowser.h"
#include "MDIChildUserMessenger.h"

#include "FavoriteMenu.h"
#include "DonutView.h"
#include "AddressBar.h"
#include "MDITabCtrl.h"

#include "option/DLControlOption.h"
#include "option/StartUpOption.h"
#include "option/DonutConfirmOption.h"
#include "option/IgnoreURLsOption.h"
#include "option/CloseTitleOption.h"

#include "FileNotification.h"
#include "DonutPFunc.h" 		//+++
//#include "IEToolBar.h"		//+++

#include "IGeckoBrowserEventListener.h"

class nsIWebBrowser;
class CChildFrame
		: public CMDIChildWindowImplFixed<CChildFrame, 2>
//		, public IWebBrowserEvents2Impl<CChildFrame, ID_DONUTVIEW>
        , public IGeckoBrowserEventListener
		, public CWebBrowserCommandHandler<CChildFrame>
		, public CUpdateCmdUI<CChildFrame>
		, public CMDIChildUserMessenger<CChildFrame>
		, public CGeckoBrowser
{
public:
	typedef CMDIChildWindowImplFixed<CChildFrame, 2> baseClass;
	// Declarations

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)
	DECLARE_FRAME_WND_MENU(NULL)

	DECL_GECKOEVENTLISTENER

	enum {	//+++ 実際に参照があるのは、最期のNOREFRESHのみだが念のため.
		FLAG_SE_IMAGE		=	0x00000001,
		FLAG_SE_FRAME		=	0x00000002,
		FLAG_SE_PLUGIN		=	0x00000004,
		FLAG_SE_SCRIPT		=	0x00000008,
		FLAG_SE_REDIR		=	0x00000010,
		FLAG_SE_DNS 		=	0x00000020,
//		FLAG_SE_JAVA			=	0x00000040, //plus7

		FLAG_SE_NAVIGATELOCK	=	0x00000080,
	  #ifdef USE_ORG_UNDONUT_INI				//+++ unDonut オリジナル版の値
		FLAG_SE_MSGFILTER		=	0x00000100,
		FLAG_SE_MOUSEGESTURE	=	0x00000200,
		FLAG_SE_BLOCKMAILTO 	=	0x00000400,
		FLAG_SE_FLATVIEW		=	0x00000800, //+++ オリジナルにはないが設定.
	  #else 									//+++ unDonut+ の値.
		FLAG_SE_FLATVIEW		=	0x00000100,
		FLAG_SE_MSGFILTER		=	0x00000200,
		FLAG_SE_MOUSEGESTURE	=	0x00000400,
		FLAG_SE_BLOCKMAILTO 	=	0x00000800,
	  #endif

		FLAG_SE_VIEWED			=	0x00001000,

		FLAG_SE_REFRESH_NONE	=	0x00010000,
		FLAG_SE_REFRESH_15		=	0x00020000,
		FLAG_SE_REFRESH_30		=	0x00040000,
		FLAG_SE_REFRESH_1M		=	0x00080000,
		FLAG_SE_REFRESH_2M		=	0x00100000,
		FLAG_SE_REFRESH_5M		=	0x00200000,
		FLAG_SE_REFRESH_USER	=	0x00400000,

		FLAG_SE_NOREFRESH		=	0x01000000,
	};

public:
	// Constructor/Destructor
	CChildFrame(CMDITabCtrl &MDITab, CDonutAddressBar &addressbar, bool bNewWindow2, DWORD dwDefaultDLControlFlags, DWORD dwDefaultExtendedStyleFlags);
	~CChildFrame();

	static CChildFrame *NewWindow(
			HWND				hWndMDIClient,
			CMDITabCtrl &		tabMDI,
			CDonutAddressBar &	adBar,
			bool				bNewWindow2 = false,
			DWORD				dwDLFlags	= CDLControlOption::s_dwDLControlFlags,
			DWORD				dwESFlags   = CDLControlOption::s_dwExtendedStyleFlags);

	virtual void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	int ActivateFrame(int nCmdShow = -1);
	int ActivateFrameForNewWindowDelayed(int nCmdShow = -1);

	CDonutView&		  view() 		{ return m_view; }
	const CDonutView& view() const  { return m_view; }

	int  		 _AddGroupOption(const CString& strFileName);
	static void  SetMainframeCloseFlag() { s_bMainframeClose = true; } 			//+++ mainfrmがcloseするときonにされる.

	void searchEngines(const CString &strKeyWord );
	CString GetSelectedText();
	CString GetSelectedTextLine();		//+++
	bool	HaveSelectedText() const;	//+++

	// Event handlers
	void OnSetSecureLockIcon(long nSecureLockIcon) { m_nSecureLockIcon = nSecureLockIcon; }
	void OnPrivacyImpactedStateChange(bool bPrivacyImpacted) { m_bPrivacyImpacted = bPrivacyImpacted; ATLTRACE( _T("[OnPrivacyImpactedStateChange]\n") ); }

	void OnBeforeNavigate2(IDispatch *			pDisp,
						   const CString &		strURL,
						   DWORD				nFlags,
						   const CString &		strTargetFrameName,
						   CSimpleArray<BYTE>&	baPostedData,
						   const CString &		strHeaders,
						   bool &				bCancel );

	void OnStateConnecting();
	void OnStateDownloading() { m_MDITab.SetDownloading(m_hWnd); }
	void OnStateCompleted();

	__inline const CString& strStatusBar() const { return m_strStatusBar; }

	LRESULT OnGetExtendedTabStyle();
	void 	OnSetExtendedTabStyle(DWORD dwStyle);

	CChildFrame *OnGetChildFrame() { return this; }

	void 	SetViewExStyle(DWORD dwStyle, BOOL bExProp = FALSE);
	void 	SetSearchWordAutoHilight( const CString& str, bool autoHilightSw );
	void 	SetArrayHist(std::vector< std::pair<CString, CString> > &	ArrayFore, std::vector< std::pair<CString, CString> > &	ArrayBack );
	void	SetDfgFileNameSection(const CString &strFileName, const CString &strSection);

	void	SetUrlSecurityExStyle(LPCTSTR lpszFile);

  #if 1 //+++ 強制的に、その場でメッセージをさばく...もう不要かも...
	int ForceMessageLoop();
  #endif

public:
	// Message map and handlers
	BEGIN_MSG_MAP(CChildFrame)
		try {	//+++
		PASS_MSG_MAP_MDICHILD_TO_MDITAB  (m_MDITab)
		PASS_MSG_MAP_MENUOWNER_TO_EXPMENU(m_FavMenu)

		MESSAGE_HANDLER 	( WM_SIZE, OnSize	)
		MSG_WM_SYSCOMMAND	( OnSysCommand		)
		MSG_WM_CREATE		( OnCreate			)
		MSG_WM_CLOSE		( OnClose			)
		MSG_WM_DESTROY		( OnDestroy 		)
		MSG_WM_MDIACTIVATE	( OnMDIActivate 	)
		MSG_WM_FORWARDMSG	( OnForwardMsg		)
		HANDLE_MESSAGE		( WM_SETFOCUS		)			// baseClass handler is not effective
		MSG_WM_USER_GET_IWEBBROWSER( )
		USER_MSG_WM_SAVE_OPTION( OnSaveOption	)

		// UH
		USER_MSG_WM_MENU_GOBACK 	( OnMenuGoBack		)
		USER_MSG_WM_MENU_GOFORWARD	( OnMenuGoForward	)
		MSG_WM_USER_IS_SELECTED 	( OnIsSelectedTab	)
		MSG_WM_USER_HILIGHT 		( OnHilight 		)
		MSG_WM_USER_FIND_KEYWORD	( OnFindKeyWord 	)
		COMMAND_ID_HANDLER			( ID_WINDOW_TILE_HORZ, OnWindowTile )
		COMMAND_ID_HANDLER			( ID_WINDOW_TILE_VERT, OnWindowTile )
		MESSAGE_HANDLER 			( WM_USER_SIZE_CHG_EX, OnSizeChgEx	)
		MSG_WM_USER_USED_MOUSE_GESTURE( OnUsedMouseGesture )
	  #ifndef NO_STYLESHEET
		COMMAND_ID_HANDLER			( ID_STYLESHEET_BASE , OnStyleSheet )
		COMMAND_ID_HANDLER			( ID_STYLESHEET_OFF  , OnStyleSheet )
		//MSG_WM_USER_CHANGE_CSS( OnChangeCSS )
	  #endif
		//^^^

		//minit
		USER_MSG_WM_GET_EXTENDED_TABSTYLE( OnGetExtendedTabStyle )
		USER_MSG_WM_SET_EXTENDED_TABSTYLE( OnSetExtendedTabStyle )
		USER_MSG_WM_SET_CHILDFRAME( OnGetChildFrame )

		COMMAND_ID_HANDLER_EX( ID_FILE_CLOSE			, OnFileClose				)
		COMMAND_ID_HANDLER_EX( ID_WINDOW_CLOSE_ALL		, OnWindowCloseAll			)	// UH
		COMMAND_ID_HANDLER_EX( ID_EDIT_IGNORE			, OnEditIgnore				)
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_REF, OnEditOpenSelectedRef 	)
		COMMAND_ID_HANDLER_EX( ID_EDIT_OPEN_SELECTED_TEXT, OnEditOpenSelectedText	)

		COMMAND_ID_HANDLER_EX( ID_EDIT_CLOSE_TITLE		, OnEditCloseTitle			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND_MAX 		, OnEditFindMax 			)	//moved from CWebBrowserCommandHandler by minit

		COMMAND_ID_HANDLER_EX( ATL_IDS_SCPREVWINDOW 	, OnWindowPrev				)
		COMMAND_ID_HANDLER_EX( ATL_IDS_SCNEXTWINDOW 	, OnWindowNext				)

		COMMAND_ID_HANDLER_EX( ID_FAVORITE_GROUP_ADD	, OnFavoriteGroupAdd		)
		COMMAND_ID_HANDLER_EX( ID_FAVORITE_ADD			, OnFavoriteAdd 			)

		COMMAND_ID_HANDLER_EX( ID_VIEW_SETFOCUS 		, OnViewSetFocus			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH			, OnViewRefresh 			)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP 			, OnViewStop				)
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION			, OnViewOption				)
		COMMAND_ID_HANDLER_EX( ID_FILE_SAVE_AS			, OnFileSaveAs				)
		COMMAND_ID_HANDLER_EX( ID_FILE_PAGE_SETUP		, OnFilePageSetup			)
		COMMAND_ID_HANDLER_EX( ID_FILE_PRINT			, OnFilePrint				)
		COMMAND_ID_HANDLER_EX( ID_FILE_PROPERTIES		, OnFileProperties			)

		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND 			, OnEditFind				)

		COMMAND_ID_HANDLER( ID_WINDOW_CLOSE_EXCEPT		, OnWindowCloseExcept		)
		COMMAND_ID_HANDLER( ID_WINDOW_REFRESH_EXCEPT	, OnWindowRefreshExcept 	)

		COMMAND_ID_HANDLER( ID_LEFT_CLOSE				, OnLeftRightClose			)
		COMMAND_ID_HANDLER( ID_RIGHT_CLOSE				, OnLeftRightClose			)

		COMMAND_ID_HANDLER( ID_VIEW_REFRESH_FAVBAR		, OnViewRefreshFavBar		)

		COMMAND_ID_HANDLER_EX( ID_DOCHOSTUI_OPENNEWWIN	, OnDocHostUIOpenNewWin 	)
//		COMMAND_ID_HANDLER_EX( ID_REGISTER_AS_BROWSER	, OnRegisterAsBrowser		)

		COMMAND_RANGE_HANDLER_EX( ID_VIEW_BACK1   , ID_VIEW_BACK9	, OnViewBackX	)
		COMMAND_RANGE_HANDLER_EX( ID_VIEW_FORWARD1, ID_VIEW_FORWARD9, OnViewForwardX)

		COMMAND_ID_HANDLER( ID_HTMLZOOM_MENU			, OnHtmlZoomMenu			)	//+++
		COMMAND_ID_HANDLER( ID_HTMLZOOM_ADD				, OnHtmlZoomAdd				)	//+++
		COMMAND_ID_HANDLER( ID_HTMLZOOM_SUB				, OnHtmlZoomSub				)	//+++
		COMMAND_ID_HANDLER( ID_HTMLZOOM_100TOGLE		, OnHtmlZoom100Togle		)	//+++
		COMMAND_RANGE_HANDLER_EX( ID_HTMLZOOM_400 , ID_HTMLZOOM_050 , OnHtmlZoom    )	//+++
		//COMMAND_RANGE_HANDLER_EX(ID_INSERTPOINT_SEARCHENGINE, ID_INSERTPOINT_SEARCHENGINE_END, OnSearchWeb_engineId)	//+++

		COMMAND_ID_HANDLER_EX( ID_TITLE_COPY			, OnTitleCopy				)
		COMMAND_ID_HANDLER_EX( ID_URL_COPY				, OnUrlCopy 				)
		COMMAND_ID_HANDLER_EX( ID_COPY_TITLEANDURL		, OnTitleAndUrlCopy 		)

		COMMAND_ID_HANDLER_EX( ID_SAVE_FAVORITE_TO		, OnSaveFavoriteTo			)


		CHAIN_COMMANDS( CWebBrowserCommandHandler<CChildFrame> )
		CHAIN_MSG_MAP( CUpdateCmdUI<CChildFrame>			)
		CHAIN_MSG_MAP( CMDIChildUserMessenger<CChildFrame>	)

		CHAIN_COMMANDS_MEMBER( m_view ) // CHAIN_CLIENT_COMMANDS() not send, why?
		CHAIN_MSG_MAP( baseClass )
		} catch (...) {
			ATLASSERT(0);
		}
	ALT_MSG_MAP(7)						// to hook WM_CLOSE from Browser internal window (for script: "window.close()")
		MSG_WM_CLOSE( OnBrowserClose )
	END_MSG_MAP()


public:
	// Update Command UI Map
	BEGIN_UPDATE_COMMAND_UI_MAP( CChildFrame )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_view )

		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_VIEW_BACK   , m_bNavigateBack   , true ) 	// with popup
		UPDATE_COMMAND_UI_ENABLE_IF 		  ( ID_VIEW_FORWARD, m_bNavigateForward )

		// UPDATE_COMMAND_UI_SETDEFAULT_PASS( ID_VIEW_BACK1    )
		// UPDATE_COMMAND_UI_SETDEFAULT_PASS( ID_VIEW_FORWARD1 )

		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_VIEW_BACK1   , m_bNavigateBack	)
		UPDATE_COMMAND_UI_POPUP_ENABLE_IF( ID_VIEW_FORWARD1, m_bNavigateForward )

		UPDATE_COMMAND_UI(	ID_VIEW_FONT_SMALLEST, OnUpdateFontSmallestUI )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_SMALLER , OnUpdateFontSmallerUI  )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_MEDIUM  , OnUpdateFontMediumUI   )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_LARGER  , OnUpdateFontLargerUI   )
		UPDATE_COMMAND_UI(	ID_VIEW_FONT_LARGEST , OnUpdateFontLargestUI  ) 			// with popup

		UPDATE_COMMAND_UI(	ID_DEFAULT_PANE , OnUpdateStatusBarUI )
		UPDATE_COMMAND_UI(	IDC_PROGRESS	, OnUpdateProgressUI  )
		UPDATE_COMMAND_UI(	ID_SECURE_PANE	, OnUpdateSecureUI	  )
		UPDATE_COMMAND_UI(	ID_PRIVACY_PANE , OnUpdatePrivacyUI   )

		UPDATE_COMMAND_UI_SETCHECK_IF_PASS( ID_SEARCHBAR_HILIGHT, m_bNowHilight )

		UPDATE_COMMAND_UI_ENABLE_IF_WITH_POPUP( ID_WINDOW_RESTORE, _GetShowCmd() != SW_SHOWNORMAL && !CMainOption::s_bTabMode, true )	// with popup
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_MOVE    , _GetShowCmd() != SW_SHOWMAXIMIZED && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_SIZE    , _GetShowCmd() != SW_SHOWMAXIMIZED && _GetShowCmd() != SW_SHOWMINIMIZED && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_MINIMIZE, _GetShowCmd() != SW_SHOWMINIMIZED && !CMainOption::s_bTabMode )
		UPDATE_COMMAND_UI_ENABLE_IF( ID_WINDOW_MAXIMIZE, _GetShowCmd() != SW_SHOWMAXIMIZED && !CMainOption::s_bTabMode )

	  #ifndef NO_STYLESHEET
		UPDATE_COMMAND_UI( ID_STYLESHEET_BASE, OnStyleSheetBaseUI )
	  #endif
	END_UPDATE_COMMAND_UI_MAP()


private:
	// Command overrides
   #ifndef NO_STYLESHEET
	//LRESULT OnChangeCSS(LPCTSTR lpszStyleSheet);
	LRESULT OnStyleSheet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 	SetDefaultStyleSheet(const CString& strStyleSheet);
   #endif

	LRESULT OnUsedMouseGesture() { return (m_view.m_ViewOption.m_dwExStyle & DVS_EX_MOUSE_GESTURE) != 0; }
	LRESULT OnSizeChgEx(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) { m_nCmdType = SC_RESTORE; return 0; }
	LRESULT OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/) { m_nCmdType = SC_RESTORE; return 0; }
	BOOL	OnIsSelectedTab(HWND hWnd) { return (GetTabItemState(hWnd) & TCISTATE_MSELECTED) != 0; }
	LRESULT OnMenuGoBack(HMENU hMenu) { MenuChgGoBack(hMenu); return 0; }
	LRESULT OnMenuGoForward(HMENU hMenu) { MenuChgGoForward(hMenu); return 0; }
	//void 	OnRegisterAsBrowser(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/) { SetRegisterAsBrowser(wNotifyCode == NM_ON); }

	void 	OnDocHostUIOpenNewWin(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void 	OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnViewStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnFavoriteGroupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void 	OnClose();
	void 	OnPaint(HDC hDc);	//+++ 追加

	//+++ 失敗 void OnLButtonDown(WORD wparam, const WTL::CPoint& pt);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);	//+++ 追加
	void 	OnSysCommand(UINT uCmdType, CPoint pt);

	void 	OnWindowPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)	{ MDINext(m_hWnd, TRUE); }
	void 	OnWindowNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/) { MDINext(m_hWnd, FALSE); }

	LRESULT OnWindowCloseExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnLeftRightClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	LRESULT OnForwardMsg(LPMSG pMsg, DWORD);
	static void CALLBACK OncePaintReq(HWND hWnd, UINT wparam, UINT_PTR lparam, DWORD );	///< +++ タイマーで無理やりまって、画面をリサイズで描画

	LRESULT OnCreate(LPCREATESTRUCT lpcreatestruct);
	void 	_InitDragDropSetting();
	void 	__CalcDefaultRect(CRect &rc);

	void 	OnDestroy();

	LRESULT OnViewRefreshFavBar(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	void 	OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditIgnore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditCloseTitle(WORD , WORD , HWND );
	void 	OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void 	OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void  	OnEditFindMax(WORD wNotifyCode, WORD wID, HWND hWndCtl) { _OnEditFindMax(wNotifyCode, wID, hWndCtl); }

	CString _ReplaceCRLF(CString &str, CString &strRep);
	LRESULT OnHilight(LPCTSTR lpszKeyWord);	//+++ , BOOL bToggle = TRUE, BOOL bFlag = FALSE);

  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	LRESULT OnHilightOnce(nsIWebBrowser *pWb, LPCTSTR lpszKeyWord);
  #endif

	LRESULT OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bFindDown);
	BOOL 	_FindKeyWordOne(IHTMLDocument2 *pDocument, const CString& rStrKeyWord, BOOL bFindDown);	//ページ内検索
	void 	ScrollBy(IHTMLDocument2 *pDoc2);

	struct _Function_SelectEmpt;
	struct _Function_Hilight;

  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	// gae _Function_Hilightと引数の意味が違うので注意
	struct _Function_Hilight2;
  #endif	// KIKE_UNDONUT_G

	struct _Function_OpenSelectedRef;

	void OnBrowserClose();

  #if 1
	void OnSaveOption(LPCTSTR lpszFileName, int nIndex);
  #endif

	void OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void OnSaveFavoriteTo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/) { }

	void OnMDIActivate(HWND hwndChildDeact, HWND hwndChildAct);

   #ifndef NO_STYLESHEET
	void ApplyDefaultStyleSheet();
   #endif
	void SetTravelLogData();

	BOOL _Load_TravelData(std::vector<std::pair<CString, CString> >&	arrFore,
						  std::vector<std::pair<CString, CString> >&	arrBack,
						  CString & 									strFileName,
						  CString & 									strSection);

	BOOL _Load_TravelLog(	std::vector<std::pair<CString, CString> >&	arrLog,
							nsCOMPtr<nsIWebBrowser> pWB,
							BOOL bFore);

	LPOLESTR _ConvertString(LPCTSTR lpstrBuffer, int nBufferSize);

	LRESULT OnHtmlZoomMenu(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnHtmlZoomAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnHtmlZoomSub(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

	///+++ 100% とその他の比率をトグル切替.
	LRESULT OnHtmlZoom100Togle(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL &);
	void 	OnHtmlZoom(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	void 	SetBodyStyleZoom(int addSub, int scl);

	//プログレスペインの幅を設定
	BOOL 	SetProgressPaneWidth(int cxWidth);

	DWORD 	_GetInheritedDLControlFlags();
	DWORD 	_GetInheritedExtendedStyleFlags();

	void 	_SaveFocus() { m_hWndFocus = ::GetFocus(); }
	void 	_RestoreFocus();
	void 	_SetPageFocus();
	void 	_SetupWindowCloseHook();
	static BOOL CALLBACK _EnumProc(HWND hWnd, LPARAM lParam);

	// Update Command UI Handlers
   #ifndef NO_STYLESHEET
	void 	OnStyleSheetBaseUI(CCmdUI *pCmdUI);
   #endif
	void 	OnUpdateFontSmallestUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontSmallerUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontMediumUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontLargerUI(CCmdUI *pCmdUI);
	void 	OnUpdateFontLargestUI(CCmdUI *pCmdUI);
	void 	OnUpdateStatusBarUI(CCmdUI *pCmdUI);
	void 	OnUpdateSecureUI(CCmdUI *pCmdUI);
	void 	OnUpdatePrivacyUI(CCmdUI *pCmdUI);
	void 	OnUpdateProgressUI(CCmdUI *pCmdUI);

	int 	_GetShowCmd();

	BYTE 	GetTabItemState(HWND hTarWnd);

	virtual void PreDocumentComplete( /*[in]*/ IDispatch *pDisp, /*[in]*/ VARIANT *URL);


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
	void InsertTypeOf(CMenu& menuPopup,
	                  DWORD *menupos,
					  DWORD dwID,
					  CSimpleArray<HMENU> &toDestroy,
					  CSimpleMap<DWORD, DWORD> &toRemove,
					  CSimpleArray<DWORD> &commands);
    bool DocHasSelection(nsIDOMDocument *doc);
public:
	CString                                     m_ctxLink;
	CString                                     m_ctxImage;
	nsCOMPtr<nsIDOMNode>                        m_ctxTargetNode;            //コンテキストメニューで選択されたノード
private:
	// Constants
	enum { _nPosFavoriteMenuOfTab = 8 };

	// Data members
	CDonutView									m_view;

	MTL::CMDITabCtrl &							m_MDITab;
	CDonutAddressBar &							m_AddressBar;
	CContainedWindow							m_wndBrowser;
	CChildFavoriteMenu<CChildFrame> 			m_FavMenu;
	CString 									m_strStatusBar;
	HWND										m_hWndFocus;
	HWND										m_hWndF;
	HWND										m_hWndA;
	CComBSTR/*BSTR*/							m_strBookmark;
	CImageList									m_imgList;

	CString 									m_strDfgFileName;
	CString 									m_strSection;

	std::vector<std::pair<CString, CString> >	m_ArrayHistFore;
	std::vector<std::pair<CString, CString> >	m_ArrayHistBack;

	CString 									m_strSearchWord;

	//std::auto_ptr<CIEToolBar>					m_ieToolBar;				//+++ 実験...失敗.

	long										m_nProgress;
	long										m_nProgressMax;
	int 										m_nPainBookmark;
	int 										m_nCmdType;
	int 										m_nSecureLockIcon;
	int 										m_nImgWidth;
	int 										m_nImgHeight;

	int											m_nImgScl;					//+++ zoom,imgサイズ自動設定での設定値.
	int											m_nImgSclSav;				//+++ zoom,imgサイズの100%とのトグル切り替え用
	int											m_nImgSclSw;				//+++ zoom,imgサイズの100%とのトグル切り替え用
	int											m_japanCharSet;				//+++ -1:未調査 0:不明 1:sjis 2:euc 3:utf8  CSearchBar::ECharEncode に同じ.

	BYTE/*bool BOOL*/							m_bNavigateBack;
	BYTE/*bool BOOL*/							m_bNavigateForward;
	BYTE/*bool*/								m_bNewWindow2;
	BYTE/*bool*/								m_bClosing;
	BYTE/*bool*/								m_bPageFocusInitialized;
	BYTE/*bool*/								m_bWaitNavigateLock;
	BYTE/*bool*/								m_bPrivacyImpacted;
	BYTE/*bool BOOL*/							m_bInitTravelLog;			//minit
	BYTE/*bool*/								m_bOperateDrag;
	BYTE/*bool*/								m_bExPropLock;
	BYTE/*bool*/								m_bAutoHilight;
	BYTE/*bool*/								m_bNowHilight;
	BYTE/*bool*/								m_bImg;						//+++ urlが画像かどうか
	BYTE/*bool*/								m_bImageAutoSizeReq;		//+++ OnSizeされたか?(自動画像調整のため)
	BYTE/*bool*/								m_bImgAuto_NouseLClk;		//+++ 画像自動リサイズで左クリックを使わない.

	static bool 								s_bMainframeClose;			//+++ mainfrmがcloseするときonにされる.
};



#if 1	//+++ 手抜き. CMainFrameが終了するときにtrueにする.
__declspec(selectany) bool	   CChildFrame::s_bMainframeClose	= 0;
#endif



/////////////////////////////////////////////////////////////////////////////
