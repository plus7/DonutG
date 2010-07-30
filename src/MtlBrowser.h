/**
 *	@file	MtlBrowser.h
 *	@brief	MTL : ブラウザ関係
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// MtlBrowser.h: Last updated: March 17, 2001
/////////////////////////////////////////////////////////////////////////////

#ifndef __MTLBROWSER_H__
#define __MTLBROWSER_H__

#pragma once
#include <tlogstg.h>
#include "MtlCom.h"
#include "MtlFile.h"
#include "resource.h"	//+++
#include "SaveUtil.h"

#include "nsIBaseWindow.h"
#include "nsISHistory.h"
#include "nsIHistoryEntry.h"
#include "nsIWindowWatcher.h"
#include "nsIDOMHTMLImageElement.h"
#include "nsIDOMHTMLMediaElement.h"
#include "nsIWebPageDescriptor.h"
#include "nsIDOMWindowUtils.h"
#include "imgICache.h"
#include "nsIMIMEHeaderParam.h"
#include "nsIDOMLocation.h"
#include "nsIWebBrowserFocus.h"
#include "nsISupportsPrimitives.h"
#include "nsIDOMNSDocument.h"
#include "nsIWebBrowserPersist.h"
#include "nsITextToSubURI.h"
namespace MTL {


/////////////////////////////////////////////////////////////////////////////

// nsIWebBrowser wrapper
class CGeckoBrowser {
public:
	nsCOMPtr<nsIWebBrowser>	m_spBrowser;

public:
	// Ctor
	__inline CGeckoBrowser(nsIWebBrowser * pBrowser = NULL) : m_spBrowser(pBrowser)
	{
	}


	__inline bool IsBrowserNull()
	{
		if(m_spBrowser) return false;
		else return true;
	}


	// properties

public:

	void SetVisible(bool bNewValue)
	{
		nsCOMPtr<nsIBaseWindow> baseWin = do_QueryInterface(m_spBrowser);
		baseWin->SetVisibility(bNewValue ? PR_TRUE : PR_FALSE);
	}

	void SetOffline(bool bNewValue)
	{
		nsCOMPtr<nsIIOService> ios = do_GetService(
			"@mozilla.org/network/io-service;1");
		if(!ios) return;
		ios->SetOffline(bNewValue ? PR_TRUE : PR_FALSE);
	}

public:
	void GoBack()
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nav->GoBack();
	}

	void GoBack(int step)
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nsCOMPtr<nsISHistory> sh;
		nav->GetSessionHistory(getter_AddRefs(sh));
		PRInt32 index;
		sh->GetIndex(&index);
		nav->GotoIndex(index - step);
	}

	void GoForward()
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nav->GoForward();
	}

	void GoForward(int step)
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nsCOMPtr<nsISHistory> sh;
		nav->GetSessionHistory(getter_AddRefs(sh));
		PRInt32 index;
		sh->GetIndex(&index);
		nav->GotoIndex(index + step);
	}

	void GoHome()
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nav->LoadURI(NS_ConvertASCIItoUTF16("http://donut.tnose.net/").get(), 0, NULL, NULL, NULL);
	}


private:
	void GoSearch()
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nav->LoadURI(NS_ConvertASCIItoUTF16("http://www.google.com/").get(), 0, NULL, NULL, NULL);
	}


public:
	void Refresh()
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nav->Reload(0);
	}


	void Refresh2(int nLevel)
	{
		//TODO: nLevelの反映
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nav->Reload(0);
	}


	void Stop()
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nav->Stop(nsIWebNavigation::STOP_ALL);
	}


private:

	bool GetVisible() const
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");

		PRBool rv;
		nsCOMPtr<nsIBaseWindow> baseWin = do_QueryInterface(m_spBrowser);
		baseWin->GetVisibility(&rv);
		return (rv == PR_TRUE);
	}

public:
	const CString GetLocationName() const
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nsCOMPtr<nsIBaseWindow> baseWin = do_QueryInterface(nav);
		nsEmbedString szLocationName;
		baseWin->GetTitle(getter_Copies(szLocationName));
		if (!szLocationName.get()){
			return CString();
		}else{
			return CString(szLocationName.get());
		}
	}


	const CString GetLocationURL() const
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		if(!nav) return CString();
		nsCOMPtr<nsIURI> uri;
		nav->GetCurrentURI(getter_AddRefs(uri));
		if(!uri){
			return CString();
		}else{
			nsEmbedCString locationURL;
			uri->GetAsciiSpec(locationURL);
			if (!locationURL.get()){
				return CString();
			}else{
				return CString(locationURL.get());
			}
		}
	}


private:
	bool GetBusy() const
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");

		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRUint32 state;
		docShell->GetBusyFlags(&state);
		if(state != nsIDocShell::BUSY_FLAGS_NONE) return true;
		return false;
	}


	READYSTATE GetReadyState() const
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");

		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(m_spBrowser));
		PRUint32 state;
		docShell->GetBusyFlags(&state);
		switch(state){
			case nsIDocShell::BUSY_FLAGS_BEFORE_PAGE_LOAD:
				return READYSTATE_UNINITIALIZED;
				break;
			case nsIDocShell::BUSY_FLAGS_PAGE_LOADING:
				return READYSTATE_LOADING;
				break;
			case nsIDocShell::BUSY_FLAGS_NONE:
				return READYSTATE_COMPLETE;
				break;
			default:
				return READYSTATE_UNINITIALIZED;
				break;
		}
	}


	bool GetOffline() const
	{
		nsCOMPtr<nsIIOService> ios = do_GetService(
			"@mozilla.org/network/io-service;1");
		NS_ENSURE_TRUE(ios, false);
		PRBool offline = PR_FALSE;
		ios->GetOffline(&offline);
		return offline ? false : true;
	}

	nsIDOMDocument *GetDOMDocument() const
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");

		nsCOMPtr<nsIDOMWindow> win;
		m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));
		nsCOMPtr<nsIDOMDocument> doc;
		win->GetDocument(getter_AddRefs(doc));
		return doc;
	}

private:

  #if 0 //+++ 未使用のよう?
	// operations
	bool LoadFromResource(LPCTSTR lpszResource)
	{
	  #if 1 //+++
		CString str = GetExeFileName();
		if (str.IsEmpty())
			return false;
		CString   strResourceURL;
		strResourceURL.Format(_T("res://%s/%s"), LPCTSTR(str), lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
		return true;
	  #else
		HINSTANCE hInstance  = _Module.GetModuleInstance();

		ATLASSERT(hInstance != NULL);

		CString   strResourceURL;
		bool	  bRetVal	 = true;
		LPTSTR	  lpszModule = new TCHAR[_MAX_PATH];

		if ( ::GetModuleFileName(hInstance, lpszModule, _MAX_PATH) ) {
			strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
			Navigate(strResourceURL, 0, 0, 0);
		} else
			bRetVal = false;

		delete[] lpszModule;
		return bRetVal;
	  #endif
	}


	bool LoadFromResource(UINT nRes)
	{
	  #if 1 //+++
		CString str = GetExeFileName();
		if (str.IsEmpty())
			return false;
		CString   strResourceURL;
		strResourceURL.Format(_T("res://%s/%d"), LPCTSTR(str), nRes);
		Navigate(strResourceURL, 0, 0, 0);
		return true;
	  #else
		HINSTANCE hInstance  = _Module.GetModuleInstance();

		ATLASSERT(hInstance != NULL);

		CString   strResourceURL;
		bool	  bRetVal	 = true;
		LPTSTR	  lpszModule = new TCHAR[_MAX_PATH];

		if ( ::GetModuleFileName(hInstance, lpszModule, _MAX_PATH) ) {
			strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
			Navigate(strResourceURL, 0, 0, 0);
		} else
			bRetVal = false;

		delete[] lpszModule;
		return bRetVal;
	  #endif
	}
  #endif

public:
	void Navigate(LPCTSTR	lpszURL,
				  DWORD 	dwFlags 			= 0 ,
				  LPCTSTR	lpszTargetFrameName = NULL,
				  LPCTSTR	lpszHeaders 		= NULL,
				  LPVOID	lpvPostData 		= NULL,
				  DWORD 	dwPostDataLen		= 0)
	{
		//TODO: Referer, nsIStringInputStreamの適切な扱い
		bool openInNewWin = false;
		PRUint32 loadFlags = nsIWebNavigation::LOAD_FLAGS_NONE;
		nsCOMPtr<nsIWebNavigation> nav;
		//フラグの処理
		if(dwFlags & navOpenInNewWindow){
			openInNewWin = true;
		}
		if(dwFlags & navNoHistory){
			loadFlags |= nsIWebNavigation::LOAD_FLAGS_BYPASS_HISTORY;
		}
		if(dwFlags & navNoReadFromCache){
			loadFlags |= nsIWebNavigation::LOAD_FLAGS_BYPASS_CACHE;
		}
		if(dwFlags & navNoWriteToCache){
			loadFlags |= nsIWebNavigation::LOAD_FLAGS_BYPASS_CACHE;
		}

		//ターゲットフレームの処理
		nsCOMPtr<nsIWindowWatcher> ww = do_GetService(NS_WINDOWWATCHER_CONTRACTID);
		nsCOMPtr<nsIDOMWindow> win;
		m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));

		if(lpszTargetFrameName){
			nsCOMPtr<nsIDOMWindow> target;
			ww->GetWindowByName(lpszTargetFrameName, win, getter_AddRefs(target));
			nav = do_GetInterface(target);
			if(!nav) openInNewWin = true;
		}else{
			nav = do_QueryInterface(m_spBrowser);
		}
		//NS_ConvertUTF16toUTF8は適切かわからない
		//Mozilla ActiveXコントロールではWideCharToMultiByte(CP_ACP～を使っている？
		//ヘッダ
		nsCOMPtr<nsIStringInputStream> headersStream;
		if(lpszHeaders) {
			headersStream = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
			headersStream->SetData(NS_ConvertUTF16toUTF8(lpszHeaders).get(), -1);// -1についてはnsIStringInputStreamのIDL参照。手抜き。
		}

		//Postするデータ
		nsCOMPtr<nsIStringInputStream> postDataStream;
		//LPVOIDってなによ？無理やり変換してるけど
		if(lpvPostData){
			postDataStream = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
			postDataStream->SetData((char*)lpvPostData, dwPostDataLen);
		}

		if(openInNewWin){
			nsCOMPtr<nsIDOMWindow> newwin;
			ww->OpenWindow(win, NS_ConvertUTF16toUTF8(lpszURL).get(), 
				           NS_ConvertUTF16toUTF8(lpszTargetFrameName).get(), NULL, NULL, getter_AddRefs(newwin));
		}else{
			nav->LoadURI(lpszURL, loadFlags, NULL, postDataStream, headersStream);
		}
	}


#if 0	//+++ 未使用ぽい
	void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL)
	{
		NS_ASSERTION(m_spBrowser.get(), "nullpo");

		CComVariant 	vPIDL;
		MtlInitVariantFromItemIDList(vPIDL, pIDL);
		CComVariant 	empty;
		CComVariant 	vTargetFrameName(lpszTargetFrameName);
		CComVariant 	vFlags( (long) dwFlags );

		m_spBrowser->Navigate2(&vPIDL, &vFlags, &vTargetFrameName, &empty, &empty);
	}
#endif


public:
	void Navigate2(
			LPCTSTR 	lpszURL,
			DWORD		dwFlags 			= 0,
			LPCTSTR 	lpszTargetFrameName = NULL,
			LPCTSTR 	lpszHeaders 		= NULL,
			LPVOID		lpvPostData 		= NULL,
			DWORD		dwPostDataLen		= 0)
	{
		Navigate(lpszURL, dwFlags, lpszTargetFrameName, lpszHeaders, lpvPostData, dwPostDataLen);
	}

public:
	void MenuChgGoBack(HMENU hMenu)
	{

		CMenuHandle 		 menu	  = hMenu; // handle

		nsresult 			 rv 	  = NS_OK;
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nsCOMPtr<nsISHistory> sh;
		nav->GetSessionHistory(getter_AddRefs(sh));
		PRInt32 count,index;
		sh->GetCount(&count);
		sh->GetIndex(&index);
		nsCOMPtr<nsIHistoryEntry> e;
		int i;
		for(i=0; i<9 && index-i>0; i++){
			sh->GetEntryAtIndex(index-(i+1), PR_FALSE, getter_AddRefs(e));
			nsEmbedString title;
			e->GetTitle(getter_Copies(title));
			menu.ModifyMenu(ID_VIEW_BACK1 + i, MF_BYCOMMAND, ID_VIEW_BACK1 + i, title.get());
			menu.EnableMenuItem(ID_VIEW_BACK1 + i, MF_BYCOMMAND | MF_ENABLED);
		}
		if (i < 9) {
			for (int ii = i; ii < 9; ii++)
				menu.RemoveMenu(ID_VIEW_BACK1 + ii, MF_BYCOMMAND);
		}
		menu.Detach();
	}


	void MenuChgGoForward(HMENU hMenu)
	{
		CMenuHandle 		 menu	  = hMenu; // handle

		nsresult 			 rv 	  = NS_OK;
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nsCOMPtr<nsISHistory> sh;
		nav->GetSessionHistory(getter_AddRefs(sh));
		PRInt32 count,index;
		sh->GetCount(&count);
		sh->GetIndex(&index);
		nsCOMPtr<nsIHistoryEntry> e;
		int i;
		for(i=0; i<9 && index+i+1<count; i++){
			sh->GetEntryAtIndex(index+i+1, PR_FALSE, getter_AddRefs(e));
			nsEmbedString title;
			e->GetTitle(getter_Copies(title));
			menu.ModifyMenu(ID_VIEW_FORWARD1 + i, MF_BYCOMMAND, ID_VIEW_FORWARD1 + i, title.get());
			menu.EnableMenuItem(ID_VIEW_FORWARD1 + i, MF_BYCOMMAND | MF_ENABLED);
		}
		if (i < 9) {
			for (int ii = i; ii < 9; ii++)
				menu.RemoveMenu(ID_VIEW_FORWARD1 + ii, MF_BYCOMMAND);
		}
		menu.Detach();
	}

	//ここらへん，contentAreaUtils.js見て実装してるけどライセンス的にはどういう扱いになるのか良くわからない
	void SaveDocument(bool prompt = true)
	{
		nsCOMPtr<nsIDOMWindow> win;
		m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));
		if(!win) return;
		nsCOMPtr<nsIDOMDocument> doc;
		win->GetDocument(getter_AddRefs(doc));
		if(!doc) return;
		SaveDocumentInternal(win, doc, prompt);
	}

	void SaveFrameDocument(nsIDOMWindow *aWin = NULL, bool prompt = true)
	{
		nsCOMPtr<nsIDOMWindow> win;
		if(!aWin){
			nsCOMPtr<nsIWebBrowserFocus> focus = do_QueryInterface(m_spBrowser);
			focus->GetFocusedWindow(getter_AddRefs(win));
		}else{
			win = aWin;
		}
		if(!win) return;
		nsCOMPtr<nsIDOMDocument> doc;
		win->GetDocument(getter_AddRefs(doc));
		if(!doc) return;
		SaveDocumentInternal(win, doc, prompt);
	}

	void SaveMedia(nsIDOMNode *aNode, bool prompt = true)
	{
		nsEmbedString src;
		nsCOMPtr<nsIDOMHTMLImageElement> image = do_QueryInterface(aNode);
		if(image){
			image->GetSrc(src);
		}else{
			nsCOMPtr<nsIDOMHTMLMediaElement> media = do_QueryInterface(aNode);
			media->GetSrc(src);
		}

		nsCOMPtr<nsIURI> uri;
		NS_NewURI(getter_AddRefs(uri), src);		

		nsCOMPtr<imgICache> imageCache = do_GetService("@mozilla.org/image/cache;1");
		nsCOMPtr<nsIProperties> props;
		imageCache->FindEntryProperties(uri, getter_AddRefs(props));
		nsCOMPtr<nsISupportsCString> contentType, contentDisposition;
		nsEmbedCString strContentType, strContentDisposition;
		if (props) {
			props->Get("type", NS_GET_IID(nsISupportsCString), getter_AddRefs(contentType));
			props->Get("content-disposition", NS_GET_IID(nsISupportsCString), getter_AddRefs(contentDisposition));
			if(contentType) contentType->GetData(strContentType);
			if(contentDisposition) contentDisposition->GetData(strContentDisposition);
		}

		CString filePath;
		if(prompt){
			CFileDialog dlg(FALSE, _T(""), 
				            GetDefaultFileName(_T(""), strContentDisposition.get(), uri, NULL), 
							OFN_OVERWRITEPROMPT,
							_T("すべてのファイル (*.*)\0*.*\0\0"));

			if(dlg.DoModal() == IDOK){
				filePath = dlg.m_szFileName;
			}
		}else{
			filePath = GetDesktopPath() + "\\" + GetDefaultFileName(_T(""), strContentDisposition.get(), uri, NULL);
		}
		if(filePath.IsEmpty()) return;
		nsCOMPtr<nsILocalFile> localFile;
		NS_NewLocalFile(nsEmbedString(filePath), PR_FALSE, getter_AddRefs(localFile));
		InternalSave(uri,localFile);
	}

	void SaveURI(const CString& strURI, const CString& contentDisposition = "", bool prompt = true)
	{	
		nsCOMPtr<nsIURI> uri;
		NS_NewURI(getter_AddRefs(uri), nsEmbedString(strURI));		
		SaveURI(uri, contentDisposition, prompt);
	}

	void SaveURI(nsIURI *uri, const CString& contentDisposition = "", bool prompt = true)
	{	
		CString filePath;
		if(prompt){
			CFileDialog dlg(FALSE, _T(""), 
				            GetDefaultFileName(_T(""), contentDisposition, uri, NULL), 
							OFN_OVERWRITEPROMPT,
							_T("すべてのファイル (*.*)\0*.*\0\0"));

			if(dlg.DoModal() == IDOK){
				filePath = dlg.m_szFileName;
			}
		}else{
			filePath = GetDesktopPath() + "\\" + GetDefaultFileName(_T(""), contentDisposition, uri, NULL);
		}
		if(filePath.IsEmpty()) return;
		nsCOMPtr<nsILocalFile> localFile;
		NS_NewLocalFile(nsEmbedString(filePath), PR_FALSE, getter_AddRefs(localFile));
		InternalSave(uri,localFile);
	}

private:
	CString GetDefaultFileName(const CString& defaultFileName,
	                           const CString& contentDisposition, 
		                       nsIURI *uri,
				               nsIDOMDocument *doc)
	{
		nsEmbedCString cvalue;
		nsEmbedString value;
		nsCOMPtr<nsIDOMNSDocument> nsdoc = do_QueryInterface(doc);
		//try getting Content-Disposition Header
		if(!contentDisposition.IsEmpty()){
			nsCOMPtr<nsIMIMEHeaderParam> mhp = do_GetService("@mozilla.org/network/mime-hdrparam;1");
			nsEmbedString charset;
			nsdoc->GetCharacterSet(charset);

			char *dummy;
			nsresult rv = mhp->GetParameter(NS_LossyConvertUTF16toASCII(contentDisposition),
				                           "filename", NS_LossyConvertUTF16toASCII(charset), 
										   PR_TRUE, &dummy, value);
			NS_Free(dummy);
			if(NS_FAILED(rv)){
				rv = mhp->GetParameter(NS_LossyConvertUTF16toASCII(contentDisposition), 
                                       "name", NS_LossyConvertUTF16toASCII(charset), 
									   PR_TRUE, &dummy, value);
				NS_Free(dummy);
			}
			if(value.Length() > 0)
				return value.get();
		}
		//actual filename
		nsCOMPtr<nsIURL> url = do_QueryInterface(uri);
		CString strFileName;
		nsEmbedCString fileName, hoge;
		url->GetFileName(fileName);
		url->GetSpec(hoge);
		ATLTRACE2(atlTraceUI, 0, "CGeckoBrowser::GetDefaultName: url.spec == %s\n", hoge.get());
		if(fileName.Length() > 0){
			nsCOMPtr<nsITextToSubURI> textToSubURI = do_GetService("@mozilla.org/intl/texttosuburi;1");
			url->GetOriginCharset(cvalue);
			if(cvalue.Length() == 0) cvalue.Assign("UTF-8");
			textToSubURI->UnEscapeURIForUI(cvalue, fileName ,value);
			strFileName = value.get();
			CString buf(strFileName);
			MtlValidateFileName(buf);
			return buf;
		}
		//Document title
		if(nsdoc){
			nsdoc->GetTitle(value);
			strFileName = value.get();
			CString buf(strFileName);
			MtlValidateFileName(buf);
			return buf;
		}
		//Provided
		if(!defaultFileName.IsEmpty()) 
	{
			CString buf(defaultFileName);
			MtlValidateFileName(buf);
			return buf;
		}

		//5, 6, 7は省略

		//どれにも一致しない
		return _T("index");
	}

	int GetSaveModeForContentType(const CString& aContentType,
		                          nsIDOMDocument* aDocument)
	{
		if(!aDocument) return SAVEMODE_FILEONLY;
		int saveMode = SAVEMODE_FILEONLY;
		if( aContentType == "text/html" ||
			aContentType == "application/xhtml+xml" ||
			aContentType == "image/svg+xml"){
				saveMode |= SAVEMODE_COMPLETE_TEXT;
		}else if(aContentType == "text/xml" || aContentType == "application/xml"){
			saveMode |= SAVEMODE_COMPLETE_DOM;
		}
		return saveMode;
	}

	CString GetDefaultExtension(const char* aFileName, nsIURI *aURI, const CString& aContentType){
		nsCOMPtr<nsIURL> url = do_CreateInstance("@mozilla.org/network/standard-url;1");
		url->SetFilePath(nsEmbedCString(aFileName));
	}

	void SaveDocumentInternal(nsIDOMWindow *aWin, nsIDOMDocument *aDoc, bool prompt)
	{
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(m_spBrowser);
		nsCOMPtr<nsIWebPageDescriptor> cacheKey = do_QueryInterface(nav);
		nsCOMPtr<nsIDOMWindowUtils> utils(do_GetInterface(aWin));
		nsEmbedString contentDisposition;
		utils->GetDocumentMetadata(NS_LITERAL_STRING("content-disposition"), contentDisposition);

		nsCOMPtr<nsIDOMNSDocument> nsdoc = do_QueryInterface(aDoc);

		nsEmbedString contentType;
		nsdoc->GetContentType(contentType);

		nsCOMPtr<nsIDOMLocation> loc;
		nsdoc->GetLocation(getter_AddRefs(loc));
		nsEmbedString href;
		loc->GetHref(href);

		nsCOMPtr<nsIURI> uri;
		NS_NewURI(getter_AddRefs(uri), href);	

		if(!GetSaveModeForContentType(CString(contentType.get()), aDoc)){
			SaveURI(uri, CString(contentDisposition.get()), prompt);
			return;
		}

		CString filePath;
		if(prompt){
			CFileDialog dlg(FALSE, _T(""), 
				            GetDefaultFileName(_T(""), CString(contentDisposition.get()), uri, aDoc), 
							OFN_OVERWRITEPROMPT,
							_T("すべてのファイル (*.*)\0*.*\0\0"));

			if(dlg.DoModal() == IDOK){
				filePath = dlg.m_szFileName;
			}
		}else{
			filePath = GetDesktopPath() + "\\" + GetDefaultFileName(_T(""), CString(contentDisposition.get()), uri, aDoc);
		}

		if(filePath.IsEmpty()) return;
		nsCOMPtr<nsILocalFile> localFile;
		NS_NewLocalFile(nsEmbedString(filePath), PR_FALSE, getter_AddRefs(localFile));

		InternalSave(aWin,
                     aDoc,
		             localFile,
                     0,//TODO
					 cacheKey);
	}

	void InternalSave(nsIURI *uri,
		              nsILocalFile *file,
					  nsISupports *cacheKey = NULL,
					  nsIURI *referer = NULL)
	{
		nsCOMPtr<nsIWebBrowserPersist> wbp = do_CreateInstance("@mozilla.org/embedding/browser/nsWebBrowserPersist;1");
		if(!wbp) return;
		PRUint32 flags = 
			nsIWebBrowserPersist::PERSIST_FLAGS_REPLACE_EXISTING_FILES | 
			nsIWebBrowserPersist::PERSIST_FLAGS_FORCE_ALLOW_COOKIES |
			nsIWebBrowserPersist::PERSIST_FLAGS_AUTODETECT_APPLY_CONVERSION |
			nsIWebBrowserPersist::PERSIST_FLAGS_FROM_CACHE;
		wbp->SetPersistFlags(flags);
		wbp->SaveURI(uri, cacheKey, referer, NULL, NULL, file);
	}

	void InternalSave(nsIDOMWindow *win,
                      nsIDOMDocument *doc,
		              nsILocalFile *file,
                      PRUint32 saveMode,
					  nsISupports *cacheKey = NULL,
					  nsIURI *referer = NULL)
	{
		nsCOMPtr<nsIWebBrowserPersist> wbp = do_CreateInstance("@mozilla.org/embedding/browser/nsWebBrowserPersist;1");
		if(!wbp) return;
		PRUint32 flags = 
			nsIWebBrowserPersist::PERSIST_FLAGS_REPLACE_EXISTING_FILES | 
			nsIWebBrowserPersist::PERSIST_FLAGS_FORCE_ALLOW_COOKIES |
			nsIWebBrowserPersist::PERSIST_FLAGS_AUTODETECT_APPLY_CONVERSION |
			nsIWebBrowserPersist::PERSIST_FLAGS_FROM_CACHE;
		wbp->SetPersistFlags(flags);
		PRUint32 encodingFlags = 0;//TODO
		wbp->SaveDocument(doc, file, NULL/*DataPath*/, NULL, encodingFlags, 80);
	}

	CString GetDesktopPath(){
		TCHAR szPath[MAX_PATH+1];
		LPITEMIDLIST pidl;

		IMalloc *pMalloc;
		SHGetMalloc( &pMalloc );

		HRESULT hr = ::SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY,&pidl);

		if(SUCCEEDED(hr)){
			SHGetPathFromIDList(pidl,szPath);
			pMalloc->Free(pidl);
		}

		pMalloc->Release();

		return CString(szPath);
	}
public:
	bool IsRefreshBeforeNavigate()
	{
		CString str(GetLocationURL());
		return	str.IsEmpty() || str == _T("about:blank");
	}
};



////////////////////////////////////////////////////////////////////////////

}		//namespace MTL

#endif	// __MTLBROWSER_H__
