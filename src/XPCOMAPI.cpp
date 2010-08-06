#include "stdafx.h"
#include "XPCOMAPI.h"

//#include "MDIChildUserMessenger.h"
//#include "option/CloseTitleOption.h"
//#include "dialog/aboutdlg.h"
//#include "ChildFrame.h"
#include "MainFrame.h"

/* Implementation file */
NS_IMPL_ISUPPORTS1(DonutAPI, donutIAPI)

NS_IMPL_ISUPPORTS1(DonutAPIFactory, nsIFactory);

DonutAPI::DonutAPI()
{
  /* member initializers and constructor code */
}

DonutAPI::~DonutAPI()
{
  /* destructor code */
}

/* void close (in long index); */
NS_IMETHODIMP DonutAPI::Close(PRInt32 index)
{
	if (!g_pMainWnd)
		return NS_OK;
	g_pMainWnd->ApiClose(index);
	return NS_OK;
}

/* nsIDOMDocument getDocumentObject (in long index); */
NS_IMETHODIMP DonutAPI::GetDocumentObject(PRInt32 index, nsIDOMDocument **_retval NS_OUTPARAM)
{
	if (!g_pMainWnd)
		return NS_OK;
	*_retval = g_pMainWnd->ApiGetDocumentObject(index);
	return NS_OK;
}

/* nsIDOMWindow getWindowObject (in long index); */
NS_IMETHODIMP DonutAPI::GetWindowObject(PRInt32 index, nsIDOMWindow **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIWebBrowser getWebBrowserObject (in long index); */
NS_IMETHODIMP DonutAPI::GetWebBrowserObject(PRInt32 index, nsIWebBrowser **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long tabIndex; */
NS_IMETHODIMP DonutAPI::GetTabIndex(PRInt32 *aTabIndex)
{
	if (!g_pMainWnd)
		return NS_OK;
	*aTabIndex = g_pMainWnd->ApiGetTabIndex();
	return NS_OK;
}
NS_IMETHODIMP DonutAPI::SetTabIndex(PRInt32 aTabIndex)
{
	if (!g_pMainWnd)
		return NS_OK;
	g_pMainWnd->ApiSetTabIndex(aTabIndex);
	return NS_OK;
}

/* readonly attribute long tabCount; */
NS_IMETHODIMP DonutAPI::GetTabCount(PRInt32 *aTabCount)
{
	if (!g_pMainWnd)
		return NS_OK;
	*aTabCount = g_pMainWnd->ApiGetTabCount();
	return NS_OK;
}

/* void moveToTab (in long wBefor, in long wAfter); */
NS_IMETHODIMP DonutAPI::MoveToTab(PRInt32 wBefor, PRInt32 wAfter)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void newWindow (in wstring url, in boolean bActive); */
NS_IMETHODIMP DonutAPI::NewWindow(const PRUnichar *url, PRBool bActive)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long messageBox (in wstring text, in wstring caption, in unsigned long type); */
NS_IMETHODIMP DonutAPI::MessageBox(const PRUnichar *text, const PRUnichar *caption, PRUint32 type, PRInt32 *_retval NS_OUTPARAM)
{
	if (!g_pMainWnd)
		return NS_OK;

	CString strText(text);
	CString strCaption(caption);
	*_retval = ::MessageBox(g_pMainWnd->m_hWnd, strText, strCaption, type);
	return NS_OK;
}

/* void showPanelBar (); */
NS_IMETHODIMP DonutAPI::ShowPanelBar()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMDocument panelDocumentObject; */
NS_IMETHODIMP DonutAPI::GetPanelDocumentObject(nsIDOMDocument * *aPanelDocumentObject)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMWindow panelWindowObject; */
NS_IMETHODIMP DonutAPI::GetPanelWindowObject(nsIDOMWindow * *aPanelWindowObject)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIWebBrowser panelWebBrowserObject; */
NS_IMETHODIMP DonutAPI::GetPanelWebBrowserObject(nsIWebBrowser * *aPanelWebBrowserObject)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long getTabState (in long index); */
NS_IMETHODIMP DonutAPI::GetTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void executeCommand (in long command); */
NS_IMETHODIMP DonutAPI::ExecuteCommand(PRInt32 command)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring searchText; */
NS_IMETHODIMP DonutAPI::GetSearchText(PRUnichar * *aSearchText)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP DonutAPI::SetSearchText(const PRUnichar * aSearchText)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring addressText; */
NS_IMETHODIMP DonutAPI::GetAddressText(PRUnichar * *aAddressText)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP DonutAPI::SetAddressText(const PRUnichar * aAddressText)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long getExtendedTabState (in long index); */
NS_IMETHODIMP DonutAPI::GetExtendedTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setExtendedTabState (in long index, in long state); */
NS_IMETHODIMP DonutAPI::SetExtendedTabState(PRInt32 index, PRInt32 state)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long keyState (in long key); */
NS_IMETHODIMP DonutAPI::KeyState(PRInt32 key, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long getProfileInt (in wstring file, in wstring section, in wstring key, in long defval); */
NS_IMETHODIMP DonutAPI::GetProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 defval, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void writeProfileInt (in wstring file, in wstring section, in wstring key, in long value); */
NS_IMETHODIMP DonutAPI::WriteProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* wstring getProfileString (in wstring file, in wstring section, in wstring key, in wstring defval); */
NS_IMETHODIMP DonutAPI::GetProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *defval, PRUnichar **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void writeProfileString (in wstring file, in wstring section, in wstring key, in wstring text); */
NS_IMETHODIMP DonutAPI::WriteProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *text)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsILocalFile scriptFolder; */
NS_IMETHODIMP DonutAPI::GetScriptFolder(nsILocalFile * *aScriptFolder)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsILocalFile cssFolder; */
NS_IMETHODIMP DonutAPI::GetCssFolder(nsILocalFile * *aCssFolder)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsILocalFile baseFolder; */
NS_IMETHODIMP DonutAPI::GetBaseFolder(nsILocalFile * *aBaseFolder)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsILocalFile exePath; */
NS_IMETHODIMP DonutAPI::GetExePath(nsILocalFile * *aExePath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
