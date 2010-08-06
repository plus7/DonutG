/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM DonutG.idl
 */

#ifndef __gen_DonutG_h__
#define __gen_DonutG_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIDOMDocument; /* forward declaration */

class nsIDOMWindow; /* forward declaration */

class nsIWebBrowser; /* forward declaration */

class nsILocalFile; /* forward declaration */


/* starting interface:    donutIAPI */
#define DONUTIAPI_IID_STR "6d70c628-b3ce-4d1f-a6e3-bbefcbb45cb1"

#define DONUTIAPI_IID \
  {0x6d70c628, 0xb3ce, 0x4d1f, \
    { 0xa6, 0xe3, 0xbb, 0xef, 0xcb, 0xb4, 0x5c, 0xb1 }}

class NS_NO_VTABLE NS_SCRIPTABLE donutIAPI : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(DONUTIAPI_IID)

  enum { CHROME_MODAL = 536870912U };

  /* void close (in long index); */
  NS_SCRIPTABLE NS_IMETHOD Close(PRInt32 index) = 0;

  /* nsIDOMDocument getDocumentObject (in long index); */
  NS_SCRIPTABLE NS_IMETHOD GetDocumentObject(PRInt32 index, nsIDOMDocument **_retval NS_OUTPARAM) = 0;

  /* nsIDOMWindow getWindowObject (in long index); */
  NS_SCRIPTABLE NS_IMETHOD GetWindowObject(PRInt32 index, nsIDOMWindow **_retval NS_OUTPARAM) = 0;

  /* nsIWebBrowser getWebBrowserObject (in long index); */
  NS_SCRIPTABLE NS_IMETHOD GetWebBrowserObject(PRInt32 index, nsIWebBrowser **_retval NS_OUTPARAM) = 0;

  /* attribute long tabIndex; */
  NS_SCRIPTABLE NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) = 0;

  /* readonly attribute long tabCount; */
  NS_SCRIPTABLE NS_IMETHOD GetTabCount(PRInt32 *aTabCount) = 0;

  /* void moveToTab (in long wBefor, in long wAfter); */
  NS_SCRIPTABLE NS_IMETHOD MoveToTab(PRInt32 wBefor, PRInt32 wAfter) = 0;

  /* void newWindow (in wstring url, in boolean bActive); */
  NS_SCRIPTABLE NS_IMETHOD NewWindow(const PRUnichar *url, PRBool bActive) = 0;

  /* long messageBox (in wstring text, in wstring caption, in unsigned long type); */
  NS_SCRIPTABLE NS_IMETHOD MessageBox(const PRUnichar *text, const PRUnichar *caption, PRUint32 type, PRInt32 *_retval NS_OUTPARAM) = 0;

  /* void showPanelBar (); */
  NS_SCRIPTABLE NS_IMETHOD ShowPanelBar(void) = 0;

  /* readonly attribute nsIDOMDocument panelDocumentObject; */
  NS_SCRIPTABLE NS_IMETHOD GetPanelDocumentObject(nsIDOMDocument * *aPanelDocumentObject) = 0;

  /* readonly attribute nsIDOMWindow panelWindowObject; */
  NS_SCRIPTABLE NS_IMETHOD GetPanelWindowObject(nsIDOMWindow * *aPanelWindowObject) = 0;

  /* readonly attribute nsIWebBrowser panelWebBrowserObject; */
  NS_SCRIPTABLE NS_IMETHOD GetPanelWebBrowserObject(nsIWebBrowser * *aPanelWebBrowserObject) = 0;

  /* long getTabState (in long index); */
  NS_SCRIPTABLE NS_IMETHOD GetTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM) = 0;

  /* void executeCommand (in long command); */
  NS_SCRIPTABLE NS_IMETHOD ExecuteCommand(PRInt32 command) = 0;

  /* attribute wstring searchText; */
  NS_SCRIPTABLE NS_IMETHOD GetSearchText(PRUnichar * *aSearchText) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetSearchText(const PRUnichar * aSearchText) = 0;

  /* attribute wstring addressText; */
  NS_SCRIPTABLE NS_IMETHOD GetAddressText(PRUnichar * *aAddressText) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetAddressText(const PRUnichar * aAddressText) = 0;

  /* long getExtendedTabState (in long index); */
  NS_SCRIPTABLE NS_IMETHOD GetExtendedTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM) = 0;

  /* void setExtendedTabState (in long index, in long state); */
  NS_SCRIPTABLE NS_IMETHOD SetExtendedTabState(PRInt32 index, PRInt32 state) = 0;

  /* long keyState (in long key); */
  NS_SCRIPTABLE NS_IMETHOD KeyState(PRInt32 key, PRInt32 *_retval NS_OUTPARAM) = 0;

  /* long getProfileInt (in wstring file, in wstring section, in wstring key, in long defval); */
  NS_SCRIPTABLE NS_IMETHOD GetProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 defval, PRInt32 *_retval NS_OUTPARAM) = 0;

  /* void writeProfileInt (in wstring file, in wstring section, in wstring key, in long value); */
  NS_SCRIPTABLE NS_IMETHOD WriteProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 value) = 0;

  /* wstring getProfileString (in wstring file, in wstring section, in wstring key, in wstring defval); */
  NS_SCRIPTABLE NS_IMETHOD GetProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *defval, PRUnichar **_retval NS_OUTPARAM) = 0;

  /* void writeProfileString (in wstring file, in wstring section, in wstring key, in wstring text); */
  NS_SCRIPTABLE NS_IMETHOD WriteProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *text) = 0;

  /* readonly attribute nsILocalFile scriptFolder; */
  NS_SCRIPTABLE NS_IMETHOD GetScriptFolder(nsILocalFile * *aScriptFolder) = 0;

  /* readonly attribute nsILocalFile cssFolder; */
  NS_SCRIPTABLE NS_IMETHOD GetCssFolder(nsILocalFile * *aCssFolder) = 0;

  /* readonly attribute nsILocalFile baseFolder; */
  NS_SCRIPTABLE NS_IMETHOD GetBaseFolder(nsILocalFile * *aBaseFolder) = 0;

  /* readonly attribute nsILocalFile exePath; */
  NS_SCRIPTABLE NS_IMETHOD GetExePath(nsILocalFile * *aExePath) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(donutIAPI, DONUTIAPI_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_DONUTIAPI \
  NS_SCRIPTABLE NS_IMETHOD Close(PRInt32 index); \
  NS_SCRIPTABLE NS_IMETHOD GetDocumentObject(PRInt32 index, nsIDOMDocument **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetWindowObject(PRInt32 index, nsIDOMWindow **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetWebBrowserObject(PRInt32 index, nsIWebBrowser **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex); \
  NS_SCRIPTABLE NS_IMETHOD SetTabIndex(PRInt32 aTabIndex); \
  NS_SCRIPTABLE NS_IMETHOD GetTabCount(PRInt32 *aTabCount); \
  NS_SCRIPTABLE NS_IMETHOD MoveToTab(PRInt32 wBefor, PRInt32 wAfter); \
  NS_SCRIPTABLE NS_IMETHOD NewWindow(const PRUnichar *url, PRBool bActive); \
  NS_SCRIPTABLE NS_IMETHOD MessageBox(const PRUnichar *text, const PRUnichar *caption, PRUint32 type, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD ShowPanelBar(void); \
  NS_SCRIPTABLE NS_IMETHOD GetPanelDocumentObject(nsIDOMDocument * *aPanelDocumentObject); \
  NS_SCRIPTABLE NS_IMETHOD GetPanelWindowObject(nsIDOMWindow * *aPanelWindowObject); \
  NS_SCRIPTABLE NS_IMETHOD GetPanelWebBrowserObject(nsIWebBrowser * *aPanelWebBrowserObject); \
  NS_SCRIPTABLE NS_IMETHOD GetTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD ExecuteCommand(PRInt32 command); \
  NS_SCRIPTABLE NS_IMETHOD GetSearchText(PRUnichar * *aSearchText); \
  NS_SCRIPTABLE NS_IMETHOD SetSearchText(const PRUnichar * aSearchText); \
  NS_SCRIPTABLE NS_IMETHOD GetAddressText(PRUnichar * *aAddressText); \
  NS_SCRIPTABLE NS_IMETHOD SetAddressText(const PRUnichar * aAddressText); \
  NS_SCRIPTABLE NS_IMETHOD GetExtendedTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD SetExtendedTabState(PRInt32 index, PRInt32 state); \
  NS_SCRIPTABLE NS_IMETHOD KeyState(PRInt32 key, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 defval, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD WriteProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 value); \
  NS_SCRIPTABLE NS_IMETHOD GetProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *defval, PRUnichar **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD WriteProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *text); \
  NS_SCRIPTABLE NS_IMETHOD GetScriptFolder(nsILocalFile * *aScriptFolder); \
  NS_SCRIPTABLE NS_IMETHOD GetCssFolder(nsILocalFile * *aCssFolder); \
  NS_SCRIPTABLE NS_IMETHOD GetBaseFolder(nsILocalFile * *aBaseFolder); \
  NS_SCRIPTABLE NS_IMETHOD GetExePath(nsILocalFile * *aExePath); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_DONUTIAPI(_to) \
  NS_SCRIPTABLE NS_IMETHOD Close(PRInt32 index) { return _to Close(index); } \
  NS_SCRIPTABLE NS_IMETHOD GetDocumentObject(PRInt32 index, nsIDOMDocument **_retval NS_OUTPARAM) { return _to GetDocumentObject(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetWindowObject(PRInt32 index, nsIDOMWindow **_retval NS_OUTPARAM) { return _to GetWindowObject(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetWebBrowserObject(PRInt32 index, nsIWebBrowser **_retval NS_OUTPARAM) { return _to GetWebBrowserObject(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return _to GetTabIndex(aTabIndex); } \
  NS_SCRIPTABLE NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return _to SetTabIndex(aTabIndex); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabCount(PRInt32 *aTabCount) { return _to GetTabCount(aTabCount); } \
  NS_SCRIPTABLE NS_IMETHOD MoveToTab(PRInt32 wBefor, PRInt32 wAfter) { return _to MoveToTab(wBefor, wAfter); } \
  NS_SCRIPTABLE NS_IMETHOD NewWindow(const PRUnichar *url, PRBool bActive) { return _to NewWindow(url, bActive); } \
  NS_SCRIPTABLE NS_IMETHOD MessageBox(const PRUnichar *text, const PRUnichar *caption, PRUint32 type, PRInt32 *_retval NS_OUTPARAM) { return _to MessageBox(text, caption, type, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ShowPanelBar(void) { return _to ShowPanelBar(); } \
  NS_SCRIPTABLE NS_IMETHOD GetPanelDocumentObject(nsIDOMDocument * *aPanelDocumentObject) { return _to GetPanelDocumentObject(aPanelDocumentObject); } \
  NS_SCRIPTABLE NS_IMETHOD GetPanelWindowObject(nsIDOMWindow * *aPanelWindowObject) { return _to GetPanelWindowObject(aPanelWindowObject); } \
  NS_SCRIPTABLE NS_IMETHOD GetPanelWebBrowserObject(nsIWebBrowser * *aPanelWebBrowserObject) { return _to GetPanelWebBrowserObject(aPanelWebBrowserObject); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM) { return _to GetTabState(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteCommand(PRInt32 command) { return _to ExecuteCommand(command); } \
  NS_SCRIPTABLE NS_IMETHOD GetSearchText(PRUnichar * *aSearchText) { return _to GetSearchText(aSearchText); } \
  NS_SCRIPTABLE NS_IMETHOD SetSearchText(const PRUnichar * aSearchText) { return _to SetSearchText(aSearchText); } \
  NS_SCRIPTABLE NS_IMETHOD GetAddressText(PRUnichar * *aAddressText) { return _to GetAddressText(aAddressText); } \
  NS_SCRIPTABLE NS_IMETHOD SetAddressText(const PRUnichar * aAddressText) { return _to SetAddressText(aAddressText); } \
  NS_SCRIPTABLE NS_IMETHOD GetExtendedTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM) { return _to GetExtendedTabState(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetExtendedTabState(PRInt32 index, PRInt32 state) { return _to SetExtendedTabState(index, state); } \
  NS_SCRIPTABLE NS_IMETHOD KeyState(PRInt32 key, PRInt32 *_retval NS_OUTPARAM) { return _to KeyState(key, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 defval, PRInt32 *_retval NS_OUTPARAM) { return _to GetProfileInt(file, section, key, defval, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD WriteProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 value) { return _to WriteProfileInt(file, section, key, value); } \
  NS_SCRIPTABLE NS_IMETHOD GetProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *defval, PRUnichar **_retval NS_OUTPARAM) { return _to GetProfileString(file, section, key, defval, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD WriteProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *text) { return _to WriteProfileString(file, section, key, text); } \
  NS_SCRIPTABLE NS_IMETHOD GetScriptFolder(nsILocalFile * *aScriptFolder) { return _to GetScriptFolder(aScriptFolder); } \
  NS_SCRIPTABLE NS_IMETHOD GetCssFolder(nsILocalFile * *aCssFolder) { return _to GetCssFolder(aCssFolder); } \
  NS_SCRIPTABLE NS_IMETHOD GetBaseFolder(nsILocalFile * *aBaseFolder) { return _to GetBaseFolder(aBaseFolder); } \
  NS_SCRIPTABLE NS_IMETHOD GetExePath(nsILocalFile * *aExePath) { return _to GetExePath(aExePath); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_DONUTIAPI(_to) \
  NS_SCRIPTABLE NS_IMETHOD Close(PRInt32 index) { return !_to ? NS_ERROR_NULL_POINTER : _to->Close(index); } \
  NS_SCRIPTABLE NS_IMETHOD GetDocumentObject(PRInt32 index, nsIDOMDocument **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDocumentObject(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetWindowObject(PRInt32 index, nsIDOMWindow **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWindowObject(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetWebBrowserObject(PRInt32 index, nsIWebBrowser **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWebBrowserObject(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabIndex(aTabIndex); } \
  NS_SCRIPTABLE NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabIndex(aTabIndex); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabCount(PRInt32 *aTabCount) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabCount(aTabCount); } \
  NS_SCRIPTABLE NS_IMETHOD MoveToTab(PRInt32 wBefor, PRInt32 wAfter) { return !_to ? NS_ERROR_NULL_POINTER : _to->MoveToTab(wBefor, wAfter); } \
  NS_SCRIPTABLE NS_IMETHOD NewWindow(const PRUnichar *url, PRBool bActive) { return !_to ? NS_ERROR_NULL_POINTER : _to->NewWindow(url, bActive); } \
  NS_SCRIPTABLE NS_IMETHOD MessageBox(const PRUnichar *text, const PRUnichar *caption, PRUint32 type, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->MessageBox(text, caption, type, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ShowPanelBar(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ShowPanelBar(); } \
  NS_SCRIPTABLE NS_IMETHOD GetPanelDocumentObject(nsIDOMDocument * *aPanelDocumentObject) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPanelDocumentObject(aPanelDocumentObject); } \
  NS_SCRIPTABLE NS_IMETHOD GetPanelWindowObject(nsIDOMWindow * *aPanelWindowObject) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPanelWindowObject(aPanelWindowObject); } \
  NS_SCRIPTABLE NS_IMETHOD GetPanelWebBrowserObject(nsIWebBrowser * *aPanelWebBrowserObject) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPanelWebBrowserObject(aPanelWebBrowserObject); } \
  NS_SCRIPTABLE NS_IMETHOD GetTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabState(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteCommand(PRInt32 command) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExecuteCommand(command); } \
  NS_SCRIPTABLE NS_IMETHOD GetSearchText(PRUnichar * *aSearchText) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSearchText(aSearchText); } \
  NS_SCRIPTABLE NS_IMETHOD SetSearchText(const PRUnichar * aSearchText) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSearchText(aSearchText); } \
  NS_SCRIPTABLE NS_IMETHOD GetAddressText(PRUnichar * *aAddressText) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAddressText(aAddressText); } \
  NS_SCRIPTABLE NS_IMETHOD SetAddressText(const PRUnichar * aAddressText) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAddressText(aAddressText); } \
  NS_SCRIPTABLE NS_IMETHOD GetExtendedTabState(PRInt32 index, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetExtendedTabState(index, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetExtendedTabState(PRInt32 index, PRInt32 state) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetExtendedTabState(index, state); } \
  NS_SCRIPTABLE NS_IMETHOD KeyState(PRInt32 key, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->KeyState(key, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 defval, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetProfileInt(file, section, key, defval, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD WriteProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, PRInt32 value) { return !_to ? NS_ERROR_NULL_POINTER : _to->WriteProfileInt(file, section, key, value); } \
  NS_SCRIPTABLE NS_IMETHOD GetProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *defval, PRUnichar **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetProfileString(file, section, key, defval, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD WriteProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar *key, const PRUnichar *text) { return !_to ? NS_ERROR_NULL_POINTER : _to->WriteProfileString(file, section, key, text); } \
  NS_SCRIPTABLE NS_IMETHOD GetScriptFolder(nsILocalFile * *aScriptFolder) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScriptFolder(aScriptFolder); } \
  NS_SCRIPTABLE NS_IMETHOD GetCssFolder(nsILocalFile * *aCssFolder) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCssFolder(aCssFolder); } \
  NS_SCRIPTABLE NS_IMETHOD GetBaseFolder(nsILocalFile * *aBaseFolder) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBaseFolder(aBaseFolder); } \
  NS_SCRIPTABLE NS_IMETHOD GetExePath(nsILocalFile * *aExePath) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetExePath(aExePath); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class DonutAPI : public donutIAPI
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_DONUTIAPI

  DonutAPI();

private:
  ~DonutAPI();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(DonutAPI, donutIAPI)

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
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMDocument getDocumentObject (in long index); */
NS_IMETHODIMP DonutAPI::GetDocumentObject(PRInt32 index, nsIDOMDocument **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
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
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP DonutAPI::SetTabIndex(PRInt32 aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long tabCount; */
NS_IMETHODIMP DonutAPI::GetTabCount(PRInt32 *aTabCount)
{
    return NS_ERROR_NOT_IMPLEMENTED;
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
    return NS_ERROR_NOT_IMPLEMENTED;
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

/* End of implementation class template. */
#endif


#endif /* __gen_DonutG_h__ */
