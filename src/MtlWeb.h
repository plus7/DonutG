/**
 *	@file	MtlWeb.h
 *	@brief	MTL : WEB関係
 */
// MTL Version 1.01
// Copyright (C) 2000 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// Last updated: July 05, 2000

#ifndef __MTLWEB_H__
#define __MTLWEB_H__

#pragma once

#ifndef _INC_SHLWAPI
	#error mtlweb.h requires shlwapi.h to be included first
#endif


#include "resource.h"
#include "DonutDefine.h"
#include "MtlBase.h"
#include "MtlMisc.h"
#include "MtlWeb.h"
#include "MtlFile.h"

#include "SaveUtil.h"
#include "mozilla/PageSetupDlg.h"

#include "nsIClipboardCommands.h"
#include "nsIDocShell.h"
#include "nsISelection.h"
#include "nsIContentViewer.h"
#include "nsIMarkupDocumentViewer.h"
#include "nsIWebBrowserPrint.h"
#include "nsIPrintSettings.h"
#include "nsIPrintSettingsService.h"
#include "nsIDOMWindowCollection.h"
#include "nsIDOMHTMLDocument.h"
#include "nsIDOMRange.h"
#include "nsIDOMNSHTMLElement.h"
#include "nsITextToSubURI.h"
namespace MTL {


bool	MtlGetFavoritesFolder(CString &rString);
bool	MtlGetHistoryFolder(CString &rString);
bool	MtlGetFavoriteLinksFolder(CString &rString);
bool	MtlCreateInternetShortcutFile(const CString &strFileName, const CString &strUrl);

// Note. If the third parameter is "CString strPath", it rarely crash. why?
void	MtlOrganizeFavorite( HWND hWnd, bool bOldShell = false, const CString &strPath_ = _T("") );
bool	__MtlAddFavoriteOldShell(const CString &strUrl, const CString &strName_, const CString &strFavDir);
bool	_MtlAddFavoriteNewShell(const CString &strUrl, const CString &strName);
bool	_MtlAddFavoriteModalSucks(const CString &strUrl, const CString &strName_, const CString &strFavPath, HWND hWnd);
bool	MtlAddFavorite(const CString& strUrl, const CString& strName, bool bOldShell = false, const CString&	strFavDir_ = _T(""), HWND hWnd = NULL);


#pragma comment( lib, "wininet" )


// cf.MSDN "Offline Browsing"
// Bug: Even if an user is working-Offline before your application runs,
//		this function always returns FALSE.
bool	MtlIsGlobalOffline();


// cf.MSDN "Offline Browsing"
void	MtlSetGlobalOffline(bool bGoOffline);


#if (_WIN32_IE >= 0x0500)
bool	MtlAutoComplete(HWND hWnd);
#endif



void	MtlShowInternetOptions();
bool	MtlCreateShortCutFile(
			const CString&	strFile,
			const CString&	strTarget,
			const CString&	strDescription	= _T(""),
			const CString&	strArgs 		= _T(""),
			const CString&	strWorkingDir	= _T(""),
			const CString&	strIconPath 	= _T(""),
			int 			iIcon			= 0,
			int 			iShowCmd		= SW_SHOWNORMAL);

///+++ MtlCtrl.hへ移動.
// bool 	MtlParseInternetShortcutFile(CString &strFilePath);
CString 	MtlGetInternetShortcutUrl(const CString &strFile);
void		_MtlExecuteWithoutDDE( CString &strPath, const CString &strArg = CString() );
bool		MtlPreOpenFile( CString &strPath, const CString &strArg = CString() );


// Undocumented command ids
static const CLSID		CGID_IWebBrowser = { 0xED016940L, 0xBD5B, 0x11cf, 0xBA, 0x4E, 0x00, 0xC0, 0x4F, 0xD7, 0x08, 0x16 };


template <class T>
class CWebBrowserCommandHandler {
	enum EHtmlId {
		HTMLID_FIND 		= 1,
		HTMLID_VIEWSOURCE	= 2,
		HTMLID_OPTIONS		= 3,
	};

public:
	// Message map and handlers
	BEGIN_MSG_MAP_EX(CWebBrowserCommandHandler)
		COMMAND_ID_HANDLER_EX( ID_FILE_SAVE_AS		, OnFileSaveAs		)
		COMMAND_ID_HANDLER_EX( ID_FILE_PAGE_SETUP	, OnFilePageSetup	)
		COMMAND_ID_HANDLER_EX( ID_FILE_PRINT		, OnFilePrint		)
		COMMAND_ID_HANDLER_EX( ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)			// suggested by DOGSTORE, thanks
		COMMAND_ID_HANDLER_EX( ID_FILE_PROPERTIES	, OnFileProperties	)
		COMMAND_ID_HANDLER_EX( ID_EDIT_CUT			, OnEditCut 		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_COPY 		, OnEditCopy		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_PASTE		, OnEditPaste		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_FIND 		, OnEditFind		)
		COMMAND_ID_HANDLER_EX( ID_EDIT_SELECT_ALL	, OnEditSelectAll	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_BACK 		, OnViewBack		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FORWARD		, OnViewForward 	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_REFRESH		, OnViewRefresh 	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_STOP 		, OnViewStop		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_HOME 		, OnViewHome		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_LARGEST , OnViewFontLargest )
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_LARGER	, OnViewFontLarger	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_MEDIUM	, OnViewFontMedium	)
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_SMALLER , OnViewFontSmaller )
		COMMAND_ID_HANDLER_EX( ID_VIEW_FONT_SMALLEST, OnViewFontSmallest)
		COMMAND_ID_HANDLER_EX( ID_VIEW_SOURCE		, OnViewSource		)
		COMMAND_ID_HANDLER_EX( ID_VIEW_OPTION		, OnViewOption		)

		COMMAND_ID_HANDLER_EX( ID_VIEW_SOURCE_SELECTED , OnViewSourceSelected ) 	// UDT DGSTR
		//COMMAND_ID_HANDLER_EX( ID_EDIT_FIND_MAX	, OnEditFindMax 	)			// UDT DGSTR //moved by minit ->CChildFrame
		COMMAND_ID_HANDLER_EX( ID_VIEW_UP			, OnViewUp			)			// UH
	END_MSG_MAP()


public:
	void OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		CGeckoBrowser b = pT->m_spBrowser;
		b.SaveDocument();
	}


	void OnFilePageSetup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebBrowserPrint> wbp = do_GetInterface(pT->m_spBrowser);
		nsCOMPtr<nsIPrintSettingsService> pss = do_GetService("@mozilla.org/gfx/printsettings-service;1");
		nsCOMPtr<nsIPrintSettings> printSettings;
		wbp->GetGlobalPrintSettings(getter_AddRefs(printSettings));
		pss->InitPrintSettingsFromPrefs(printSettings, PR_FALSE, nsIPrintSettings::kInitSaveAll);
		CPageSetupDlg dlg(printSettings);
		if(dlg.DoModal() == IDOK){
			pss->SavePrintSettingsToPrefs(printSettings, PR_FALSE, nsIPrintSettings::kInitSaveAll);
		}
	}


	void OnFilePrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebBrowserPrint> wbp = do_GetInterface(pT->m_spBrowser);
		nsCOMPtr<nsIPrintSettingsService> pss = do_GetService("@mozilla.org/gfx/printsettings-service;1");
		nsCOMPtr<nsIPrintSettings> printSettings;
		wbp->GetGlobalPrintSettings(getter_AddRefs(printSettings));
		pss->InitPrintSettingsFromPrefs(printSettings, PR_FALSE, nsIPrintSettings::kInitSaveAll);
		wbp->Print(printSettings, NULL);
	}


private:
	void OnFilePrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebBrowserPrint> wbp = do_GetInterface(pT->m_spBrowser);
		PRBool previewing; 
		wbp->GetDoingPrintPreview(&previewing);
		if(previewing){
			wbp->ExitPrintPreview();
		}else{
			nsCOMPtr<nsIPrintSettingsService> pss = do_GetService("@mozilla.org/gfx/printsettings-service;1");
			nsCOMPtr<nsIPrintSettings> printSettings;
			wbp->GetGlobalPrintSettings(getter_AddRefs(printSettings));
			pss->InitPrintSettingsFromPrefs(printSettings, PR_FALSE, nsIPrintSettings::kInitSaveAll);
			wbp->PrintPreview(printSettings, NULL, NULL); 
		}
	}


public:
	void OnFileProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
#if COMMENT
		pT->m_spBrowser->ExecWB(OLECMDID_PROPERTIES, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
#endif
	}


private:
	void OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIClipboardCommands> cc = do_GetInterface(pT->m_spBrowser);
		cc->CutSelection();
	}


	void OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIClipboardCommands> cc = do_GetInterface(pT->m_spBrowser);
		cc->CopySelection();
	}


	void OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIClipboardCommands> cc = do_GetInterface(pT->m_spBrowser);
		cc->Paste();
	}


public:
	void OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 	pT = static_cast<T *>(this);
#if COMMENT
		CComQIPtr<IOleCommandTarget> spCmdTarget = pT->m_spBrowser;
		spCmdTarget->Exec(&CGID_IWebBrowser, HTMLID_FIND, 0, NULL, NULL);
		// this is just file search
		//	m_spBrowser->ExecWB(OLECMDID_FIND, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
#endif
	}


private:
	void OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIClipboardCommands> cc = do_GetInterface(pT->m_spBrowser);
		cc->SelectAll();
	}


	void OnViewBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(pT->m_spBrowser);
		nav->GoBack();
	}


	void OnViewForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(pT->m_spBrowser);
		nav->GoForward();
	}


	void OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(pT->m_spBrowser);
		nav->Reload(0);
	}


	void OnViewStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(pT->m_spBrowser);
		nav->Stop(nsIWebNavigation::STOP_ALL);
	}


	void OnViewHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T *pT = static_cast<T *>(this);
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(pT->m_spBrowser);
		nav->LoadURI(NS_ConvertASCIItoUTF16("about:blank").get(), 0, NULL, NULL, NULL);
	}


	// UH
	void OnViewUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 	pT = static_cast<T *>(this);
		CString strURL;
		strURL = pT->GetLocationURL();

		if ( strURL.ReverseFind(_T('/')) == (strURL.GetLength() - 1) )
			strURL = strURL.Left( strURL.ReverseFind(_T('/')) );

		if (strURL.ReverseFind(_T('/')) != -1) {
			strURL = strURL.Left(strURL.ReverseFind(_T('/')) + 1);
		} else
			return;

		if (strURL[strURL.GetLength() - 2] == _T('/'))
			return;

		DonutOpenFile(pT->m_hWnd, strURL, D_OPENFILE_NOCREATE);
	}


public:
	void OnViewOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 	pT		 = static_cast<T *>(this);
		//TODO:インターネットオプションはいらない？
	}


private:
	// zoom font
	void OnViewFontLargest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(pT->m_spBrowser));
		nsCOMPtr<nsIContentViewer> cv;
		docShell->GetContentViewer(getter_AddRefs(cv));
		nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
		mv->SetFullZoom(2.0);
	}


	void OnViewFontLarger(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(pT->m_spBrowser));
		nsCOMPtr<nsIContentViewer> cv;
		docShell->GetContentViewer(getter_AddRefs(cv));
		nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
		mv->SetFullZoom(1.5);
	}


	void OnViewFontMedium(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(pT->m_spBrowser));
		nsCOMPtr<nsIContentViewer> cv;
		docShell->GetContentViewer(getter_AddRefs(cv));
		nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
		mv->SetFullZoom(1.0);
	}


	void OnViewFontSmaller(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(pT->m_spBrowser));
		nsCOMPtr<nsIContentViewer> cv;
		docShell->GetContentViewer(getter_AddRefs(cv));
		nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
		mv->SetFullZoom(0.75);
	}


	void OnViewFontSmallest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(pT->m_spBrowser));
		nsCOMPtr<nsIContentViewer> cv;
		docShell->GetContentViewer(getter_AddRefs(cv));
		nsCOMPtr<nsIMarkupDocumentViewer> mv = do_QueryInterface(cv);
		mv->SetFullZoom(0.5);
	}


	void OnViewSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		CGeckoBrowser b = pT->m_spBrowser;
		CString url = b.GetLocationURL();
		b.Navigate(CString("view-source:") + url, navOpenInNewWindow);
	}


	void OnViewSourceSelected(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		CGeckoBrowser b = pT->m_spBrowser;
		//TODO:FocusedWindowの取得に変える
		nsCOMPtr<nsIDOMWindow> win;
		pT->m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));
		nsCOMPtr<nsISelection> sel;
		nsresult rv = win->GetSelection(getter_AddRefs(sel));
		win->GetSelection(getter_AddRefs(sel));
		nsCOMPtr<nsIDOMRange> range;
		sel->GetRangeAt(0, getter_AddRefs(range));
		nsCOMPtr<nsIDOMNode> ancestor;
		range->GetCommonAncestorContainer(getter_AddRefs(ancestor));
		nsCOMPtr<nsIDOMNSHTMLElement> elementHTML = do_QueryInterface(ancestor);
		if(!elementHTML) {
			nsCOMPtr<nsIDOMNode> parent;
			ancestor->GetParentNode(getter_AddRefs(parent));
			elementHTML = do_QueryInterface(parent);
			if(!elementHTML) return;
		}
		nsEmbedString innerHTML;
		elementHTML->GetInnerHTML(innerHTML);
		nsCOMPtr<nsITextToSubURI> conv = do_GetService("@mozilla.org/intl/texttosuburi;1");
		char *val;
		conv->ConvertAndEscape("UTF-8", innerHTML.get(), &val);
		CString tmp = val;
		tmp.Replace(_T("+"), _T("%20"));
		b.Navigate(CString("view-source:data:text/html;charset=utf-8,") + tmp, navOpenInNewWindow);
		NS_Free(val);
	}

public:
	void _OnEditFindMax(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
	{
		T * 			pT = static_cast<T *>(this);
		CGeckoBrowser b = pT->m_spBrowser;
		//TODO:FocusedWindowの取得に変える or parseFramesCollectionのGecko実装
		//MtlExecFirstFrameみたいな関数でも作るか？
		nsCOMPtr<nsIDOMWindow> win;
		pT->m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));
		nsCOMPtr<nsISelection> sel;
		nsresult rv = win->GetSelection(getter_AddRefs(sel));
		PRUnichar *c;
		sel->ToString(&c);
		CString val = c;
		NS_Free(c);
		if(val != "") 
			pT->searchEngines(val);
	}


private:
	virtual void searchEngines(const CString &strKeyWord )
	{
		T * 		pT		  = static_cast<T *>(this);
		CString 	strURL;
		int 		nMaxCount = 15;
		TCHAR		szGoogleCode[INTERNET_MAX_PATH_LENGTH];

		memset(szGoogleCode, 0, INTERNET_MAX_PATH_LENGTH);
		DWORD		dwCount   = INTERNET_MAX_PATH_LENGTH;
		DWORD		dwFlags	  = 0;

		{
			CIniFileI pr0( g_szIniFileName, STR_ADDRESS_BAR );
			pr0.QueryString(szGoogleCode, _T("EnterCtrl"), &dwCount);
			pr0.QueryValue( dwFlags, _T("") );
		}

		CIniFileI	pr( g_szIniFileName, _T("INUYA") );
		TCHAR		szRetString[INTERNET_MAX_PATH_LENGTH + 1];
		TCHAR		szBuff[32];

		for (int nItem = 0; nItem < nMaxCount; nItem++) {
			wsprintf(szBuff, _T("Engine%i"), nItem);
			szRetString[0]= 0;
			DWORD dwCount = INTERNET_MAX_PATH_LENGTH;	//+++ * sizeof (TCHAR);

			if (pr.QueryString(szRetString, szBuff, &dwCount) == ERROR_SUCCESS) {
				strURL = szRetString + strKeyWord;
				DonutOpenFile( pT->m_hWnd, strURL, DonutGetStdOpenCreateFlag() );
			} else {
				if (nItem == 0) {
					strURL = szGoogleCode + strKeyWord;
					DonutOpenFile( pT->m_hWnd, strURL, DonutGetStdOpenCreateFlag() );
				}

				break;
			}
		}
	}
};



template <class T>
class CMDIFrameTitleUpsideDownMessageHandlerWeb {
	bool		m_bValid;
	CString 	m_strWorkOffline;
	CString 	m_strStatusBar; 		// UDT DGSTR

public:
	CMDIFrameTitleUpsideDownMessageHandlerWeb()
		: m_bValid(true)
		, m_strWorkOffline( _T(" - [オフライン作業]") )
		, m_strStatusBar( _T("") )						// UDT DGSTR
	{
	}


	void UpdateTitleBarUpsideDown()
	{
		if (m_bValid) {
			CString strApp;
			strApp.LoadString(IDR_MAINFRAME);
			T * 	pT = static_cast<T *>(this);
			pT->SetWindowText(strApp);
		}
	}


	// UDT DGSTR
	void UpdateTitleBarUpsideDown(const CString &strStatusBar)
	{
		m_strStatusBar = strStatusBar;

		if (m_bValid) {
			CString strApp;
			strApp.LoadString(IDR_MAINFRAME);
			T * 	pT = static_cast<T *>(this);
			pT->SetWindowText(strApp + " " + m_strStatusBar);
		}
	}
	// ENDE


public:
	//+++ メモ:GetText,SetTextを乗っ取って、タイトルバーの文字列の表示順を"ページ名 - APP名" にする処理
	//         ...のようだが、うまく機能していない模様...
	BEGIN_MSG_MAP(CMDIFrameTitleUpsideDownMessageHandlerWeb)
		MESSAGE_HANDLER( WM_GETTEXTLENGTH, OnGetTextLength	)
		MESSAGE_HANDLER( WM_GETTEXT 	 , OnGetText		)
		MESSAGE_HANDLER( WM_SETTEXT 	 , OnSetText		)	// never called by MDI system
	END_MSG_MAP()


private:
	LRESULT OnGetTextLength(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		T *pT = static_cast<T *>(this);

		return pT->DefWindowProc(uMsg, wParam, lParam) + m_strWorkOffline.GetLength() + 1;
	}


	LRESULT OnGetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UINT	cchTextMax = (UINT)   wParam;	// number of characters to copy
		LPTSTR	lpszText   = (LPTSTR) lParam;

		if (!m_bValid) {
			bHandled = FALSE;
			return 0;
		}

		T * 	pT		   = static_cast<T *>(this);

		CString strApp;
		strApp.LoadString(IDR_MAINFRAME);

		CString strTitle;
		BOOL	bMaximized = FALSE;
		HWND	hWndActive = pT->MDIGetActive(&bMaximized);

		if (hWndActive == NULL || bMaximized == FALSE)
			strTitle = strApp;
		// UDT DGSTR ( Set Information on TitleBar.
		else if ( !IsWindowVisible(pT->m_hWndStatusBar) ) {
			if ( !m_strStatusBar.IsEmpty() )
				strTitle = MtlGetWindowText(hWndActive) + _T(" - ") + m_strStatusBar;
			else
				strTitle = MtlGetWindowText(hWndActive) /*+ _T(" -") + strApp*/;
		}
		// ENDE
		else
			strTitle = MtlGetWindowText(hWndActive) + _T(" - ") + strApp;
			//strTitle = _T("[") + MtlGetWindowText(hWndActive) + _T("] - ") + strApp;	//ためし...

		if ( MtlIsGlobalOffline() )
			strTitle += m_strWorkOffline;

		::lstrcpyn(lpszText, strTitle, cchTextMax);

		bHandled = TRUE;
		return std::min(strTitle.GetLength(), (int) cchTextMax);
	}


	LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CString strText 	= (LPCTSTR) lParam;

		// first, find the mdi child's title
		int 	nFirstIndex = strText.Find( _T('[') );
		int 	nLastIndex	= strText.ReverseFind( _T(']') );

		if (nFirstIndex > nLastIndex || nFirstIndex == -1 || nFirstIndex == -1) {
			bHandled = FALSE;
			return FALSE;
		}

		CString strChild	= strText.Mid(nFirstIndex, nLastIndex - nFirstIndex + 1);

		// second, find the application's title
		CString strApp;
		strApp.LoadString(IDR_MAINFRAME);
		strText = strChild + _T(" - ") + strApp;

		T * 	pT			= static_cast<T *>(this);
		return pT->DefWindowProc(uMsg, wParam, (LPARAM) (LPCTSTR) strText);
	}
};



template <class TKey>
class CSimpleMapInt : public CSimpleMap<TKey, int> {
public:
	int Lookup(TKey key) const
	{
		int nIndex = FindKey(key);

		if (nIndex == -1)
			return -1;

		return GetValueAt(nIndex);
	}
};



// I thought this is the best, but it seems that
// idls which means the same path don't surely have the same bits.
//typedef CSimpleMapInt< CAdapt<CItemIDList> > CFavoritesOrder;

typedef CSimpleMapInt< CString >  CFavoritesOrder;



// Thanks to fub and namazu
typedef struct _tagCFavoritesOrderData {
	DWORD	   size;
	DWORD	   priority;
	ITEMIDLIST idl; // relative idlist
} _CFavoritesOrderData;



// Note. I've found the IE rarely fails to load his own favorites order, but don't care.
// Note. If the item not ordered, priority equals -5.
#define FAVORITESORDER_NOTFOUND 	-5

CString MtlMakeFavoriteToolTipText(const CString &strName_, const CString &strUrl_, int cchTextMax);
BOOL	MtlGetFavoritesOrder(CFavoritesOrder &order, const CString &strDirPath);




// Based on JOBBY's code
DWORD	MtlGetInternetZoneActionPolicy(DWORD dwAction);
bool	MtlIsInternetZoneActionPolicyDisallow(DWORD dwAction);
bool	MtlSetInternetZoneActionPolicy(DWORD dwAction, DWORD dwPolicy);

//plus7
int		GetGlobalCookiePref();
void	SetGlobalCookiePref(int val);
int		GetSiteCookiePref(const CString& uri);
void	SetSiteCookiePref(const CString& uri, int val);

// not tested yet...
bool	MtlDeleteAllCookies();


template <class _Function>
_Function _MtlForEachDOMWindow(nsIDOMWindow *pWin, _Function __f)
{
	__f(pWin);

	nsCOMPtr<nsIDOMWindowCollection> frames;
	nsresult rv = pWin->GetFrames(getter_AddRefs(frames));

	if(!NS_FAILED(rv)) return __f;

	PRUint32 count = 0;

	rv = frames->GetLength(&count);

	if(!NS_FAILED(rv)) return __f;

	for (unsigned int i = 0; i < count; ++i) {
		nsCOMPtr<nsIDOMWindow> w;
		rv = frames->Item(i, getter_AddRefs(w));
		if(NS_FAILED(rv)) continue;

		_MtlForEachDOMWindow(w, __f);
	}

	return __f;
}



template <class _Function>
_Function MtlForEachDOMWindow(nsIWebBrowser *pBrowser, _Function __f)
{
	nsCOMPtr<nsIDOMWindow> w;
	nsresult rv = pBrowser->GetContentDOMWindow(getter_AddRefs(w));
	if(NS_FAILED(rv)) return __f;
	return _MtlForEachDOMWindow(w, __f);
}

bool		__MtlSkipChar(const CString &str, int &nIndex, TCHAR ch);
void		_MtlCreateHrefUrlArray(CSimpleArray<CString> &arrUrl, const CString &strHtmlText);
CString 	_MtlGetDirectoryPathFixed(const CString &strPath, bool bAddBackSlash = false);
CString 	_MtlGetRootDirectoryPathFixed(const CString &strPath, bool bAddBackSlash = false);
bool		_MtlIsIllegalRootSlash(const CString &str);
bool		_MtlIsMailTo(const CString &str);
bool		_MtlIsHrefID(const CString &str);
CString 	_MtlRemoveIDFromUrl(const CString &str);
bool		_MtlIsRelativePath(const CString &str);
void		MtlCreateHrefUrlArray(CSimpleArray<CString> &arrUrl, const CString &strHtmlText, const CString &strLocationUrl);

}	//namespace MTL



#endif	// __MTLWEB_H__
