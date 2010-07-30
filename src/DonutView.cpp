/**
 *	@file	DonutView.cpp
 *	@brief	View
 */

#include "stdafx.h"
#include "DonutView.h"
#include "mshtmdid.h"
#include "option/DLControlOption.h"
#include "Donut.h"

#include "ScriptErrorCommandTargetImpl.h"

#include "nsEmbedCID.h"
#include "GeckoBrowserImpl.h"
#include "nsIComponentManager.h"
#include "nsIBaseWindow.h"
#include "nsIWebBrowserFocus.h"
#include "nsIURIContentListener.h"
#include "nsIDOMWindow2.h"
#include "nsIDOMEventTarget.h"
#include "nsIDOMDragEvent.h"
#include "nsIDOMDataTransfer.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




// Constructor
CDonutView::CDonutView(DWORD dwDefaultDLControlFlags, DWORD dwExStyleFlags)
	: m_ViewOption(this, dwExStyleFlags)						//+++ dwExStyleFlags追加.
	, m_dwDefaultDLControlFlags(dwDefaultDLControlFlags)
	//, m_dwDefaultExtendedStyleFlags(dwExStyleFlags)			//+++
	, m_nDDCommand(0)
  #if 1 //+++	抜け対策でかいとく.
//	, m_spAxAmbient()
   #if defined USE_ATL3_BASE_HOSTEX == 0 /*_ATL_VER >= 0x700*/	//+++
	, m_ExternalAmbientDispatch()
   #endif
  #endif
{
	NS_ADDREF(this);
}
DWORD CDonutView::GetDLControlFlags()
{
	DWORD dwDLControlFlags = CDLControlOption::DLCTL_DEFAULT;
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowImages(&b);
	if(b) dwDLControlFlags |= GECKO_DLCTL_IMAGES;
	else  dwDLControlFlags &= ~GECKO_DLCTL_IMAGES;
	docShell->GetAllowSubframes(&b);
	if(b) dwDLControlFlags |= GECKO_DLCTL_FRAMES;
	else  dwDLControlFlags &= ~GECKO_DLCTL_FRAMES;
	docShell->GetAllowPlugins(&b);
	if(b) dwDLControlFlags |= GECKO_DLCTL_PLUGINS;
	else  dwDLControlFlags &= ~GECKO_DLCTL_PLUGINS;
	docShell->GetAllowJavascript(&b);
	if(b) dwDLControlFlags |= GECKO_DLCTL_SCRIPTS;
	else  dwDLControlFlags &= ~GECKO_DLCTL_SCRIPTS;
	docShell->GetAllowMetaRedirects(&b);
	if(b) dwDLControlFlags |= GECKO_DLCTL_REDIR;
	else  dwDLControlFlags &= ~GECKO_DLCTL_REDIR;
	docShell->GetAllowDNSPrefetch(&b);
	if(b) dwDLControlFlags |= GECKO_DLCTL_DNS;
	else  dwDLControlFlags &= ~GECKO_DLCTL_DNS;

	return dwDLControlFlags;
}

void CDonutView::PutDLControlFlags(DWORD dwDLControlFlags)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	if(dwDLControlFlags & GECKO_DLCTL_IMAGES)  docShell->SetAllowImages(PR_TRUE);
	else                                       docShell->SetAllowImages(PR_FALSE);
	if(dwDLControlFlags & GECKO_DLCTL_FRAMES)  docShell->SetAllowSubframes(PR_TRUE);
	else                                       docShell->SetAllowSubframes(PR_FALSE);
	if(dwDLControlFlags & GECKO_DLCTL_PLUGINS) docShell->SetAllowPlugins(PR_TRUE);
	else                                       docShell->SetAllowPlugins(PR_FALSE);
	if(dwDLControlFlags & GECKO_DLCTL_SCRIPTS) docShell->SetAllowJavascript(PR_TRUE);
	else                                       docShell->SetAllowJavascript(PR_FALSE);
	if(dwDLControlFlags & GECKO_DLCTL_REDIR)   docShell->SetAllowMetaRedirects(PR_TRUE);
	else                                       docShell->SetAllowMetaRedirects(PR_FALSE);
	if(dwDLControlFlags & GECKO_DLCTL_DNS)     docShell->SetAllowDNSPrefetch(PR_TRUE);
	else                                       docShell->SetAllowDNSPrefetch(PR_FALSE);
}

//ドラッグドロップ時の操作を制御するかGeckoに任せるか
void CDonutView::SetOperateDragDrop(BOOL bOn, int nCommand)
{
	nsCOMPtr<nsIDOMWindow> win;
	m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));
	nsCOMPtr<nsIDOMWindow2> win2 = do_QueryInterface(win);
	nsCOMPtr<nsIDOMEventTarget> target;
	win2->GetWindowRoot(getter_AddRefs(target));
	if(bOn){
		target->AddEventListener(NS_LITERAL_STRING("dragover"), this, PR_FALSE);
		target->AddEventListener(NS_LITERAL_STRING("dragenter"), this, PR_FALSE);
		target->AddEventListener(NS_LITERAL_STRING("drop"), this, PR_FALSE);
	}else{
		target->RemoveEventListener(NS_LITERAL_STRING("dragover"), this, PR_FALSE);
		target->RemoveEventListener(NS_LITERAL_STRING("dragenter"), this, PR_FALSE);
		target->RemoveEventListener(NS_LITERAL_STRING("drop"), this, PR_FALSE);
	}
	m_nDDCommand = nCommand;
}

int CDonutView::AttachEventListener(IGeckoBrowserEventListener* intf)
{
	if(m_impl) m_impl->SetEventListener(intf);
	return 0;
}

void CDonutView::DetachEventListener()
{
	if(m_impl) m_impl->SetEventListener(NULL);
}


// Overrides
BOOL CDonutView::PreTranslateMessage(MSG *pMsg)
{
	if (   (pMsg->message < WM_KEYFIRST   || pMsg->message > WM_KEYLAST )
		&& (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST) )
	{
		return FALSE;
	}

	// give HTML page a chance to translate this message
	return SendMessage(WM_FORWARDMSG, 0, (LPARAM) pMsg) != 0;
}



LRESULT CDonutView::OnMouseWheel(UINT fwKeys, short zDelta, CPoint point)
{
	SetMsgHandled(FALSE);
	return 1;
}

void CDonutView::OnSize(UINT nType, CSize size)
{
	if(!m_spBrowser) return;
	nsCOMPtr<nsIBaseWindow> baseWin = do_QueryInterface(m_spBrowser);
	baseWin->SetPositionAndSize(0,0,size.cx, size.cy, PR_TRUE);
}

// UDT DGSTR
void CDonutView::OnMultiChg(WORD, WORD, HWND)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowImages(&b);
	docShell->SetAllowImages(!b);
	_LightRefresh();
}

void CDonutView::OnSecuChg(WORD, WORD, HWND)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowSubframes(&b);
	docShell->SetAllowSubframes(!b);
	docShell->GetAllowPlugins(&b);
	docShell->SetAllowPlugins(!b);
	docShell->GetAllowJavascript(&b);
	docShell->SetAllowJavascript(!b);
	docShell->GetAllowMetaRedirects(&b);
	docShell->SetAllowMetaRedirects(!b);
	docShell->GetAllowDNSPrefetch(&b);
	docShell->SetAllowDNSPrefetch(!b);
	_LightRefresh();
}

void CDonutView::OnAllOnOff(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool img, frame, plugin, js, meta, dns;
	docShell->GetAllowImages(&img);
	docShell->GetAllowSubframes(&frame);
	docShell->GetAllowPlugins(&plugin);
	docShell->GetAllowJavascript(&js);
	docShell->GetAllowMetaRedirects(&meta);
	docShell->GetAllowDNSPrefetch(&dns);
	switch (wID) {
	case ID_DLCTL_ON_OFF_MULTI:
		if ( img /*&& video && sound*/ )
			docShell->SetAllowImages(PR_FALSE);
		else
			docShell->SetAllowImages(PR_TRUE);
		break;

	case ID_DLCTL_ON_OFF_SECU:

		if ( frame && plugin && js && meta && dns ) {
			//チェックは全部ついている
			docShell->SetAllowSubframes(PR_FALSE);
			docShell->SetAllowPlugins(PR_FALSE);
			docShell->SetAllowJavascript(PR_FALSE);
			docShell->SetAllowMetaRedirects(PR_FALSE);
			docShell->SetAllowDNSPrefetch(PR_FALSE);
		} else {
			//チェックされていない項目がある
			docShell->SetAllowSubframes(PR_TRUE);
			docShell->SetAllowPlugins(PR_TRUE);
			docShell->SetAllowJavascript(PR_TRUE);
			docShell->SetAllowMetaRedirects(PR_TRUE);
			docShell->SetAllowDNSPrefetch(PR_TRUE);
		}

		break;
	}
	_LightRefresh();
}



LRESULT CDonutView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CDonutView::OnCreate in\n") );
	// Let me initialize itself
	LRESULT 	lRet	= DefWindowProc(uMsg, wParam, lParam);

	m_spBrowser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID);
	if(!m_spBrowser) { /*"failed to get nsIWebBrowser"*/return 0; }
	m_impl = new CGeckoBrowserImpl();
	m_impl->SetWebBrowser(m_spBrowser);
	m_impl->AddRef();
	m_impl->SetParentWindow(this->m_hWnd);
	nsresult rv = m_spBrowser->SetContainerWindow(static_cast<nsIWebBrowserChrome*>(m_impl));
	nsCOMPtr<nsIBaseWindow> baseWin = do_QueryInterface(m_spBrowser);
	rv = baseWin->InitWindow( this->m_hWnd,
		nsnull, 0, 0, 400, 400);
    rv = baseWin->Create();
	baseWin->SetVisibility(PR_TRUE);

	nsIWebProgressListener *p = static_cast<nsIWebProgressListener*>(m_impl);
    nsCOMPtr<nsIWebProgressListener> listener(p);
    nsCOMPtr<nsIWeakReference> ref_l(do_GetWeakReference(listener));
    rv = m_spBrowser->AddWebBrowserListener(ref_l, 
        NS_GET_IID(nsIWebProgressListener));

    rv = m_spBrowser->AddWebBrowserListener(ref_l, NS_GET_IID(nsISHistoryListener));

	rv = m_spBrowser->SetParentURIContentListener(static_cast<nsIURIContentListener*>(m_impl));

	nsCOMPtr<nsIWebBrowserFocus> focus = do_QueryInterface(m_spBrowser);
	focus->Activate();

	_InitDLControlFlags();

  #if 1	//+++
	if (m_ViewOption.m_dwExStyle == (DWORD)-1)
		m_ViewOption.m_dwExStyle = CDLControlOption::s_dwExtendedStyleFlags; //_dwViewStyle;
  #else
	ATLASSERT(m_ViewOption.m_dwExStyle == 0);
	m_ViewOption.m_dwExStyle = CDLControlOption::s_dwExtendedStyleFlags; //_dwViewStyle;
  #endif

	ATLTRACE2( atlTraceGeneral, 4, _T("CDonutView::OnCreate out\n") );
	return lRet;
}



void CDonutView::OnDestroy()
{
	SetMsgHandled(FALSE);

	m_ViewOption.Uninit();
}

void CDonutView::OnMultiImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowImages(&b);
	docShell->SetAllowImages(!b);
	if (b) _LightRefresh();
}

void CDonutView::OnSecurFrames(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowSubframes(&b);
	docShell->SetAllowSubframes(!b);
	if (b) _LightRefresh();
}
void CDonutView::OnSecurPlugins(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowPlugins(&b);
	docShell->SetAllowPlugins(!b);
	if (b) _LightRefresh();
}
void CDonutView::OnSecurScripts(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowJavascript(&b);
	docShell->SetAllowJavascript(!b);
	if (b) _LightRefresh();
}
void CDonutView::OnSecurMetaRedir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowMetaRedirects(&b);
	docShell->SetAllowMetaRedirects(!b);
	if (b) _LightRefresh();
}
void CDonutView::OnSecurDNSPrefetch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
	PRBool b;
	docShell->GetAllowDNSPrefetch(&b);
	docShell->SetAllowDNSPrefetch(!b);
	if (b) _LightRefresh();
}

void CDonutView::_InitDLControlFlags()
{
	PutDLControlFlags(m_dwDefaultDLControlFlags);
}

DWORD CDonutView::_GetDLControlFlags()
{
	return GetDLControlFlags();
}

//+++
DWORD	CDonutView::_GetExtendedStypeFlags()
{
	//return m_dwDefaultExtendedStyleFlags;
	return m_ViewOption.m_dwExStyle;
}

void CDonutView::_LightRefresh()
{
	if (::GetKeyState(VK_CONTROL) < 0)
		return;

	CString strURL = GetLocationURL();
	Navigate2(strURL);
}
void CDonutView::OnUpdateDLCTL_ChgMulti(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		BOOL bSts = TRUE;

		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowImages(&b);        if(b == PR_FALSE) bSts = FALSE;

		pCmdUI->SetCheck(bSts);
	}
}

void CDonutView::OnUpdateDLCTL_ChgSecu(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		BOOL bSts = TRUE;

		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowSubframes(&b);     if(b == PR_FALSE) bSts = FALSE;
		docShell->GetAllowPlugins(&b);       if(b == PR_FALSE) bSts = FALSE;
		docShell->GetAllowJavascript(&b);    if(b == PR_FALSE) bSts = FALSE;
		docShell->GetAllowMetaRedirects(&b); if(b == PR_FALSE) bSts = FALSE;
		docShell->GetAllowDNSPrefetch(&b);   if(b == PR_FALSE) bSts = FALSE;

		pCmdUI->SetCheck(bSts);
	}
}



void CDonutView::OnUpdateDLCTL_IMAGES(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowImages(&b);
		pCmdUI->SetCheck(b == PR_TRUE? true : false);
	}
}
void CDonutView::OnUpdateDLCTL_FRAMES(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowSubframes(&b);
		pCmdUI->SetCheck(b == PR_TRUE? true : false);
	}
}
void CDonutView::OnUpdateDLCTL_PLUGINS(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowPlugins(&b);
		pCmdUI->SetCheck(b == PR_TRUE? true : false);
	}
}
void CDonutView::OnUpdateDLCTL_SCRIPTS(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowJavascript(&b);
		pCmdUI->SetCheck(b == PR_TRUE? true : false);
	}
}
void CDonutView::OnUpdateDLCTL_REDIR(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowMetaRedirects(&b);
		pCmdUI->SetCheck(b == PR_TRUE? true : false);
	}
}
void CDonutView::OnUpdateDLCTL_DNS(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_menuSub.m_hMenu) {		// popup menu
		pCmdUI->m_menu.EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | MF_ENABLED);
	} else {
		pCmdUI->Enable();
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRBool b;
		docShell->GetAllowDNSPrefetch(&b);
		pCmdUI->SetCheck(b == PR_TRUE? true : false);
	}
}
void CDonutView::OnUpdateDocHostUIOpenNewWinUI(CCmdUI *pCmdUI)
{
	DWORD dwDocHostFlags = 0;		//+++ 念のため初期値設定.
#if COMMENT
	m_spAxAmbient->get_DocHostFlags(&dwDocHostFlags);
	pCmdUI->Enable();
	pCmdUI->SetCheck((dwDocHostFlags & docHostUIFlagOPENNEWWIN) != 0 /*? 1 : 0*/);
#endif
}



bool CDonutView::OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll)
{
	return false;
}



// Overrides

HRESULT STDMETHODCALLTYPE CDonutView::GetHostInfo(DWORD  *pdwFlags, DWORD  *pdwDoubleClick)
{
#if COMMENT
	m_spAxAmbient->get_DocHostFlags(pdwFlags);
	m_spAxAmbient->get_DocHostDoubleClickFlags(pdwDoubleClick);
#endif
	return S_OK;
}



HRESULT STDMETHODCALLTYPE CDonutView::GetDropTarget(IUnknown  *pDropTarget, IUnknown  **ppDropTarget)
{
	*ppDropTarget = (IUnknown *) (_IDropTargetLocator *) this;
	return S_OK;
}



DROPEFFECT CDonutView::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	return _MtlStandardDropEffect(dwKeyState);
}



void CDonutView::OnDragLeave()
{
}



void CDonutView::_DrawDragEffect(bool bRemove)
{
	CClientDC dc(m_hWnd);

	CRect	  rect;
	GetClientRect(rect);

	if (bRemove)
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(0, 0), &rect, CSize(2, 2), NULL, NULL);
	else
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(2, 2), NULL , CSize(2, 2), NULL, NULL);
}



DROPEFFECT CDonutView::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	CString 				strText;
	CSimpleArray<CString>	arrFiles;

	if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
		unsigned  df   = DonutGetStdOpenFlag();
		unsigned  size = arrFiles.GetSize();
		// if (size > 1)						//+++ 別に単ファイルを特別にする必要ない気がしてきたのでコメントアウト.
			df &= ~D_OPENFILE_NOCREATE; 		//+++ 複数ファイルがあるときは、必ず別タブ表示にする.
		for (unsigned i = 0; i < size; ++i)
			DonutOpenFile(m_hWnd, arrFiles[i], df);
		return DROPEFFECT_COPY;
	}

	if (   MtlGetHGlobalText(pDataObject, strText)
		|| MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		::SendMessage(GetTopLevelParent(), WM_COMMAND_DIRECT, m_nDDCommand, (LPARAM) (LPCTSTR) strText);
		return DROPEFFECT_NONE;
	} else {
		return DROPEFFECT_NONE;//CDonutViewFileDropTarget<CDonutView>::OnDrop(pDataObject, dropEffect, dropEffectList, point);
	}

	return DROPEFFECT_NONE;
}

NS_IMPL_ISUPPORTS1(CDonutView, nsIDOMEventListener);

NS_IMETHODIMP CDonutView::HandleEvent(nsIDOMEvent *event)
{
	nsCOMPtr<nsIDOMDragEvent> dragEvent = do_QueryInterface(event);
	if(dragEvent){
		nsEmbedString type;
		dragEvent->GetType(type);
		if(type.Compare(_T("dragenter")) == 0){
			event->PreventDefault();
		}else if(type.Compare(_T("dragover")) == 0){
			event->PreventDefault();
		}else if(type.Compare(_T("drop")) == 0){
			nsCOMPtr<nsIDOMDataTransfer> trans;
			dragEvent->GetDataTransfer(getter_AddRefs(trans));
			nsEmbedString data;
			trans->GetData(NS_LITERAL_STRING("text/plain"), data);
			CString strText(data.get());
			::SendMessage(GetTopLevelParent(), WM_COMMAND_DIRECT, m_nDDCommand, (LPARAM) (LPCTSTR) strText);
			event->StopPropagation();
		}
	}
	return NS_OK;
}