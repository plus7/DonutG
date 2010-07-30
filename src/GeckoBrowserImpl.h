#pragma once

#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserChromeFocus.h"
#include "nsIContextMenuListener2.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIObserver.h"
#include "nsISHistoryListener.h"
#include "nsITooltipListener.h"
#include "nsIWebProgressListener.h"
#include "nsIURIContentListener.h"

class IGeckoBrowserEventListener;
class CGeckoBrowserImpl :  //public nsISupports,
	                       public nsIWebBrowserChrome,
                           public nsIWebBrowserChromeFocus,
                           public nsIWebProgressListener,
                           public nsIEmbeddingSiteWindow,
                           public nsIInterfaceRequestor,
                           public nsISHistoryListener,
                           public nsIObserver,
                           public nsIContextMenuListener2,
                           public nsITooltipListener,
						   public nsIURIContentListener,
                           public nsSupportsWeakReference
{
public:
	CGeckoBrowserImpl(void);
	virtual ~CGeckoBrowserImpl(void);

    NS_DECL_ISUPPORTS
    NS_DECL_NSIWEBBROWSERCHROME
    NS_DECL_NSIWEBBROWSERCHROMEFOCUS
    NS_DECL_NSIWEBPROGRESSLISTENER
    NS_DECL_NSIEMBEDDINGSITEWINDOW
    NS_DECL_NSIINTERFACEREQUESTOR
    NS_DECL_NSISHISTORYLISTENER
    NS_DECL_NSIOBSERVER
    NS_DECL_NSICONTEXTMENULISTENER2
    NS_DECL_NSITOOLTIPLISTENER
	NS_DECL_NSIURICONTENTLISTENER

	void SetEventListener(IGeckoBrowserEventListener *listener);
	IGeckoBrowserEventListener* GetEventListener(){ return m_listener; }
	void SetParentWindow(HWND hwnd);
private:
    IGeckoBrowserEventListener *m_listener;
	nsCOMPtr<nsIWebBrowser> m_browser;
	HWND m_hwnd;
	PRUint32 m_chromeFlags;
};

