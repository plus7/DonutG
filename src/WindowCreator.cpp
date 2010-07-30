#include "stdafx.h"
#include "WindowCreator.h"
#include "IGeckoBrowserEventListener.h"
#include "GeckoBrowserImpl.h"

NS_IMPL_ISUPPORTS1(CWindowCreator, nsIWindowCreator)

CWindowCreator::CWindowCreator(void)
{
}

CWindowCreator::~CWindowCreator(void)
{
}

NS_IMETHODIMP CWindowCreator::CreateChromeWindow(nsIWebBrowserChrome *aParent, PRUint32 aChromeFlags, nsIWebBrowserChrome **_retval)
{
	NS_ENSURE_ARG_POINTER(_retval);
	*_retval = NULL;
	if(!aParent) return NS_ERROR_FAILURE;
	CGeckoBrowserImpl *pImpl = static_cast<CGeckoBrowserImpl*>(aParent);
	nsCOMPtr<nsIWebBrowser> b;
	bool cancel = false;
	IGeckoBrowserEventListener *listener = pImpl->GetEventListener();
	if(listener) listener->OnNewWindow(getter_AddRefs(b), &cancel);
	if(!cancel && b){
		nsresult rv = b->GetContainerWindow(_retval);
		return rv;
	}
	return NS_ERROR_FAILURE;
}