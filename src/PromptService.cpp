#include "stdafx.h"
#include "PromptService.h"
#include "GeckoPromptDialog.h"
#include "GeckoSelectDialog.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWindowWatcher.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIAuthInformation.h"

CPromptService::CPromptService(void)
{
}

CPromptService::~CPromptService(void)
{
}

NS_IMPL_ISUPPORTS2(CPromptService, nsIPromptService, nsIPromptService2)

NS_IMPL_ISUPPORTS1(CPromptServiceFactory, nsIFactory);

HWND CPromptService::GetHwndForWindow(nsIDOMWindow *win)
{
	nsCOMPtr<nsIWindowWatcher> ww = do_GetService("@mozilla.org/embedcomp/window-watcher;1");
	if(!ww) return NULL;
	nsCOMPtr<nsIWebBrowserChrome> chrome;
	ww->GetChromeForWindow(win, getter_AddRefs(chrome));
	if(!chrome) return NULL;
	nsCOMPtr<nsIEmbeddingSiteWindow> siteWindow = do_QueryInterface(chrome);
	if(!siteWindow) return NULL;
	HWND *hWnd;
	siteWindow->GetSiteWindow((void**)&hWnd);
	return *hWnd;
}


NS_IMETHODIMP
CPromptService::Alert(nsIDOMWindow* aParent, const PRUnichar* aDialogTitle, 
					  const PRUnichar* aDialogText)
{
	::MessageBox(GetHwndForWindow(aParent), aDialogText, aDialogTitle, MB_OK | MB_ICONWARNING);
	return NS_OK;
}

NS_IMETHODIMP
CPromptService::AlertCheck(nsIDOMWindow* aParent,
						   const PRUnichar* aDialogTitle,
						   const PRUnichar* aDialogText,
						   const PRUnichar* aCheckMsg, PRBool* aCheckValue)
{
    NS_ENSURE_ARG_POINTER(aCheckValue);

    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
CPromptService::Confirm(nsIDOMWindow* aParent,
						const PRUnichar* aDialogTitle,
						const PRUnichar* aDialogText, PRBool* aConfirm)
{
    NS_ENSURE_ARG_POINTER(aConfirm);
	int rv = ::MessageBox(GetHwndForWindow(aParent), aDialogText, aDialogTitle, MB_OKCANCEL);
	if(rv == IDOK){
		*aConfirm = PR_TRUE;
	}else{
		*aConfirm = PR_FALSE;
	}
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::ConfirmCheck(nsIDOMWindow* aParent,
							 const PRUnichar* aDialogTitle,
							 const PRUnichar* aDialogText,
							 const PRUnichar* aCheckMsg,
							 PRBool* aCheckValue, PRBool* aConfirm)
{
    NS_ENSURE_ARG_POINTER(aCheckValue);
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
CPromptService::ConfirmEx(nsIDOMWindow* aParent,
						  const PRUnichar* aDialogTitle,
						  const PRUnichar* aDialogText,
						  PRUint32 aButtonFlags,
						  const PRUnichar* aButton0Title,
						  const PRUnichar* aButton1Title,
						  const PRUnichar* aButton2Title,
						  const PRUnichar* aCheckMsg, PRBool* aCheckValue,
						  PRInt32* aRetVal)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
CPromptService::Prompt(nsIDOMWindow* aParent, const PRUnichar* aDialogTitle,
					   const PRUnichar* aDialogText, PRUnichar** aValue,
					   const PRUnichar* aCheckMsg, PRBool* aCheckValue,
					   PRBool* aConfirm)
{
	CGeckoPromptDlg dlg;
	dlg.m_strTitle = CString(aDialogTitle);
	dlg.m_strMsg = CString(aDialogText);
	dlg.m_strEdit = CString(*aValue);
	dlg.m_showCheck = (aCheckMsg != NULL) && (aCheckValue != NULL);
	dlg.m_showPwd = false;
	if(dlg.m_showCheck){
		dlg.m_strCheck = CString( aCheckMsg );
		dlg.m_isChecked = (*aCheckValue == PR_TRUE);
	}

	if ( dlg.DoModal() == IDOK ) {
		NS_Free(*aValue);
		*aValue = (PRUnichar*)NS_Alloc(sizeof(PRUnichar) * (dlg.m_strEdit.GetLength() + 1));
		wcscpy(*aValue, dlg.m_strEdit);
		if(aCheckValue) *aCheckValue = dlg.m_isChecked;
		if(aConfirm) *aConfirm = PR_TRUE;
	}else{
		if(aConfirm) *aConfirm = PR_FALSE;
	}
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::PromptUsernameAndPassword(nsIDOMWindow* aParent,
										  const PRUnichar* aDialogTitle,
										  const PRUnichar* aDialogText,
										  PRUnichar** aUsername,
										  PRUnichar** aPassword,
										  const PRUnichar* aCheckMsg,
										  PRBool* aCheckValue,
										  PRBool* aConfirm)
{
	CGeckoPromptDlg dlg;
	dlg.m_strTitle = CString(aDialogTitle);
	dlg.m_strMsg = CString(aDialogText);
	dlg.m_strEdit = CString(*aUsername);
	dlg.m_strPwd = CString(*aPassword);
	dlg.m_showCheck = (aCheckMsg != NULL) && (aCheckValue != NULL);
	dlg.m_showPwd = true;

	if(dlg.m_showCheck){
		dlg.m_strCheck = CString( aCheckMsg );
		dlg.m_isChecked = (*aCheckValue == PR_TRUE);
	}

	if ( dlg.DoModal() == IDOK ) {
		NS_Free(*aUsername);
		*aUsername = (PRUnichar*)NS_Alloc(sizeof(PRUnichar) * (dlg.m_strEdit.GetLength() + 1));
		wcscpy(*aUsername, dlg.m_strEdit);

		NS_Free(*aPassword);
		*aPassword = (PRUnichar*)NS_Alloc(sizeof(PRUnichar) * (dlg.m_strPwd.GetLength() + 1));
		wcscpy(*aPassword, dlg.m_strPwd);

		if(aCheckValue) *aCheckValue = dlg.m_isChecked;
		if(aConfirm) *aConfirm = PR_TRUE;
	}else{
		if(aConfirm) *aConfirm = PR_FALSE;
	}
    return NS_OK;
}

NS_IMETHODIMP
CPromptService::PromptPassword(nsIDOMWindow* aParent,
							   const PRUnichar* aDialogTitle,
							   const PRUnichar* aDialogText,
							   PRUnichar** aPassword,
							   const PRUnichar* aCheckMsg,
							   PRBool* aCheckValue, PRBool* aConfirm)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
CPromptService::Select(nsIDOMWindow* aParent, const PRUnichar* aDialogTitle,
					   const PRUnichar* aDialogText, PRUint32 aCount,
					   const PRUnichar** aSelectList, PRInt32* outSelection,
					   PRBool* aConfirm)
{
	CGeckoSelectDlg dlg;
	dlg.m_strTitle = CString(aDialogTitle);
	dlg.m_strMsg = CString(aDialogText);

	for(unsigned int i=0;i<aCount;i++){
		dlg.m_strList.Add(CString(aSelectList[i]));
	}

	if ( dlg.DoModal() == IDOK ) {
		if(outSelection) *outSelection = dlg.m_selection;
		if(aConfirm) *aConfirm = PR_TRUE;
	}else{
		if(aConfirm) *aConfirm = PR_FALSE;
	}
    return NS_OK;
}

/* boolean promptAuth (in nsIDOMWindow aParent, in nsIChannel aChannel, in PRUint32 level, in nsIAuthInformation authInfo, in wstring checkboxLabel, inout boolean checkValue); */
NS_IMETHODIMP CPromptService::PromptAuth(nsIDOMWindow *aParent,
										 nsIChannel *aChannel, 
										 PRUint32 level, 
										 nsIAuthInformation *aAuthInfo, 
										 const PRUnichar *aCheckMsg,
										 PRBool *aCheckValue NS_INOUTPARAM, 
										 PRBool *aConfirm NS_OUTPARAM)
{
	nsEmbedString realm,username,password,domain;
	aAuthInfo->GetDomain(domain);
	aAuthInfo->GetRealm(realm);
	aAuthInfo->GetPassword(password);
	aAuthInfo->GetUsername(username);

	CGeckoPromptDlg dlg;
	dlg.m_strTitle = CString(_T("ユーザー名とパスワードを入力してください"));
	dlg.m_strMsg = /*TODO: CString(domain.get()) +*/ _T("\"") + CString(realm.get()) + _T("\"に対するユーザー名とパスワードを入力してください");
	dlg.m_strEdit = CString(username.get());
	dlg.m_strPwd = CString(password.get());
	dlg.m_showCheck = (aCheckMsg != NULL) && (aCheckValue != NULL);
	dlg.m_showPwd = true;

	if(dlg.m_showCheck){
		dlg.m_strCheck = CString( aCheckMsg );
		dlg.m_isChecked = (*aCheckValue == PR_TRUE);
	}

	if ( dlg.DoModal() == IDOK ) {
		aAuthInfo->SetUsername(nsEmbedString(dlg.m_strEdit));
		aAuthInfo->SetPassword(nsEmbedString(dlg.m_strPwd));

		if(aCheckValue) *aCheckValue = dlg.m_isChecked;
		if(aConfirm) *aConfirm = PR_TRUE;
	}else{
		if(aConfirm) *aConfirm = PR_FALSE;
	}
    return NS_OK;
}

/* nsICancelable asyncPromptAuth (in nsIDOMWindow aParent, in nsIChannel aChannel, in nsIAuthPromptCallback aCallback, in nsISupports aContext, in PRUint32 level, in nsIAuthInformation authInfo, in wstring checkboxLabel, inout boolean checkValue); */
NS_IMETHODIMP CPromptService::AsyncPromptAuth(nsIDOMWindow *aParent, nsIChannel *aChannel, nsIAuthPromptCallback *aCallback, nsISupports *aContext, PRUint32 level, nsIAuthInformation *authInfo, const PRUnichar *checkboxLabel, PRBool *checkValue NS_INOUTPARAM, nsICancelable **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
