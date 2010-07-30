/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   NOSE, Takafumi <ahya365@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "stdafx.h"

#include "GeckoPrintingPromptDialog.h"
#include "PageSetupDlg.h"

#include "nsPrintingPromptService.h"
#include "nsIPrintingPromptService.h"
#include "nsIFactory.h"
#include "nsPIDOMWindow.h"
//#include "nsReadableUtils.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIServiceManager.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWindowWatcher.h"
#include "nsPrintDialogUtil.h"

// Printing Progress Includes
#include "nsPrintProgress.h"
#include "nsPrintProgressParams.h"
#include "nsIWebProgressListener.h"

// XP Dialog includes
#include "nsIDialogParamBlock.h"
#include "nsISupportsUtils.h"
#include "nsISupportsArray.h"

// Includes need to locate the native Window
#include "nsIWidget.h"
#include "nsIBaseWindow.h"
#include "nsIWebBrowserChrome.h"
#include "nsIDocShellTreeOwner.h"
#include "nsIDocShellTreeItem.h"
#include "nsIDocShell.h"
#include "nsIInterfaceRequestorUtils.h"


static const char *kPrintProgressDialogURL  = "chrome://global/content/printProgress.xul";
static const char *kPrtPrvProgressDialogURL = "chrome://global/content/printPreviewProgress.xul";
static const char *kPageSetupDialogURL      = "chrome://global/content/printPageSetup.xul";

// Static Data 
static HINSTANCE gInstance;

/****************************************************************
 ************************* ParamBlock ***************************
 ****************************************************************/

class ParamBlock {

public:
    ParamBlock() 
    {
        mBlock = 0;
    }
    ~ParamBlock() 
    {
        NS_IF_RELEASE(mBlock);
    }
    nsresult Init() {
      return CallCreateInstance(NS_DIALOGPARAMBLOCK_CONTRACTID, &mBlock);
    }
    nsIDialogParamBlock * operator->() const { return mBlock; }
    operator nsIDialogParamBlock * const ()  { return mBlock; }

private:
    nsIDialogParamBlock *mBlock;
};

//*****************************************************************************   

NS_IMPL_ISUPPORTS2(nsPrintingPromptService, nsIPrintingPromptService, nsIWebProgressListener)

nsPrintingPromptService::nsPrintingPromptService() 
{
}

//-----------------------------------------------------------
nsPrintingPromptService::~nsPrintingPromptService()
{
}

//-----------------------------------------------------------
nsresult
nsPrintingPromptService::Init()
{
    nsresult rv;
    mWatcher = do_GetService(NS_WINDOWWATCHER_CONTRACTID, &rv);
    return rv;
}

//-----------------------------------------------------------
HWND
nsPrintingPromptService::GetHWNDForDOMWindow(nsIDOMWindow *aWindow)
{
    nsCOMPtr<nsIWebBrowserChrome> chrome;
    HWND hWnd = NULL;

    // We might be embedded so check this path first
    if (mWatcher) {
        nsCOMPtr<nsIDOMWindow> fosterParent;
        if (!aWindow) 
        {   // it will be a dependent window. try to find a foster parent.
            mWatcher->GetActiveWindow(getter_AddRefs(fosterParent));
            aWindow = fosterParent;
        }
        mWatcher->GetChromeForWindow(aWindow, getter_AddRefs(chrome));
    }

    if (chrome) {
        nsCOMPtr<nsIEmbeddingSiteWindow> site(do_QueryInterface(chrome));
        if (site) 
        {
            HWND w;
            site->GetSiteWindow(reinterpret_cast<void **>(&w));
            return w;
        }
    }

    // Now we might be the Browser so check this path
    nsCOMPtr<nsPIDOMWindow> window(do_QueryInterface(aWindow));

    nsCOMPtr<nsIDocShellTreeItem> treeItem =
        do_QueryInterface(window->GetDocShell());
    if (!treeItem) return nsnull;

    nsCOMPtr<nsIDocShellTreeOwner> treeOwner;
    treeItem->GetTreeOwner(getter_AddRefs(treeOwner));
    if (!treeOwner) return nsnull;

    nsCOMPtr<nsIWebBrowserChrome> webBrowserChrome(do_GetInterface(treeOwner));
    if (!webBrowserChrome) return nsnull;

    nsCOMPtr<nsIBaseWindow> baseWin(do_QueryInterface(webBrowserChrome));
    if (!baseWin) return nsnull;

    nsCOMPtr<nsIWidget> widget;
    baseWin->GetMainWidget(getter_AddRefs(widget));
    if (!widget) return nsnull;

    return (HWND)widget->GetNativeData(NS_NATIVE_WINDOW);

}


///////////////////////////////////////////////////////////////////////////////
// nsIPrintingPrompt

//-----------------------------------------------------------
NS_IMETHODIMP 
nsPrintingPromptService::ShowPrintDialog(nsIDOMWindow *parent, nsIWebBrowserPrint *webBrowserPrint, nsIPrintSettings *printSettings)
{
    NS_ENSURE_ARG(parent);

    HWND hWnd = GetHWNDForDOMWindow(parent);
    NS_ASSERTION(hWnd, "Couldn't get native window for PRint Dialog!");

    return NativeShowPrintDialog(hWnd, webBrowserPrint, printSettings);
}


/* void showProgress (in nsIDOMWindow parent, in nsIWebBrowserPrint webBrowserPrint, in nsIPrintSettings printSettings, in nsIObserver openDialogObserver, in boolean isForPrinting, out nsIWebProgressListener webProgressListener, out nsIPrintProgressParams printProgressParams, out boolean notifyOnOpen); */
NS_IMETHODIMP 
nsPrintingPromptService::ShowProgress(nsIDOMWindow*            parent, 
                                      nsIWebBrowserPrint*      webBrowserPrint,    // ok to be null
                                      nsIPrintSettings*        printSettings,      // ok to be null
                                      nsIObserver*             openDialogObserver, // ok to be null
                                      PRBool                   isForPrinting,
                                      nsIWebProgressListener** webProgressListener,
                                      nsIPrintProgressParams** printProgressParams,
                                      PRBool*                  notifyOnOpen)
{
    NS_ENSURE_ARG(webProgressListener);
    NS_ENSURE_ARG(printProgressParams);
    NS_ENSURE_ARG(notifyOnOpen);

    //*notifyOnOpen = PR_FALSE;

	nsresult rv;

    nsPrintProgressParams* prtProgressParams = new nsPrintProgressParams();
    rv = prtProgressParams->QueryInterface(NS_GET_IID(nsIPrintProgressParams), (void**)printProgressParams);
    NS_ENSURE_SUCCESS(rv, rv);

	if(!m_dlg) m_dlg = new CGeckoPrintingPromptDlg();
	m_dlg->Create(NULL);
	m_dlg->ShowWindow(SW_SHOW);

	if(*notifyOnOpen == PR_TRUE && openDialogObserver)
		openDialogObserver->Observe(nsnull, nsnull, nsnull);

    *webProgressListener = static_cast<nsIWebProgressListener*>(this);
    NS_ADDREF(*webProgressListener);

    return NS_OK;//TODO:rv;
}

/* void showPageSetup (in nsIDOMWindow parent, in nsIPrintSettings printSettings); */
NS_IMETHODIMP 
nsPrintingPromptService::ShowPageSetup(nsIDOMWindow *parent, nsIPrintSettings *printSettings, nsIObserver *aObs)
{
    NS_ENSURE_ARG(printSettings);
	nsresult rv;
	CPageSetupDlg dlg(printSettings);
	if(dlg.DoModal() == IDOK) return NS_OK;
	else return NS_ERROR_ABORT;
}

/* void showPrinterProperties (in nsIDOMWindow parent, in wstring printerName, in nsIPrintSettings printSettings); */
NS_IMETHODIMP 
nsPrintingPromptService::ShowPrinterProperties(nsIDOMWindow *parent, const PRUnichar *printerName, nsIPrintSettings *printSettings)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

//////////////////////////////////////////////////////////////////////
// nsIWebProgressListener
//////////////////////////////////////////////////////////////////////

/* void onStateChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in long aStateFlags, in nsresult aStatus); */
NS_IMETHODIMP 
nsPrintingPromptService::OnStateChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 aStateFlags, nsresult aStatus)
{
    if ((aStateFlags & STATE_STOP))// && mWebProgressListener) 
    {
        //mWebProgressListener->OnStateChange(aWebProgress, aRequest, aStateFlags, aStatus);
		if(m_dlg) 
			m_dlg->DestroyWindow();
        mWebProgressListener = nsnull;
    }
    return NS_OK;
}

/* void onProgressChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in long aCurSelfProgress, in long aMaxSelfProgress, in long aCurTotalProgress, in long aMaxTotalProgress); */
NS_IMETHODIMP 
nsPrintingPromptService::OnProgressChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRInt32 aCurSelfProgress, PRInt32 aMaxSelfProgress, PRInt32 aCurTotalProgress, PRInt32 aMaxTotalProgress)
{
  /*if (mWebProgressListener) 
  {
      return mWebProgressListener->OnProgressChange(aWebProgress, aRequest, aCurSelfProgress, aMaxSelfProgress, aCurTotalProgress, aMaxTotalProgress);
  }*/
  if(m_dlg){
	  m_dlg->m_progressbar.SetRange(0, aMaxTotalProgress);
	  m_dlg->m_progressbar.SetPos(aCurTotalProgress);
	  return NS_OK;
  }
  return NS_ERROR_FAILURE;
}

/* void onLocationChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsIURI location); */
NS_IMETHODIMP 
nsPrintingPromptService::OnLocationChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsIURI *location)
{
  if (mWebProgressListener) 
  {
      return mWebProgressListener->OnLocationChange(aWebProgress, aRequest, location);
  }
  return NS_OK;
  return NS_ERROR_FAILURE;
}

/* void onStatusChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsresult aStatus, in wstring aMessage); */
NS_IMETHODIMP 
nsPrintingPromptService::OnStatusChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsresult aStatus, const PRUnichar *aMessage)
{
  if (mWebProgressListener) 
  {
      return mWebProgressListener->OnStatusChange(aWebProgress, aRequest, aStatus, aMessage);
  }
  return NS_OK;
  return NS_ERROR_FAILURE;
}

/* void onSecurityChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long state); */
NS_IMETHODIMP 
nsPrintingPromptService::OnSecurityChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 state)
{
  if (mWebProgressListener) 
  {
      return mWebProgressListener->OnSecurityChange(aWebProgress, aRequest, state);
  }
  return NS_OK;
  return NS_ERROR_FAILURE;
}


