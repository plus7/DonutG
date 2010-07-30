/**
 *	@file	ChildFrame.cpp
 *	@brief	タブページ１つの処理.
 */

#include "stdafx.h"

//#include <vector>
#include "PluginManager.h"
#include "MtlBrowser.h"

#include "FavoriteMenu.h"
#include "DonutFavoritesMenu.h"
#include "DonutView.h"
#include "AddressBar.h"
#include "MDITabCtrl.h"
#include "MDIChildUserMessenger.h"

#include "option/DLControlOption.h"
#include "option/StartUpOption.h"
#include "option/DonutConfirmOption.h"
#include "option/IgnoreURLsOption.h"
#include "option/CloseTitleOption.h"
#include "option/UrlSecurityOption.h"	//+++
#include "ExStyle.h"					//+++

#include "ParseInternetShortcutFile.h"	//+++

#include "FileNotification.h"
#include "DonutPFunc.h" 		//+++
#include "ChildFrame.h"

#include "StyleSheetUtils.h"

//#include "MainFrame.h"			//+++ debug
#include "nsIWebBrowserFocus.h"
#include "nsIWebBrowserFind.h"
#include "nsIContentViewer.h"
#include "nsIMarkupDocumentViewer.h"
#include "nsIDOMNodeSelector.h"
#include "nsIDOMDocumentFragment.h"
#include "nsIDOMHTMLDocument.h"
#include "nsContentCID.h"
#include "nsIDocumentEncoder.h"
#include "nsIDOMHTMLElement.h"
#include "nsIDOMNodeList.h"
#include "nsIDOMHTMLCollection.h"
#include "nsIDOMHTMLAnchorElement.h"
#include "nsIContextMenuListener2.h"
#include "nsIDOMDocumentView.h"
#include "nsIDOMAbstractView.h"
#include "nsIFind.h"
#include "nsIDOMDocumentFragment.h"
#include "nsIDOMDocumentRange.h"

//#include "MDIChildUserMessenger.h"
#ifdef _DEBUG
	const bool _Donut_ChildFrame_traceOn = true;	//false;
	#define dcfTRACE	if (_Donut_ChildFrame_traceOn) ATLTRACE
#else
	#define dcfTRACE
#endif

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const int	g_LIGHTMAX		= 5;

#if 1 //+++ 実験でoff ... やっぱり、このまま.
#if 0 //def USE_ORG_UNDONUT_INI
static const LPCTSTR		g_lpszLight[] = {
	"<span id='DonutP' style='color:black;background:FFFF00'>",
	"<span id='DonutP' style='color:black;background:00FFFF'>",
	"<span id='DonutP' style='color:black;background:FF00FF'>",
	"<span id='DonutP' style='color:black;background:7FFF00'>",
	"<span id='DonutP' style='color:black;background:1F8FFF'>"
};
#else
static const LPCTSTR		g_lpszLight[] = {
  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	_T("color:black;background:#FFFF00"),
	_T("color:black;background:#00FFFF"),
	_T("color:black;background:#FF00FF"),
	_T("color:black;background:#7FFF00"),
	_T("color:black;background:#1F8FFF"),
  #else
	"<span id='Donut' style='color:black;background:FFFF00'>",
	"<span id='Donut' style='color:black;background:00FFFF'>",
	"<span id='Donut' style='color:black;background:FF00FF'>",
	"<span id='Donut' style='color:black;background:7FFF00'>",
	"<span id='Donut' style='color:black;background:1F8FFF'>"
  #endif
};
#endif
#endif



// ===========================================================================
// 初期化・ページ読み込み

CChildFrame::CChildFrame(CMDITabCtrl &MDITab, CDonutAddressBar &addressbar, bool bNewWindow2, DWORD dwDefaultDLControlFlags, DWORD dwDefaultExtendedStyleFlags)
	: m_bNavigateBack(FALSE)
	, m_bNavigateForward(FALSE)
	, m_MDITab(MDITab)
	, m_AddressBar(addressbar)
	, m_bNewWindow2(bNewWindow2)
	, m_wndBrowser(this, 7)
	, m_FavMenu(this, ID_INSERTPOINT_CHILDFAVORITEMENU)
	, m_nProgress(-1)
	, m_nProgressMax(0)
	, m_bPageFocusInitialized(false)
	, m_bClosing(false)
	, m_bWaitNavigateLock(true)
	, m_hWndFocus(NULL)
	, m_view(dwDefaultDLControlFlags, dwDefaultExtendedStyleFlags)	//+++引数追加.
	, m_bOperateDrag(false)
	, m_bExPropLock(false)
	, m_hWndF(NULL)
	, m_hWndA(NULL)
	, m_strBookmark(NULL)
	, m_nPainBookmark(0)
	, m_nSecureLockIcon( secureLockIconUnsecure )
	, m_bPrivacyImpacted( TRUE )
	, m_bInitTravelLog( TRUE )
	, m_nImgWidth( -1 )
	, m_nImgHeight( -1 )
	, m_bNowHilight(FALSE)
	, m_bAutoHilight( false )
  #if 1	//+++
	, m_nCmdType(0)
	, m_bImageAutoSizeReq(0)
	, m_nImgScl(100)
	, m_nImgSclSav(100)
	, m_bImg(0)
	, m_japanCharSet(-1)
  #endif
{
  #if 1	//+++
	{
		static bool bInit				= false;
		static bool bImgAuto_NouseLClk	= false;
		static int  nImgSclSw			= 0;
		if (bInit == 0) {
			CIniFileI pr( g_szIniFileName, _T("ETC") );
			bImgAuto_NouseLClk	= pr.GetValue(_T("ImageAutoSize_NouseLClick")) != 0;
			nImgSclSw			= pr.GetValue(_T("ImageAutoSize_FirstOn")) != 0;
		}
		m_bImgAuto_NouseLClk	= bImgAuto_NouseLClk;
		m_nImgSclSw				= nImgSclSw;
	}
  #endif
}


/** リンクを新しいタブで開く、等
 */
void CChildFrame::OnDocHostUIOpenNewWin(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (wNotifyCode == NM_ON) {
		m_view.m_ViewOption.m_dwExStyle |= DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, true);
	} else if (wNotifyCode == NM_OFF) {
		m_view.m_ViewOption.m_dwExStyle &= ~DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, false);
	} else if (m_view.m_ViewOption.m_dwExStyle & DVS_EX_OPENNEWWIN) {
		m_view.m_ViewOption.m_dwExStyle &= ~DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, false);
	} else {
		m_view.m_ViewOption.m_dwExStyle |= DVS_EX_OPENNEWWIN;
		m_MDITab.NavigateLockTab(m_hWnd, true);
	}
}


void CChildFrame::OnNewWindow(nsIWebBrowser **ppSupp, bool *bCancel)
{
	dcfTRACE( _T("OnNewWindow\n") );

	if (CMainOption::s_bAppClosing) {
		dcfTRACE( _T(" application closing, ignore this event\n") );
		*bCancel = true;
		return;
	}

	DWORD		 exStyle= _GetInheritedExtendedStyleFlags();	//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...

	m_MDITab.SetLinkState(LINKSTATE_A_ON);
	CChildFrame *pChild = CChildFrame::NewWindow(	m_hWndMDIClient,
													m_MDITab,
													m_AddressBar,
													true,
													_GetInheritedDLControlFlags(),
													exStyle );	//+++ 引数追加.

	//m_MDITab.SetLinkState(LINKSTATE_OFF);
	// Note: In this case, must be activated in BeforeNavigate2 strangely
	// pChild->ActivateFrameForNewWindowDelayed();
	pChild->ActivateFrame();
	if (pChild == NULL) {
		*bCancel = true;
		return;
	}

	//Raise Plugin Event
	//CComBSTR bstr;
	//pChild->m_spBrowser->get_LocationURL(&bstr);
	//CPluginManager::ChainCast_PluginEvent(DEVT_TAB_OPENED,(DWORD)(LPCTSTR)CString(bstr),0);

	pChild->m_strSearchWord = m_strSearchWord;

  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	pChild->m_bNowHilight	= m_bNowHilight;
  #endif
	
	*ppSupp = pChild->m_spBrowser;
	//NS_ADDREF(*ppSupp);
}
void CChildFrame::OnLocationChange(const CString& strURL)
{
	if (MDIGetActive() == m_hWnd)
		m_AddressBar.SetWindowText(strURL);
}

void CChildFrame::InsertTypeOf(CMenu& menuPopup,
							   DWORD *menupos,
							   DWORD dwID,
							   CSimpleArray<HMENU> &toDestroy,
							   CSimpleMap<DWORD, DWORD> &toRemove,
							   CSimpleArray<DWORD> &commands)
{
	CString 	strSection;
	strSection.Format(_T("Type%d"), dwID);
	CString 	strPath 	= _GetFilePath( _T("Menu.ini") );
	CIniFileI	pr(strPath, strSection);
	DWORD		dwCount 	= pr.GetValue(_T("FrontCount"), 0 );

	DWORD		 jj;
	for (jj = 0; jj < dwCount; jj++) {
		CString strKey;
		strKey.Format(_T("Front%02d"), jj);
		DWORD	dwCmd = pr.GetValue(strKey, 0);

		DWORD dwID = 0;
		HMENU hSubMenu = 0;

		switch (dwCmd) {
		case ID_FAVORITES_DROPDOWN:
			hSubMenu = (HMENU) ::SendMessage(GetTopLevelWindow(), WM_MENU_GET_FAV	   , 0, 0);
			toRemove.Add(jj + *menupos, dwCmd);
			break;

		case ID_FAVORITES_GROUP_DROPDOWN:
			hSubMenu = (HMENU) ::SendMessage(GetTopLevelWindow(), WM_MENU_GET_FAV_GROUP, 0, 0);
			toRemove.Add(jj + *menupos, dwCmd);
			break;

		case ID_SCRIPT:
			hSubMenu = (HMENU) ::SendMessage(GetTopLevelWindow(), WM_MENU_GET_SCRIPT   , 0, 0);
			toRemove.Add(jj + *menupos, dwCmd);
			break;

		case ID_SEARCHENGINE_MENU:
			{
				CMenuHandle menu = CDonutSearchBar::GetInstance()->GetSearchEngineMenuHandle();
				hSubMenu = menu.m_hMenu;
				toRemove.Add(jj + *menupos, dwCmd);
			}
			break;

		case ID_DLCTL_CHG_MULTI:	dwID = IDR_MULTIMEDIA;		break;
		case ID_DLCTL_CHG_SECU: 	dwID = IDR_SECURITY;		break;
		case ID_VIEW_FONT_SIZE: 	dwID = IDR_VIEW_FONT_SIZE;	break;
		case ID_COOKIE: 			dwID = IDR_COOKIE;			break;
		case ID_HTMLZOOM_MENU: 		dwID = IDR_ZOOM_MENU;		break;
		default:												break;
		}

		if (dwID != 0) {
			CMenuHandle menu;
			menu.LoadMenu(dwID);
			hSubMenu = menu.GetSubMenu(0);
			toDestroy.Add(menu);
		}

		CString strCmd;
		CToolTipManager::LoadToolTipText(dwCmd, strCmd);			//StringTableから文字列を取得

		if(hSubMenu!=0){
			if (dwCmd == ID_FAVORITES_DROPDOWN) {
				strCmd = _T("お気に入り(&A)"); 			//メインメニューから文字列を取得できないため
			} else if (dwCmd == ID_FAVORITES_GROUP_DROPDOWN) {
				strCmd = _T("お気に入りグループ(&G)"); 	//上に同じ
			} else if (dwCmd == ID_SCRIPT) {
				strCmd = _T("スクリプト(&S)"); 			//上に同じ
			}
			menuPopup.AppendMenu(MF_STRING, hSubMenu, strCmd);
			_BeforeInitSpecialMenu(dwCmd);
		} else if (strCmd.IsEmpty() == FALSE) {
			commands.Add(dwCmd);
			menuPopup.AppendMenu(MF_STRING, dwCmd, strCmd);
		} else {
			menuPopup.AppendMenu(MF_SEPARATOR);
		}
	}
	*menupos += jj;
}

bool CChildFrame::DocHasSelection(nsIDOMDocument *doc)
{
    nsCOMPtr<nsIDOMDocumentView> docview = do_QueryInterface(doc);
    if (!docview) return false;

	nsCOMPtr<nsIDOMAbstractView> abstractView;
	docview->GetDefaultView(getter_AddRefs(abstractView));
	nsCOMPtr<nsIDOMWindow> win = do_QueryInterface(abstractView);
    if (!win) return false;

    nsCOMPtr<nsISelection> selection;
    win->GetSelection(getter_AddRefs(selection));
    if (!selection) return false;

	PRUnichar *hoge;
	selection->ToString(&hoge);
	CString fuga(hoge);
	NS_Free(hoge);
	if(fuga.IsEmpty()) return false;
	else return true;
}

void CChildFrame::OnShowContextMenu(PRUint32 aContextFlags, nsIContextMenuInfo *aUtils)
{
	nsCOMPtr<nsIDOMNode> domNode;
	aUtils->GetTargetNode(getter_AddRefs(domNode));
	if(!domNode) return;
    nsCOMPtr<nsIDOMDocument> doc;
    domNode->GetOwnerDocument(getter_AddRefs(doc));
	if(!doc) return;
	bool hasSelection = DocHasSelection(doc);

	nsCOMPtr<nsIDOMEvent> evt;
	aUtils->GetMouseEvent(getter_AddRefs(evt));
	nsCOMPtr<nsIDOMMouseEvent> mouseEvt = do_QueryInterface(evt);
    PRInt32 scrX, scrY;
	mouseEvt->GetScreenX(&scrX);
	mouseEvt->GetScreenY(&scrY);

	CMenu menuPopup;
	menuPopup.CreatePopupMenu();

	CSimpleArray<HMENU> toDestroy;
	CSimpleMap<DWORD, DWORD> toRemove;
	CSimpleArray<DWORD> commands;

	DWORD idx = 0;

	if(hasSelection){
		InsertTypeOf(menuPopup, &idx, CONTEXT_MENU_TEXTSELECT, toDestroy, toRemove, commands);
	}

	if(aContextFlags & nsIContextMenuListener2::CONTEXT_LINK){
		if(idx>0){ menuPopup.AppendMenu(MF_SEPARATOR); idx++; }
		InsertTypeOf(menuPopup, &idx, CONTEXT_MENU_ANCHOR, toDestroy, toRemove, commands);
	}

	if(aContextFlags & nsIContextMenuListener2::CONTEXT_IMAGE){
		if(idx>0){ menuPopup.AppendMenu(MF_SEPARATOR); idx++; }
		InsertTypeOf(menuPopup, &idx, CONTEXT_MENU_IMAGE, toDestroy, toRemove, commands);
	}
	
	if(idx == 0)
		InsertTypeOf(menuPopup, &idx, CONTEXT_MENU_DEFAULT, toDestroy, toRemove, commands);

	m_ctxTargetNode = domNode;
	nsEmbedString tmp;
	aUtils->GetAssociatedLink(tmp);
	m_ctxLink = tmp.get();
	nsCOMPtr<nsIURI> uri;
	aUtils->GetImageSrc(getter_AddRefs(uri));
	if(uri){
		nsEmbedCString tmpc;
		uri->GetSpec(tmpc);
		m_ctxImage = tmpc.get();
	}

	int 	iSelection	= ::TrackPopupMenu(   menuPopup
		, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD
		, scrX
		, scrY
		, 0
		, GetTopLevelWindow()
		, (RECT *) NULL );
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

	//+++ 検索エンジン
	else if (ID_INSERTPOINT_SEARCHENGINE <= iSelection && iSelection <= ID_INSERTPOINT_SEARCHENGINE_END)
		bSendFrm = TRUE;

	if ( bSendFrm == FALSE && commands.Find( (DWORD) iSelection ) != -1 )
		bSendFrm = TRUE;

	if(bSendFrm) ::SendMessage(GetTopLevelWindow(), WM_COMMAND, iSelection, NULL);

	//Clean up
	m_ctxTargetNode = NULL;
	m_ctxLink = "";
	m_ctxImage = "";

	for(int i = toRemove.GetSize()-1; i>=0; i--){
		menuPopup.RemoveMenu(toRemove.GetKeyAt(i), MF_BYPOSITION);
		_RestoreSpecialMenu(toRemove.GetValueAt(i));
	}
	for(int i=0;i<toDestroy.GetSize();i++){
		//TODO:いるのかいらないのかわからない
	}
}


CChildFrame *CChildFrame::NewWindow(
		HWND				hWndMDIClient,
		CMDITabCtrl &		tabMDI,
		CDonutAddressBar &	adBar,
		bool				bNewWindow2 /*= false*/,
		DWORD				dwDLFlags	/*= CDLControlOption::s_dwDLControlFlags*/,
		DWORD				dwESFlags	/*= CDLControlOption::s_dwExtendedStyleFlags*/	//+++ 追加
){
	int nCount = tabMDI.GetItemCount();
	if ( !CMainOption::IsQualify( nCount ) )
		return NULL;

	if (dwDLFlags == (DWORD)-1)
		dwDLFlags = CDLControlOption::s_dwDLControlFlags;

	if (dwESFlags == (DWORD)-1)									//+++
		dwESFlags = CDLControlOption::s_dwExtendedStyleFlags;	//+++

	//ATLTRY(CChildFrame* pChild = new CChildFrame(tabMDI, adBar, bNewWindow2, dwDLFlags));
	CChildFrame *pChild = new CChildFrame(tabMDI, adBar, bNewWindow2, dwDLFlags, dwESFlags);

	//んー?
	if (pChild != NULL) {
		pChild->CreateEx(hWndMDIClient);
		//+++ もう直接やりとりで...
		//CDonutSearchBar *pSearch = (CDonutSearchBar *) ::SendMessage(pChild->GetTopLevelParent(), WM_GET_SEARCHBAR, 0, 0);
		CDonutSearchBar *pSearch = CDonutSearchBar::GetInstance();
		if (pSearch) {
			pChild->m_strSearchWord = pSearch->RemoveShortcutWord( pSearch->GetSearchStr() );
		}
	}

	return pChild;
}

void CChildFrame::OnHoge(){

}

//void CChildFrame::OnStatusTextChange(const CString &strText){}

//void CChildFrame::OnProgressChange(long progress, long progressMax)[}

void CChildFrame::OnDownloadBegin(){}

//void CChildFrame::OnDownloadComplete(){}

//void CChildFrame::OnTitleChange(const CString &strTitle){}

void CChildFrame::OnNavigateComplete2(nsISupports *pSupp, const CString &strURL){}

void CChildFrame::OnBeforeNavigate(const CString& strURL, bool &cancel)
{

	{
		//プラグインイベント - ナビゲート前
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = m_MDITab.GetTabIndex(m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) NULL/*strTargetFrameName*/;
		int 	nRet		 = CPluginManager::ChainCast_PluginEvent(DEVT_TAB_BEFORENAVIGATE, stn.nIndex, (SPARAM) &stn);

		if (nRet == -1) {
			//ナビゲートキャンセル
			cancel = true;
			return;
		} else if (nRet == -2) {
			//クローズ
			cancel 	  = true;
			m_bNewWindow2 = false;
			m_bClosing	  = true;
			PostMessage(WM_CLOSE);		// It's possible to post twice, but don't care.
			return;
		}
	}

	if (m_view.m_ViewOption.m_dwExStyle & DVS_EX_BLOCK_MAILTO) {
		CString strMailto = strURL.Left(6);
		strMailto.MakeUpper();

		if (strMailto.Compare( _T("MAILTO") ) == 0) {
			cancel = true;
			return;
		}
	}

	if (m_bClosing) {
		cancel = true;
		return;
	}

	// deter popups
	//COMMENT IsPageIWebBrowser
	if ( m_bNewWindow2 && /*IsPageIWebBrowser(pDisp) &&*/ CIgnoredURLsOption::SearchString(strURL) ) {	// close ignored url
		cancel 	  = true;
		m_bNewWindow2 = false;
		m_bClosing	  = true;
		PostMessage(WM_CLOSE);			// It's possible to post twice, but don't care.

		{
			//プラグインイベント - ポップアップ抑止
			CPluginManager::BroadCast_PluginEvent(DEVT_BLOCKPOPUP, (FPARAM) (LPCTSTR) strURL, 0);
		}
		return;
	}

	// javascriptから始まるNewWindowはclose
	if (m_bNewWindow2) {
		CString strJava = strURL.Left(10);
		strJava.MakeUpper();

		if (strJava.Find(_T("JAVASCRIPT")) != -1) {
			cancel 	  = true;
			m_bNewWindow2 = false;
			m_bClosing	  = true;
			PostMessage(WM_CLOSE);		// It's possible to post twice, but don't care.

			DonutOpenFile(m_hWnd, strURL);
			return;
		}
	}

  #if 1	//+++ Url別拡張プロパティ設定の処理. 主にリンク用.(通常のopenの場合は、CMainFrame::OnUserOpenFileな処理を通るので、そちらで設定ずみ)
	DWORD exopts = 0xFFFFFFFF, dlCtlFlg = 0xFFFFFFFF, exstyle = 0xFFFFFFFF, autoRefresh = 0xFFFFFFFF, dwExPropOpt = 8;
	if (CUrlSecurityOption::FindUrl( strURL, &exopts, &dwExPropOpt, 0 )) {
		CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, exopts, dwExPropOpt);
		dlCtlFlg	= ExProp.GetDLControlFlags();
		exstyle		= ExProp.GetExtendedStyleFlags();
		autoRefresh = ExProp.GetAutoRefreshFlag();
	}
  #endif

	// Navigate lock
	if ( !m_bExPropLock
	   && _check_flag(DVS_EX_OPENNEWWIN, m_view.m_ViewOption.m_dwExStyle)
	   && m_bWaitNavigateLock == false
	   && !IsRefreshBeforeNavigate() //TODO: plus7: 本当に必要か？IE限定では？
)
	{
		m_bExPropLock = FALSE;
		dcfTRACE( _T(" Locked, open new window\n") );
		if (dlCtlFlg == 0xFFFFFFFF)
			dlCtlFlg =  _GetInheritedDLControlFlags();
		if (exstyle  == 0xFFFFFFFF)			//+++
			exstyle  = _GetInheritedExtendedStyleFlags();	//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...
		CChildFrame *pChild = CChildFrame::NewWindow( m_hWndMDIClient, m_MDITab, m_AddressBar, false, dlCtlFlg, exstyle );
		if (pChild) {
			pChild->ActivateFrameForNewWindowDelayed();
			pChild->Navigate2(strURL);
			pChild->m_strSearchWord = m_strSearchWord;
		  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
			pChild->m_bNowHilight	= m_bNowHilight;
		  #endif
			pChild->ForceMessageLoop(); 	//*+++

		  #if 1	//+++ url別拡張プロパティの処理.
			if (exopts != 0xFFFFFFFF) {
				pChild->view().PutDLControlFlags( dlCtlFlg );
				pChild->SetViewExStyle( exstyle );
				pChild->view().m_ViewOption.SetAutoRefreshStyle( autoRefresh );
			}
		  #endif
		}
		cancel 	  = true;
		return;
	}

  #if 1	//+++ url別拡張プロパティの処理....
		//+++	戻る・進むでの拡張プロパティ情報の頁ごとの保存ができていないので、破綻する...
		//+++	が、バグっても反映されることのほうが意味ありそうなので利用
	if (CUrlSecurityOption::s_bValid) {
		if (exopts != 0xFFFFFFFF && CUrlSecurityOption::activePageToo()) {
			this->view().PutDLControlFlags( dlCtlFlg );
			this->view().m_ViewOption.SetAutoRefreshStyle( autoRefresh );
			this->SetViewExStyle( exstyle );	//+++メモ: マウス中ボタンクリックでの、リンク別タブ表示の場合、まだタブ位置未決定のため設定できない...
		//} else {
		}
	}
  #endif

	m_bExPropLock	= FALSE;

	// Note: Some ActivateFrame would go mad, because MDI client window not setup yet.
	if (m_bNewWindow2) {				// delayed activation if OnNewWindow2 called
		ActivateFrameForNewWindowDelayed();
		m_bNewWindow2 = false;
	}

	// UH
	// ハイライト失踪･･･
  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	//m_bNowHilight=FALSE;	// gae: 一度適用したハイライトは明示的に解除されるまで維持される
  #else
	m_bNowHilight	= FALSE;
  #endif
	m_nPainBookmark = 0;
	m_strBookmark	= LPCOLESTR(NULL);

	{
		//イベント発生 - ナビゲート
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = m_MDITab.GetTabIndex(m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) NULL /*strTargetFrameName;*/;
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_NAVIGATE, stn.nIndex, (SPARAM) &stn);
	}
}

LRESULT CChildFrame::OnCreate(LPCREATESTRUCT lpcreatestruct)
{
	dcfTRACE( _T("CChildFrame::OnCreate\n") );

	//RtlSetMinProcWorkingSetSize();				//+++ ( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 ) ... ひょっとするとあまりよろしくないかもだが... やっぱり起動時はあまりよろしくないかもで、やめにしとく...

	// Let me initialize itself
	LRESULT 	lRet		   = DefWindowProc();

	//TODO: Replace with a URL of your choice
	CRect		rc			   = rcDefault;
	__CalcDefaultRect(rc);
	m_hWndClient = m_view.Create(m_hWnd, rc, _T("about:blank"), WS_CHILD | WS_VISIBLE  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, ID_DONUTVIEW);

	//ドラッグドロップの処理を設定
	_InitDragDropSetting();

    m_spBrowser = m_view.m_spBrowser;    

	m_view.AttachEventListener(this);    

/*	if ( _check_flag(MAIN_EX_REGISTER_AS_BROWSER, CMainOption::s_dwMainExtendedStyle) )
		SetRegisterAsBrowser(true); 	// for target name resolution
*/

	_SetupWindowCloseHook();

	// set up for mdi tab ctrl

	CMenuHandle menu0		   = m_MDITab.m_menuPopup.GetSubMenu(0);
	HMENU		hMenu		   = menu0.GetSubMenu(_nPosFavoriteMenuOfTab);
	m_FavMenu.InstallExplorerMenu(hMenu);

	if (!m_bNewWindow2)
		SetVisible(true);

	CFavoritesMenuOption::Add(m_hWnd);

	m_japanCharSet = -1;

	//m_ieToolBar    = std::auto_ptr<CIEToolBar>(new CIEToolBar(((CWindow*)g_pMainWnd)->m_hWnd/*GetTopLevelParent()*/, m_spBrowser.p, _T("{625AA53D-1F10-44FE-B907-91FE25220D3F}")));	//+++ 実験.
	//m_ieToolBar    = std::auto_ptr<CIEToolBar>(new CIEToolBar(((CWindow*)g_pMainWnd)->m_hWnd/*GetTopLevelParent()*/, m_spBrowser.p, _T("32099AAC-C132-4136-9E9A-4E364A424E17")));	//+++ 実験.

	return lRet;
}



void CChildFrame::__CalcDefaultRect(CRect &rc)
{
	BOOL bMaximized = FALSE;
	HWND hWndActive = MDIGetActive(&bMaximized);

	if (hWndActive == NULL || bMaximized) {
		::GetClientRect(m_hWndMDIClient, &rc);
	}
}



void CChildFrame::_SetupWindowCloseHook()
{
#if COMMENT
	HWND hWndBrowser;

	::EnumChildWindows(m_view.m_hWnd, (WNDENUMPROC) _EnumProc, (LPARAM) &hWndBrowser);
	ATLASSERT( ::IsWindow(hWndBrowser) );
	m_wndBrowser.SubclassWindow(hWndBrowser);
#endif
}



//static
BOOL CALLBACK CChildFrame::_EnumProc(HWND hWnd, LPARAM lParam)
{
	HWND *phWnd = (HWND *) lParam;
	ATLASSERT( phWnd != 0 );
	*phWnd = hWnd;
	return FALSE;
}



void CChildFrame::_InitDragDropSetting()
{
	CIniFileI	pr( _GetFilePath("MouseEdit.ini"), _T("MouseCtrl") );
	DWORD			dwCommand = pr.GetValue(_T("DragDrop"), 0);
	m_bOperateDrag = (dwCommand != 0);
	pr.Close();
	if (m_bOperateDrag) {
		m_view.SetOperateDragDrop(TRUE, dwCommand);
	}
}



//public:
void CChildFrame::OnBeforeNavigate2(
					IDispatch *			pDisp,
					const CString &		strURL,
					DWORD				nFlags,
					const CString &		strTargetFrameName,
					CSimpleArray<BYTE>&	baPostedData,
					const CString &		strHeaders,
					bool &				bCancel )
{
	dcfTRACE(_T("CChildFrame::BeforeNavigate2 url(%s) frame(%s)\n"), strURL.Left(100), strTargetFrameName);

  #if 1	//+++ 自動リサイズ可能なページに新たに絵でないページを表示すると、そのページもリサイズ可能になってしまっていたのを対処.
	if (m_nImgWidth >= 0) {
		m_nImgWidth			= -1;		//+++ 拡張子(再)チェックのために、初期化
		m_bImg				= 0;
		m_bImageAutoSizeReq = 1;
	}
  #endif

	{
		//プラグインイベント - ナビゲート前
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = m_MDITab.GetTabIndex(m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) strTargetFrameName;
		int 	nRet		 = CPluginManager::ChainCast_PluginEvent(DEVT_TAB_BEFORENAVIGATE, stn.nIndex, (SPARAM) &stn);

		if (nRet == -1) {
			//ナビゲートキャンセル
			bCancel = true;
			return;
		} else if (nRet == -2) {
			//クローズ
			bCancel 	  = true;
			m_bNewWindow2 = false;
			m_bClosing	  = true;
			PostMessage(WM_CLOSE);		// It's possible to post twice, but don't care.
			return;
		}
	}

	if (m_view.m_ViewOption.m_dwExStyle & DVS_EX_BLOCK_MAILTO) {
		CString strMailto = strURL.Left(6);
		strMailto.MakeUpper();

		if (strMailto.Compare( _T("MAILTO") ) == 0) {
			bCancel = true;
			return;
		}
	}

	if (m_bClosing) {
		bCancel = true;
		return;
	}

	// deter popups
	//COMMENT IsPageIWebBrowser
	if ( m_bNewWindow2 && /*IsPageIWebBrowser(pDisp) &&*/ CIgnoredURLsOption::SearchString(strURL) ) {	// close ignored url
		bCancel 	  = true;
		m_bNewWindow2 = false;
		m_bClosing	  = true;
		PostMessage(WM_CLOSE);			// It's possible to post twice, but don't care.

		{
			//プラグインイベント - ポップアップ抑止
			CPluginManager::BroadCast_PluginEvent(DEVT_BLOCKPOPUP, (FPARAM) (LPCTSTR) strURL, 0);
		}
		return;
	}

	// javascriptから始まるNewWindowはclose
	if (m_bNewWindow2) {
		CString strJava = strURL.Left(10);
		strJava.MakeUpper();

		if (strJava.Find(_T("JAVASCRIPT")) != -1) {
			bCancel 	  = true;
			m_bNewWindow2 = false;
			m_bClosing	  = true;
			PostMessage(WM_CLOSE);		// It's possible to post twice, but don't care.

			DonutOpenFile(m_hWnd, strURL);
			return;
		}
	}

  #if 1	//+++ Url別拡張プロパティ設定の処理. 主にリンク用.(通常のopenの場合は、CMainFrame::OnUserOpenFileな処理を通るので、そちらで設定ずみ)
	DWORD exopts = 0xFFFFFFFF, dlCtlFlg = 0xFFFFFFFF, exstyle = 0xFFFFFFFF, autoRefresh = 0xFFFFFFFF, dwExPropOpt = 8;
	if (CUrlSecurityOption::FindUrl( strURL, &exopts, &dwExPropOpt, 0 )) {
		CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, exopts, dwExPropOpt);
		dlCtlFlg	= ExProp.GetDLControlFlags();
		exstyle		= ExProp.GetExtendedStyleFlags();
		autoRefresh = ExProp.GetAutoRefreshFlag();
	}
  #endif

	// Navigate lock
	if ( !m_bExPropLock
	   && _check_flag(DVS_EX_OPENNEWWIN, m_view.m_ViewOption.m_dwExStyle)
	   && m_bWaitNavigateLock == false
	   && !IsRefreshBeforeNavigate() //TODO: plus7: 本当に必要か？IE限定では？
)
	{
		m_bExPropLock = FALSE;
		dcfTRACE( _T(" Locked, open new window\n") );
		if (dlCtlFlg == 0xFFFFFFFF)
			dlCtlFlg =  _GetInheritedDLControlFlags();
		if (exstyle  == 0xFFFFFFFF)			//+++
			exstyle  = _GetInheritedExtendedStyleFlags();	//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...
		CChildFrame *pChild = CChildFrame::NewWindow( m_hWndMDIClient, m_MDITab, m_AddressBar, false, dlCtlFlg, exstyle );
		if (pChild) {
			pChild->ActivateFrameForNewWindowDelayed();
			pChild->Navigate2(strURL);
			pChild->m_strSearchWord = m_strSearchWord;
		  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
			pChild->m_bNowHilight	= m_bNowHilight;
		  #endif
			pChild->ForceMessageLoop(); 	//*+++

		  #if 1	//+++ url別拡張プロパティの処理.
			if (exopts != 0xFFFFFFFF) {
				pChild->view().PutDLControlFlags( dlCtlFlg );
				pChild->SetViewExStyle( exstyle );
				pChild->view().m_ViewOption.SetAutoRefreshStyle( autoRefresh );
			}
		  #endif
		}
		bCancel 	  = true;
		return;
	}

  #if 1	//+++ url別拡張プロパティの処理....
		//+++	戻る・進むでの拡張プロパティ情報の頁ごとの保存ができていないので、破綻する...
		//+++	が、バグっても反映されることのほうが意味ありそうなので利用
	if (CUrlSecurityOption::s_bValid) {
		if (exopts != 0xFFFFFFFF && CUrlSecurityOption::activePageToo()) {
			this->view().PutDLControlFlags( dlCtlFlg );
			this->view().m_ViewOption.SetAutoRefreshStyle( autoRefresh );
			this->SetViewExStyle( exstyle );	//+++メモ: マウス中ボタンクリックでの、リンク別タブ表示の場合、まだタブ位置未決定のため設定できない...
		//} else {
		}
	}
  #endif

	m_bExPropLock	= FALSE;

	// Note: Some ActivateFrame would go mad, because MDI client window not setup yet.
	if (m_bNewWindow2) {				// delayed activation if OnNewWindow2 called
		ActivateFrameForNewWindowDelayed();
		m_bNewWindow2 = false;
	}

	// UH
	// ハイライト失踪･･･
  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
	//m_bNowHilight=FALSE;	// gae: 一度適用したハイライトは明示的に解除されるまで維持される
  #else
	m_bNowHilight	= FALSE;
  #endif
	m_nPainBookmark = 0;
	m_strBookmark	= LPCOLESTR(NULL);

	{
		//イベント発生 - ナビゲート
		DEVTS_TAB_NAVIGATE stn;
		stn.nIndex			 = m_MDITab.GetTabIndex(m_hWnd);
		stn.lpstrURL		 = (LPCTSTR) strURL;
		stn.lpstrTargetFrame = (LPCTSTR) strTargetFrameName;
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_NAVIGATE, stn.nIndex, (SPARAM) &stn);
	}
}



DWORD CChildFrame::_GetInheritedDLControlFlags()
{
	DWORD dwDLFlags;

	if (this &&  _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) )
		dwDLFlags = m_view._GetDLControlFlags();
	else
		dwDLFlags = CDLControlOption::s_dwDLControlFlags;

	// default
	return dwDLFlags;
}


//+++ 新規に子供を作るときの、その子供に渡す ExtendedStyle を求める...
DWORD CChildFrame::_GetInheritedExtendedStyleFlags()
{
	DWORD dwExFlags;

	if (this &&  _check_flag(MAIN_EX_INHERIT_OPTIONS, CMainOption::s_dwMainExtendedStyle) ) {
		dwExFlags = m_view._GetExtendedStypeFlags();
	  #if 1	//+++ ナビゲートロックに関しては継承しない....
		dwExFlags &= ~DVS_EX_OPENNEWWIN;											//+++ off
		dwExFlags |= CDLControlOption::s_dwExtendedStyleFlags & DVS_EX_OPENNEWWIN;	//+++ けど、デフォルト設定があれば、それを反映.
	  #endif
	} else {
		dwExFlags = CDLControlOption::s_dwExtendedStyleFlags;
	}

	// default
	return dwExFlags;
}



int CChildFrame::ActivateFrameForNewWindowDelayed(int nCmdShow /*= -1*/)
{
	dcfTRACE( _T("CChildFrame::ActivateFrameForNewWindowDelayed - now activating!\n") );

	HWND	hWndActive = MDIGetActive();
	if (hWndActive != NULL && CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE_NEWWIN)
		nCmdShow = SW_SHOWNOACTIVATE;

	return ActivateFrame(nCmdShow);
}



int CChildFrame::ActivateFrame(int nCmdShow /*= -1*/)
{
	ATLASSERT( ::IsWindow(m_hWnd) );
	if (m_MDITab.GetTabIndex(m_hWnd) == -1) 	//なんだかMDITabのMDIActivateの方が先に呼び出されてしまうので
		m_MDITab.OnMDIChildCreate(m_hWnd);		//いろいろと順番が狂っている

	if ( _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)) {
		m_MDITab.NavigateLockTab(m_hWnd, true);
	}

	//Raise Plugin Event
	int nNewIndex = m_MDITab.GetTabIndex(m_hWnd);
	CPluginManager::ChainCast_PluginEvent(DEVT_TAB_OPENED, nNewIndex, 0);

	return baseClass::ActivateFrame(nCmdShow);
}


void CChildFrame::OnDocumentComplete(nsISupports *pSupp, const CString &strURL)
{
	if ( true/*IsPageIWebBrowser(pDisp)*/ ) {	// it means a page complete downloading	//COMMENT IsPageIWebBrowser
		dcfTRACE( _T("CChildFrame::OnDocumentComplete\n") );
		m_bWaitNavigateLock = false;	// you are allowed!
		m_bExPropLock		= FALSE;

		if ( MDIGetActive() == m_hWnd && DonutBrowserCanSetFocus(m_hWnd) ) {
			_SetPageFocus();
		}
	}

	//戻る・進むの履歴をロードする
	SetTravelLogData();

	//オートハイライト
	if (m_bAutoHilight)
		CDonutSearchBar::GetInstance()->ForceSetHilightBtnOn(true);

	bool bHilight = m_bAutoHilight || CDonutSearchBar::GetInstance()->GetHilightSw();
	//if (bHilight && !m_bNowHilight)
	if (bHilight)
	{

	  #if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
		if (!m_strSearchWord.IsEmpty()){
			nsCOMPtr<nsIWebBrowser> wb = do_QueryInterface(pSupp);
			if(wb) OnHilightOnce(wb, m_strSearchWord);
		}
	  #else
		if (!m_strSearchWord.IsEmpty())
			OnHilight(m_strSearchWord);
	  #endif
	}

	//::PostMessage(GetTopLevelParent(), WM_COMMAND, (WPARAM)ID_SEARCHBAR_HILIGHT, 0);

	{
		//プラグインイベント - ロード完了
		nsCOMPtr<nsIWebBrowser> wb = do_QueryInterface(pSupp);
		CGeckoBrowser b = wb;
		if (wb) {
			DEVTS_TAB_DOCUMENTCOMPLETE	dc;
			dc.lpstrURL   = (LPCTSTR) strURL;
			dc.lpstrTitle = (LPCTSTR) b.GetLocationName();
			dc.nIndex	  = m_MDITab.GetTabIndex(m_hWnd);
			dc.bMainDoc   = true/*IsPageIWebBrowser(pDisp);*/; //COMMENT IsPageIWebBrowser
			CPluginManager::BroadCast_PluginEvent(DEVT_TAB_DOCUMENTCOMPLETE, dc.nIndex, (DWORD_PTR) &dc);
		}
	}
}



void CChildFrame::OnDownloadComplete()
{
	dcfTRACE( _T("CChildFrame::OnDownloadComplete\n") );
}



void CChildFrame::OnStatusTextChange(const CString &strText)
{
	{
		//プラグインイベント - ステータステキスト変更
		int nRet = CPluginManager::ChainCast_PluginEvent(DEVT_CHANGESTATUSTEXT, (FPARAM) (LPCTSTR) strText, 0);
		if (nRet < 0)
			return;
	}

	BOOL bFilter = FALSE;
	if (m_view.m_ViewOption.m_dwExStyle & DVS_EX_MESSAGE_FILTER) {
		if (strText.Find(_T("http"  )) != -1)						bFilter = TRUE;
		if (strText.Find(_T("ftp"	)) != -1)						bFilter = TRUE;
		if (strText.Find(_T("接続"  )) != -1)						bFilter = TRUE;
		if (strText.Find(_T("mailto")) != -1)						bFilter = TRUE;
		if (strText.Find(_T("file"  )) != -1)						bFilter = TRUE;
		if (strText.Find(_T("java"  )) != -1)						bFilter = TRUE;
		if ( strText.IsEmpty()	 )									bFilter = TRUE;
		if (strText.Compare(_T("ページが表示されました")) == 0) 	bFilter = TRUE; 	//minit
		if (bFilter == FALSE)										return;
	}

	if (m_strStatusBar != strText)
		m_strStatusBar = strText;

	if (MDIGetActive() == m_hWnd) {
		CWindow wndFrame   = GetTopLevelParent();
		HWND	hWndStatus = wndFrame.GetDlgItem(ATL_IDW_STATUS_BAR);
		int 	nMode	   = (int) ::SendMessage(hWndStatus, WM_GET_OWNERDRAWMODE, 0, 0);
		if (nMode)
			MtlSetStatusText(hWndStatus, strText);
		else
			MtlSetWindowText(hWndStatus, strText);

		// UDT DGSTR ( update
		LPCTSTR lpszText   = strText;
		SendMessage(GetTopLevelParent(), WM_UPDATE_TITLEBAR, (WPARAM) lpszText, (LPARAM) 0);
		// ENDE
	}
}



void CChildFrame::OnTitleChange(const CString &strTitle)
{
	CString strURL = GetLocationURL();

	if ( CCloseTitlesOption::SearchString(strTitle) ) {
		m_bClosing = true;
		PostMessage(WM_CLOSE);
		{
			//プラグインイベント - タイトル抑止
			CPluginManager::BroadCast_PluginEvent(DEVT_BLOCKTITLE, (FPARAM) (LPCTSTR) strTitle, 0);
		}
		return;
	}

	dcfTRACE( _T("OnTitleChange : %s\n"), strTitle.Left(100) );
	// change title
	SetWindowText( MtlCompactString(strTitle, 255) );

  #if 0 //+++ 強引に、ページ窓開け時のabout:blank設定をキャンセルする... +mod.1.17以前の不具合対策で、どうも以降では帰って不具合に化ける場合がありそなんで、止め.
	// change address bar
	if (MDIGetActive() == m_hWnd) {
		if ((strURL == "about:blank" || strURL.IsEmpty()) && IsWaitBeforeNavigate2Flag()) { //+++
			;
		} else {	//+++ 通常.
			m_AddressBar.SetWindowText(strURL);
		}
	}
  #else
	// change address bar
	if (MDIGetActive() == m_hWnd)
		m_AddressBar.SetWindowText(strURL);
  #endif
}


void CChildFrame::OnCommandStateChange(long Command, bool bEnable)
{
	if (Command == CSC_NAVIGATEBACK) {
		m_bNavigateBack 	= bEnable /*? TRUE : FALSE*/;
	} else if (Command == CSC_NAVIGATEFORWARD) {
		m_bNavigateForward	= bEnable /*? TRUE : FALSE*/;
	} else if (Command == CSC_UPDATECOMMANDS) {
	}
}


void CChildFrame::OnStateConnecting()
{
	m_MDITab.SetConnecting(m_hWnd);
	m_bPrivacyImpacted = TRUE;	// ページ読み込み後もOnStateConnectingが呼ばれるページがあるので
								// クッキー制限アイコンが表示されないページがある
	//m_nSecureLockIcon = secureLockIconUnsecure;
}


//void CChildFrame::OnStateDownloading()
//{
//	m_MDITab.SetDownloading(m_hWnd);
//}


void CChildFrame::OnStateCompleted()
{
	m_MDITab.SetComplete(m_hWnd);

	HWND hWndA2 = ::GetActiveWindow();
	HWND hWndF2 = ::GetFocus();

	if (m_hWndA)
		::SetActiveWindow(m_hWndA);

	if (m_hWndF)
		::SetFocus(m_hWndF);
}



#if 1 //+++ 強制的に、その場でメッセージをさばく...もう不要かも...
int CChildFrame::ForceMessageLoop()
{
	HWND	hWnd	= m_hWnd;
	HWND	hWndA = ::GetActiveWindow();
	if (hWnd != hWnd)
		return 1;
	int 	ret;
	MSG 	msg 	= {0};
	int 	n		= 0;
	while ((ret = ::PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE)) != 0) {
		if (!GetMessage (&msg,hWnd,0,0))	/* メッセージ処理. QUITだったら、速攻かえる*/
			return 0;	//x return msg.wParam ;
		if (OnForwardMsg(&msg, 0) == 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		++n;
	}
	return 1;
}
#endif



// ===========================================================================
// 終了処理

/** +++メモ:自分以外のタブを閉じる
 */
LRESULT CChildFrame::OnWindowCloseExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if ( !CDonutConfirmOption::OnCloseAllExcept( GetTopLevelParent() ) )
		return 0;

	CWaitCursor 		 cur;
	CLockRedrawMDIClient lock(m_hWndMDIClient);
	CLockRedraw 		 lock2(m_MDITab);
	MtlCloseAllMDIChildrenExcept(m_hWndMDIClient, m_hWnd);
	return 0;
}



/** +++メモ:タブの右側/左側をすべて閉じる
 */
LRESULT CChildFrame::OnLeftRightClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	bool	bLeft	= (wID == ID_LEFT_CLOSE);

	if ( !CDonutConfirmOption::OnCloseLeftRight( GetTopLevelParent(), bLeft ) )
		return 0;

	CWaitCursor cur;

	HWND		hWndActive = (HWND) ::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM) NULL);
	int 		nCurSel    = m_MDITab.GetTabIndex(m_hWnd);
	int 		nCurSel2   = m_MDITab.GetTabIndex(hWndActive);
	int 		nCurSel3   = m_MDITab.GetTabIndex(m_hWndMDIClient);

	if (nCurSel == -1)
		return 0;

	::SendMessage(GetTopLevelParent(), WM_USER_WINDOWS_CLOSE_CMP, (WPARAM) nCurSel, (LPARAM) bLeft);
	return 0;
}


/** 全てのタブを閉じる
 */
void CChildFrame::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	::SendMessage(::GetParent( ::GetParent(m_hWnd) ), WM_COMMAND, ID_WINDOW_CLOSE_ALL, 0);
}




///+++メモ:ブラウザを閉じる
void CChildFrame::OnBrowserClose()
{
	dcfTRACE( _T("CChildFrame::OnBrowserClose\n") );
	SetMsgHandled(FALSE);
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}


/** +++メモ:ファイルの閉じる.
 */
void CChildFrame::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
  #if 1 //+++ タブロック対策... OnClose側のだけで大丈夫のようだけれど、とりあえず.
	if ( s_bMainframeClose == 0 												//+++ unDonut終了時以外で
		&& _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)		//+++ ナビゲートロックのページで
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK)	//+++ ナビゲートロックのページを閉じない、の指定があれば
	){
		m_bClosing = false;
		return; 																//+++ 閉じずに帰る
	}
  #endif
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}



//======================

/** +++メモ:閉じる
 */
void CChildFrame::OnClose()
{
  #if 1 //+++ タブロック対策...
	if ( s_bMainframeClose == 0 												//+++ unDonut終了時以外で
		&& _check_flag(m_view.m_ViewOption.m_dwExStyle, DVS_EX_OPENNEWWIN)		//+++ ナビゲートロックのページで
		&& (CMainOption::s_dwMainExtendedStyle2 & MAIN_EX2_NOCLOSE_NAVILOCK)	//+++ ナビゲートロックのページを閉じない、の指定があれば
	){
		m_bClosing = false;
		return; 																//+++ 閉じずに帰る
	}
  #endif

	SetMsgHandled(FALSE);
	m_bClosing = true;

	int  nIndex 		= m_MDITab.GetTabIndex(m_hWnd);

	// for mdi tab ctrl
	HWND hWndActiveNext = NULL;

	if (MDIGetActive() == m_hWnd) {
		int nIndex = m_MDITab.ManageClose(m_hWnd);

		if (nIndex != -1) {
			hWndActiveNext = m_MDITab.GetTabHwnd(nIndex);
			ATLASSERT( ::IsWindow(hWndActiveNext) );
		}
	}

	m_MDITab.OnMDIChildDestroy(m_hWnd);

	if (hWndActiveNext)
		MDIActivate(hWndActiveNext);

	if ( !CMainOption::s_bAppClosing ||  !(CStartUpOption::s_dwFlags == CStartUpOption::STARTUP_LATEST) ) { 	//minit
		CString strURL	 = GetLocationURL();
		if ( !strURL.IsEmpty() ) {	//+++ 先にチェック.
			CString strTitle = GetLocationName();
			SDfgSaveInfo::List	arrFore;
			SDfgSaveInfo::List	arrBack;

			if (CMainOption::s_bTravelLogClose) {
				m_view.m_ViewOption._OutPut_TravelLogs(arrFore, arrBack);
			}
			if (CMainOption::s_pMru && strURL.Left(12) != _T("view-source:"))	//+++
				CMainOption::s_pMru->AddToList(strURL, &arrFore, &arrBack, strTitle);
		}
	}

	{
		//プラグインイベント - クローズ
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_CLOSE, nIndex, 0);
	}

	//+++ destroyへ //+++ DonutRAPTよりパク( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	//+++           RtlSetMinProcWorkingSetSize();
}


/** 終了開放
 */
void CChildFrame::OnDestroy()
{
	dcfTRACE( _T("CChildFrame::OnDestroy\n") );
	SetMsgHandled(FALSE);
	m_bClosing							  = true;

	if (m_bOperateDrag)
		m_view.SetOperateDragDrop(FALSE, 0);

//	SetRegisterAsBrowser(false);

	// m_view.DestroyWindow makes offline state reset, so
	// save global offline state
	bool bGlobalOffline = MtlIsGlobalOffline();

	// Before AtlAdviseSinkMap(this, false) destroy view,
	// cuz I wanna handle OnNewWindow2 event
	// And while destroying view, a meaningless WM_DRAWCLIPBOARD may be sent, so ignore it.

	CMainOption::s_bIgnoreUpdateClipboard = true;
	dcfTRACE( _T(" m_view.Destroy\n") );

	if ( m_view.IsWindow() )
		m_view.DestroyWindow();

	CMainOption::s_bIgnoreUpdateClipboard = false;

	dcfTRACE( _T(" DispEventUnadvise\n") );

	m_view.DetachEventListener();

	// restore global offline state
	MtlSetGlobalOffline(bGlobalOffline);

	//+++ DonutRAPTよりパク( メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等 )
	RtlSetMinProcWorkingSetSize();
}



CChildFrame::~CChildFrame()
{
	dcfTRACE( _T("CChildFrame::~CChildFrame\n") );

	if ( m_wndBrowser.IsWindow() )				// scary
		m_wndBrowser.UnsubclassWindow();

	ATLASSERT(m_hMenu == NULL); 				// no menu
}



// ===========================================================================
// 基本的な処理


int CChildFrame::_GetShowCmd()
{
	CWindowPlacement	wndpl;
	if ( m_hWnd && ::GetWindowPlacement(m_hWnd, &wndpl) ) {
		return wndpl.showCmd;
	}
	return -1;
}

#if 0	//+++	失敗
void CChildFrame::OnLButtonDown(WORD wparam, const WTL::CPoint& pt)
{
	CRect	rect;
	GetClientRect(&rect);
	if (rect.PtInRect(pt)) {
		MtlSendCommand(m_hWnd, ID_HTMLZOOM_100TOGLE);
	}
}
#endif

//+++
LRESULT CChildFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	BOOL	bMaxActive = FALSE;
	HWND	hWndActive = MDIGetActive(&bMaxActive);

	if (bMaxActive == FALSE)
		bMaxActive = ::IsZoomed(hWndActive) != 0;

	int 	nCmdType   = m_nCmdType;
	m_nCmdType = 0;

   #ifndef USE_DIET
	//CheckImageAutoSize(NULL, FALSE);		//+++ このタイミングで更新するのは不味いみたい.
	m_bImageAutoSizeReq = true;				//+++ 別のタイミングでサイズ調整するリクエスト
   #endif

	dcfTRACE(_T("CChildFrame::OnSize type(%d)\n"), wParam);

	if ( CMainOption::s_bTabMode && (wParam == SIZE_RESTORED || wParam == SIZE_MINIMIZED) ) {
		dcfTRACE( _T(" restored\n") );
		bHandled = TRUE;			// eat it!!
		return 0;
	} else if ( bMaxActive
				&& (   (nCmdType != SC_RESTORE && nCmdType != SC_MINIMIZE)
					&& (wParam == SIZE_RESTORED || wParam == SIZE_MINIMIZED) ) )
	{
		dcfTRACE( _T(" restored\n") );
		bHandled = TRUE;			// eat it!!
		return 0;
	} else {
		bHandled = FALSE;
		return -1;
	}
}


void CChildFrame::OnSysCommand(UINT uCmdType, CPoint pt)
{
  #if 1	//+++ CTRL(+SHIFT)+TABでのタブ移動の順番を見た目通りにする処理
	if ((m_MDITab.GetMDITabExtendedStyle() & MTB_EX_CTRLTAB_MDI) == 0) {	//+++ 元のMDI式にするのでなければ
		if (uCmdType == SC_PREVWINDOW) {
			m_MDITab.LeftTab();
			SetMsgHandled(TRUE);
			return;
		} else if (uCmdType == SC_NEXTWINDOW) {
			m_MDITab.RightTab();
			SetMsgHandled(TRUE);
			return;
		}
	}
  #endif

	m_nCmdType = uCmdType;

	if ( CMainOption::s_bTabMode && (uCmdType == SC_MINIMIZE || uCmdType == SC_RESTORE) )
		SetMsgHandled(TRUE);
	else
		SetMsgHandled(FALSE);
}



LRESULT CChildFrame::OnForwardMsg(LPMSG pMsg, DWORD)
{
  #if 1	//+++	画像表示のとき、窓内でクリックしたら、拡大率を切り替える
	if (m_bImg && m_bImgAuto_NouseLClk == 0 && pMsg->message == WM_LBUTTONDOWN /*&& pMsg->hwnd == m_hWnd*/) {
		CPoint 	pt;
		::GetCursorPos(&pt);
		CRect	rect;
		//CRect	rectB;
		::GetClientRect(pMsg->hwnd, &rect);
		//CClientDC dc(pMsg->hwnd);
		//::GetBoundsRect(dc, &rectB, 0/*DCB_RESET*/);
		ClientToScreen(&rect);
		rect.right -= GetSystemMetrics(SM_CXVSCROLL);
		rect.bottom-= GetSystemMetrics(SM_CYHSCROLL);
		if (rect.PtInRect(pt)) {
			MtlSendCommand(m_hWnd, ID_HTMLZOOM_100TOGLE);
		}
	}
  #endif

  #if 1	//+++	F5でリロードしたときに、ZOOMが戻されちゃうのを強制するために、無理やり調整
	if ( baseClass::PreTranslateMessage(pMsg) )
		return TRUE;
	LRESULT lr = m_view.PreTranslateMessage(pMsg);

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5 && m_nImgScl != 100) {
		m_bImageAutoSizeReq = true;
		//SendMessage(WM_PAINT);
	  #if _ATL_VER >= 0x700		//+++ atl3ではバグることになるけど、しかたなし
		SetTimer(1, 20, OncePaintReq);
	  #endif
	}

	//if (m_ieToolBar.get())						//+++ 実験.
	//	m_ieToolBar->PreTranslateMessage(pMsg); 	//+++ 実験.

	return lr;
  #else
	if ( baseClass::PreTranslateMessage(pMsg) )
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
  #endif
}


//+++ タイマーで無理やりまって、画面をリサイズで描画
void CALLBACK CChildFrame::OncePaintReq(HWND hWnd, UINT wparam, UINT_PTR lparam, DWORD )
{
	//m_bImageAutoSizeReq = true;
	::SendMessage(hWnd, WM_PAINT, wparam, lparam);
	::KillTimer(hWnd, 1);
}



// It will be a template method.
void CChildFrame::OnMDIActivate(HWND hwndChildDeact, HWND hwndChildAct)
{
	dcfTRACE( _T("CChildFrame::OnMDIActivate\n") );
	SetMsgHandled(FALSE);

	if (hwndChildAct == m_hWnd) {		// I'm activated
	  #if 0 //+++ 強引に、ページ窓開け時のabout:blank設定をキャンセルする... +mod.1.17以前の不具合対策で、どうも以降では帰って不具合に化ける場合がありそなんで、止め.
		CString strURL = GetLocationURL();
		if ((strURL == "about:blank" || strURL.IsEmpty()) && IsWaitBeforeNavigate2Flag()) { //+++
			;
		} else {	//+++ 通常.
			m_AddressBar.SetWindowText(strURL);
		}
	  #else
		m_AddressBar.SetWindowText( GetLocationURL() );
	  #endif
		_SetPageFocus();
		_RestoreFocus();
		return;
	}

	if (hwndChildDeact == m_hWnd) { 	// I'm deactivated
		_SaveFocus();
	}

	{
		//プラグインイベント - アクティブタブ変更
		int nIndexDeact = m_MDITab.GetTabIndex(hwndChildDeact);
		int nIndexAct	= m_MDITab.GetTabIndex(hwndChildAct);
		CPluginManager::BroadCast_PluginEvent(DEVT_TAB_CHANGEACTIVE, nIndexDeact, nIndexAct);
	}
}



///+++ メモ:ID_VIEW_SETFOCUS:"現在のページのフォーカスを初期化します。フォーカスのリセット"
void CChildFrame::OnViewSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// NOTE. _SetPageFoucs would fail if document not completed.
	m_bPageFocusInitialized = false;			// reset focus
	_SetPageFocus();
}



void CChildFrame::_SetPageFocus()
{
	if (m_bPageFocusInitialized)
		return;

	dcfTRACE( _T("CChildFrame::_SetPageFocus(src:%d)\n"), ::GetFocus() );

	if ( !MtlIsApplicationActive(m_hWnd) )
		return;

	if ( IsBrowserNull() )
		return;

	nsCOMPtr<nsIWebBrowserFocus> focus = do_QueryInterface(m_spBrowser);
    nsresult rv = focus->Activate();

	dcfTRACE( _T("focus->Activate(): %d\n"), rv );

	nsCOMPtr<nsIDOMWindow> win;
	m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));

	focus->SetFocusAtFirstElement();

	/*nsCOMPtr<nsIWindowWatcher> ww = do_GetService("@mozilla.org/embedcomp/window-watcher;1");
	if(!ww) return;

	nsCOMPtr<nsIWebBrowserChrome> chrome;
	ww->GetChromeForWindow(win, getter_AddRefs(chrome));
	if(!chrome) return ;

	nsCOMPtr<nsIEmbeddingSiteWindow> siteWindow = do_QueryInterface(chrome);
	if(!siteWindow) return ;

	HWND *hWnd;
	siteWindow->GetSiteWindow((void**)&hWnd);

	::SetFocus(*hWnd);*/

	m_bPageFocusInitialized = true;
}






void CChildFrame::_RestoreFocus()
{
	if (m_hWndFocus && IsChild(m_hWndFocus) && ::GetFocus() != m_hWndFocus)
	{
		::SetFocus(m_hWndFocus);
	}
}

// ===========================================================================
// 個別関係


#if 1	//+++ url別拡張プロパティの処理.
void CChildFrame::SetUrlSecurityExStyle(LPCTSTR lpszFile)
{
	DWORD opts  = 0;
	DWORD opts2 = 0;
	if (CUrlSecurityOption::FindUrl( lpszFile, &opts, &opts2, 0 )) {
		CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, opts, opts2);
		this->view().PutDLControlFlags( ExProp.GetDLControlFlags() );
		this->SetViewExStyle(ExProp.GetExtendedStyleFlags(), TRUE);
		this->view().m_ViewOption.SetAutoRefreshStyle( ExProp.GetAutoRefreshFlag() );
	}
}
#endif



/**+++メモ:ID_VIEW_REFRESH:"現在のページのコンテンツを最新の情報に更新します。"
 */
void CChildFrame::OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (MDIGetActive() == m_hWnd) {
		m_hWndA = NULL;
		m_hWndF = NULL;
	} else {
		m_hWndA = ::GetActiveWindow();
		m_hWndF = ::GetFocus();
	}

	if (::GetAsyncKeyState(VK_CONTROL) < 0) 	// Inspired by DOGSTORE, Thanks
		Refresh2(REFRESH_COMPLETELY);
	else
		Refresh();
}



/**+++メモ:ID_VIEW_STOP:"現在のページの読み込みを中止します。"
 */
void CChildFrame::OnViewStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	Stop();
	// make sure
	//TODO: m_nDownloadCounter = 0;
	OnStateCompleted();
}

/**+++メモ:ID_FAVORITE_ADD:"現在のページをお気に入りの一覧に追加します。"
 */
void CChildFrame::OnFavoriteAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	bool bOldShell = _check_flag(MAIN_EX_ADDFAVORITEOLDSHELL, CMainOption::s_dwMainExtendedStyle);

	MtlAddFavorite(GetLocationURL(), MtlGetWindowText(m_hWnd), bOldShell, DonutGetFavoritesFolder(), m_hWnd);

	::SendMessage(GetTopLevelParent(), WM_REFRESH_EXPBAR, 0, 0);
}


/**+++メモ:ID_FAVORITE_GROUP_ADD:"このページをお気に入りグループに追加します。"
 */
void CChildFrame::OnFavoriteGroupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	TCHAR		szOldPath[MAX_PATH];			// save current directory
	szOldPath[0]	= 0;	//+++
	::GetCurrentDirectory(MAX_PATH, szOldPath);

	CString		dir	= DonutGetFavoriteGroupFolder();
	::SetCurrentDirectory( dir );

	const TCHAR szFilter[] = _T("Donut Favorite Groupファイル(*.dfg)\0*.dfg\0\0");
	CFileDialog fileDlg(FALSE, _T("dfg"), NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrInitialDir = dir;
	fileDlg.m_ofn.lpstrTitle	  = _T("お気に入りグループに追加");

	if (fileDlg.DoModal() == IDOK) {
		_AddGroupOption(fileDlg.m_szFileName);
		::SendMessage(GetTopLevelParent(), WM_REFRESH_EXPBAR, 1, 0);
	}

	// restore current directory
	::SetCurrentDirectory(szOldPath);
}



/**+++メモ：お気に入りグループに追加ページのオプション情報を追加
 */
int  CChildFrame::_AddGroupOption(const CString& strFileName)
{
	CIniFileIO	pr( strFileName, _T("Header") );
	DWORD		dwCount = pr.GetValue( _T("count"), 0 );
	OnSaveOption(strFileName, dwCount);
	++dwCount;
	pr.SetValue( dwCount, _T("count") );
	return dwCount;
}



/** 子窓のオプション情報をiniにセーブ
 */
void CChildFrame::OnSaveOption(LPCTSTR lpszFileName, int nIndex)
{
	m_view.m_ViewOption.WriteProfile(lpszFileName, nIndex);
}



//vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// U.H
/**+++メモ:ID_WINDOW_REFRESH_EXCEPT:"このウィンドウ以外を更新"
 */
LRESULT CChildFrame::OnWindowRefreshExcept(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CWaitCursor 		 cur;
	CLockRedrawMDIClient lock(m_hWndMDIClient);
	CLockRedraw 		 lock2(m_MDITab);

	MtlRefreshAllMDIChildrenExcept(m_hWndMDIClient, m_hWnd);
	return 0;
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


/**+++メモ:お気に入りバーの更新
 */
LRESULT CChildFrame::OnViewRefreshFavBar(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	m_FavMenu.SetRootDirectoryPath( DonutGetFavoritesFolder() );
	m_FavMenu.RefreshMenu();
	return 0;
}


/**+++メモ:ID_EDIT_IGNORE:"ポップアップ抑止に追加して閉じます。"
 */
void CChildFrame::OnEditIgnore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	dcfTRACE( _T("CChildFrame::OnEditIgnore\n") );
	CIgnoredURLsOption::Add( GetLocationURL() );
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}


// UDT DGSTR ( close Title , but don't post WM_CLOSE so far. )
/**+++ メモ:タイトル抑止に追加
 */
void CChildFrame::OnEditCloseTitle(WORD , WORD , HWND )
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CChildFrame::OnEditCloseTitle\n") );
	CCloseTitlesOption::Add( MtlGetWindowText(m_hWnd) );
	m_bClosing = true;
	PostMessage(WM_CLOSE);
}
// ENDE



/** +++メモ:nページ戻る
 */
void CChildFrame::OnViewBackX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_BACK1) + 1;
	CLockRedraw lock(m_hWnd);

	GoBack(nStep);
}



/** +++メモ:nページ進む
 */
void CChildFrame::OnViewForwardX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	int 		nStep = (wID - ID_VIEW_FORWARD1) + 1;
	CLockRedraw lock(m_hWnd);

    GoForward(nStep);
}



/**+++メモ:ID_TITLE_COPY:"タイトルをクリップボードにコピーします。"
 */
void CChildFrame::OnTitleCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(MtlGetWindowText(m_hWnd), m_hWnd);
}



/**+++メモ:ID_URL_COPY:"アドレスをクリップボードにコピーします。"
 */
void CChildFrame::OnUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	MtlSetClipboardText(GetLocationURL(), m_hWnd);
}



/**+++メモ:ID_COPY_TITLEANDURL:"タイトルとアドレスをクリップボードにコピーします。"
 */
void CChildFrame::OnTitleAndUrlCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	CString strText = MtlGetWindowText(m_hWnd) + _T("\r\n");

	strText += GetLocationURL();
	MtlSetClipboardText(strText, m_hWnd);
}



/** プログレスペインの幅を設定
 */
BOOL CChildFrame::SetProgressPaneWidth(int cxWidth)
{
  #if _ATL_VER >= 0x700
	CStatusBarCtrl statusbar   = GetTopLevelParent().GetDlgItem(ATL_IDW_STATUS_BAR);
  #else //+++
	CStatusBarCtrl statusbar   = CWindow( GetTopLevelParent() ).GetDlgItem(ATL_IDW_STATUS_BAR);
  #endif

	const int		nPanes	   = 5;
	//+++ プログレスペインとプロキシペインの交換時の不具合修正.
	int 			nIndex	   = (g_bSwapProxy == 0) ? 1 : 4;

	// get pane positions
	int *			pPanesPos  = (int *) _alloca( nPanes * sizeof (int) );

	statusbar.GetParts(nPanes, pPanesPos);

	// calculate offset
	int 		   cxPaneWidth = pPanesPos[nIndex] - ( (nIndex == 0) ? 0 : pPanesPos[nIndex - 1] );
	int 		   cxOff	   = cxWidth - cxPaneWidth;

	// find variable width pane
	int 		   nDef 	   = 0;

	// resize
	if (nIndex < nDef) {						// before default pane
		for (int i = nIndex; i < nDef; i++)
			pPanesPos[i] += cxOff;
	} else {									// after default one
		for (int i = nDef; i < nIndex; i++)
			pPanesPos[i] -= cxOff;
	}

	// set pane postions
	return	statusbar.SetParts(nPanes, pPanesPos) != 0;
}



// ==========================================================================
// UI

// Update Command UI Handlers


#ifndef NO_STYLESHEET
void CChildFrame::OnStyleSheetBaseUI(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	}
}
#endif


void CChildFrame::OnUpdateFontSmallestUI(CCmdUI *pCmdUI)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	nsCOMPtr<nsIContentViewer> cv;
	docShell->GetContentViewer(getter_AddRefs(cv));
	nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
	float zoom;
	mv->GetFullZoom(&zoom);
	pCmdUI->SetRadio(zoom == 0.5);
}


void CChildFrame::OnUpdateFontSmallerUI(CCmdUI *pCmdUI)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	nsCOMPtr<nsIContentViewer> cv;
	docShell->GetContentViewer(getter_AddRefs(cv));
	nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
	float zoom;
	mv->GetFullZoom(&zoom);
	pCmdUI->SetRadio(zoom == 0.75);
}


void CChildFrame::OnUpdateFontMediumUI(CCmdUI *pCmdUI)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	nsCOMPtr<nsIContentViewer> cv;
	docShell->GetContentViewer(getter_AddRefs(cv));
	nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
	float zoom;
	mv->GetFullZoom(&zoom);
	pCmdUI->SetRadio(zoom == 1.0);
}


void CChildFrame::OnUpdateFontLargerUI(CCmdUI *pCmdUI)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	nsCOMPtr<nsIContentViewer> cv;
	docShell->GetContentViewer(getter_AddRefs(cv));
	nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
	float zoom;
	mv->GetFullZoom(&zoom);
	pCmdUI->SetRadio(zoom == 1.5);
}


void CChildFrame::OnUpdateFontLargestUI(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) { // popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		nsCOMPtr<nsIContentViewer> cv;
		docShell->GetContentViewer(getter_AddRefs(cv));
		nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
		float zoom;
		mv->GetFullZoom(&zoom);
		pCmdUI->SetRadio(zoom == 2.0);
	}
}

void CChildFrame::OnUpdateStatusBarUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetText(m_strStatusBar);
}


void CChildFrame::OnUpdateSecureUI(CCmdUI *pCmdUI)
{
	CStatusBarCtrl wndStatus = pCmdUI->m_wndOther;

	if (m_nSecureLockIcon != secureLockIconUnsecure) {
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_SECURE_PANE, 0), 0);

		// ツールチップのセット
		CString tip = _T("SSL 保護付き ");

		switch (m_nSecureLockIcon) {
		case secureLockIconUnsecure:			tip  = "";					break;
		case secureLockIconMixed:				tip += _T("(複数）");		break;
		case secureLockIconSecureUnknownBits:	tip += _T("(不明）");		break;
		case secureLockIconSecure40Bit: 		tip += _T("(40ビット)");	break;
		case secureLockIconSecure56Bit: 		tip += _T("(56ビット)");	break;
		case secureLockIconSecureFortezza:		tip += _T("(Fortezza)");	break;
		case secureLockIconSecure128Bit:		tip += _T("(128ビット)");	break;
		default:								tip  = "";
		}
		wndStatus.SendMessage( WM_STATUS_SETTIPTEXT, (WPARAM) ID_SECURE_PANE, (LPARAM) tip.GetBuffer(0) );

	} else {
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_SECURE_PANE, -1), 0);
	}
}


void CChildFrame::OnUpdatePrivacyUI(CCmdUI *pCmdUI)
{
	CStatusBarCtrl wndStatus = pCmdUI->m_wndOther;

	if (m_bPrivacyImpacted == FALSE)
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_PRIVACY_PANE, 1), 0);
	else
		wndStatus.SendMessage(WM_STATUS_SETICON, MAKEWPARAM(ID_PRIVACY_PANE, -1), 0);

	wndStatus.SendMessage( WM_STATUS_SETTIPTEXT, (WPARAM) ID_PRIVACY_PANE, (LPARAM) _T("プライバシー レポート") );
}


///+++ メモ:ページ読み込みプログレスバーの状態更新.
void CChildFrame::OnProgressChange(long progress, long progressMax)
{
	m_nProgress    = progress;
	m_nProgressMax = progressMax;
}



///+++ メモ:ページ読み込みプログレスバーの表示
void CChildFrame::OnUpdateProgressUI(CCmdUI *pCmdUI)
{
	CProgressBarCtrl progressbar = pCmdUI->m_wndOther;
	CStatusBarCtrl	 statusbar	 = pCmdUI->m_wndOther.GetParent();


	if ( m_nProgress == -1 || (m_nProgress == 0 && m_nProgressMax == 0) ) {
		if (g_bSwapProxy == 0) {		//+++ 通常(左側にある)時のみ幅を0にする. プロキシと位置を交換してるときはやめ.
			SetProgressPaneWidth(0);
			progressbar.ShowWindow(SW_HIDE);
		} else {						//+++ 交換してるときは、とりあえず、長さ0で出しっぱなしで対処.
			progressbar.SetPos(0);		//+++ ただしゲージバー自体は消しておく
		}
		return;
	}

	CRect			 rcProgressPart;
	if (g_bSwapProxy == false)
		statusbar.GetRect(1, rcProgressPart);
	else
		statusbar.GetRect(4, rcProgressPart);

	rcProgressPart.DeflateRect(2, 2);
	progressbar.MoveWindow(rcProgressPart, TRUE);
	progressbar.SetRange32(0, m_nProgressMax);
	//x 0になっているときに毎フレームSetPos(0)するようにしたので、ここでは不要
	//x (というか、すこしでも最大値描画が行われるようにするため、無いほうがよい)
	//x if (m_nProgress >= m_nProgressMax && m_nProgressMax && g_bSwapProxy)
	//x progressbar.SetPos(0);
	//x else
	progressbar.SetPos(m_nProgress);
	progressbar.ShowWindow(SW_SHOWNORMAL);

	//+++ サイズ固定でオプション設定でのサイズが反映されていなかったのでそれを反映. グローバル変数なのはとりあえず...
	SetProgressPaneWidth( g_dwProgressPainWidth );
}



// ==========================================================================
// 拡張プロパティ

BYTE CChildFrame::GetTabItemState(HWND hTarWnd)
{
	int  nTabIndex = m_MDITab.GetTabIndex(hTarWnd);
	BYTE bytData   = 0;

	m_MDITab.GetItemState(nTabIndex, bytData);
	return bytData;
}



LRESULT CChildFrame::OnGetExtendedTabStyle()
{
	DWORD dwFlags		 = 0;

	DWORD dwDLFlags 	 = m_view.GetDLControlFlags();
	DWORD dwViewExStyle  = m_view.m_ViewOption.m_dwExStyle;
	DWORD dwRefreshStyle = m_view.m_ViewOption.m_dwAutoRefreshStyle;

	if (dwDLFlags & GECKO_DLCTL_IMAGES) 				dwFlags |= FLAG_SE_IMAGE;
	if (dwDLFlags & GECKO_DLCTL_FRAMES) 				dwFlags |= FLAG_SE_FRAME;
	if (dwDLFlags & GECKO_DLCTL_PLUGINS) 				dwFlags |= FLAG_SE_PLUGIN;
	if (dwDLFlags & GECKO_DLCTL_SCRIPTS) 				dwFlags |= FLAG_SE_SCRIPT;
	if (dwDLFlags & GECKO_DLCTL_REDIR) 					dwFlags |= FLAG_SE_REDIR;
	if (dwDLFlags & GECKO_DLCTL_DNS) 					dwFlags |= FLAG_SE_DNS;

	if (dwViewExStyle & DVS_EX_OPENNEWWIN)			dwFlags |= FLAG_SE_NAVIGATELOCK;
	if (dwViewExStyle & DVS_EX_FLATVIEW)			dwFlags |= FLAG_SE_FLATVIEW;
	if (dwViewExStyle & DVS_EX_MESSAGE_FILTER)		dwFlags |= FLAG_SE_MSGFILTER;
	if (dwViewExStyle & DVS_EX_MOUSE_GESTURE)		dwFlags |= FLAG_SE_MOUSEGESTURE;
	if (dwViewExStyle & DVS_EX_BLOCK_MAILTO)		dwFlags |= FLAG_SE_BLOCKMAILTO;

	BYTE bytState = GetTabItemState(m_hWnd);
	if ( !(bytState & TCISTATE_INACTIVE) )			dwFlags |= FLAG_SE_VIEWED;

	if (dwRefreshStyle == 0)
		dwFlags |= FLAG_SE_REFRESH_NONE;
	else
		dwFlags |= ( dwRefreshStyle * ( FLAG_SE_REFRESH_15 / DVS_AUTOREFRESH_15SEC) );

	return dwFlags;
}



void CChildFrame::OnSetExtendedTabStyle(DWORD dwStyle)
{
	DWORD dwDLFlags 	 = m_view.GetDLControlFlags();
	DWORD dwViewExStyle  = m_view.m_ViewOption.m_dwExStyle;
	DWORD dwRefreshStyle = ( dwStyle /	( FLAG_SE_REFRESH_15 / DVS_AUTOREFRESH_15SEC) ) & DVS_AUTOREFRESH_OR;

	if (dwStyle & FLAG_SE_IMAGE) 		dwDLFlags |=  GECKO_DLCTL_IMAGES;
	else								dwDLFlags &= ~GECKO_DLCTL_IMAGES;
	if (dwStyle & FLAG_SE_FRAME) 		dwDLFlags |=  GECKO_DLCTL_FRAMES;
	else								dwDLFlags &= ~GECKO_DLCTL_FRAMES;
	if (dwStyle & FLAG_SE_PLUGIN) 		dwDLFlags |=  GECKO_DLCTL_PLUGINS;
	else								dwDLFlags &= ~GECKO_DLCTL_PLUGINS;
	if (dwStyle & FLAG_SE_SCRIPT) 		dwDLFlags |=  GECKO_DLCTL_SCRIPTS;
	else								dwDLFlags &= ~GECKO_DLCTL_SCRIPTS;
	if (dwStyle & FLAG_SE_REDIR) 		dwDLFlags |=  GECKO_DLCTL_REDIR;
	else								dwDLFlags &= ~GECKO_DLCTL_REDIR;
	if (dwStyle & FLAG_SE_DNS) 	 		dwDLFlags |=  GECKO_DLCTL_DNS;
	else								dwDLFlags &= ~GECKO_DLCTL_DNS;

	if (dwStyle & FLAG_SE_NAVIGATELOCK) 	dwViewExStyle |=  DVS_EX_OPENNEWWIN;
	else									dwViewExStyle &= ~DVS_EX_OPENNEWWIN;
	if (dwStyle & FLAG_SE_FLATVIEW) 		dwViewExStyle |=  DVS_EX_FLATVIEW;
	else									dwViewExStyle &= ~DVS_EX_FLATVIEW;			//* +++ ここに ~ がないのはワザとなのかバグなのか? おそらくバグだろうで~しとく.
	if (dwStyle & FLAG_SE_MSGFILTER)		dwViewExStyle |=  DVS_EX_MESSAGE_FILTER;
	else									dwViewExStyle &= ~DVS_EX_MESSAGE_FILTER;
	if (dwStyle & FLAG_SE_MOUSEGESTURE) 	dwViewExStyle |=  DVS_EX_MOUSE_GESTURE;
	else									dwViewExStyle &= ~DVS_EX_MOUSE_GESTURE;
	if (dwStyle & FLAG_SE_BLOCKMAILTO)		dwViewExStyle |=  DVS_EX_BLOCK_MAILTO;
	else									dwViewExStyle &= ~DVS_EX_BLOCK_MAILTO;

	int nIndex = m_MDITab.GetTabIndex(m_hWndA);

	if (dwStyle & FLAG_SE_VIEWED)
		m_MDITab.SetItemActive(nIndex);
	else
		m_MDITab.SetItemInactive(nIndex);

	//ナビゲートロックの変更を反映させる
	// if ( (m_view.m_ViewOption.m_dwExStyle & DVS_EX_OPENNEWWIN) != (dwStyle & FLAG_SE_NAVIGATELOCK) )
	//+++ なにかよくわかりにくい条件式だ...DVS_EX_OPENNEWWIN=0x01, FLAG_SE_NAVIGATELOCK=0x80なので、
	//+++ !=が偽になるのは両辺が共に0のときのみ...なので、どちらかがonになっていればif文の中にはいる.
	//+++ とりあえず、そのように記述しなおす
	if ( (m_view.m_ViewOption.m_dwExStyle & DVS_EX_OPENNEWWIN) || (dwStyle & FLAG_SE_NAVIGATELOCK) )
	{
		m_MDITab.NavigateLockTab(m_hWnd, (dwStyle & FLAG_SE_NAVIGATELOCK) != 0/*? true : false*/);
	}

	m_MDITab.InvalidateRect(NULL);

	//フラグを変更する
	m_view.PutDLControlFlags(dwDLFlags);
	m_view.m_ViewOption.m_dwExStyle = dwViewExStyle;
	m_view.m_ViewOption.SetAutoRefreshStyle(dwRefreshStyle);

	//変更を適用するためにリフレッシュする
	//m_view.Navigate2(m_view.GetLocationURL());
	if ( !(dwStyle & FLAG_SE_NOREFRESH) )
		::SendMessage(m_hWnd, WM_COMMAND, (WPARAM) (ID_VIEW_REFRESH & 0xFFFF), 0);
}



void CChildFrame::SetViewExStyle(DWORD dwStyle, BOOL bExProp /*= FALSE*/)
{
	bool bNavigateLock = (dwStyle & DVS_EX_OPENNEWWIN) != 0/*? true : false*/;

	m_MDITab.NavigateLockTab(m_hWnd, bNavigateLock);
	//m_view.m_dwDefaultExtendedStyleFlags	= dwStyle;			//+++
	m_view.m_ViewOption.m_dwExStyle = dwStyle;
	m_bExPropLock					= (bNavigateLock && bExProp) /*? true : false*/;
}



// ==========================================================================
// 進む戻る


void	CChildFrame::SetDfgFileNameSection(const CString &strFileName, const CString &strSection)
{
	m_strDfgFileName = strFileName;
	m_strSection	 = strSection;
}



void CChildFrame::SetArrayHist(
			std::vector< std::pair<CString, CString> > &	ArrayFore,
			std::vector< std::pair<CString, CString> > &	ArrayBack
){
	m_ArrayHistFore = ArrayFore;
	m_ArrayHistBack = ArrayBack;
}



void CChildFrame::SetTravelLogData()
{
	if (m_bInitTravelLog) {
		if (m_ArrayHistBack.size() == 0 && m_ArrayHistFore.size() == 0) {
			//iniから読み込む
			std::vector<std::pair<CString, CString> > ArrFore;
			std::vector<std::pair<CString, CString> > ArrBack;
			_Load_TravelData(ArrFore, ArrBack, m_strDfgFileName, m_strSection);
			_Load_TravelLog(ArrFore, m_spBrowser, TRUE);
			_Load_TravelLog(ArrBack, m_spBrowser, FALSE);
		} else {
			//配列のデータを使う
			_Load_TravelLog(m_ArrayHistFore, m_spBrowser, TRUE);
			_Load_TravelLog(m_ArrayHistBack, m_spBrowser, FALSE);
			m_ArrayHistBack.clear();
			m_ArrayHistFore.clear();
		}
	}
	m_bInitTravelLog = FALSE;
}



BOOL CChildFrame::_Load_TravelData(
	std::vector<std::pair<CString, CString> >&	arrFore,
	std::vector<std::pair<CString, CString> >&	arrBack,
	CString & 									strFileName,
	CString & 									strSection)
{
	if ( strFileName.IsEmpty() )
		return FALSE;

	if ( strSection.IsEmpty() )
		return FALSE;

	CIniFileI	pr(strFileName, strSection);
	CString 		strKey;
	for (unsigned i = 0; i < 0x7fffffff; ++i) {
		strKey.Format(_T("Fore_URL%d"), i);
		CString strURL = pr.GetString(strKey/*, NULL, MAX_PATH*/);
		if (strURL.IsEmpty())
			break;
		strKey.Format(_T("Fore_Title%d"), i);
		CString strTitle = pr.GetStringUW(strKey/*, NULL, 1024*/);
		if (strTitle.IsEmpty())
			break;
		arrFore.push_back( std::make_pair(strTitle, strURL) );
	}

	for (unsigned i = 0; i < 0x7fffffff; ++i) {
		strKey.Format(_T("Back_URL%d"), i);
		CString strURL = pr.GetString(strKey/*, NULL, MAX_PATH*/);
		if (strURL.IsEmpty())
			break;
		strKey.Format(_T("Back_Title%d"), i);
		CString strTitle = pr.GetStringUW(strKey/*, NULL, 1024*/);
		if (strTitle.IsEmpty())
			break;
		arrBack.push_back( std::make_pair(strTitle, strURL) );
	}

	return TRUE;
}

BOOL CChildFrame::_Load_TravelLog(
	std::vector<std::pair<CString, CString> >&	arrLog,
	nsCOMPtr<nsIWebBrowser>                     pWB,
	BOOL 										bFore)
{
#if COMMENT
	if (arrLog.size() == 0)
		return FALSE;

	HRESULT 					 hr;
	int 						 nDir;

	nDir   = bFore ? TLEF_RELATIVE_FORE : TLEF_RELATIVE_BACK;

	if (pWB2 == NULL)
		return FALSE;

	int		nLast	= ( 10 > arrLog.size() ) ? (int) arrLog.size() : 10;
	for (int i = 0; i < nLast; i++) {

		std::vector<wchar_t>	title = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].first ) );
		std::vector<wchar_t>	url   = Misc::tcs_to_wcs( LPCTSTR( arrLog[i].second) );
	}
#endif
	return TRUE;
}



#if 0	//+++ UNICODE対応の折、不要化
LPOLESTR CChildFrame::_ConvertString(LPCTSTR lpstrBuffer, int nBufferSize)
{
	LPOLESTR pszwDest = new OLECHAR[ nBufferSize ];

	::MultiByteToWideChar( CP_ACP, 0, lpstrBuffer, nBufferSize, pszwDest, nBufferSize * sizeof (OLECHAR) );
	return pszwDest;
}
#endif




// ===========================================================================
// 拡大関係

///+++
LRESULT CChildFrame::OnHtmlZoomMenu(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	::SetForegroundWindow(m_hWnd);
	CMenu/*Handle*/ 	menu0;
	menu0.LoadMenu(IDR_ZOOM_MENU);
	if (menu0.m_hMenu == NULL)
		return 0;
	CMenuHandle menu = menu0.GetSubMenu(0);
	if (menu.m_hMenu == NULL)
		return 0;

	// ポップアップメニューを開く.
	POINT 	pt;
	::GetCursorPos(&pt);
	HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON| TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
	if (hr)
		MtlSendCommand(m_hWnd, WORD(hr));
	return 0;
}



///+++
LRESULT CChildFrame::OnHtmlZoomAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	SetBodyStyleZoom(10, 0);
	return 0;
}



///+++
LRESULT CChildFrame::OnHtmlZoomSub(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	SetBodyStyleZoom(-10, 0);
	return 0;
}



///+++ 100% とその他の比率をトグル切替.
LRESULT CChildFrame::OnHtmlZoom100Togle(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL &)
{
	if (m_nImgScl == 100) {
		m_nImgScl	 = m_nImgSclSav;
		m_nImgSclSw	 = 1;
	} else {
		m_nImgSclSav = m_nImgScl;
		m_nImgScl    = 100;
		m_nImgSclSw	 = 0;
	}
	SetBodyStyleZoom(0, m_nImgScl);
	return 0;
}



///+++
void CChildFrame::OnHtmlZoom(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	ATLASSERT(wID >= ID_HTMLZOOM_400 && wID <= ID_HTMLZOOM_050);
	static const int scls[] = { 400, 200, 150, 125, 100, 75, 50 };
	int  n = wID - ID_HTMLZOOM_400;
	if (n < 0 || n > ID_HTMLZOOM_050-ID_HTMLZOOM_400)
		return;
	SetBodyStyleZoom(0, scls[n]);
	return;
}



///+++ htmlを拡大縮小
void CChildFrame::SetBodyStyleZoom(int addSub, int scl)
{
	//TODO:
}

// ===========================================================================
// スタイルシート

#ifndef NO_STYLESHEET

LRESULT CChildFrame::OnStyleSheet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString strSheet;

	nsCOMPtr<nsIDOMWindow> win;
	m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));

	switch (wID) {
	case ID_STYLESHEET_BASE:
		SwitchStyleSheetInFrame(win, "_default");
		break;
	case ID_STYLESHEET_OFF:
		SwitchStyleSheetInFrame(win, "_nostyle");
		break;
	}


	//StyleSheet( strSheet, bOff, _T("") );
	//SetDefaultStyleSheet( _T("") );

	return 1;
}

void CChildFrame::SetDefaultStyleSheet(const CString& strStyleSheet)
{
	CIniFileIO	pr( g_szIniFileName, _T("StyleSheet") );
	DWORD		dwSetDefault = pr.GetValue( _T("SetDefault") );
	if (dwSetDefault)
		pr.SetString( strStyleSheet, _T("Default") );
}

//private:
void CChildFrame::ApplyDefaultStyleSheet()
{
	CIniFileI pr( g_szIniFileName, _T("StyleSheet") );
	CString 	strSheet = pr.GetString(_T("Default"));
	pr.Close();

	if ( !strSheet.IsEmpty() ) {
		//StyleSheet(MtlGetFileName(strSheet), FALSE, strSheet);
	}
}

#endif	// NO_STYLESHEET



// ===========================================================================
// 範囲選択関係


///+++ フレーム内の各ページの、選択範囲の文字列を取得(すべてを舐めるが、状況的に見つかるのは１つのはず）
struct Function_GetSelectedText {

	Function_GetSelectedText(CString& rSel) : m_rStrSelText(rSel) {;}

	void operator ()(nsIDOMWindow *pWin)
	{
		nsCOMPtr<nsISelection> sel;
		nsresult rv = pWin->GetSelection(getter_AddRefs(sel));
		if(NS_FAILED(rv)) return;
		PRInt32 rcount = 0;
		sel->GetRangeCount(&rcount);
		for(int i=0;i<rcount;i++){
			nsCOMPtr<nsIDOMRange> r;
			sel->GetRangeAt(i, getter_AddRefs(r));
			nsEmbedString str;
			r->ToString(str);
			m_rStrSelText += CString(str.get());
		}
	}

private:
	CString&		m_rStrSelText;
};


///+++ 選択範囲があるかどうかを返す
bool CChildFrame::HaveSelectedText() const
{
	if (this == 0)
		return false;
	CString 	strSelText;

	MtlForEachDOMWindow( m_spBrowser, Function_GetSelectedText(strSelText) );

	return strSelText.IsEmpty() == 0;
}


///+++ 選択範囲の文字列を取得(フレーム対応版)
CString CChildFrame::GetSelectedText()
{
	CString 	strSelText;
	//m_MDITab.SetLinkState(LINKSTATE_B_ON);

	MtlForEachDOMWindow( m_spBrowser, Function_GetSelectedText(strSelText) );

	//m_MDITab.SetLinkState(LINKSTATE_OFF);

	if (strSelText.IsEmpty()) {
		//+++ 選択範囲がない場合、右クリックメニュー経由のことがあるので、それ対策を強引に...
		CCustomContextMenu* pMenu = CCustomContextMenu::GetInstance();
		if (pMenu && pMenu->GetContextMenuMode() == CONTEXT_MENU_ANCHOR && pMenu->GetAnchorUrl().IsEmpty() == 0) {
			//bstrText		= pMenu->GetAnchorUrl();
			//bstrLocationUrl = pMenu->GetAnchorUrl();
			strSelText = CString(pMenu->GetAnchorUrl());
		}
	}
	return strSelText;
}



//+++ _OpenSelectedText()より分離.
CString CChildFrame::GetSelectedTextLine()
{
	CString str = GetSelectedText();
	if (str.IsEmpty() == 0) {
		int 	n	= str.Find(_T("\r"));
		int 	n2	= str.Find(_T("\n"));
		if ((n < 0 || n > n2) && n2 >= 0)
			n = n2;
		if (n >= 0) {
			str = str.Left(n);
		}
	}
	return str;
}




struct CChildFrame::_Function_OpenSelectedRef {
private:
	HWND			m_hWnd;
	CChildFrame*	m_pChild;

public:
	_Function_OpenSelectedRef(HWND hWnd, CChildFrame* pChild) : m_hWnd(hWnd), m_pChild(pChild) { }

	void operator ()(nsIDOMWindow *pWin)
	{
		nsCOMPtr<nsISelection> sel;
		nsresult rv = pWin->GetSelection(getter_AddRefs(sel));
		if(NS_FAILED(rv)) return;

		nsCOMPtr<nsIDOMDocument> doc;
		rv = pWin->GetDocument(getter_AddRefs(doc));
		if(NS_FAILED(rv)) return;

		//選択範囲のソースを得る．絶対パスに変換するためにnsIDocumentEncoderを使う
		nsCOMPtr<nsIDocumentEncoder> docEncoder = do_CreateInstance(NS_HTMLCOPY_ENCODER_CONTRACTID, &rv);
		if(NS_FAILED(rv)) return;
		docEncoder->Init(doc, NS_LITERAL_STRING("text/html"),
			             nsIDocumentEncoder::OutputAbsoluteLinks |
                         nsIDocumentEncoder::OutputEncodeW3CEntities);
		docEncoder->SetSelection(sel);
		nsEmbedString context, info, html;
		docEncoder->EncodeToStringWithContext(context, info, html);

		dcfTRACE( _T("CChildFrame::_Function_OpenSelectedRef\n") );
		dcfTRACE(html.get());
		dcfTRACE( _T("\n") );

		//テンポラリなHTMLDocumentを生成
		const nsCID hoge = NS_HTMLDOCUMENT_CID;
		nsCOMPtr<nsIDOMHTMLDocument> htmlDocument(do_CreateInstance(hoge, &rv));
		if(NS_FAILED(rv)) return;

		//html, bodyを挿入
		nsCOMPtr<nsIDOMElement> element, body;
		nsCOMPtr<nsIDOMNode> dummy;
		rv = htmlDocument->CreateElement(NS_LITERAL_STRING("html"), getter_AddRefs(element));
		if(NS_FAILED(rv)) return;
		rv = htmlDocument->AppendChild(element, getter_AddRefs(dummy));
		if(NS_FAILED(rv)) return;
		rv = htmlDocument->CreateElement(NS_LITERAL_STRING("body"), getter_AddRefs(body));
		if(NS_FAILED(rv)) return;
		rv = element->AppendChild(body, getter_AddRefs(dummy));
		if(NS_FAILED(rv)) return;

		//さっき切り取ってきたhtmlの断片を挿入
		nsCOMPtr<nsIDOMNSHTMLElement> nsbody = do_QueryInterface(body, &rv);
		if(NS_FAILED(rv)) return;
		rv = nsbody->SetInnerHTML(html); //本当に突っ込んで大丈夫なんだろうか(scriptとか実行されないだろうな)
		if(NS_FAILED(rv)) return;

		//リンクを開く
		nsCOMPtr<nsIDOMNodeList> l;
		htmlDocument->GetElementsByTagName(NS_LITERAL_STRING("a"), getter_AddRefs(l));
		PRUint32 count;
		l->GetLength(&count);
		nsCOMPtr<nsIDOMNode> node;
		nsCOMPtr<nsIDOMHTMLAnchorElement> a;
		//DWORD dwOpenFlags = DonutGetStdOpenFlag();
		DWORD dwOpenFlags = 0;
		for(unsigned int i=0;i<count;i++){
			l->Item(i, getter_AddRefs(node));
			a = do_QueryInterface(node);
			nsEmbedString str;
			a->GetHref(str);
			CString uri(str.get());
			if(uri.Left(4) == "http")
				DonutOpenFile(m_hWnd, uri, dwOpenFlags);
		}
	}
};


void CChildFrame::OnEditOpenSelectedRef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	dcfTRACE( _T("CChildFrame::OnEditOpenSelectedRef\n") );
	m_MDITab.SetLinkState(LINKSTATE_B_ON);

	MtlForEachDOMWindow( m_spBrowser, _Function_OpenSelectedRef(m_hWnd, this) );

	m_MDITab.SetLinkState(LINKSTATE_OFF);
}

void CChildFrame::OnEditOpenSelectedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	dcfTRACE( _T("CChildFrame::OnEditOpenSelectedTitle\n") );
	CString strText = GetSelectedText();
	if (strText.IsEmpty())
		return;

	std::vector<CString> lines;
	lines.reserve(20);
	Misc::SeptTextToLines(lines, strText);
	size_t	size = lines.size();
  #if 1	//+++ 選択文字列中にurlぽいものがなかったら、文字列検索にしてみる.
	bool	f	= 0;
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		f |= (strUrl.Find(_T(':')) >= 0) || (strUrl.Find(_T('/')) >= 0) || (strUrl.Find(_T('.')) >= 0) ||  (strUrl.Find(_T('\\')) >= 0);
		if (f)
			break;
	}
	if (f == 0) {	// urlぽい文字列がなかった...
		CDonutSearchBar*	pSearchBar = CDonutSearchBar::GetInstance();
		if (pSearchBar) {
			CEdit edit = pSearchBar->GetEditCtrl();
			edit.SendMessage(WM_CHAR, 'P');
			CString str		= lines[0];
			LPCTSTR		strExcept  = _T(" \t\"\r\n　");
			str.TrimLeft(strExcept);
			str.TrimRight(strExcept);
			edit.SetWindowText(str);
			pSearchBar->SearchWeb();
			return;
		}
	}
  #endif
	for (unsigned i = 0; i < size; ++i) {
		CString& strUrl = lines[i];
		Misc::StrToNormalUrl(strUrl);		//+++ 関数化
		DonutOpenFile(m_hWnd, strUrl);
		::Sleep(10);						//+++ なんとなく追加... いらないけど...
	}
}

// ===========================================================================
// 検索

//public:
void CChildFrame::searchEngines(const CString &strKeyWord )
{
	CString 	strSearchWord = strKeyWord;

	if (m_AddressBar.GetAddressBarExtendedStyle() & ABR_EX_SEARCH_REPLACE)
		strSearchWord.Replace( _T("　"), _T(" ") );

	//_ReplaceCRLF(strSearchWord,CString(_T(" ")));
	//strSearchWord.Replace('\n',' ');
	//strSearchWord.Remove('\r');
	strSearchWord.Replace( _T("\r\n"), _T("") );

	CIniFileI	pr( g_szIniFileName, STR_ADDRESS_BAR );
	CString 		strEngin = pr.GetStringUW( _T("EnterCtrlEngin"), NULL, 256 );
	pr.Close();
	::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) strSearchWord, (LPARAM) (LPCTSTR) strEngin);

	//CWebBrowserCommandHandler<CChildFrame>::searchEngines(strSearchWord);
}


#if 0
//private:
CString CChildFrame::_ReplaceCRLF(CString &str, CString &strRep)
{
	CString strRet;
	TCHAR	ch;
	int 	len = str.GetLength();
	for (int i = 0; i < len; i++) {
		ch = str.GetAt(i);

		if ( (ch & 0xFF) == 0xa ) {
			strRet += strRep;
		} else if ( (ch & 0xFF) == 0xd ) {

		} else {
			strRet += ch;
		}
	}

	return strRep;
}
#endif



//
// ページ内検索
//
LRESULT CChildFrame::OnFindKeyWord(LPCTSTR lpszKeyWord, BOOL bFindDown)
{
	// アクティブウィンドウの取得
	HWND	hWndActive = MDIGetActive();
	if (hWndActive == NULL)
		return 0;

	if (!m_spBrowser)
		return 0;
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	nsCOMPtr<nsIWebBrowserFind> find = do_GetInterface(docShell);
	if(!find) return 0;

	find->SetSearchString(lpszKeyWord);
	find->SetFindBackwards(!bFindDown);
	PRBool r;
	find->FindNext(&r);

	return 1;
}

#if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
// gae _Function_Hilightと引数の意味が違うので注意
struct CChildFrame::_Function_Hilight2 {
	LPCTSTR 	m_lpszKeyWord;
	BOOL		m_bHilight;

	_Function_Hilight2(LPCTSTR lpszKeyWord, BOOL bHilight)
		: m_lpszKeyWord(lpszKeyWord), m_bHilight(bHilight)
	{
	}

	void operator ()(nsIDOMWindow* pWin)
	{
		if (m_bHilight) {
			if ( !FindHilight(pWin) ) {
				MakeHilight(pWin);
			}
		} else {
			RemoveHilight(pWin);
		}
	}


	void MakeHilight(nsIDOMWindow* pWin)
	{
		// // 待機中はカーソルを砂時計に変更する
		// HCURSOR hCursor 	= SetCursor( LoadCursor(NULL, IDC_WAIT) );

		// キーワードの最初の一語を取得
		CString		strKeyWord(m_lpszKeyWord);

		//+++ 単語区切りを調整
		LPCTSTR		strExcept	= _T(" \t\"\r\n　");
		strKeyWord = _tcstok( (LPTSTR) strKeyWord.GetBuffer(0), LPCTSTR(strExcept) );
		strKeyWord.TrimLeft(strExcept);
		strKeyWord.TrimRight(strExcept);

		int 	nLightIndex = 0;
//		nsresult rv;
		
		nsCOMPtr<nsIDOMDocument> doc;
		nsCOMPtr<nsIDOMHTMLDocument> htmlDoc;
		nsCOMPtr<nsIDOMDocumentRange> docRange;
		nsCOMPtr<nsIDOMHTMLElement> body;
		nsCOMPtr<nsIDOMElement> elm, containerElm;
		nsCOMPtr<nsIDOMRange> foundRange, searchRange, start, end;
		nsCOMPtr<nsIFind> finder;
		nsCOMPtr<nsIDOMNode> dummy, container, parent, parent2;

		pWin->GetDocument(getter_AddRefs(doc));
		if(!doc) return;

		docRange = do_QueryInterface(doc);
		if(!docRange) return;

		htmlDoc = do_QueryInterface(doc);
		if(!htmlDoc) return;

		htmlDoc->GetBody(getter_AddRefs(body));

		docRange->CreateRange(getter_AddRefs(searchRange));
		docRange->CreateRange(getter_AddRefs(start));
		docRange->CreateRange(getter_AddRefs(end));


		finder = do_CreateInstance("@mozilla.org/embedcomp/rangefind;1");
		if(!finder) return;

		// キーワードが空になるまで繰り返し
		while ( !strKeyWord.IsEmpty() ) {

			searchRange->SelectNode(body);
			start->SelectNode(body);
			start->Collapse(PR_TRUE);
			end->SelectNode(body);
			end->Collapse(PR_FALSE);

			do
			{
				finder->Find(strKeyWord, searchRange, start, end, getter_AddRefs(foundRange));
				if(foundRange){

					//Input要素などの中も検索できてしまうので，それを検出して回避する
					foundRange->GetStartContainer(getter_AddRefs(container));
					container->GetParentNode(getter_AddRefs(parent));
					if(parent) parent->GetParentNode(getter_AddRefs(parent2));
					containerElm = do_QueryInterface(parent2/*dummy*/);
					if(containerElm){
						nsEmbedString tagName;
						containerElm->GetTagName(tagName);
						dcfTRACE(tagName.get());
						if(!tagName.Compare(_T("textarea")) ||
 						   !tagName.Compare(_T("TEXTAREA")) ||
						   !tagName.Compare(_T("INPUT")) ||
						   !tagName.Compare(_T("input")) )
						{
							foundRange->Collapse(PR_FALSE);
							foundRange->CloneRange(getter_AddRefs(start));
							continue;//次へ
						}
					}

					//色づけ
					nsCOMPtr<nsIDOMDocumentFragment> fragment;
					foundRange->ExtractContents(getter_AddRefs(fragment));
					foundRange->DeleteContents();
					doc->CreateElement(NS_LITERAL_STRING("span"), getter_AddRefs(elm));
					elm->SetAttribute(NS_LITERAL_STRING("class"), NS_LITERAL_STRING("donut_g_hilight"));
					elm->SetAttribute(NS_LITERAL_STRING("style"), nsEmbedString(g_lpszLight[nLightIndex]));
					elm->AppendChild(fragment, getter_AddRefs(dummy));
					foundRange->InsertNode(elm);

					//次の検索範囲の設定
					start->SetStartAfter(elm);
					start->Collapse(PR_TRUE);
				}
			}while(foundRange);

			++nLightIndex;

			if (nLightIndex >= g_LIGHTMAX)
				nLightIndex = 0;

			strKeyWord = _tcstok(NULL, LPCTSTR(strExcept) );
			strKeyWord.TrimLeft(strExcept);
			strKeyWord.TrimRight(strExcept);
		}

		// // カーソルを元に戻す
		// SetCursor(hCursor);
	}

	void RemoveHilight(nsIDOMWindow* pWin)
	{
		nsCOMPtr<nsIDOMDocument> doc;
		pWin->GetDocument(getter_AddRefs(doc));
		if(!doc) return;

		nsCOMPtr<nsIDOMDocumentRange> docRange;
		docRange = do_QueryInterface(doc);
		if(!docRange) return;

		nsCOMPtr<nsIDOMRange> range;
		docRange->CreateRange(getter_AddRefs(range));
		if(!range) return;

		nsCOMPtr<nsIDOMNodeSelector> nodeSelector = do_QueryInterface(doc);
		if(!nodeSelector) return;

		nsEmbedString cls(_T(".donut_g_hilight"));
		nsCOMPtr<nsIDOMNode> node;
		nsCOMPtr<nsIDOMNodeList> list;
		nodeSelector->QuerySelectorAll(cls, getter_AddRefs(list));
		PRUint32 len;
		list->GetLength(&len);
		for(int i=len-1;i>=0;i--){
			list->Item(i, getter_AddRefs(node));
			range->SelectNodeContents(node);
			nsCOMPtr<nsIDOMDocumentFragment> fragment;
			range->ExtractContents(getter_AddRefs(fragment));
			range->SetStartBefore(node);
			range->InsertNode(fragment);
			range->SelectNode(node);
			range->DeleteContents();
		}
	}

	// ハイライトがすでにされているか確認する
	BOOL FindHilight(nsIDOMWindow* pWin)
	{
		nsCOMPtr<nsIDOMDocument> doc;
		pWin->GetDocument(getter_AddRefs(doc));
		if(!doc) return FALSE;

		nsCOMPtr<nsIDOMNodeSelector> nodeSelector = do_QueryInterface(doc);
		if(!nodeSelector) return FALSE;

		nsEmbedString cls(_T(".donut_g_hilight"));
		nsCOMPtr<nsIDOMElement> elm;
		nodeSelector->QuerySelector(cls, getter_AddRefs(elm));
		if(elm) return TRUE;
		else return FALSE;
	}
};
#endif	// KIKE_UNDONUT_G





struct CChildFrame::_Function_SelectEmpt {
	_Function_SelectEmpt(){ }

	void operator ()(nsIDOMWindow *pWin)
	{
		nsCOMPtr<nsISelection> sel;
		pWin->GetSelection(getter_AddRefs(sel));
		sel->CollapseToStart();
	}
};

//+++
void CChildFrame::SetSearchWordAutoHilight( const CString& str, bool autoHilightSw )
{
	m_strSearchWord = str;
	m_bAutoHilight	= autoHilightSw ;
}

//vvv
// UH
LRESULT CChildFrame::OnHilight(LPCTSTR lpszKeyWord)	//+++ 未使用な引数を削除. , BOOL bToggle /*= TRUE*/, BOOL bFlag /*= FALSE*/)
{
	HWND hWndActive = MDIGetActive();
	if (hWndActive == NULL)
		return 0;

  #if 0	//+++ 実質未使用の仕組みを削除
	if (!bToggle)
		m_bNowHilight = bFlag;
  #endif
  #if 1	//+++
	bool bHilightSw = CDonutSearchBar::GetInstance()->GetHilightSw();
	//if (m_bNowHilight != bHilightSw)
	{
		//MtlForEachDOMWindow( m_spBrowser, _Function_SelectEmpt() );
		//MtlForEachDOMWindow( m_spBrowser, _Function_Hilight(lpszKeyWord, m_bNowHilight) );
		m_bNowHilight	= bHilightSw;
		if (bHilightSw)
			m_strSearchWord = lpszKeyWord;
		else
			m_strSearchWord.Empty();

		MtlForEachDOMWindow(m_spBrowser, _Function_SelectEmpt());
		MtlForEachDOMWindow(m_spBrowser, _Function_Hilight2(lpszKeyWord, bHilightSw));

	}
  #elif 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
   #if 1	//+++
	m_bNowHilight = CDonutSearchBar::GetInstance()->GetHilightSw();
   #else
	m_bNowHilight = !m_bNowHilight;
   #endif
	if (m_bNowHilight)
		m_strSearchWord = lpszKeyWord;
	else
		m_strSearchWord.Empty();

	MtlForEachDOMWindow(m_spBrowser, _Function_SelectEmpt());
	MtlForEachDOMWindow(m_spBrowser, _Function_Hilight2(lpszKeyWord, m_bNowHilight));

  #else

	MtlForEachDOMWindow( m_spBrowser, _Function_SelectEmpt() );
	MtlForEachDOMWindow( m_spBrowser, _Function_Hilight(lpszKeyWord, m_bNowHilight) );

	+++ m_bNowHilight = !m_bNowHilight;
  #endif
	return 1;
}


#if 1 //def USE_UNDONUT_G_SRC		//+++ gae氏のunDonut_g の処理を反映してみる場合.
LRESULT CChildFrame::OnHilightOnce(nsIWebBrowser *pWb, LPCTSTR lpszKeyWord)
{
	if (pWb!=NULL) {
		nsCOMPtr<nsIDOMWindow> win;
		pWb->GetContentDOMWindow(getter_AddRefs(win));
		if(win){

			_Function_SelectEmpt	se;
			se(win);

			_Function_Hilight2		hl(lpszKeyWord, TRUE);
			hl(win);

		}
	}
	return 1;
}
#endif



#if 0	//+++
void CChildFrame::OnSearchWeb_engineId(UINT code, int id, HWND hWnd)
{
	ATLASSERT(ID_INSERTPOINT_SEARCHENGINE <= id && id <= ID_INSERTPOINT_SEARCHENGINE_END);
	MtlSendCommand(CDonutSearchBar::GetInstance()->m_hWnd, WORD(id));
}
#endif

