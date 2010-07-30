#include "stdafx.h"
#include "GeckoBrowserImpl.h"
#include "nsIWebProgress.h"
#include "nsIWebNavigationInfo.h"
#include "IGeckoBrowserEventListener.h"

CGeckoBrowserImpl::CGeckoBrowserImpl(void):
    m_listener(NULL), m_hwnd(NULL)
{
}

CGeckoBrowserImpl::~CGeckoBrowserImpl(void)
{
}

void CGeckoBrowserImpl::SetEventListener(IGeckoBrowserEventListener *listener)
{
	m_listener = listener;
}

NS_IMPL_ADDREF(CGeckoBrowserImpl)
NS_IMPL_RELEASE(CGeckoBrowserImpl)

NS_INTERFACE_MAP_BEGIN(CGeckoBrowserImpl)
   NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIWebBrowserChrome)
   NS_INTERFACE_MAP_ENTRY(nsIInterfaceRequestor)
   NS_INTERFACE_MAP_ENTRY(nsIWebBrowserChrome)
   NS_INTERFACE_MAP_ENTRY(nsIEmbeddingSiteWindow)
   NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener) // optional
   NS_INTERFACE_MAP_ENTRY(nsISHistoryListener)
   NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference)
   NS_INTERFACE_MAP_ENTRY(nsIObserver)
   NS_INTERFACE_MAP_ENTRY(nsIContextMenuListener2)
   NS_INTERFACE_MAP_ENTRY(nsITooltipListener)
   NS_INTERFACE_MAP_ENTRY(nsIURIContentListener)
NS_INTERFACE_MAP_END

// nsIInterfaceRequestor

NS_IMETHODIMP CGeckoBrowserImpl::GetInterface(const nsIID &aIID, void **aInstancePtr)
{
    NS_ENSURE_ARG_POINTER(aInstancePtr);

    *aInstancePtr = 0;

    if (aIID.Equals(NS_GET_IID(nsIDOMWindow)))
    {
        if (m_browser)
        {
            return m_browser->GetContentDOMWindow((nsIDOMWindow**) aInstancePtr);
        }
        return NS_ERROR_NOT_INITIALIZED;
    }

    return QueryInterface(aIID, aInstancePtr);
}

// nsIWebBrowserChrome

NS_IMETHODIMP CGeckoBrowserImpl::SetStatus(PRUint32 aType, const PRUnichar *aStatus)
{
	m_listener->OnStatusTextChange(aStatus);
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetWebBrowser(nsIWebBrowser **aWebBrowser)
{
    NS_ENSURE_ARG_POINTER(aWebBrowser);
    *aWebBrowser = m_browser;
    NS_IF_ADDREF(*aWebBrowser);
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::SetWebBrowser(nsIWebBrowser *aWebBrowser)
{
    m_browser = aWebBrowser;
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetChromeFlags(PRUint32 *aChromeMask)
{
    *aChromeMask = m_chromeFlags;
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::SetChromeFlags(PRUint32 aChromeMask)
{
    m_chromeFlags = aChromeMask;
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::DestroyBrowserWindow(void)
{
    //TODO:
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::SizeBrowserTo(PRInt32 aWidth, PRInt32 aHeight)
{
	return NS_OK;
}


NS_IMETHODIMP CGeckoBrowserImpl::ShowAsModal(void)
{
	//TODO:
	return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::IsWindowModal(PRBool *_retval)
{
    *_retval = PR_FALSE;
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::ExitModalEventLoop(nsresult aStatus)
{
	//TODO:
	return NS_OK;
}

// nsIWebBrowserChromeFocus

NS_IMETHODIMP CGeckoBrowserImpl::FocusNextElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::FocusPrevElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

// nsIWebProgressListener 

NS_IMETHODIMP CGeckoBrowserImpl::OnProgressChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest,
                                                  PRInt32 aCurSelfProgress, PRInt32 aMaxSelfProgress,
                                                  PRInt32 aCurTotalProgress, PRInt32 aMaxTotalProgress)
{
	if(m_listener)
		m_listener->OnProgressChange(aCurTotalProgress, aMaxTotalProgress);
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::OnStateChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest,
                                               PRUint32 aStateFlags, nsresult aStatus)
{
	if(!m_listener) return NS_OK;
    if((aStateFlags & STATE_START) && (aStateFlags & STATE_IS_DOCUMENT))
    {
		m_listener->OnDownloadBegin();
    }

    if((aStateFlags & STATE_STOP) && (aStateFlags & STATE_IS_DOCUMENT))
    {
		m_listener->OnProgressChange(0, 0);
		m_listener->OnDownloadComplete();
		m_listener->OnDocumentComplete(m_browser, CString());
		m_listener->OnStatusTextChange(CString(_T("")));
    }

	if((aStateFlags & STATE_STOP) && (aStateFlags & STATE_IS_NETWORK))
	{
		m_listener->OnNavigateComplete2(NULL, CString());
	}

	if((aStateFlags & STATE_IS_DOCUMENT) && 
	   ((aStateFlags & STATE_START) || (aStateFlags & STATE_STOP)))
	{
		nsCOMPtr<nsIWebNavigation> nav(do_QueryInterface(m_browser));
		PRBool fwd;
		nav->GetCanGoForward(&fwd);
		m_listener->OnCommandStateChange(CSC_NAVIGATEFORWARD, fwd == PR_TRUE);
		PRBool back;
		nav->GetCanGoBack(&back);
		m_listener->OnCommandStateChange(CSC_NAVIGATEBACK, back == PR_TRUE);
	}
    return NS_OK;
}


NS_IMETHODIMP CGeckoBrowserImpl::OnLocationChange(nsIWebProgress *aWebProgress,
                                                 nsIRequest *aRequest,
                                                 nsIURI *aLocation)
{
	if(!m_listener) return NS_OK;

	if(aWebProgress) {
		nsCOMPtr<nsIDOMWindow> win;
		nsCOMPtr<nsIDOMWindow> topWin;
		aWebProgress->GetDOMWindow(getter_AddRefs(win));
		if(win) win->GetTop(getter_AddRefs(topWin));
		if(win == topWin){ //the window location changed is the top window
			nsEmbedCString str;
			aLocation->GetSpec(str);
			m_listener->OnLocationChange(str.get());
		}
	}

	return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::OnStatusChange(nsIWebProgress *aWebProgress,
												nsIRequest *aRequest,
												nsresult aStatus,
												const PRUnichar *aMessage)
{
	if(m_listener) m_listener->OnStatusTextChange(aMessage);
    return NS_OK;
}



NS_IMETHODIMP CGeckoBrowserImpl::OnSecurityChange(nsIWebProgress *aWebProgress, 
												  nsIRequest *aRequest, 
												  PRUint32 aState)
{
    return NS_OK;
}

// nsISHistoryListener 

NS_IMETHODIMP CGeckoBrowserImpl::OnHistoryNewEntry(nsIURI *aNewURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::OnHistoryGoBack(nsIURI *aBackURI, PRBool *_retval)
{
    *_retval = PR_TRUE;
    return  NS_ERROR_NOT_IMPLEMENTED;
}


NS_IMETHODIMP CGeckoBrowserImpl::OnHistoryGoForward(nsIURI *aForwardURI, PRBool *_retval)
{
    *_retval = PR_TRUE;
    return NS_ERROR_NOT_IMPLEMENTED;
}


NS_IMETHODIMP CGeckoBrowserImpl::OnHistoryGotoIndex(PRInt32 aIndex, nsIURI *aGotoURI, PRBool *_retval)
{
    *_retval = PR_TRUE;
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::OnHistoryReload(nsIURI *aURI, PRUint32 aReloadFlags, PRBool *_retval)
{
    *_retval = PR_TRUE;
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::OnHistoryPurge(PRInt32 aNumEntries, PRBool *_retval)
{
    *_retval = PR_TRUE;
    return NS_ERROR_NOT_IMPLEMENTED;
}

// nsIEmbeddingSiteWindow

NS_IMETHODIMP CGeckoBrowserImpl::SetDimensions(PRUint32 aFlags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetDimensions(PRUint32 aFlags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy)
{
    if(aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_POSITION)
    {
        *x = 0;
        *y = 0;
    }
    if(aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_INNER ||
       aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_OUTER)
    {
        *cx = 0;
        *cy = 0;
    }
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::SetFocus()
{
	::SetFocus(m_hwnd);
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetTitle(PRUnichar **aTitle)
{
   NS_ENSURE_ARG_POINTER(aTitle);

   *aTitle = nsnull;
   
   return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::SetTitle(const PRUnichar *aTitle)
{
    if(m_listener) m_listener->OnTitleChange(CString(aTitle));
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetVisibility(PRBool *aVisibility)
{
    NS_ENSURE_ARG_POINTER(aVisibility);
    *aVisibility = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::SetVisibility(PRBool aVisibility)
{
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetSiteWindow(void **aSiteWindow)
{
   NS_ENSURE_ARG_POINTER(aSiteWindow);

   *aSiteWindow = &m_hwnd;
   return NS_OK;
}

void CGeckoBrowserImpl::SetParentWindow(HWND hwnd)
{
	m_hwnd = hwnd;
}


// nsIObserver   

NS_IMETHODIMP CGeckoBrowserImpl::Observe(nsISupports *aSubject, const char *aTopic, const PRUnichar *aData)
{
    nsresult rv = NS_OK;
	//TODO: detect profile change
    return rv;
}

// nsIContextMenuListener2

NS_IMETHODIMP CGeckoBrowserImpl::OnShowContextMenu(PRUint32 aContextFlags, nsIContextMenuInfo *aUtils)
{
	if(m_listener) m_listener->OnShowContextMenu(aContextFlags, aUtils);
    return NS_OK;
}

// nsITooltipListener

NS_IMETHODIMP CGeckoBrowserImpl::OnShowTooltip(PRInt32 aXCoords, PRInt32 aYCoords, const PRUnichar *aTipText)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::OnHideTooltip()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

// nsIURIContentListener

NS_IMETHODIMP CGeckoBrowserImpl::OnStartURIOpen(nsIURI *aURI, PRBool *_retval)
{
	if(!m_listener) return NS_ERROR_ABORT;
	nsEmbedCString spec;
	nsresult rv = aURI->GetSpec(spec);
	if(NS_FAILED(rv)) return rv;
	bool cancel = false;
	m_listener->OnBeforeNavigate(CString(spec.get()), cancel);
	*_retval = cancel ? PR_TRUE : PR_FALSE;
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::DoContent(const char *aContentType, PRBool aIsContentPreferred, nsIRequest *aRequest, nsIStreamListener **aContentHandler , PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::IsPreferred(const char *aContentType, char **aDesiredContentType , PRBool *_retval )
{
	return CanHandleContent(aContentType, PR_TRUE, aDesiredContentType, _retval);
}

NS_IMETHODIMP CGeckoBrowserImpl::CanHandleContent(const char *aContentType, PRBool aIsContentPreferred, char **aDesiredContentType, PRBool *_retval)
{
	*_retval = PR_FALSE;
	*aDesiredContentType = NULL;
	if(aContentType){
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_browser);
		nsCOMPtr<nsIWebNavigationInfo> info = do_GetService("@mozilla.org/webnavigation-info;1");
         if(info)
         {
             PRUint32 canHandle;
			 nsresult rv = info->IsTypeSupported(nsEmbedCString(aContentType), nav, &canHandle);
			 if(NS_FAILED(rv)) return rv;
			 *_retval = (canHandle != nsIWebNavigationInfo::UNSUPPORTED) ? PR_TRUE : PR_FALSE;
         }
	}
    return NS_OK;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetLoadCookie(nsISupports **aLoadCookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::SetLoadCookie(nsISupports *aLoadCookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::GetParentContentListener(nsIURIContentListener **aParentContentListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP CGeckoBrowserImpl::SetParentContentListener(nsIURIContentListener *aParentContentListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}