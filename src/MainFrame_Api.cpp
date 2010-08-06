/**
 *	@file	MainFrame_Api.cpp
 *	@brief	メインフレームのDonutP API関係
 *	@note
 *		+++ mainfrm.h→MainFrame.cpp より、api関係を分離
 */

#include "stdafx.h"

#include "MainFrame.h"								//+++ "MainFrm.h"
//#include "api.h"
//#include "PluginEventImpl.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




//public:
/////////////////////////////////////////////
// DonutP API
/////////////////////////////////////////////


nsIDOMDocument *CMainFrame::ApiGetDocumentObject(int nTabIndex)
{
	HWND					  hTabWnd = m_MDITab.GetTabHwnd(nTabIndex);
	if (hTabWnd == NULL)
		return NULL;

	CGeckoBrowser			  browser = DonutGetNsIWebBrowser(hTabWnd);

	nsCOMPtr<nsIDOMWindow> win;
	browser.m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));

	nsCOMPtr<nsIDOMDocument> doc;
	win->GetDocument(getter_AddRefs(doc));

	NS_ADDREF(doc);
	return doc;
}


nsIDOMWindow *CMainFrame::ApiGetWindowObject(int nTabIndex)
{
	HWND					  hTabWnd = m_MDITab.GetTabHwnd(nTabIndex);
	if (hTabWnd == NULL)
		return NULL;

	CGeckoBrowser			  browser = DonutGetNsIWebBrowser(hTabWnd);

	nsCOMPtr<nsIDOMWindow> win;
	browser.m_spBrowser->GetContentDOMWindow(getter_AddRefs(win));

	NS_ADDREF(win);

	return win;
}


nsIWebBrowser *CMainFrame::ApiGetWebBrowserObject(int nTabIndex)
{
	HWND					  hTabWnd = m_MDITab.GetTabHwnd(nTabIndex);
	if (hTabWnd == NULL)
		return NULL;

	CGeckoBrowser			  browser = DonutGetNsIWebBrowser(hTabWnd);

	NS_ADDREF(browser.m_spBrowser);

	return browser.m_spBrowser;
}


long CMainFrame::ApiGetTabIndex()
{
	return m_MDITab.GetCurSel();
}


void CMainFrame::ApiSetTabIndex(int nTabIndex)
{
	m_MDITab.SetCurSelEx(nTabIndex);
}


void CMainFrame::ApiClose(int nTabIndex)
{
	HWND hTabWnd = m_MDITab.GetTabHwnd(nTabIndex);

	::SendMessage(hTabWnd, WM_COMMAND, ID_FILE_CLOSE, 0);
}


long CMainFrame::ApiGetTabCount()
{
	return m_MDITab.GetItemCount();
}


void CMainFrame::ApiMoveToTab(int nBeforIndex, int nAfterIndex)
{
	CSimpleArray<int> aryBefor;
	aryBefor.Add(nBeforIndex);
	m_MDITab.MoveItems(nAfterIndex + 1, aryBefor);
}


int CMainFrame::ApiNewWindow(const PRUnichar *url, BOOL bActive)
{
	CString 	 strURL(url);

	int 		 nCmdShow = bActive ? -1 : SW_SHOWNOACTIVATE;

	if (m_MDITab.GetItemCount() == 0)
		nCmdShow = -1;

	CChildFrame *pChild   = CChildFrame::NewWindow(m_hWndMDIClient, m_MDITab, m_AddressBar);
	if (pChild == NULL)
		return -1;

	pChild->ActivateFrame(nCmdShow);

	if ( strURL.IsEmpty() )
		strURL = _T("about:blank");
	pChild->Navigate2(strURL);
	pChild->ForceMessageLoop();								//+++ 無理やり... すでに不要だろうがとりあえず残す...
	return m_MDITab.GetTabIndex(pChild->m_hWnd);
}


void CMainFrame::ApiShowPanelBar()
{
	if ( m_ExplorerBar.IsPanelBarVisible() )
		return;

	SendMessage(WM_COMMAND, ID_VIEW_PANELBAR, 0);
}


long CMainFrame::ApiGetTabState(int nIndex)
{
	BYTE bytData = 0;

	if (m_MDITab.GetItemState(nIndex, bytData) == false)
		return -1;

	long nRet	 = 0;

	if (bytData & TCISTATE_SELECTED) {
		nRet = 1;
	} else if (bytData & TCISTATE_MSELECTED) {
		nRet = 2;
	}

	return nRet;
}


nsIWebBrowser *CMainFrame::ApiGetPanelWebBrowserObject()
{
	if ( !m_ExplorerBar.m_PanelBar.IsWindow() )
		m_ExplorerBar.m_PanelBar.CreatePanelBar(m_hWnd, FALSE);

	return m_ExplorerBar.m_PanelBar.GetPanelWebBrowserObject();
}


nsIDOMWindow *CMainFrame::ApiGetPanelWindowObject()
{
	if ( !m_ExplorerBar.m_PanelBar.IsWindow() )
		m_ExplorerBar.m_PanelBar.CreatePanelBar(m_hWnd, FALSE);

	return m_ExplorerBar.m_PanelBar.GetPanelWindowObject();
}


nsIDOMDocument *CMainFrame::ApiGetPanelDocumentObject()
{
	if ( !m_ExplorerBar.m_PanelBar.IsWindow() )
		m_ExplorerBar.m_PanelBar.CreatePanelBar(m_hWnd, FALSE);

	return m_ExplorerBar.m_PanelBar.GetPanelDocumentObject();
}


//IAPI2 by minit
void CMainFrame::ApiExecuteCommand(int nCommand)
{
	::SendMessage(m_hWnd, WM_COMMAND, (WPARAM) (nCommand & 0xFFFF), 0);
}


void CMainFrame::ApiGetSearchText(PRUnichar **text)
{
	CString strBuf;
	CEdit	edit = m_SearchBar.GetEditCtrl();
	int 	len  = edit.GetWindowTextLength() + 1;

	edit.GetWindowText(strBuf.GetBuffer(len), len);
	strBuf.ReleaseBuffer();
	*text = (PRUnichar*)NS_Alloc(sizeof(PRUnichar) * (strBuf.GetLength() + 1));
	wcscpy(*text, strBuf);
}


void CMainFrame::ApiSetSearchText(const PRUnichar *text)
{
	CString strText = text;
	CEdit	edit	= m_SearchBar.GetEditCtrl();

	edit.SendMessage(WM_CHAR, 'P');
	edit.SetWindowText(strText);
}


void CMainFrame::ApiGetAddressText(PRUnichar **text)
{
	CString strBuf;
	CEdit	edit = m_AddressBar.GetEditCtrl();
	int 	len  = edit.GetWindowTextLength() + 1;

	edit.GetWindowText(strBuf.GetBuffer(len), len);
	strBuf.ReleaseBuffer();
	*text = (PRUnichar*)NS_Alloc(sizeof(PRUnichar) * (strBuf.GetLength() + 1));
	wcscpy(*text, strBuf);
}


void CMainFrame::ApiSetAddressText(const PRUnichar *text)
{
	CString strText = text;
	CEdit	edit	= m_AddressBar.GetEditCtrl();

	edit.SendMessage(WM_CHAR, 'P');
	edit.SetWindowText(strText);
}


LRESULT CMainFrame::ApiGetExtendedTabState(int nIndex)
{
	HWND hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return 0x80000000;

	return ::SendMessage(hWndChild, WM_GET_EXTENDED_TABSTYLE, 0, 0);
}


void CMainFrame::ApiSetExtendedTabState(int nIndex, long nState)
{
	HWND hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return;

	::SendMessage(hWndChild, WM_SET_EXTENDED_TABSTYLE, (WPARAM) nState, 0);
}


LRESULT CMainFrame::ApiGetKeyState(int nKey)
{
	//return (::GetAsyncKeyState(nKey) & 0x80000000) != 0;		//+++ これだと、既存スクリプトで不具合でることあるかもなんで、返型のほうを現状に合わせた.
	return (::GetAsyncKeyState(nKey) & 0x80000000);
}


long CMainFrame::ApiGetProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar* key, int nDefault)
{
	CString 	strFile    = file;
	CString 	strSection = section;
	CString 	strKey	   = key;
	CIniFileI	pr(strFile, strSection);
	return (long)pr.GetValue( strKey, nDefault );
}


void CMainFrame::ApiWriteProfileInt(const PRUnichar *file, const PRUnichar *section, const PRUnichar* key, int nValue)
{
	CString 	strFile    = file;
	CString 	strSection = section;
	CString 	strKey	   = key;
	CIniFileO	pr(strFile, strSection);
	pr.SetValue(nValue, strKey);
	//x pr.Close(); 	//+++
}


void CMainFrame::ApiGetProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar* key, const PRUnichar* Default, PRUnichar **text)
{
	CString 	strFile 	= file;
	CString 	strSection	= section;
	CString 	strKey		= key;
	CString 	strDefault	= Default;
	CIniFileI	pr(strFile, strSection);
	CString 	strBuf		= pr.GetStringUW(strKey, strDefault, 0xFFFFFFFF); //+++ size=0xFFFFFFFFの場合はバッファサイズ拡張ありで取得.
	*text = (PRUnichar*)NS_Alloc(sizeof(PRUnichar) * (strBuf.GetLength() + 1));
	wcscpy(*text, strBuf);
}


void CMainFrame::ApiWriteProfileString(const PRUnichar *file, const PRUnichar *section, const PRUnichar* key, const PRUnichar *text)
{
	CString 		strFile    = file;
	CString 		strSection = section;
	CString 		strKey	   = key;
	CString 		strText    = text;
	CIniFileO	pr(strFile, strSection);
	pr.SetStringUW( strText, strKey );
}


void CMainFrame::ApiGetScriptFolder( /*[out, retval]*/ BSTR *bstrFolder)
{
	ATLASSERT(bstrFolder != 0);
	CString 	strBuf = Misc::GetExeDirectory() + _T("Script\\");
	if (::GetFileAttributes(strBuf) == 0xFFFFFFFF)
		strBuf.Empty();

	*bstrFolder = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiGetCSSFolder( /*[out, retval]*/ BSTR *bstrFolder)
{
	ATLASSERT(bstrFolder != 0);
	CString 	strBuf = Misc::GetExeDirectory() + _T("CSS\\");
	if (::GetFileAttributes(strBuf) == 0xFFFFFFFF)
		strBuf.Empty();

	*bstrFolder = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiGetBaseFolder( /*[out, retval]*/ BSTR *bstrFolder)
{
	ATLASSERT(bstrFolder != 0);
	CString 	strBuf = Misc::GetExeDirectory();
	if (::GetFileAttributes(strBuf) == 0xFFFFFFFF)
		strBuf.Empty();

	*bstrFolder = CComBSTR(strBuf).Copy();
}


void CMainFrame::ApiGetExePath( /*[out, retval]*/ BSTR *bstrPath)
{
	ATLASSERT(bstrPath != 0);
	TCHAR Buf[MAX_PATH];
	Buf[0]		= 0;	//+++
	::GetModuleFileName(_Module.GetModuleInstance(), Buf, MAX_PATH);
	*bstrPath 	= CComBSTR(Buf).Copy();
}


void CMainFrame::ApiSetStyleSheet(int nIndex, BSTR bstrStyleSheet, BOOL bOff)
{
  #ifndef NO_STYLESHEET
	HWND		 hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return;

	CChildFrame *pChild    = GetChildFrame(hWndChild);
	if (pChild == NULL)
		return;

	CString 	 strStylePath(bstrStyleSheet);
	CString 	 strStyleTitle;

	if ( bOff || strStylePath.IsEmpty() ) {
		// Off or Default
	} else {
		// Set New StyleSheet
		strStyleTitle = strStylePath.Mid(strStylePath.ReverseFind('\\') + 1);
	}

	//pChild->StyleSheet(strStyleTitle, bOff, strStylePath); TODO:
  #endif
}


//IAPI3
void CMainFrame::ApiSaveGroup(BSTR bstrGroupFile)
{
	CString strFile = bstrGroupFile;

	_SaveGroupOption(strFile);
}


void CMainFrame::ApiLoadGroup(BSTR bstrGroupFile, BOOL bClose)
{
	CString strFile = bstrGroupFile;

	_LoadGroupOption(strFile, bClose != 0/*? true : false*/);
}


//private:
///+++ プラグイン側から渡されるiniファイル情報...
struct CMainFrame::_ExtendProfileInfo {
	/*const*/LPCTSTR	lpstrIniPath;
	/*const*/LPCTSTR	lpstrSection;
	BOOL			  	bGroup;
};


//public:
int CMainFrame::ApiNewWindow3(BSTR bstrURL, BOOL bActive, long ExStyle, void *pExInfo)
{
	CString 	 strURL   = bstrURL;

	int 		 nCmdShow = bActive ? -1 : SW_SHOWNOACTIVATE;

	if (m_MDITab.GetItemCount() == 0)
		nCmdShow = -1;

	CChildFrame *pChild   = CChildFrame::NewWindow(m_hWndMDIClient, m_MDITab, m_AddressBar);

	if (pChild == NULL)
		return -1;

	pChild->ActivateFrame(nCmdShow);

	if ( strURL.IsEmpty() )
		strURL = _T("about:blank");

	if (pExInfo && *(INT_PTR *) pExInfo) {
		_ExtendProfileInfo *_pExInfo    = (_ExtendProfileInfo *) pExInfo;
	  #ifdef UNICODE
		const char*			s			= (const char*)_pExInfo->lpstrIniPath;
		const char*			t			= (const char*)_pExInfo->lpstrSection;
		CString 			strPath;
		CString 			strSection;
		if (*s && s[1] == 0) {	//+++ パス名の最初はほぼ半角だろうで、かつ、1バイトのpathはないだろうとして、utf16と判断.
			strPath     = (TCHAR*)s;
			strSection  = (TCHAR*)t;
		} else {				//+++ プラグイン側がMB版でコンパイルされている場合を考慮.
			strPath     = (char*)s;
			strSection  = (char*)t;
		  #if 0	//+++ ファイルだなくて URL なんだから、まずい
			if (Misc::IsExistFile(strPath) == 0) {	// ファイルが存在しない場合は、やっぱりutf16として扱ってみる.
				strPath     = (TCHAR*)s;
				strSection  = (TCHAR*)t;
			}
		  #endif
		}
	  #else
		CString 			strPath     = _pExInfo->lpstrIniPath;
		CString 			strSection  = _pExInfo->lpstrSection;
	  #endif
		try {
			_Load_OptionalData(pChild, strPath, strSection);
		} catch (...) {
			ErrorLogPrintf(_T("ApiNewWindow3"));
			MessageBox( _T("例外エラーが発生しました。(In NewWindow3 Function)\n")
						_T("安全のためプログラムを再起動させてください。")			);
		}

		if (_pExInfo->bGroup) {
			pChild->view().m_ViewOption._GetProfile(strPath, strSection, !CMainOption::s_bTabMode);
			//+++ _GetProfile中で Navigate2 しているので、直後でまたNavigate2する必要ないから、ここで帰っておく
			return m_MDITab.GetTabIndex(pChild->m_hWnd);
		} else {
			pChild->OnSetExtendedTabStyle(ExStyle | CChildFrame::FLAG_SE_NOREFRESH);
		}
	} else {
		pChild->OnSetExtendedTabStyle(ExStyle | CChildFrame::FLAG_SE_NOREFRESH);
	}
	pChild->Navigate2(strURL);
	pChild->ForceMessageLoop();						//+++ 無理やり... すでに不要だろうがとりあえず残す....

	return m_MDITab.GetTabIndex(pChild->m_hWnd);
}


long CMainFrame::ApiAddGroupItem(BSTR bstrGroupFile, int nIndex)
{
	HWND		 hWndChild = m_MDITab.GetTabHwnd(nIndex);

	if ( !::IsWindow(hWndChild) )
		return -1;

	CChildFrame *pChild    = GetChildFrame(hWndChild);

	if (pChild == NULL)
		return -1;

	return pChild->_AddGroupOption(bstrGroupFile);
}


long CMainFrame::ApiDeleteGroupItem(BSTR bstrGroupFile, int nIndex)
{
	CString 		strGroupFile = bstrGroupFile;
	{
		CString 	strSection;
		strSection.Format(_T("Window%d"), nIndex);
		CIniFileO	pr(strGroupFile, strSection);
		bool		bRet = pr.DeleteSection();
		pr.Close();
		if (!bRet) {
			MessageBox(_T("Error1: セクションの削除に失敗しました。"));
			return 0;
		}
	}

	std::list<CString>	buf;
	bool bRet = FileReadString(strGroupFile, &buf);
	if (!bRet) {
		MessageBox(_T("Error2: ファイルの読み込みに失敗しました。"));
		return 0;
	}

	std::list<CString>::iterator str;

	for (str = buf.begin(); str != buf.end(); ++str) {
		CString strCheck = str->Left(7);
		strCheck.MakeUpper();

		if ( strCheck == _T("[WINDOW") ) {
			int nSecIndex = _ttoi( str->Mid(7, str->GetLength() - 7 - 1) );
			if (nSecIndex > nIndex) {
				str->Format(_T("[Window%d]"), nSecIndex - 1);
			}
		}
	}

	bRet = FileWriteString(strGroupFile, &buf);
	if (!bRet) {
		MessageBox(_T("Error3: ファイルの書き込みに失敗しました。"));
		return 0;
	}

	DWORD		dwCount = 0, dwActive = 0, dwMaximize = 0;
	CIniFileIO	pr( strGroupFile, _T("Header") );
	pr.QueryValue( dwCount, _T("Count") );
	if (dwCount) {
		dwCount--;
		pr.SetValue( dwCount, _T("Count") );
	}

	pr.QueryValue( dwActive, _T("active") );
	if (dwCount == 0)
		pr.DeleteValue( _T("active") );
	else if ( (int) dwActive > nIndex )
		pr.SetValue( dwActive - 1, _T("active") );

	pr.QueryValue( dwMaximize, _T("maximized") );

	if (dwCount == 0)
		pr.DeleteValue( _T("maximized") );
	else if ( (int) dwMaximize > nIndex )
		pr.SetValue( dwMaximize - 1, _T("maximized") );

	return 1;
}


//IAPI4
HWND CMainFrame::ApiGetHWND(long nType)
{
	if (nType == 0)
		return m_hWnd;
	return NULL;
}

