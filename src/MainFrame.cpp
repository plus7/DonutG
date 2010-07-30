/**
 *	@file	MainFrame.cpp
 *	@brief	メインフレームの実装
 *	@note
 *		+++ mainfrm.h を ヘッダとcppに分割。また、クラス名に併せてファイル名もMainFrameに変更.
 */
#include "stdafx.h"

#include "MainFrame.h"

#include "DialogKiller.h"
#include "dialog/DebugWindow.h"
#include "PropertySheet.h"
#include "MenuEncode.h"
#include "RebarSupport.h"
#ifndef NO_STYLESHEET
#include "StyleSheetOption.h"
#endif
#include "ExStyle.h"
#include "MenuDropTargetWindow.h"
#include "ParseInternetShortcutFile.h"

#include "option/AddressBarPropertyPage.h"	//+++ AddressBar.hより分離
#include "option/SearchPropertyPage.h"		//+++ SearchBar.hより分離
#include "option/LinkBarPropertyPage.h" 	//+++
#include "option/UrlSecurityOption.h"		//+++

#include "nsICookiePermission.h"

#ifdef _DEBUG
	const bool _Donut_MainFrame_traceOn = false;
	#define dmfTRACE	if (_Donut_MainFrame_traceOn)  ATLTRACE
#else
	#define dmfTRACE
#endif


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




static const CLSID CLSID_IEnumPrivacyRecords = { 0x3050f844, 0x98b5, 0x11cf, { 0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b } };

const UINT		CMainFrame::STDBAR_ID[] 	= { ATL_IDW_COMMAND_BAR, ATL_IDW_TOOLBAR				, IDC_ADDRESSBAR, IDC_MDITAB , IDC_LINKBAR				   , IDC_SEARCHBAR					};
const UINT		CMainFrame::STDBAR_STYLE[]	= { RBBS_USECHEVRON    , RBBS_USECHEVRON | RBBS_BREAK	, RBBS_BREAK	, RBBS_BREAK , RBBS_USECHEVRON | RBBS_BREAK, RBBS_BREAK 					};
//const UINT	CMainFrame::STDBAR_STYLE[]	= { RBBS_USECHEVRON    , RBBS_USECHEVRON | RBBS_BREAK	, RBBS_BREAK	, RBBS_BREAK , RBBS_USECHEVRON | RBBS_BREAK, RBBS_USECHEVRON | RBBS_BREAK	};
const LPTSTR	CMainFrame::STDBAR_TEXT[]	= { _T("")/*NULL*/	   , _T("")/*NULL*/ 				,_T("アドレス") , NULL		 , _T("リンク") 			   , _T("検索") 					};	// memo. NULL だと一番左のボタンをシェブロンに含めることができない




// ===========================================================================

void CMainFrame::OnTimer(UINT_PTR nIDEvent, TIMERPROC dmy /*= 0*/)
{
	switch (nIDEvent) {
	case ENT_READ_ACCEL:		//+++ メモ:起動時のみの、アクセラキー読み込み(遅延のためタイマーを使う)
		if (m_hAccel) {
			CAccelerManager 	accelManager(m_hAccel);
			m_hAccel = accelManager.LoadAccelaratorState(m_hAccel);
			::KillTimer(m_hWnd, nIDEvent);
		}
		break;

	default:
		//+++ 自動セーブが設定されていた場合、default.dfgをオートバックアップ.
		if (m_nBackUpTimerID == nIDEvent && m_nBackUpTimerID) {
			_SaveGroupOption( CStartUpOption::GetDefaultDFGFilePath(), true );
		} else {
			SetMsgHandled(FALSE);
			return;
		}
		break;
	}
}



BOOL CMainFrame::OnIdle()
{
	// Note. under 0.01 sec (in dbg-mode on 330mhz cpu)
	CmdUIUpdateToolBars();
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_DEFAULT_PANE	);
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_SECURE_PANE	);
	CmdUIUpdateStatusBar	(m_hWndStatusBar, ID_PRIVACY_PANE	);
	CmdUIUpdateChildWindow	(m_hWndStatusBar, IDC_PROGRESS		);
	m_mcCmdBar.UpdateMDIMenuControl();
	_FocusChecker();

	if ( _check_flag(MAIN_EX_KILLDIALOG, CMainOption::s_dwMainExtendedStyle) )
		CDialogKiller2::KillDialog();

	return FALSE;
}


void CMainFrame::_OnIdleCritical()
{
	if ( MtlIsApplicationActive(m_hWnd) ) {
		CmdUIUpdateToolBars();
		CmdUIUpdateChildWindow(m_hWndStatusBar, IDC_PROGRESS);
	}

	_FocusChecker();

	if ( _check_flag(MAIN_EX_KILLDIALOG, CMainOption::s_dwMainExtendedStyle) )
		CDialogKiller2::KillDialog();
}


void CMainFrame::_FocusChecker()
{
	// Note. On idle time, give focus back to browser window...
	dmfTRACE( _T("_FocusChecker\n") );

	HWND hWndActive 				 = MDIGetActive();
	if (hWndActive == NULL)
		return;

	HWND hWndForeground 			 = ::GetForegroundWindow();
	bool bMysteriousWindowForeground = MtlIsKindOf( hWndForeground, _T("Internet Explorer_Hidden") );

	if ( bMysteriousWindowForeground && MtlIsWindowCurrentProcess(hWndForeground) ) {
		IfTrayRestoreWindow();							//+++ トレイ状態だったら復活.
		if (m_bOldMaximized == 0 && m_bFullScreen == 0) //+++
			ShowWindow_Restore(1);		//ShowWindow(SW_RESTORE);		//+++ サイズを戻す
		MtlSetForegroundWindow(m_hWnd);
	}

	if ( !MtlIsApplicationActive(m_hWnd) ) {
		dmfTRACE(_T(" application is not active\n"), hWndForeground);
		return;
	}

	HWND hWndFocus					 = ::GetFocus();
	if ( hWndFocus != NULL && !MtlIsFamily(m_hWnd, hWndFocus) ) {
		dmfTRACE(_T(" not family(focus=%x)\n"), hWndFocus);
		return;
	}

	if ( ::IsChild(m_hWndToolBar, hWndFocus) ) {
		dmfTRACE( _T(" on rebar\n") );
		return;
	}

	if ( ::IsChild(m_ExplorerBar, hWndFocus) ) {
		dmfTRACE( _T(" on explorer bar\n") );
		return;
	}

	if ( hWndFocus == NULL || m_hWnd == hWndFocus || hWndActive == hWndFocus
	   || MtlIsKindOf( hWndFocus, _T("Shell DocObject View") ) )
	{
		dmfTRACE( _T(" Go!!!!!\n") );
		MtlSendCommand(hWndActive, ID_VIEW_SETFOCUS);
	}
}



// ===========================================================================

// UDT DGSTR
LRESULT CMainFrame::UpdateTitleBar(LPCTSTR lpszStatusBar, DWORD /*dwReserved*/)
{
	if ((GetStyle() & WS_CAPTION) != 0 && m_bTray == 0) {	//+++ チェック追加:タイトルバーが表示されているとき
		if ( !::IsWindowVisible(m_hWndStatusBar) )			//+++ メモ:ステータスバーが表示されていなかったら
			UpdateTitleBarUpsideDown(lpszStatusBar);		//+++ メモ:タイトルバーに、ステータスバー文字列を出す...
	}
	return 0;
}
// ENDE


// UH -  minit
LRESULT CMainFrame::OnMenuGetFav()
{
	if ( !m_DropFavoriteMenu.m_menu.IsMenu() ) {
		CMenuHandle menu;
		menu.LoadMenu(IDR_DROPDOWN_FAV);
		m_DropFavoriteMenu.InstallExplorerMenu(menu);
		ATLTRACE("menu loaded\n");
	}

	return (LRESULT) m_DropFavoriteMenu.m_menu.m_hMenu;
}


LRESULT CMainFrame::OnMenuGetFavGroup()
{
	if ( !m_DropFavGroupMenu.m_menu.IsMenu() ) {
		CMenuHandle menu;
		menu.LoadMenu(IDR_DROPDOWN_FAVGROUP);
		m_DropFavGroupMenu.InstallExplorerMenu(menu);
	}

	return (LRESULT) m_DropFavGroupMenu.m_menu.m_hMenu;
}


LRESULT CMainFrame::OnMenuGetScript()
{
	if ( !m_DropScriptMenu.m_menu.IsMenu() ) {
		CMenuHandle menu;
		menu.LoadMenu(IDR_DROPDOWN_SCRIPT);
		m_DropScriptMenu.SetRootDirectoryPath( Misc::GetExeDirectory() + _T("Script") );
		m_DropScriptMenu.SetTargetWindow(m_hWnd);
		m_DropScriptMenu.ResetIDCount(TRUE);
		m_DropScriptMenu.InstallExplorerMenu(menu);
	}

	return (LRESULT) m_DropScriptMenu.m_menu.m_hMenu;
}


LRESULT CMainFrame::OnMenuGoBack(HMENU hMenu)
{
	HWND hMDIActive = MDIGetActive();

	return ::SendMessage(hMDIActive, WM_MENU_GOBACK, (WPARAM) (HMENU) hMenu, (LPARAM) 0);
}


LRESULT CMainFrame::OnMenuGoForward(HMENU hMenu)
{
	HWND hMDIActive = MDIGetActive();

	return ::SendMessage(hMDIActive, WM_MENU_GOFORWARD, (WPARAM) (HMENU) hMenu, (LPARAM) 0);
}


LRESULT CMainFrame::OnMenuRefreshFav(BOOL bInit)
{
	ATLTRACE("menu refreshed\n");

	if ( m_DropFavoriteMenu.m_menu.IsMenu() ) {
		if (bInit == FALSE) {
			ATLTRACE("menu terminated\n");
			m_DropFavoriteMenu.m_menu.DestroyMenu();
			m_bShow = FALSE;
		} else {
			ATLTRACE("menu initialized\n");
			m_bShow = TRUE;
		}
	}

	return S_OK;
}


LRESULT CMainFrame::OnMenuRefreshFavGroup(BOOL bInit)
{
	if ( m_DropFavGroupMenu.m_menu.IsMenu() ) {
		if (bInit == FALSE) {
			m_DropFavGroupMenu.m_menu.DestroyMenu();
			m_bShow = FALSE;
		} else {
			m_bShow = TRUE;
		}
	}

	return S_OK;
}


LRESULT CMainFrame::OnMenuRefreshScript(BOOL bInit)
{
	if ( m_DropScriptMenu.m_menu.IsMenu() ) {
		if (bInit == FALSE) {
			m_DropScriptMenu.m_menu.DestroyMenu();
			m_bShow = FALSE;
		} else {
			m_bShow = TRUE;
		}
	}

	return S_OK;
}


LRESULT CMainFrame::OnRestrictMessage(BOOL bOn)
{
	m_bShow = bOn;
	return S_OK;
}


LRESULT CMainFrame::OnMenuDrag(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return m_wndMenuDropTarget.OnMenuDrag(uMsg, wParam, lParam, bHandled);
}



LRESULT CMainFrame::OnMenuGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return m_wndMenuDropTarget.OnMenuGetObject(uMsg, wParam, lParam, bHandled);
}



//////////////////////////////////////////////////////////////////
// to avoid the flicker on resizing

LRESULT CMainFrame::OnMDIClientEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
  #if 0 //+++ 描画範囲の更新がちゃんと制御できていないので駄目
	BOOL	bMaximized = 0;
	HWND	hWndActive = MDIGetActive(&bMaximized);
	if (hWndActive != NULL && (CMainOption::s_bTabMode || bMaximized))
		return 1;
  #else
	HWND	hWndActive = MDIGetActive();
	if (hWndActive != NULL && CMainOption::s_bTabMode)
		return 1;
  #endif

  #if 1 //*+++ BG描画指定.
	if (drawMainFrameBg((HDC)wParam))
		return 1;
  #endif

	// no need to erase it
	bHandled = FALSE;
	return 0;
}


LRESULT CMainFrame::OnMDIClientSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PostMessage(WM_COMMAND, MAKEWPARAM(ID_RESIZED, 0), 0);

	BOOL	bMaximized = FALSE;
	HWND	hWndActive = MDIGetActive(&bMaximized);

	if (CMainOption::s_bTabMode || hWndActive == NULL || bMaximized == FALSE) { // pass to default
		bHandled = FALSE;
		return 1;
	}

	// NOTE. If you do the following, you can avoid the flicker on resizing.
	//		 I can't understand why it is effective...
	//
	//		 But still you can get a glimpse of other mdi child window's frames.
	//		 I guess it's MDI's bug, but I can't get the way MFC fixed.
	CWindow wndActive(hWndActive);
	CWindow wndView    = wndActive.GetDlgItem(ID_DONUTVIEW);
	wndActive.ModifyStyle(0, WS_CLIPCHILDREN);									// add WS_CLIPCHILDREN
	wndView.ModifyStyle(0, WS_CLIPCHILDREN);
	LRESULT lRet	   = m_wndMDIClient.DefWindowProc(uMsg, wParam, lParam);
	m_wndMDIClient.UpdateWindow();
	wndActive.ModifyStyle(WS_CLIPCHILDREN, 0);
	wndView.ModifyStyle(WS_CLIPCHILDREN, 0);

	bHandled = FALSE;
	return lRet;
}


//////////////////////////////////////////////////////////////////
// custom draw of addressbar
LRESULT CMainFrame::OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;

	LPNMHDR lpnmh = (LPNMHDR) lParam;
	LRESULT lRet  = 0;

	if (lpnmh->code == NM_CUSTOMDRAW && lpnmh->hwndFrom == m_hWndToolBar) { 	// from rebar
		lRet = m_AddressBar.OnCustomDraw(0, lpnmh, bHandled);
	}

	if (lpnmh->code == TBN_DROPDOWN) {
		int nCount = CPluginManager::GetCount(PLT_TOOLBAR);

		for (int nIndex = 0; nIndex < nCount; nIndex++) {
			HWND	hWndBar = CPluginManager::GetHWND(PLT_TOOLBAR, nIndex);

			if (lpnmh->hwndFrom != hWndBar)
				continue;

			CPluginManager::Call_ShowToolBarMenu(PLT_TOOLBAR, nIndex, ( (LPNMTOOLBAR) lpnmh )->iItem);

			bHandled = TRUE;
			break;
		}
	}

	return lRet;
}


//////////////////////////////////////////////////////////////////
// the custom message from MDI child
void CMainFrame::OnMDIChild(HWND hWnd, UINT nCode)
{
	SetMsgHandled(FALSE);

	if (nCode == MDICHILD_USER_ALLCLOSED) {
		m_AddressBar.SetWindowText( _T("") );
		::SetWindowText( m_hWndStatusBar, _T("レディ") );
		m_CmdBar.EnableButton(_nPosWindowMenu, false);
	} else if (nCode == MDICHILD_USER_FIRSTCREATED) {
		m_CmdBar.EnableButton(_nPosWindowMenu, true);
	} else if (nCode == MDICHILD_USER_ACTIVATED) {
		_OnMDIActivate(hWnd);
		OnIdle();						// make sure when cpu is busy
	} else if (nCode == MDICHILD_USER_TITLECHANGED) {
		//OnTitleChanged();
	}
}


BOOL CMainFrame::OnBrowserCanSetFocus()
{										// asked by browser
	HWND hWndFocus = ::GetFocus();

	if (hWndFocus == NULL)
		return TRUE;

	if ( ::IsChild(m_hWndToolBar, hWndFocus) )
		return FALSE;

	if ( m_ExplorerBar.IsChild(hWndFocus) )
		return FALSE;

	return TRUE;
}


void CMainFrame::OnParentNotify(UINT fwEvent, UINT idChild, LPARAM lParam)
{
	if (fwEvent != WM_RBUTTONDOWN)
		return;

	CPoint		pt( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
	ClientToScreen(&pt);

	CRect		rc;
	::GetClientRect(m_hWndToolBar, &rc);
	CPoint		ptRebar  = pt;
	::ScreenToClient(m_hWndToolBar, &ptRebar);

	if ( !rc.PtInRect(ptRebar) )		// not on rebar
		return;

	HWND		hWnd	 = ::WindowFromPoint(pt);

	if (hWnd == m_MDITab.m_hWnd) {		// on tab bar
		CPoint ptTab = pt;
		m_MDITab.ScreenToClient(&ptTab);

		if (m_MDITab.HitTest(ptTab) != -1)
			return;
	}

	CMenuHandle menuView = ::GetSubMenu(m_CmdBar.GetMenu(), _nPosViewMenu);
	CMenuHandle menu	 = menuView.GetSubMenu(0);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd, NULL);
}


void CMainFrame::OnActivate(UINT nState, BOOL bMinimized, HWND hWndOther)
{
	if (nState == WA_INACTIVE) {
		m_hWndFocus = ::GetFocus();
	} else {
		if (m_hWndFocus)
			::SetFocus(m_hWndFocus);
	}
}


//minit
BOOL CMainFrame::_IsRebarBandLocked()
{
	CReBarCtrl	  rebar(m_hWndToolBar);
	REBARBANDINFO rbbi = { 0 };

	rbbi.cbSize = sizeof (REBARBANDINFO);
	rbbi.fMask	= RBBIM_STYLE;

	if ( !rebar.GetBandInfo(0, &rbbi) )
		return FALSE;

	return (rbbi.fStyle & RBBS_NOGRIPPER) != 0;
}


LRESULT CMainFrame::OnViewToolBarLock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CReBarSupport RebarSup(m_hWndToolBar);

	RebarSup.SetBandsLock( !_IsRebarBandLocked() );

	return 0;
}


void CMainFrame::OnExplorerBarAutoShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW)
		CMainOption::s_dwExplorerBarStyle &= ~MAIN_EXPLORER_AUTOSHOW;
	else
		CMainOption::s_dwExplorerBarStyle |= MAIN_EXPLORER_AUTOSHOW;
}


LRESULT CMainFrame::OnViewRefreshFavBar(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_FavoriteMenu.RefreshMenu();
	m_FavGroupMenu.RefreshMenu();
   #ifndef NO_STYLESHEET
	m_styleSheetMenu.RefreshMenu();
   #endif
	return 0;
}


//LRESULT CMainFrame::OnViewIdle(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
//{
//	_OnIdleCritical();
//	return 0;
//}


////////////////////////////////////////////////////////////////////////////////
//ファイルメニュー
//　コマンドハンドラ
////////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnViewHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	bool bNew = ::GetAsyncKeyState(VK_CONTROL) < 0 || ::GetAsyncKeyState(VK_SHIFT) < 0;

	if (bNew || MDIGetActive() == NULL) {
		OnFileNewHome(0, 0, 0);
	} else {
		SetMsgHandled(FALSE);
	}
}


void CMainFrame::OnResized(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_wndStatusBar.m_hWnd == NULL)
		return;

	{
		RECT r;
		m_wndAdditionalSplit.GetClientRect(&r);
		m_wndAdditionalSplit.SetSplitterPos(r.bottom - r.top - 25);
		//m_wndAdditionalSplit.UpdateSplitterLayout();
	}

	//+++ メモ：ステータスバーのプロクシ・コンボボックスのレサイズ
	{
		CRect	rcComboPart;
		if (g_bSwapProxy == false)
			m_wndStatusBar.GetRect(4, rcComboPart);
		else
			m_wndStatusBar.GetRect(1, rcComboPart);

		rcComboPart.top   -= 1;
		rcComboPart.bottom = rcComboPart.top + rcComboPart.Height() * 10;
		m_cmbBox.MoveWindow(rcComboPart, TRUE);
	}

	{
		//プラグインイベント - リサイズ
		CPluginManager::BroadCast_PluginEvent(DEVT_CHANGESIZE, 0, 0);
	}
}


// U.H
void CMainFrame::ShowLinkText(BOOL bShow)
{
	REBARBANDINFO rbBand;

	memset( &rbBand, 0, sizeof (REBARBANDINFO) );
	rbBand.cbSize = sizeof (REBARBANDINFO);

	int 		  nCount = int ( ::SendMessage(m_hWndToolBar, RB_GETBANDCOUNT, 0, 0) );

	for (int ii = 0; ii < nCount; ii++) {
		rbBand.fMask = RBBIM_ID;
		::SendMessage(m_hWndToolBar, RB_GETBANDINFO, (WPARAM) ii, (LPARAM) (LPREBARBANDINFO) &rbBand);

		if (rbBand.wID != IDC_LINKBAR && rbBand.wID != IDC_SEARCHBAR && rbBand.wID != IDC_ADDRESSBAR)
			continue;

		// UDT JOBBY

		rbBand.fMask = RBBIM_TEXT;

		if (bShow) {
			// UDT JOBBY
			if (rbBand.wID == IDC_ADDRESSBAR)
				rbBand.lpText = _T("アドレス");
			else if (rbBand.wID == IDC_LINKBAR)
				rbBand.lpText = _T("リンク");
			else
				rbBand.lpText = _T("検索");

			// ENDE
		}

		::SendMessage(m_hWndToolBar, RB_SETBANDINFO, (WPARAM) ii, (LPARAM) (LPREBARBANDINFO) &rbBand);
	}
}
// END



// U.H
void CMainFrame::OnFileNewSelectText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	SendMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_COPY, 0), 0);
	SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_CLIPBOARD, 0), 0);
}


// U.H
void CMainFrame::OnShowTextChg(BOOL bShow)
{
	ShowLinkText(bShow);
	m_AddressBar.ShowAddresText(m_hWndToolBar, bShow);
}


// U.H
LRESULT CMainFrame::OnSysCommand(UINT nID, CPoint point)
{
	switch (nID) {
	case ID_VIEW_COMMANDBAR:
		SendMessage(m_hWnd, WM_COMMAND, ID_VIEW_COMMANDBAR, 0);
		SetMsgHandled(TRUE);
		break;

  #if 1	//+++ 最小化ボタンを押した時に、タスクトレイに入るようにしてみる.
	case SC_MINIMIZE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY)	//+++ 最小化ボタンでタスクトレイに入れる設定のとき、
			&& (point.x || point.y)											//+++ x,yが0,0ならタスクバーでクリックした場合だろうで、トレイにいれず、最小化だけしてみる.
		) {
			OnGetOut(0,0,0);
			SetMsgHandled(TRUE);
			break;
		}
		SetMsgHandled(FALSE);
		break;

	case SC_CLOSE:
		if ((CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_CLOSEBTN2TRAY)	//+++ 最小化ボタンでタスクトレイに入れる設定のとき、
			&& (point.x || point.y)											//+++ x,yが0,0ならタスクバーでクリックした場合だろうで、トレイにいれず、最小化だけしてみる.
		) {
			OnGetOut(0,0,0);
			SetMsgHandled(TRUE);
			break;
		}
		SetMsgHandled(FALSE);
		break;
  #endif

	default:
		SetMsgHandled(FALSE);
		break;
	}

	return 0;
}


LRESULT CMainFrame::OnPopupClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CIgnoredURLsOption::s_bValid = !CIgnoredURLsOption::s_bValid;
	return 0;
}


LRESULT CMainFrame::OnTitleClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CCloseTitlesOption::s_bValid = !CCloseTitlesOption::s_bValid;
	return 0;
}


LRESULT CMainFrame::OnDoubleClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CIgnoredURLsOption::s_bValid = !CIgnoredURLsOption::s_bValid;
	CCloseTitlesOption::s_bValid = !CCloseTitlesOption::s_bValid;
	return 0;
}


LRESULT CMainFrame::OnPrivacyReport(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	//TODO: どうやって実装する？そもそも必要？
	return 0;
}

LRESULT CMainFrame::OnGlobalCookies(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	switch (wID) {
	case ID_COOKIES_ALL:        SetGlobalCookiePref(0); break;
	case ID_COOKIES_SAMEORIGIN:	SetGlobalCookiePref(1); break;
	case ID_COOKIES_BLOCK:      SetGlobalCookiePref(2); break;
	case ID_COOKIES_CUSTOM:		SetGlobalCookiePref(3); break;
	}
	return 0;
}

LRESULT CMainFrame::OnSiteCookies(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CGeckoBrowser browser = DonutGetNsIWebBrowser(DonutGetActiveWindow(m_hWnd));

	if (browser.m_spBrowser == NULL) return 0;

	CString strURI	 = browser.GetLocationURL();

	switch (wID) {
	case ID_SITECOOKIES_BLOCK:      SetSiteCookiePref(strURI, nsICookiePermission::ACCESS_DENY);    break;
	case ID_SITECOOKIES_DEFAULT:    SetSiteCookiePref(strURI, nsICookiePermission::ACCESS_DEFAULT); break;
	case ID_SITECOOKIES_SESSION:    SetSiteCookiePref(strURI, nsICookiePermission::ACCESS_SESSION); break;
	case ID_SITECOOKIES_ALL:        SetSiteCookiePref(strURI, nsICookiePermission::ACCESS_ALLOW);   break;
	}
	return 0;
}

LRESULT CMainFrame::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if ( !CDonutConfirmOption::OnCloseAll(m_hWnd) )
		return 0;

	CWaitCursor 		 cur;
	CLockRedrawMDIClient lock(m_hWndMDIClient);
	CLockRedraw 		 lock2(m_MDITab);
	MtlCloseAllMDIChildren(m_hWndMDIClient);

	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	return 0;
}



LRESULT CMainFrame::OnWindowCloseCmp(int nTarIndex, BOOL bLeft)
{
	HWND			   hWndActive = MDIGetActive();

	if (hWndActive == NULL)
		return 0;

	CSimpleArray<HWND> arrWnd;
	int 			   ii;

	for (ii = 0; ii < m_MDITab.GetItemCount(); ii++) {
		HWND hWnd = m_MDITab.GetTabHwnd(ii);

		if (bLeft && ii < nTarIndex)
			arrWnd.Add(hWnd);
		else if (bLeft == FALSE && ii > nTarIndex)
			arrWnd.Add(hWnd);
	}

	for (ii = 0; ii < arrWnd.GetSize(); ii++) {
		SendMessage(arrWnd[ii], WM_CLOSE, 0, 0);
	}

	return 1;
}


// U.H
LRESULT CMainFrame::OnInitMenuPopup(HMENU hMenuPopup, UINT uPos, BOOL bSystemMenu)
{
	// システムメニューは、処理しない
	if (bSystemMenu)
		return 0;

	if (m_FavoriteMenu.m_menu == hMenuPopup)
		return 0;

	if (m_FavGroupMenu.m_menu == hMenuPopup)
		return 0;

   #ifndef NO_STYLESHEET
	if (m_styleSheetMenu.m_menu == hMenuPopup)
		return 0;
   #endif

	CMenuHandle 	menu = hMenuPopup;
	CAccelerManager accel(m_hAccel);

	// 一番目のメニューが得られない時はそれを消して編集しない
	CString 		strCmd;

	if (menu.GetMenuString(0, strCmd, MF_BYPOSITION) == 0) {
		menu.RemoveMenu(0, MF_BYPOSITION);
		return 0;
	}

	if (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCSTMMENU)
		return 0;

	for (int ii = 0; ii < menu.GetMenuItemCount(); ii++) {
		strCmd = _T("");
		UINT	nID    = menu.GetMenuItemID(ii);
		menu.GetMenuString(nID, strCmd, MF_BYCOMMAND);

		if ( strCmd.IsEmpty() ) {
			// NT4用かな
			if (strCmd.LoadString(nID) == FALSE)
				continue;
		}

		CString strShorCut;

		if ( !accel.FindAccelerator(nID, strShorCut) )	//逆になってたので修正 minit
			continue;

		if (strCmd.Find(_T("\t")) != -1) {
			strCmd = strCmd.Left( strCmd.Find(_T("\t")) );
		}

		if (strShorCut.IsEmpty() == FALSE)
			strCmd = strCmd + _T("\t") + strShorCut;

		UINT	uState = menu.GetMenuState(nID, MF_BYCOMMAND);
		menu.ModifyMenu(nID, MF_BYCOMMAND, nID, strCmd);

		if (uState & MF_CHECKED)
			menu.CheckMenuItem(nID, MF_CHECKED);

		if (uState & MF_GRAYED)
			menu.EnableMenuItem(nID, MF_GRAYED);
	}

	return 0;
}


void CMainFrame::OnRegisterAsBrowser(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (wNotifyCode == NM_ON) {
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_ON) );
	} else if (wNotifyCode == NM_OFF) {
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_OFF) );
	} else if ( !_check_flag(MAIN_EX_REGISTER_AS_BROWSER, CMainOption::s_dwMainExtendedStyle) ) {
		CMainOption::s_dwMainExtendedStyle |= MAIN_EX_REGISTER_AS_BROWSER;
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_ON) );
	} else {
		CMainOption::s_dwMainExtendedStyle &= ~MAIN_EX_REGISTER_AS_BROWSER;
		MtlForEachMDIChild( m_hWndMDIClient, CSendCommand(ID_REGISTER_AS_BROWSER, NM_OFF) );
	}
}


void CMainFrame::OnFileWorkOffline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	MtlSetGlobalOffline( !MtlIsGlobalOffline() );
	UpdateTitleBarUpsideDown();
}


LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}


void CMainFrame::OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.Cut();
	} else if (::GetFocus() == editSearch) {
		editSearch.Cut();
	} else {
		SetMsgHandled(FALSE);							// MtlWeb.hのCWebBrowserCommandHandlerが処理
		return;
	}
}


void CMainFrame::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.Copy();
	} else if (::GetFocus() == editSearch) {
		editSearch.Copy();
	} else {
		SetMsgHandled(FALSE);							// MtlWeb.hのCWebBrowserCommandHandlerが処理
		return;
	}
}


void CMainFrame::OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.Paste();
	} else if (::GetFocus() == editSearch) {
		editSearch.Paste();
	} else {
		SetMsgHandled(FALSE);
		return;
	}
}


void CMainFrame::OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CEdit editAddress = m_AddressBar.GetEditCtrl();
	CEdit editSearch  = m_SearchBar.GetEditCtrl();

	if (::GetFocus() == editAddress) {
		editAddress.SetSelAll();
	} else if (::GetFocus() == editSearch) {
		editSearch.SetSelAll();
	} else {
		SetMsgHandled(FALSE);							// MtlWeb.hのCWebBrowserCommandHandlerが処理
		return;
	}
}


// U.H
LRESULT CMainFrame::OnViewCommandBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, ATL_IDW_COMMAND_BAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	// It sucks, band index is dynamic.
	// ::SendMessage(m_hWndToolBar, RB_SHOWBAND, 0, bNew);	// toolbar is band #0
	MtlToggleBandVisible(m_hWndToolBar, ATL_IDW_TOOLBAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewAddressBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_ADDRESSBAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewLinkBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_LINKBAR);
	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewTabBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_MDITAB);
	UpdateLayout();
	return 0;
}



LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	BOOL bNew = !::IsWindowVisible(m_hWndStatusBar);

	UpdateTitleBar(_T(""), 0);		//+++		statusオフからオンにしたときに、タイトルバーに出してしたステータス文字列を消すため.

	m_bStatusBarVisibleUnlessFullScreen = bNew; // save
	::ShowWindow(m_hWndStatusBar, bNew ? SW_SHOWNOACTIVATE : SW_HIDE);

	UpdateLayout();
	return 0;
}


LRESULT CMainFrame::OnViewToolBarCust(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_ToolBar.StdToolBar_Customize();
	return 0;
}


void CMainFrame::OnFavoriteAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	::SendMessage(MDIGetActive(), WM_COMMAND, (WPARAM) ID_FAVORITE_ADD, 0);
}


void CMainFrame::OnFavoriteOrganize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	bool bOldShell = _check_flag(MAIN_EX_ORGFAVORITEOLDSHELL, CMainOption::s_dwMainExtendedStyle);

	//		CString strPath = DonutGetFavoritesFolder();
	//		MtlOrganizeFavorite(m_hWnd, bOldShell, strPath);
	MtlOrganizeFavorite( m_hWnd, bOldShell, DonutGetFavoritesFolder() );
}


//public:
///+++	オートバックアップの開始設定.
//void CMainFrame::SetAutoBackUp() {
//	OnBackUpOptionChanged(0,0,0);
//}


//private:
void CMainFrame::OnViewOptionDonut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	ShowNewStyleDonutOption();
	return;
}


void CMainFrame::ShowNewStyleDonutOption()
{
	BOOL							bSkinChange = FALSE;

	CMenu							menu		= CExMenuManager::LoadFullMenu();

	CMainPropertyPage				pageMain(m_hWnd);
	CMainPropertyPage2				pageMain2(m_hWnd);
	CDLControlPropertyPage			pageDLC(m_hWnd);
	CMDITabPropertyPage 			pageTab(&m_MDITab, menu.m_hMenu);
	CDonutAddressBarPropertyPage	pageAddress(m_AddressBar, m_SearchBar);
	CDonutFavoritesMenuPropertyPage pageFav;
	CFileNewPropertyPage			pageFileNew;
	CStartUpPropertyPage			pageStartUp;
	CProxyPropertyPage				pageProxy;
	CKeyBoardPropertyPage			pageKeyBoard(m_hAccel, menu.m_hMenu);
	CToolBarPropertyPage			pageToolBar(menu.m_hMenu, &m_ToolBar.m_arrStdBtn, &bSkinChange);
	CMousePropertyPage				pageMouse(menu.m_hMenu);
	CMouseGesturePropertyPage		pageGesture(menu.m_hMenu);
	CSearchPropertyPage 			pageSearch;
	CMenuPropertyPage				pageMenu(menu.m_hMenu, m_CmdBar);
	CExplorerPropertyPage			pageExplorer;
	CDestroyPropertyPage			pageDestroy;
	CSkinPropertyPage				pageSkin(m_hWnd, &bSkinChange);
	CLinkBarPropertyPage			pageLinks(m_LinkBar);

	CString 						strURL, strTitle;
	HWND							hWndActive	= MDIGetActive();

	if (hWndActive) {
		CGeckoBrowser browser = DonutGetNsIWebBrowser(hWndActive);

		if (browser.m_spBrowser != NULL) {
			strURL	 = browser.GetLocationURL();
			strTitle = MtlGetWindowText(hWndActive); // pChild->get_LocationName();
		}
	}

	CIgnoredURLsPropertyPage		pageURLs(strURL);
	CCloseTitlesPropertyPage		pageTitles( strTitle );
	CUrlSecurityPropertyPage		pageUrlSecu(strURL);		//+++
	CDonutExecutablePropertyPage	pageExe;
	CDonutConfirmPropertyPage		pageCnf;
	CPluginPropertyPage 			pagePlugin;

	CTreeViewPropertySheet			sheet( _T("Donutのオプション") );

	sheet.AddPage( pageMain 		 );
	sheet.AddPage( pageMain2	 , 1 );
	sheet.AddPage( pageToolBar		 );
	sheet.AddPage( pageTab		 , 1 );
	sheet.AddPage( pageAddress	 , 0 );
	sheet.AddPage( pageSearch	 , 0 );
	sheet.AddPage( pageLinks	 , 0 );
	sheet.AddPage( pageExplorer 	 );
	sheet.AddPage( pageMenu 		 );
	sheet.AddPage( pageFav		 , 1 );
	sheet.AddPage( pageKeyBoard 	 );
	sheet.AddPage( pageMouse		 );
	sheet.AddPage( pageGesture	 , 1 );
	sheet.AddPage( pageFileNew		 );
	sheet.AddPage( pageStartUp		 );
	sheet.AddPage( pageDestroy		 );
	sheet.AddPage( pageDLC			 );
	sheet.AddPage( pageURLs 	 , 1 );
	sheet.AddPage( pageTitles	 , 0 );
	sheet.AddPage( pageUrlSecu 	 , 0 );				//+++
	//sheet.AddPage( pageDeterrent	 );

	sheet.AddPage( pageExe			 );
	sheet.AddPage( pageCnf			 );
	sheet.AddPage( pageProxy		 );
	sheet.AddPage( pageSkin 		 );
	sheet.AddPage( pagePlugin		 );

	sheet.DoModal();

	m_cmbBox.ResetProxyList();

	// キーの呼出
	CAccelerManager accelManager(m_hAccel);
	m_hAccel = accelManager.LoadAccelaratorState(m_hAccel);

  #if 0 //+++	"タブモードで起動する"のチェックを反映... やっぱりしないでおく.
	m_mcCmdBar.setOnlyCloseButton(CMainOption::s_bTabMode);
  #endif
	RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
}


void CMainFrame::OnViewOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (MDIGetActive() == NULL)
		MtlShowInternetOptions();
}


void CMainFrame::OnViewRefreshAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
	MtlForEachMDIChild( m_hWndMDIClient, CPostCommand(ID_VIEW_REFRESH) );
}


void CMainFrame::OnViewStopAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	MtlForEachMDIChild( m_hWndMDIClient, CPostCommand(ID_VIEW_STOP) );
	RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
}


// Implementation

//private:


//public:
BOOL CMainFrame::AddSimpleReBarBandCtrl(
	HWND	hWndReBar,
	HWND	hWndBand,
	int 	nID,
	LPTSTR	lpstrTitle,
	UINT	fStyle,
	int 	cxWidth,
	BOOL	bFullWidthAlways,
	HBITMAP hBitmap /* = NULL*/)
{
	dmfTRACE( _T("CMainFrame::AddSimpleReBarBandCtrl\n") );
	ATLASSERT( ::IsWindow(hWndReBar) ); 	// must be already created
	ATLASSERT( ::IsWindow(hWndBand) );		// must be already created
	MTLASSERT_KINDOF(REBARCLASSNAME, hWndReBar);

	// Set band info structure
	REBARBANDINFO rbBand;
	rbBand.cbSize	 = sizeof (REBARBANDINFO);
  #if (_WIN32_IE >= 0x0400)
	rbBand.fMask	 = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE | RBBIM_IDEALSIZE;
  #else
	rbBand.fMask	 = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE;
  #endif	//!(_WIN32_IE >= 0x0400)

	rbBand.fMask	|= RBBIM_BACKGROUND | RBBIM_TEXT;
	rbBand.fStyle	 = fStyle;
	rbBand.lpText	 = lpstrTitle;
	rbBand.hwndChild = hWndBand;
	rbBand.wID		 = nID;

	rbBand.hbmBack	 = hBitmap;
	rbBand.fStyle	|= RBBS_FIXEDBMP;

	// Calculate the size of the band
	BOOL		  bRet;
	RECT		  rcTmp;
	int 		  nBtnCount = (int) ::SendMessage(hWndBand, TB_BUTTONCOUNT, 0, 0L);

	if (nBtnCount > 0) {
		bRet				= ::SendMessage(hWndBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM) &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cx			= (cxWidth != 0) ? cxWidth : rcTmp.right;
		rbBand.cyMinChild	= rcTmp.bottom - rcTmp.top;

		if (bFullWidthAlways) {
			rbBand.cxMinChild = rbBand.cx;
		} else if (lpstrTitle == 0) {
			CRect rcTmp;					// check!!
			bRet			  = ::SendMessage(hWndBand, TB_GETITEMRECT, 0, (LPARAM) &rcTmp) != 0;
			ATLASSERT(bRet);
			rbBand.cxMinChild = rcTmp.right;
		} else {
			rbBand.cxMinChild = 0;
		}
	} else {								// no buttons, either not a toolbar or really has no buttons
		bRet				= ::GetWindowRect(hWndBand, &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cx			= (cxWidth != 0) ? cxWidth : (rcTmp.right - rcTmp.left);
		rbBand.cxMinChild	= (bFullWidthAlways) ? rbBand.cx : 0;
		rbBand.cyMinChild	= rcTmp.bottom - rcTmp.top;
	}

  #if (_WIN32_IE >= 0x0400)
	// NOTE: cxIdeal used for CHEVRON, if MDI cxIdeal changed dynamically.
	rbBand.cxIdeal = rcTmp.right;			//rbBand.cx is not good.
  #endif		//(_WIN32_IE >= 0x0400)

	// Add the band
	LRESULT 	  lRes = ::SendMessage(hWndReBar, RB_INSERTBAND, (WPARAM) -1, (LPARAM) &rbBand);

	if (lRes == 0) {
		ATLTRACE2( atlTraceUI, 0, _T("Failed to add a band to the rebar.\n") );
		return FALSE;
	}

  #if (_WIN32_IE >= 0x0501)
	if (nID == IDC_LINKBAR)
		::SendMessage(hWndBand, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_HIDECLIPPEDBUTTONS);
  #endif		//(_WIN32_IE >= 0x0501)

	return TRUE;
}


// UPDATE_COMMAND_UI_MAP()






// UDT DGSTR
bool CMainFrame::_IsExistHTMLHelp(void)
{
	return ( 0xFFFFFFFF != GetFileAttributes( MtlGetHTMLHelpPath() ) );
}
// ENDE



bool CMainFrame::ChkGlobalCookies(UINT nID)
{
	bool bSts = false;
	switch (nID) {
        case ID_COOKIES_ALL:
		if (GetGlobalCookiePref() == 0)
			bSts = true;
		break;

		case ID_COOKIES_SAMEORIGIN:
		if (GetGlobalCookiePref() == 1)
			bSts = true;
		break;

		case ID_COOKIES_BLOCK:

		if (GetGlobalCookiePref() == 2)
			bSts = true;
		break;

		case ID_COOKIES_CUSTOM:
		if (GetGlobalCookiePref() == 3)
			bSts = true;
		break;
	}           
	return bSts;
}

bool CMainFrame::ChkSiteCookies(UINT nID)
{
	CGeckoBrowser browser = DonutGetNsIWebBrowser(DonutGetActiveWindow(m_hWnd));

	if (browser.m_spBrowser == NULL) return false;

	CString strURI	 = browser.GetLocationURL();

	bool bSts = false;
	switch (nID) {
        case ID_SITECOOKIES_BLOCK:
		if (GetSiteCookiePref(strURI) == nsICookiePermission::ACCESS_DENY)
			bSts = true;
		break;
        case ID_SITECOOKIES_DEFAULT:
		if (GetSiteCookiePref(strURI) == nsICookiePermission::ACCESS_DEFAULT)
			bSts = true;
		break;
        case ID_SITECOOKIES_SESSION:
		if (GetSiteCookiePref(strURI) == nsICookiePermission::ACCESS_SESSION)
			bSts = true;
		break;
        case ID_SITECOOKIES_ALL:
		if (GetSiteCookiePref(strURI) == nsICookiePermission::ACCESS_ALLOW)
			bSts = true;
		break;
	}           
	return bSts;
}

void CMainFrame::OnUpdateWindowArrange(CCmdUI *pCmdUI)
{
	// toolbar has not this command button, called only on menu popuping
	MDIRefreshMenu();
	MtlCompactMDIWindowMenuDocumentList( m_menuWindow, _nWindowMenuItemCount,
										CFavoritesMenuOption::GetMaxMenuItemTextLength() );

	pCmdUI->Enable(MDIGetActive() != NULL && !CMainOption::s_bTabMode);
}


void CMainFrame::OnUpdateProgressUI(CCmdUI *pCmdUI)
{
	CProgressBarCtrl progressbar = pCmdUI->m_wndOther;

	progressbar.ShowWindow(SW_HIDE);
}


void CMainFrame::OnUpdateStautsIcon(CCmdUI *pCmdUI)
{
	pCmdUI->m_wndOther.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(pCmdUI->m_nID, -1), 0);
}


bool CMainFrame::_IsClipboardAvailable()
{
	return ::IsClipboardFormatAvailable(MTL_CF_TEXT) == TRUE;		//+++ UNICODE対応(MTL_CF_TEXT)
}


//minit
LRESULT CMainFrame::OnSpecialKeys(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	int nCode = 0;
	switch (wID) {
	case ID_SPECIAL_HOME:		nCode = VK_HOME;	break;
	case ID_SPECIAL_END:		nCode = VK_END; 	break;
	case ID_SPECIAL_PAGEUP: 	nCode = VK_PRIOR;	break;
	case ID_SPECIAL_PAGEDOWN:	nCode = VK_NEXT;	break;
	case ID_SPECIAL_UP: 		nCode = VK_UP;		break;
	case ID_SPECIAL_DOWN:		nCode = VK_DOWN;	break;
	default:	ATLASSERT(FALSE);
	}
	PostMessage(WM_KEYDOWN, nCode, 0);
	return S_OK;
}


LRESULT CMainFrame::OnSecurityReport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	//TODO:
	return S_OK;
}


#ifdef USE_THUMBNAIL
//+++ ウィンドウ・サムネール表示(ページ選択)
LRESULT CMainFrame::OnWindowThumbnail(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
  #if 1	//+++ ページがなかったらかえる
	HWND	hWndActive = MDIGetActive();
	if (!hWndActive)
		return 0;
  #endif
  #if 1 //+++	元々コメントだったのを、ためし復活用に#if化.
	CThumbnailDlg	dlg;
	dlg.DoModal(m_hWnd, (LPARAM)m_hWndMDIClient);
  #endif
	return S_OK;
}
#endif


LRESULT CMainFrame::_OnMDIActivate(HWND hWndActive)
{

	//プラグインにタブが変更されたことを伝える
	int 		  nTabIndex;
	nsIWebBrowser *pWB;

	if (hWndActive == NULL) {
		nTabIndex = -1;
		pWB	  = NULL;
	} else {
		nTabIndex = m_MDITab.GetTabIndex(hWndActive);
		pWB	      = DonutGetNsIWebBrowser(hWndActive);
	}

	int 		  nCount = CPluginManager::GetCount(PLT_TOOLBAR);

	for (int i = 0; i < nCount; i++) {
		if ( CPluginManager::Call_Event_TabChanged(PLT_TOOLBAR, i, nTabIndex, NULL/*TODO:pWB2*/) )
			return TRUE;
	}

	return 0;
}


LRESULT CMainFrame::OnShowActiveMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND	hWndActive = MDIGetActive();
	if (!hWndActive)
		return 0;

	int  nIndex 	= m_MDITab.GetTabIndex(hWndActive);
	m_MDITab.ShowTabMenu(nIndex);

	return 0;
}


#ifndef NO_STYLESHEET
//public:
LRESULT CMainFrame::OnUseUserStyleSheet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	bool	bNowFlag = !CStyleSheetOption::GetUseUserSheet();

	CStyleSheetOption::SetUseUserSheet(bNowFlag);
	return 0;
}


//private:
LRESULT CMainFrame::OnSetUserStyleSheet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	bool	bNowFlag = !CStyleSheetOption::s_bSetUserSheet;

	CStyleSheetOption::s_bSetUserSheet = bNowFlag;
	return 0;
}
#endif

LRESULT 	CMainFrame::OnSaveImageAs		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND hWnd = m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
	CChildFrame *child = GetChildFrame(hWnd);
	CGeckoBrowser browser = DonutGetNsIWebBrowser(hWnd);
	nsCOMPtr<nsIDOMNode> node = child->m_ctxTargetNode;
	browser.SaveMedia(node);
	return 0;
}

LRESULT 	CMainFrame::OnOpenImageNewTab	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND hWnd = m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
	CChildFrame *child = GetChildFrame(hWnd);
	DonutOpenFile(m_hWnd, child->m_ctxImage);
	return 0;
}

LRESULT 	CMainFrame::OnCopyImageURI		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND hWnd = m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
	CChildFrame *child = GetChildFrame(hWnd);
	MtlSetClipboardText(child->m_ctxImage, m_hWnd);
	return 0;
}

LRESULT 	CMainFrame::OnSaveLinkAs		(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND hWnd = m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
	CChildFrame *child = GetChildFrame(hWnd);
	CGeckoBrowser browser = DonutGetNsIWebBrowser(hWnd);
	browser.SaveURI(child->m_ctxLink);
	return 0;
}

LRESULT 	CMainFrame::OnOpenLinkNewTab	(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND hWnd = m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
	CChildFrame *child = GetChildFrame(hWnd);
	DonutOpenFile(m_hWnd, child->m_ctxLink);
	return 0;
}

LRESULT 	CMainFrame::OnCopyLink			(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND hWnd = m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
	CChildFrame *child = GetChildFrame(hWnd);
	MtlSetClipboardText(child->m_ctxLink, m_hWnd);
	return 0;
}

LRESULT CMainFrame::OnShowExMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CMenu menu = CExMenuManager::LoadExMenu(CExMenuManager::EXMENU_ID_FIRST);
	POINT pos;

	GetCursorPos(&pos);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pos.x, pos.y, m_hWnd, NULL);
	return 0;
}


CChildFrame *CMainFrame::GetChildFrame(HWND hWndChild)
{
	if ( !::IsWindow(hWndChild) )
		return NULL;

	//成功　ChildFrame ポインタ
	//失敗　NULL((BOOL)FALSE)
	return (CChildFrame *) ::SendMessage(hWndChild, WM_GET_CHILDFRAME, 0, 0);
}


CChildFrame *CMainFrame::GetActiveChildFrame()
{
	HWND	hWnd = MDIGetActive();
	if (hWnd)
		return GetChildFrame(hWnd);
	return NULL;
}


//private:

HRESULT CMainFrame::LoadWebBrowserFromStream(IWebBrowser *pWebBrowser, IStream *pStream)
{
	HRESULT 			hr;
	IDispatch * 		pHtmlDoc		   = NULL;
	IPersistStreamInit *pPersistStreamInit = NULL;

	// Retrieve the document object.
	hr = pWebBrowser->get_Document( &pHtmlDoc );
	if ( SUCCEEDED(hr) ) {
		// Query for IPersistStreamInit.
		hr = pHtmlDoc->QueryInterface( IID_IPersistStreamInit,	(void **) &pPersistStreamInit );
		if ( SUCCEEDED(hr) ) {
			// Initialize the document.
			hr = pPersistStreamInit->InitNew();
			if ( SUCCEEDED(hr) ) {
				// Load the contents of the stream.
				hr = pPersistStreamInit->Load( pStream );
			}
			pPersistStreamInit->Release();
		}
	}

	return S_OK;
}


LRESULT CMainFrame::OnShowToolBarMenu()
{
	POINT		pt = {0};

	::GetCursorPos(&pt);

	CMenuHandle submenu = ::GetSubMenu(::GetSubMenu(m_hMenu, 2), 0);
	if ( submenu.IsMenu() )
		submenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	return 0;
}



///+++ メモ:.url に対する拡張プロパティ
LRESULT CMainFrame::OnSetExProperty(LPCTSTR lpstrUrlFile)
{
	CString 		  strUrlFile = lpstrUrlFile;

	if ( !MtlIsExt( strUrlFile, _T(".url") ) )
		return 0;

	CExPropertyDialog dlg(strUrlFile);
	dlg.DoModal();

	return 0;
}


// ==========================================================================
// 戻る・進む

BOOL CMainFrame::_Load_OptionalData(CChildFrame *pChild, const CString &strFileName, CString &strSection)
{
	pChild->SetDfgFileNameSection(strFileName, strSection);
	return TRUE;
}


BOOL CMainFrame::_Load_OptionalData2(CChildFrame *pChild,
						 std::vector<std::pair<CString, CString> > &ArrayFore,
						 std::vector<std::pair<CString, CString> > &ArrayBack)
{
	pChild->SetArrayHist( ArrayFore, ArrayBack );
	return TRUE;
}



LRESULT CMainFrame::OnMenuRecentDocument(HMENU hMenu)
{
	CRecentDocumentListFixed *pList;
	pList = CMainOption::s_pMru;
	CMenuHandle 			  menu	 = hMenu;

	if (menu.m_hMenu == NULL)
		return 0;

	if (pList == NULL)
		return 0;

	if (menu.GetMenuItemCount() == 0)
		menu.AppendMenu(MF_ENABLED | MF_STRING, ID_RECENTDOCUMENT_FIRST, (LPCTSTR) NULL);
	else if (menu.GetMenuItemID(0) != ID_RECENTDOCUMENT_FIRST)
		return 0;

	HMENU					  hMenuT = pList->GetMenuHandle();
	pList->SetMenuHandle(menu);
	pList->UpdateMenu();
	pList->SetMenuHandle(hMenuT);

	return 1;
}


LRESULT CMainFrame::OnMenuRecentLast(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	return ::SendMessage(m_hWnd, WM_COMMAND, ID_RECENTDOCUMENT_FIRST, 0);
}


int CMainFrame::_GetRecentCount()
{
	return CMainOption::s_pMru->m_arrDocs.GetSize();
}



// ==========================================================================
// タブ関係


LRESULT CMainFrame::OnMouseWheel(UINT fwKeys, short zDelta, CPoint point)
{
	// I don't have a wheel mouse...
	if ( (m_MDITab.GetMDITabExtendedStyle() & MTB_EX_WHEEL)
	   && MtlIsBandVisible(m_hWndToolBar, IDC_MDITAB) )
	{
		CRect rcTab;
		m_MDITab.GetWindowRect(&rcTab);

		if ( rcTab.PtInRect(point) ) {
			if (zDelta > 0) {
				m_MDITab.LeftTab();
			} else {
				m_MDITab.RightTab();
			}

			SetMsgHandled(TRUE);
			return 0;
		}
	}

	// 右クリックされていたら - スクロールのあるビュー上だとビューがスクロールされるバグ
	if (fwKeys == VK_RBUTTON) {
		if (zDelta > 0) {
			m_MDITab.LeftTab();
		} else {
			m_MDITab.RightTab();
		}

		SetMsgHandled(TRUE);
		return 0;
	}

	SetMsgHandled(FALSE);
	return 1;
}



LRESULT CMainFrame::OnViewTabBarMulti(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	DWORD dwOldStyle = m_MDITab.GetMDITabExtendedStyle();

	if (dwOldStyle & MTB_EX_MULTILINE)
		dwOldStyle &= ~MTB_EX_MULTILINE;
	else
		dwOldStyle |=  MTB_EX_MULTILINE;

	m_MDITab.SetMDITabExtendedStyle(dwOldStyle);
	return 0;
}



void CMainFrame::OnTabLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	m_MDITab.LeftTab();
}


void CMainFrame::OnTabRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	m_MDITab.RightTab();
}


#if 1 //+++
//+++ 先頭タブから1..8選択. 9は最後のタブを選択
LRESULT CMainFrame::OnTabIdx(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL & /*bHandled*/)
{
	ATLASSERT(wID >= ID_TAB_IDX_1 && wID <= ID_TAB_IDX_LAST);
	int nCount = m_MDITab.GetItemCount();

	if (wID == ID_TAB_IDX_LAST) {
		wID =  nCount - 1;
	} else {
		wID -= ID_TAB_IDX_1;
		if (wID < 0 || wID >=8 || wID >= nCount)
			return 0;
	}
	m_MDITab.SetCurSelEx(wID);
	return 0;
}
#endif



LRESULT CMainFrame::OnTabListDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CPoint pos;
	::GetCursorPos(&pos);

	int    nRet = m_MDITab.ShowTabListMenuDefault(pos.x, pos.y);

	if (nRet == -1)
		return 0;

	HWND   hWnd = m_MDITab.GetTabHwnd(nRet);

	if ( ::IsWindow(hWnd) )
		::SendMessage(m_wndMDIClient, WM_MDIACTIVATE, (WPARAM) hWnd, 0);

	return 0;
}


LRESULT CMainFrame::OnTabListVisible(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CPoint pos;
	::GetCursorPos(&pos);

	int    nRet = m_MDITab.ShowTabListMenuVisible(pos.x, pos.y);

	if (nRet == -1)
		return 0;

	HWND   hWnd = m_MDITab.GetTabHwnd(nRet);

	if ( ::IsWindow(hWnd) )
		::SendMessage(m_wndMDIClient, WM_MDIACTIVATE, (WPARAM) hWnd, 0);

	return 0;
}


LRESULT CMainFrame::OnTabListAlphabet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CPoint pos;
	::GetCursorPos(&pos);

	int    nRet = m_MDITab.ShowTabListMenuAlphabet(pos.x, pos.y);

	if (nRet == -1)
		return 0;

	HWND   hWnd = m_MDITab.GetTabHwnd(nRet);

	if ( ::IsWindow(hWnd) )
		::SendMessage(m_wndMDIClient, WM_MDIACTIVATE, (WPARAM) hWnd, 0);

	return 0;
}



// ==========================================================================
// 窓サイズ関係


#if 0	//+++	失敗
LRESULT CMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  #if 1	//+++ お試し
	m_bMinimized	= (wParam == SIZE_MINIMIZED);
	if (m_bFullScreen == 0 && m_bMinimized == 0)		//+++ フルスクリーン時や縮小化以外の場合は
		m_bOldMaximized = (wParam == SIZE_MAXIMIZED);	//+++ 最大化しているかどうかをチェック
  #endif
	return CMDIFrameWindowImpl< CMainFrame >::OnSize(uMsg, wParam, lParam, bHandled);
}
#endif

#if 1	//+++
#if 1	//+++ 失敗のごまかし
void CMainFrame::ShowWindow_Restore(bool flag)
{
	int nShow = flag ? SW_RESTORE : SW_SHOW;
	ShowWindow( nShow );
}
#else	//失敗
void CMainFrame::ShowWindow_Restore(bool)
{
	int nShow = SW_SHOWNORMAL;	//SW_RESTORE;
	if (m_bMinimized)
		nShow = SW_SHOWMINIMIZED;
	if (m_bOldMaximized)		//+++ m_bFullScreen,m_bOldMaximizedが正しく設定されているとして処理.
		nShow = SW_SHOWMAXIMIZED;
	ShowWindow( nShow );
}
#endif
#endif


LRESULT CMainFrame::OnMainFrameMinimize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	ShowWindow(SW_MINIMIZE);
	//m_bOldMaximized = 0;
	// //m_bMinimized    = 1;
	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	return 0;
}


LRESULT CMainFrame::OnMainFrameMaximize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	ShowWindow(SW_MAXIMIZE);
	m_bOldMaximized = 1;
	// //m_bMinimized    = 0;
	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	return 0;
}


LRESULT CMainFrame::OnMainFrameNormMaxSize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if (m_bOldMaximized) {
		//ShowWindow(SW_RESTORE);
		ShowWindow_Restore(1);	//ShowWindow(SW_RESTORE);		//サイズを戻す
		m_bOldMaximized = 0;
	} else {
		ShowWindow(SW_MAXIMIZE);
		m_bOldMaximized = 1;
	}
	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	return 0;
}


// ==================================================
// フルスクリーン関係

//+++ 起動初回の窓サイズ設定.
void CMainFrame::startupMainFrameStayle(int nCmdShow, bool bTray)
{
	if ((nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE)
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_MINBTN2TRAY))		//+++ .lnkとかで最小化で、起動されたとき、トレイに入る設定ならトレイへ.
	{
		bTray    = true;
		nCmdShow = SW_SHOWNORMAL;		// うまくいかない...が、残す...
	}
	CIniFileI  pr( g_szIniFileName, _T("Main") );
	int wndSML	= MtlGetProfileMainFrameState(pr, *this, nCmdShow);				//+++ s,m,l,fullを判別しておく(暫定版)
	pr.Close();
	m_bOldMaximized 	= 0;
	if (wndSML == 2)
		m_bOldMaximized = 1;

	if (bTray) {
		OnGetOut(0,0,0);
	} else if (CMainOption::s_dwMainExtendedStyle & MAIN_EX_FULLSCREEN) {
		_FullScreen(TRUE);
	}
}


#if 0
bool CMainFrame::IsFullScreen()
{
  #if 1 //+++
	return m_bFullScreen;
  #else
	return (GetStyle() & WS_CAPTION) == 0;
  #endif
}
#endif



void CMainFrame::OnViewFullScreen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (IsFullScreen() == 0)			//+++
	{
		_FullScreen(TRUE);
	} else {
		_FullScreen(FALSE);
	}
}



void CMainFrame::_FullScreen(BOOL bOn)
{
	//static bool m_bOldMaximized = 0;
	_ShowBandsFullScreen(bOn);

	if (bOn) {							// remove caption
		// save prev visible
		CWindowPlacement	wndpl;
		GetWindowPlacement(&wndpl);

		{
			CIniFileO	pr( g_szIniFileName, _T("Main") );
			wndpl.WriteProfile(pr, _T("frame."));
			//x pr.Close();
		}

		//m_bMinimized		= 0;
		m_bOldMaximized 	= (wndpl.showCmd == SW_SHOWMAXIMIZED);
		ModifyStyle(WS_CAPTION, 0);

		ShowWindow(SW_HIDE);
		SetMenu(NULL);

		m_mcToolBar.m_bVisible = true;

		ShowWindow(SW_MAXIMIZE);
		if (m_bOldMaximized == 0) {
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		m_bFullScreen = true;
	} else {
		m_mcToolBar.m_bVisible = false;
		// restore prev visible
	  #if 1 //+++	キャプションの着けはづしがありの場合
		if (CSkinOption::s_nMainFrameCaption)
			ModifyStyle(0, WS_CAPTION);
	  #else
		ModifyStyle(0, WS_CAPTION);
	  #endif
		if (m_bOldMaximized == 0) {
			ShowWindow_Restore(1);		//ShowWindow(SW_RESTORE);		//+++ サイズを戻す
		} else {
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
		m_bFullScreen = false;
	}
}



void CMainFrame::_ShowBandsFullScreen(BOOL bOn, bool bInit /*= false*/)
{
	static BOOL 	s_bOldVisibleStatusBar;
	int 			nIndex = 0;
	if (bOn) { // remove caption
		// save prev visible
		int 		nToolbarPluginCount = CPluginManager::GetCount(PLT_TOOLBAR);
		m_mapToolbarOldVisible.RemoveAll();

		for (nIndex = 0; nIndex < _countof(STDBAR_ID); nIndex++) {
			m_mapToolbarOldVisible.Add( STDBAR_ID[nIndex], MtlIsBandVisible( m_hWndToolBar, STDBAR_ID[nIndex] ) );
		}
		for (nIndex = 0; nIndex < nToolbarPluginCount; nIndex++) {
			m_mapToolbarOldVisible.Add( IDC_PLUGIN_TOOLBAR + nIndex,
										MtlIsBandVisible( m_hWndToolBar, IDC_PLUGIN_TOOLBAR + nIndex ) );
		}
		s_bOldVisibleStatusBar = ::IsWindowVisible(m_hWndStatusBar) != 0;

		{
			CIniFileI pr( g_szIniFileName, _T("FullScreen") );
			MtlShowBand(m_hWndToolBar, ATL_IDW_COMMAND_BAR	, pr.GetValue(_T("ShowMenu")	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, ATL_IDW_TOOLBAR		, pr.GetValue(_T("ShowToolBar") , FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_ADDRESSBAR		, pr.GetValue(_T("ShowAdress")	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_MDITAB			, pr.GetValue(_T("ShowTab") 	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_LINKBAR			, pr.GetValue(_T("ShowLink")	, FALSE) != 0);
			MtlShowBand(m_hWndToolBar, IDC_SEARCHBAR		, pr.GetValue(_T("ShowSearch")	, FALSE) != 0);
			if (pr.GetValue(_T("ShowStatus"), FALSE ) == FALSE)
				::ShowWindow(m_hWndStatusBar, SW_HIDE);
			else
				::ShowWindow(m_hWndStatusBar, SW_SHOWNOACTIVATE);
		}
		for (nIndex = 0; nIndex < nToolbarPluginCount; nIndex++)
			MtlShowBand(m_hWndToolBar, IDC_PLUGIN_TOOLBAR + nIndex, FALSE);
	} else {
		for (nIndex = 0; nIndex < m_mapToolbarOldVisible.GetSize(); nIndex++)
			MtlShowBand( m_hWndToolBar, m_mapToolbarOldVisible.GetKeyAt(nIndex), m_mapToolbarOldVisible.GetValueAt(nIndex) );

		::ShowWindow(m_hWndStatusBar, s_bOldVisibleStatusBar ? SW_SHOWNOACTIVATE : SW_HIDE);
	}
}



// ==================================================
// トレイアイコン関係


// UDT DGSTR
void CMainFrame::OnGetOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	static int nShow = SW_SHOWNORMAL;

	if ( IsWindowVisible() ) {
		//+++ メモ:窓状態のときはトレイ化
		m_bTray = true; 		//+++
		WINDOWPLACEMENT wp;
		wp.length = sizeof (WINDOWPLACEMENT);
		::GetWindowPlacement(m_hWnd, &wp);
		nShow		  = wp.showCmd;
	  #if 1 //+++ フルスクリーン以外なら今が最大かどうかを控える
		if (m_bFullScreen == 0)
			m_bOldMaximized = (nShow == SW_MAXIMIZE);
	  #endif
		SetHideTrayIcon();	//+++ トレイアイコン化
		Sleep(100); 		// UDT TODO
	} else {				// just db F9 press , come here :p
		m_bTray      = false;	//+++
		//m_bMinimized = 0;
		ShowWindow_Restore(0);	//x ShowWindow( SW_SHOW /*nShow*/ );

		//+++ TrayMessage(m_hWnd, NIM_DELETE, TM_TRAY, 0, NULL);
		DeleteTrayIcon();	//+++ トレイアイコン削除
	}
}
// ENDE



// UDT DGSTR ( hide window & display icon )
LRESULT CMainFrame::OnMyNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled*/)
{
	if ( IsWindowVisible() ) {
		return -1;
	} else {
		switch (lParam) {
		case WM_LBUTTONUP:
			ShowWindow_Restore(0);	//ShowWindow(SW_SHOW);
			DeleteTrayIcon();	//+++
			return 0;
			break;

		case WM_RBUTTONUP:
		  #if 1	//+++
			{
				::SetForegroundWindow(m_hWnd);
				CMenu/*Handle*/ 	menu0;
				menu0.LoadMenu(IDR_TRAYICON_MENU);
				if (menu0.m_hMenu == NULL)
					return 0;
				CMenuHandle menu = menu0.GetSubMenu(0);
				if (menu.m_hMenu == NULL)
					return 0;

				// ポップアップメニューを開く.
				POINT 	pt;
				::GetCursorPos(&pt);
				HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON| TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
				if (hr == 57666/*復帰*/) {
					ShowWindow_Restore(0);	//ShowWindow(SW_SHOW);
					DeleteTrayIcon();	//+++
					return 0;
				}
				if (hr == 57665/*終了*/) {
					DeleteTrayIcon();	//+++
					PostMessage(WM_CLOSE, 0, 0);
					break;
				}
			}
		  #else
			DeleteTrayIcon();	//+++
			PostMessage(WM_CLOSE, 0, 0);
			break;
		  #endif
		}

		return -1;
	}
}
// ENDE



#if 1 //+++ トレイアイコンの設定.
void CMainFrame::SetHideTrayIcon()
{
  #if 1 //+++
	RtlSetMinProcWorkingSetSize();		//+++ (メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等)
	HICON hIcon = 0;
	if (Misc::IsExistFile(m_strIconSm))
		hIcon	= (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), m_strIconSm, IMAGE_ICON, 16, 16, LR_SHARED|LR_LOADFROMFILE);
	if (hIcon == 0)
		hIcon = LoadIcon( _Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME) );
	TrayMessage( m_hWnd, NIM_ADD, TM_TRAY, hIcon, DONUT_NAME/*_T("unDonut")*/ );		//+++
  #else //元
	//x TrayMessage( m_hWnd, NIM_ADD, TM_TRAY, IDR_MAINFRAME, DONUT_NAME/*_T("unDonut")*/ );
  #endif
	ShowWindow(SW_HIDE);
}
#endif


//+++ トレイ化の終了/トレイアイコンの削除.
void CMainFrame::DeleteTrayIcon()
{
	TrayMessage(m_hWnd, NIM_DELETE, TM_TRAY, 0, NULL);
	//x m_bTrayFlag = false;

	RtlSetMinProcWorkingSetSize();		//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
}




// ===========================================================================
// エクスプローラバー

// UDT DGSTR
LRESULT CMainFrame::UpdateExpBar(LPCTSTR lpszExpBar, DWORD dwReserved)
{
	//if(lpszExpBar == NULL) return 0;
	m_ExplorerBar.SetTitle(lpszExpBar);
	return 0;
}
// ENDE


//minit
LRESULT CMainFrame::OnRefreshExpBar(int nType)
{
	m_ExplorerBar.RefreshExpBar(nType);
	return 0;
}


// UDT DGSTR //Update minit
LRESULT CMainFrame::OnFavoriteExpBar(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_ExplorerBar.ShowBar(true);
	//m_ExplorerBar.Show();
	MtlSendCommand(m_ExplorerBar.m_FavBar.m_hWnd, wID);
	m_wndSplit.SetSinglePaneMode();
	return 0;
}


void CMainFrame::OnFileNewClipBoardEx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_ExplorerBar.m_ClipBar.OpenClipboardUrl();
}



LRESULT CMainFrame::OnSelectUserFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlSendCommand(m_ExplorerBar.m_FavBar.m_hWnd, ID_SELECT_USERFOLDER);
	return 0;
}



// ===========================================================================
// アドレスバー


// U.H
void CMainFrame::OnGoAddressBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_AddressBar.SetFocus();
}


LRESULT CMainFrame::OnViewGoButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if ( _check_flag( ABR_EX_GOBTNVISIBLE, m_AddressBar.GetAddressBarExtendedStyle() ) )
		m_AddressBar.ModifyAddressBarExtendedStyle(ABR_EX_GOBTNVISIBLE, 0);
	else
		m_AddressBar.ModifyAddressBarExtendedStyle(0, ABR_EX_GOBTNVISIBLE);

	return 0;
}


LRESULT CMainFrame::OnViewAddBarDropDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if ( m_AddressBar.GetDroppedStateEx() )
		m_AddressBar.ShowDropDown(FALSE);
	else
		m_AddressBar.ShowDropDown(TRUE);

	return 0;
}


void CMainFrame::OnSetFocusAddressBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	::SetFocus( m_AddressBar.GetEditCtrl() );
}


void CMainFrame::__AddToAddressBoxUnique(const CString &strURL)
{
	COMBOBOXEXITEM item;

	item.mask	 = CBEIF_TEXT;
	item.iItem	 = 0;
	item.pszText = (LPTSTR) (LPCTSTR) strURL;

	// search the same string
	int 		   nCount = m_AddressBar.GetCount();

	for (int n = 0; n < nCount; ++n) {
		CString str;
		MtlGetLBTextFixed(m_AddressBar, n, str);

		if (strURL == str) {
			m_AddressBar.DeleteItem(n);
			break;
		}
	}

	m_AddressBar.InsertItem(&item);
}



void CMainFrame::__MoveToTopAddressBox(const CString &strURL)
{
	COMBOBOXEXITEM item;

	item.mask	 = CBEIF_TEXT;
	item.iItem	 = 0;
	item.pszText = (LPTSTR) (LPCTSTR) strURL;

	// search the same string
	int 		   nCount = m_AddressBar.GetCount();

	for (int n = 0; n < nCount; ++n) {
		CString str;
		MtlGetLBTextFixed(m_AddressBar, n, str);

		if (strURL == str) {
			m_AddressBar.DeleteItem(n);
			m_AddressBar.InsertItem(&item);
			break;
		}
	}
}



// ===========================================================================
// 検索

CString CMainFrame::GetActiveSelectedText()
{
	CChildFrame *pChild = GetActiveChildFrame();
	if (pChild)
		return pChild->GetSelectedTextLine();
	return CString();
}


LRESULT CMainFrame::OnCommandDirect(int nCommand, LPCTSTR lpstr)
{
	CString str(lpstr);

	if (str.Find( _T("tp://") ) != -1
	  #if 1	//+++
		|| str.Find( _T("https://") ) != -1
		|| str.Find( _T("file://") ) != -1
//|| str.Left(11) == _T("javascript:")		//*+++ 実験
	  #endif
	){
		Misc::StrToNormalUrl(str);		//+++
		DonutOpenFile( m_hWnd, str, DonutGetStdOpenFlag() );
	} else {
		if (nCommand == ID_SEARCH_DIRECT) {
			CEdit edit = m_SearchBar.GetEditCtrl();
			edit.SendMessage(WM_CHAR, 'P');
			edit.SetWindowText(lpstr);
			m_SearchBar.SearchWeb();
		} else if (nCommand == ID_OPENLINK_DIRECT) {
			SendMessage(WM_COMMAND, ID_EDIT_OPEN_SELECTED_REF, 0);
		}
	}

	return 0;
}



LRESULT CMainFrame::OnSearchBarCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	HWND	hWndActive = MDIGetActive();

	if (hWndActive == NULL)
		return 0;

	CEdit	edit	   = m_SearchBar.GetEditCtrl();
	CString str;	//+++   = MtlGetWindowText(edit);
	str = _GetSelectText(edit);

	if ( str.IsEmpty() )
		return 0;

	switch (wID) {
	case ID_SEARCHBAR_SEL_UP:
		m_SearchBar.SearchPage(FALSE);
		//SendMessage(hWndActive, WM_USER_FIND_KEYWORD, (WPARAM)str.GetBuffer(0), (LPARAM)FALSE);
		break;

	case ID_SEARCHBAR_SEL_DOWN:
		m_SearchBar.SearchPage(TRUE);
		//SendMessage(hWndActive, WM_USER_FIND_KEYWORD, (WPARAM)str.GetBuffer(0), (LPARAM)TRUE);
		break;

	case ID_SEARCHBAR_HILIGHT:
		m_SearchBar.SearchHilight();
		//SendMessage(hWndActive, WM_USER_HILIGHT, (WPARAM)str.GetBuffer(0), (LPARAM)0);
		break;
	}

	return 0;
}


LRESULT CMainFrame::OnHilight(LPCTSTR lpszKeyWord)
{
	HWND hWndActive = MDIGetActive();

	if (hWndActive == NULL)
		return 0;

	return SendMessage(hWndActive, WM_USER_HILIGHT, (WPARAM) lpszKeyWord, 0);
}


LRESULT CMainFrame::OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bBack)
{
	HWND hWndActive = MDIGetActive();

	if (hWndActive == NULL)
		return 0;

	return SendMessage(hWndActive, WM_USER_FIND_KEYWORD, (WPARAM) lpszKeyWord, (LPARAM) bBack);
}


// U.H
LRESULT CMainFrame::OnViewSearchBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlToggleBandVisible(m_hWndToolBar, IDC_SEARCHBAR);
	UpdateLayout();
	return 0;
}


//minit
LRESULT CMainFrame::OnViewSearchButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_SearchBar.ShowToolBarIcon( !m_SearchBar.GetToolIconState() );
	return 0;
}


LRESULT CMainFrame::OnSearchHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlSendCommand(m_ExplorerBar.m_FavBar.m_hWnd, ID_SEARCH_HISTORY);
	return 0;
}


void CMainFrame::OnSetFocusSearchBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{
	::SetFocus( m_SearchBar.GetEditCtrl() );
}


void CMainFrame::OnSetFocusSearchBarEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl)
{ //minit
	m_SearchBar.SetFocusToEngine();
}


LRESULT CMainFrame::OnSpecialRefreshSearchEngine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_SearchBar.RefreshEngine();
	return 0;
}


HRESULT CMainFrame::OnSearchWebSelText(LPCTSTR lpstrText, LPCTSTR lpstrEngine)
{
	int 	nLoopCnt = 0;
	BOOL	bFirst	 = (DonutGetStdOpenCreateFlag() & D_OPENFILE_ACTIVATE) != 0;	//+++ ? TRUE : FALSE;

	m_SearchBar.OpenSearch(lpstrText, lpstrEngine, nLoopCnt, bFirst);

	return S_OK;
}


//+++
void CMainFrame::OnSearchWeb_engineId(UINT code, int id, HWND hWnd)
{
	ATLASSERT(ID_INSERTPOINT_SEARCHENGINE <= id && id <= ID_INSERTPOINT_SEARCHENGINE_END);
	MtlSendCommand(m_SearchBar.m_hWnd, WORD(id));
}


//+++
HRESULT	CMainFrame::OnSearchEngineMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	MtlSendCommand(m_SearchBar.m_hWnd, WORD(ID_SEARCHENGINE_MENU));
	return 0;
}

