/**
 *	@file	MainOption.h
 *	@brief	donutのオプション : 全般, 全般2
 */

#pragma once

#include "../RecentDocumentListFixed.h"
#include "../MtlUpdateCmdUI.h"
#include "../MtlProfile.h"
#include "../MtlUser.h"

enum EMain_Ex {
	MAIN_EX_NEWWINDOW				= 0x00000001L,
	MAIN_EX_NOACTIVATE				= 0x00000002L,
	// MAIN_EX_NOINITWND			= 0x00000004L,	// obsolete
	MAIN_EX_ONEINSTANCE 			= 0x00000008L,
	MAIN_EX_WINDOWLIMIT 			= 0x00000010L,
	MAIN_EX_NOCLOSEDFG				= 0x00000020L,
	MAIN_EX_NOMDI					= 0x00000040L,
	// MAIN_EX_VIEWCLIPBOARD		= 0x00000080L,
	MAIN_EX_FULLSCREEN				= 0x00000100L,
	MAIN_EX_BACKUP					= 0x00000200L,
	MAIN_EX_NOACTIVATE_NEWWIN		= 0x00000400L,
	MAIN_EX_ADDFAVORITEOLDSHELL 	= 0x00000800L,
	MAIN_EX_ORGFAVORITEOLDSHELL 	= 0x00001000L,
	MAIN_EX_GLOBALOFFLINE			= 0x00002000L,
	MAIN_EX_LOADGLOBALOFFLINE		= 0x00004000L,
	MAIN_EX_KILLDIALOG				= 0x00008000L,
	MAIN_EX_REGISTER_AS_BROWSER 	= 0x00010000L,
	MAIN_EX_INHERIT_OPTIONS 		= 0x00020000L,
	// MAIN_EX_OPTIONSTYLE			= 0x00040000L,
	// MAIN_EX_NOCAPTION			= 0x00080000L,		//+++ 廃案:メインフレームのキャプションをなくす. (とりあえずskinのほうで行うことにした)
};



enum EMain_Ex2 {
	MAIN_EX2_NOCSTMMENU 			= 0x00000001L,		//+++ メモ:MainOptionで設定.
	MAIN_EX2_DEL_CASH				= 0x00000002L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_DEL_COOKIE 			= 0x00000004L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_DEL_HISTORY			= 0x00000008L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_MAKECASH				= 0x00000010L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)
	MAIN_EX2_DEL_RECENTCLOSE		= 0x00000020L,		//+++ メモ:実処理はDestroyDialog("終了処理"のオプション)

	//+++ +mod版追加
	MAIN_EX2_NOCLOSE_NAVILOCK		= 0x00000040L,		//+++ ナビゲートLock時、Closeできなくする. 実処理はDLControlOption
	MAIN_EX2_MINBTN2TRAY			= 0x00000080L,		//+++ 最小化ボタンでタスクトレイに入るようにする.
	MAIN_EX2_CLOSEBTN2TRAY			= 0x00000100L,		//+++ 閉じるボタンでタスクトレイに入るようにする.
	MAIN_EX2_USER_AGENT_FLAG		= 0x00000400L,		//+++ このフラグがonのときのみ UserAgent文字列を設定する(offならieのまま)
  #if 0	//+++ 失敗
	MAIN_EX2_TITLEBAR_STR_SWAP		= 0x00000200L,		//+++ タイトルバーの表示を,"ページ名 - unDonut"にする(通常は "unDonut - ページ名")
  #endif
};



class CMainOption {
	friend class CMainPropertyPage;
	friend class CMainPropertyPage2;

public:
	static DWORD						s_dwMainExtendedStyle;
	static DWORD						s_dwMainExtendedStyle2;
	static DWORD						s_dwExplorerBarStyle;

private:
	static DWORD						s_dwMaxWindowCount;

public:
	static DWORD						s_dwBackUpTime;
	static DWORD						s_dwAutoRefreshTime;		// UDT DGSTR ( dai
	static bool 						s_bTabMode;
	static volatile bool				s_bAppClosing;

	static volatile bool 				s_bIgnoreUpdateClipboard;	// for insane WM_DRAWCLIBOARD
	static CRecentDocumentListFixed*	s_pMru;

private:
	static bool 						s_bNoCstmMenu;
	static DWORD						s_dwErrorBlock;

	static CString *					s_pstrExplorerUserDirectory;
public:
	static bool 						s_bTravelLogGroup;
	static bool 						s_bTravelLogClose;
	static bool 						s_bStretchImage;

public:
	// Constructor
	CMainOption();

	static void 	GetProfile();
	static void 	WriteProfile();
	static void 	SetExplorerUserDirectory(const CString &strPath);
	static const CString&	GetExplorerUserDirectory();
	static void 	SetMRUMenuHandle(HMENU hMenu, int nPos);
	static bool 	IsQualify(int nWindowCount);

public:
	// Message map and handlers
	BEGIN_MSG_MAP(CMainFrame)
		COMMAND_ID_HANDLER_EX( ID_MAIN_EX_NEWWINDOW 		, OnMainExNewWindow 		)
		COMMAND_ID_HANDLER_EX( ID_MAIN_EX_NOACTIVATE		, OnMainExNoActivate		)
		COMMAND_ID_HANDLER_EX( ID_MAIN_EX_NOACTIVATE_NEWWIN , OnMainExNoActivateNewWin	)
	END_MSG_MAP()

private:
	void	OnMainExNewWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMainExNoActivate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnMainExNoActivateNewWin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

public:
	// Update command UI and handlers
	BEGIN_UPDATE_COMMAND_UI_MAP(CMainOption)
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_MAIN_EX_NEWWINDOW		 , MAIN_EX_NEWWINDOW		  , s_dwMainExtendedStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_MAIN_EX_NOACTIVATE		 , MAIN_EX_NOACTIVATE		  , s_dwMainExtendedStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_MAIN_EX_NOACTIVATE_NEWWIN, MAIN_EX_NOACTIVATE_NEWWIN  , s_dwMainExtendedStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG( ID_REGISTER_AS_BROWSER 	 , MAIN_EX_REGISTER_AS_BROWSER, s_dwMainExtendedStyle)
	END_UPDATE_COMMAND_UI_MAP()
};



class CMainPropertyPage
	: public CPropertyPageImpl<CMainPropertyPage>
	, public CWinDataExchange<CMainPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MAIN };

private:
	// Data members
	int 			m_nNewWindow;
	int 			m_nNoActivate;
	int 			m_nNoActivateNewWin;
	int 			m_nOneInstance;
	int 			m_nLimit;
	int 			m_nNoCloseDFG;
	int 			m_nNoMDI;
	int 			m_nBackUp;
	int 			m_nAddFavoriteOldShell;
	int 			m_nOrgFavoriteOldShell;
	int 			m_nRegisterAsBrowser;
	int 			m_nMaxWindowCount;
	int 			m_nBackUpTime;
	int 			m_nAutoRefreshTime;
	int 			m_nAutoRefTimeMin;
	int 			m_nAutoRefTimeSec; // UDT DGSTR ( dai
	int 			m_nLoadGlobalOffline;
	int 			m_nKillDialog;
	int 			m_nInheritOptions;
	//+++ int		m_nNoCloseNL;			//+++ ナビロック時閉じれなくするフラグ

	CWindow 		m_wnd;
	MTL::CLogFont	m_lf;

public:
	// DDX map
	BEGIN_DDX_MAP( CMainPropertyPage )
		DDX_CHECK( IDC_CHECK_MAIN_NEWWINDOW 			, m_nNewWindow			)
		DDX_CHECK( IDC_CHECK_MAIN_NOACTIVATE			, m_nNoActivate 		)
		DDX_CHECK( IDC_CHECK_MAIN_NOACTIVATE_NEWWIN 	, m_nNoActivateNewWin	)
		DDX_CHECK( IDC_CHECK_MAIN_NOMDI 				, m_nNoMDI				)
		DDX_CHECK( IDC_CHECK_MAIN_INHERIT_OPTIONS		, m_nInheritOptions 	)
		DDX_CHECK( IDC_CHECK_ONEINSTANCE				, m_nOneInstance		)
		DDX_CHECK( IDC_CHECK_MAIN_LIMIT 				, m_nLimit				)
		DDX_CHECK( IDC_CHECK_MAIN_BACKUP				, m_nBackUp 			)
		DDX_CHECK( IDC_CHECK_MAIN_NOCLOSEDFG			, m_nNoCloseDFG 		)
		DDX_CHECK( IDC_CHECK_MAIN_ADDFAVORITEOLDSHELL	, m_nAddFavoriteOldShell)
		DDX_CHECK( IDC_CHECK_MAIN_ORGFAVORITEOLDSHELL	, m_nOrgFavoriteOldShell)
		DDX_CHECK( IDC_CHECK_MAIN_LOADGLOBALOFFLINE 	, m_nLoadGlobalOffline	)
		DDX_CHECK( IDC_CHECK_MAIN_KILLDIALOG			, m_nKillDialog 		)
		DDX_CHECK( IDC_CHECK_MAIN_REGISTER_AS_BROWSER	, m_nRegisterAsBrowser	)

		//+++ DDX_CHECK( IDC_CHECK_NOCLOSE_NAVILOCK 	, m_nNoCloseNL			)

		DDX_INT_RANGE( IDC_EDIT_MAIN_LIMITEDCOUNT		, m_nMaxWindowCount, 0, 255 )
		DDX_INT_RANGE( IDC_EDIT_MAIN_BACKUPTIME 		, m_nBackUpTime    , 1, 120 )

		// UDT DGSTR ( dai
		DDX_INT_RANGE( IDC_EDIT_MAIN_AUTOREFRESHTIME_MIN, m_nAutoRefTimeMin, 0,  59 )
		DDX_INT_RANGE( IDC_EDIT_MAIN_AUTOREFRESHTIME_SEC, m_nAutoRefTimeSec, 0,  59 )
		// ENDE
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CMainPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_BAR_FONT, OnFont )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMainPropertyPage> )
	END_MSG_MAP()

private:
	void OnFont(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl);

public:
	// Constructor
	CMainPropertyPage(HWND hWnd);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// Implementation
private:
	void	_GetData();
	void	_SetData();
};



class CMainPropertyPage2
	: public CInitPropertyPageImpl<CMainPropertyPage2>
	//, public CPropertyPageImpl<CMainPropertyPage2>
	, public CWinDataExchange<CMainPropertyPage2>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MAIN2 };

private:
	CWindow 	m_wnd;

	// UH
	WORD		m_nSzPain1;
	WORD		m_nSzPain2;
	int 		m_nChkSwapPain;

	int 		m_nShowMenu;
	int 		m_nShowToolBar;
	int 		m_nShowAdress;
	int 		m_nShowTab;
	int 		m_nShowLink;
	int 		m_nShowSearch;
	int 		m_nShowStatus;
	int 		m_nTravelLogGroup;
	int 		m_nTravelLogClose;

	int 		m_nMRUMenuType;
	int 		m_nMRUCount;
	int 		m_nMRUCountMin;
	int 		m_nMRUCountMax;

	int			m_nMinBtn2Tray;			//+++
  #ifndef USE_DIET
	int			m_nImgAutoResize;		//+++
  #endif
  #if 0	//+++ 失敗
	int			m_nTitleBarStrSwap;		//+++
  #endif

	BOOL		m_bInit;

public:
	// DDX map
	BEGIN_DDX_MAP( CMainPropertyPage2 )
		DDX_UINT ( IDC_EDIT_SZ_PAIN1		, m_nSzPain1	)
		DDX_UINT ( IDC_EDIT_SZ_PAIN2		, m_nSzPain2	)
		DDX_CHECK( IDC_CHK_SWAP_PAIN		, m_nChkSwapPain)

		DDX_CHECK( IDC_CHK_MENU 			, m_nShowMenu	)
		DDX_CHECK( IDC_CHK_TOOLBAR			, m_nShowToolBar)
		DDX_CHECK( IDC_CHK_ADRESS			, m_nShowAdress )
		DDX_CHECK( IDC_CHK_TAB				, m_nShowTab	)
		DDX_CHECK( IDC_CHK_LINK 			, m_nShowLink	)
		DDX_CHECK( IDC_CHK_SEARCH			, m_nShowSearch )
		DDX_CHECK( IDC_CHK_STATUS			, m_nShowStatus )
		//DDX_CHECK( IDC_CHK_MINBTN2TRAY	, m_nMinBtn2Tray )			//+++
		DDX_RADIO( IDC_RADIO_NO2TRAY		, m_nMinBtn2Tray )			//+++
	  #ifndef USE_DIET
		DDX_RADIO( IDC_RADIO_IMG_AUTO_RESIZE_NONE, m_nImgAutoResize )	//+++
	  #endif

	  #if 0	//+++ 失敗
		DDX_CHECK( IDC_CHK_TITLEBAR_STR_SWAP, m_nTitleBarStrSwap )	//+++
	  #endif
		DDX_CHECK( IDC_TRAVELLOG_GROUP		, m_nTravelLogGroup )
		DDX_CHECK( IDC_TRAVELLOG_CLOSE		, m_nTravelLogClose )

		DDX_INT_RANGE( IDC_EDIT_MRUCOUNT, m_nMRUCount, m_nMRUCountMin, m_nMRUCountMax )

		DDX_CBINDEX( IDC_COMBO_MRU_MENUTYPE, m_nMRUMenuType )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CMainPropertyPage2 )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMainPropertyPage2> )
	END_MSG_MAP()

	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

private:
	CString 			BrowseForFolder(const CString& strTitle, const CString& strNowPath);

public:
	// Constructor
	CMainPropertyPage2(HWND hWnd);

	// Overrides
	BOOL				OnSetActive();
	BOOL				OnKillActive();
	BOOL				OnApply();


private:
	void				_GetData();
	void				_SetData();
	void				InitCtrls();
};
