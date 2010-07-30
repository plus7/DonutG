#include "stdafx.h"
#include "resource.h"
#include "NemuChecker.h"

STDMETHODIMP CDonutPEvents::DocumentComplete (long nIndex, IDispatch * pDisp, BSTR bstrURL)
{
	CComBSTR url(bstrURL);

	ATLTRACE("CDonutPEvents::DocumentComplete()\n");

	CComPtr<IWebBrowser2> spBrowser;
	pDisp->QueryInterface(IID_IWebBrowser2, (void**)&spBrowser);
	
	CComBSTR bstrName;
	spBrowser->get_LocationName(&bstrName);

	CNemuToolBar::NotifyViewPage( CString(url) );

	return S_OK;
}
