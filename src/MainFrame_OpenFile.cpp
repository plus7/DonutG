/**
 *	@file	MainFrame_OpenFile.cpp
 *	@brief	CMainFrame の ファイルを開ける関係の処理.
 *	@note
 *		+++ MainFrame.cpp から分離.
 */

#include "stdafx.h"
#include "MainFrame.h"
#include "dialog/OpenURLDialog.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





bool CMainFrame::OnDDEOpenFile(const CString &strFileName)
{
	//dmfTRACE(_T("CMainFrame::OnDDEOpenFile(%s)\n"), strFileName);
  #if 1 //+++ トレイ状態からの復帰でのバグ対策.
	IfTrayRestoreWindow();							//+++ トレイ状態だったら復活.
	DWORD flags = DonutGetStdOpenFlag();
	OnUserOpenFile( strFileName, flags );
	if (CStartUpOption::s_dwActivate) {
		MtlSetForegroundWindow(m_hWnd);
	}
  #else
	OnUserOpenFile( strFileName, DonutGetStdOpenFlag() );
	if (CStartUpOption::s_dwActivate) {
		//+++ OnUserOpenFile()の時点で最大化/フルスクリーンだった場合の対策でnCmdShowを渡して考慮するように変更.
		MtlSetForegroundWindow(m_hWnd, m_OnUserOpenFile_nCmdShow);	//+++ 実はこのnCmdShowは意味が違うものを持ってきてるような気がする...があとまわし.
	}
  #endif
	// UDT DGSTR ( added by dai
	return true;
}




//public:
// Message handlers
// alternates OpenFile

#if 1	//+++ url別拡張プロパティの処理を追加
HWND CMainFrame::OnUserOpenFile(const CString& strUrl, DWORD dwOpenFlag)
{
  #if 0	//+++ CChildFrame::OnBeforeNavigate2側で処理するように変更したので、これでok ...としたい...
	return UserOpenFile(strUrl, dwOpenFlag);
  #else	//+++ CChildFrame::OnBeforeNavigate2側で処理するようにしたので、2重に検索がかかってしまうが、
		//+++ オプション反映のタイミングの都合なるべく早くフラグを設定したほうがよさそうなので、速度犠牲にするなら、こちら

	CString		str(strUrl);
	MtlRemoveStringHeaderAndFooter(str);

   #if 0	//+++ 仕様と思ったが、.url側の拡張プロパティの件があるので、ここではできない
	if ( MtlIsExt( str, _T(".url") ) ) {
		if (MTL::ParseInternetShortcutFile(str) == 0)
			return NULL;
	}
   #endif

	DWORD dwExProp	 = 0xFFFFFFFF;
	DWORD dwExProp2	 = 8;
	if (   (_check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) == 0 || CUrlSecurityOption::activePageToo())
		&& CUrlSecurityOption::FindUrl(str, &dwExProp, &dwExProp2, 0)
	) {
		return OpenUrlWithExProp(str, dwOpenFlag, dwExProp, dwExProp2);
	} else {
		return UserOpenFile(str, dwOpenFlag);
	}
  #endif
}
#endif



//+++ url別拡張プロパティ対応で、本来のOnUserOpenFileをUserOpenFileに変名. 引数を末に追加.
HWND CMainFrame::UserOpenFile(const CString& strFile0, DWORD openFlag, int dlCtrlFlag, int extededStyleFlags)
{
	CString		strFile (strFile0);
	MtlRemoveStringHeaderAndFooter(strFile);

	if ( MtlIsExt( strFile, _T(".url") ) ) {	//+++ メモ:urlのときの処理. 拡張プロパティの取得とかあるので、専用のopen処理へ...
		return OpenInternetShortcut(strFile, openFlag);
	}

	if (  !MtlIsProtocol( strFile, _T("http") )
	   && !MtlIsProtocol( strFile, _T("https") ) )
	{
		if ( MtlPreOpenFile(strFile) )
			return NULL;	// handled
	  #if 1	//+++	タブがあるとき
		if (m_MDITab.GetItemCount() > 0) {
			if (strFile.Find(':') < 0 && strFile.Left(1) != _T("/") && strFile.Left(1) != _T("\\")) {	// すでに具体的なパスでないとき、
				HWND			hWndChild 	= m_MDITab.GetTabHwnd(m_MDITab.GetCurSel());
				CChildFrame*	pChild 		= GetChildFrame(hWndChild);
				if (pChild) {		//+++ アクティブページをみて
					CString strBase  = pChild->GetLocationURL();
					CString str7     = strBase.Left(7);
					if (str7 == "file://") {	//+++ file://だったらば、ローカルでのディレクトリ移動だろうとみなして
						strFile = Misc::MakeFullPath( strBase, strFile );	//+++ フルパス化
					}
				}
			}
		}
	  #endif
	}

	if (strFile.GetLength() > INTERNET_MAX_PATH_LENGTH)
		return NULL;

	// UH (JavaScript dont create window)
	CString 	 strJava = strFile.Left(11);
	strJava.MakeLower();
	if ( strJava == _T("javascript:") ) {
		if ( strFile == _T("javascript:location.reload()") )
			return NULL;
		openFlag |= D_OPENFILE_NOCREATE;
	}

	// dfg files
	if ( MtlIsExt( strFile, _T(".dfg") ) ) {
		if ( !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOCLOSEDFG) )
			_LoadGroupOption(strFile, true);
		else
			_LoadGroupOption(strFile, false);
		return NULL;
	}

  #ifndef USE_DIET	//+++	サイタマ封印(about:blankの不具合修正で誤ってコメントアウト...
					//+++	だったがunDonut+より、ちゃんと機能していないようだし?、わざわざ復活する必要もないかな、と)
					//+++   ちょっと気が変わったので条件付の暫定復活
	// minit(about:* pages)
	{
		CString strAbout = strFile.Left(6);
		if ( strAbout == _T("about:") && strFile != _T("about:blank") ) {
			HWND hWndAbout =  _OpenAboutFile(strFile);
			if (hWndAbout)
				return hWndAbout;
		}
	}
  #endif

	HWND	hWndActive = MDIGetActive();

	int 	nCmdShow   = _check_flag(D_OPENFILE_ACTIVATE, openFlag) ? -1 : SW_SHOWNOACTIVATE;
	if (hWndActive == NULL) {					// no window yet
		nCmdShow = -1;							// always default
	}

	if ( hWndActive != NULL && _check_flag(D_OPENFILE_NOCREATE, openFlag) ) {
		CGeckoBrowser browser = DonutGetNsIWebBrowser(hWndActive);

		if ( !browser.IsBrowserNull() ) {
			browser.Navigate2(strFile);

			if ( !_check_flag(D_OPENFILE_NOSETFOCUS, openFlag) ) {
				// reset focus
				::SetFocus(NULL);
				MtlSendCommand(hWndActive, ID_VIEW_SETFOCUS);
			}

			return NULL;
			//return hWndActive;
		}
	}

 	//+++
	//x if (CMainOption::s_dwMainExtendedStyle & (/*MAIN_EX_NOACTIVATE_NEWWIN|*/MAIN_EX_NEWWINDOW))
	{
		//+++ if (dlCtrlFlag < 0)
		//+++	dlCtrlFlag = CDLControlOption::s_dwDLControlFlags;
		CChildFrame* pChild 	  = CChildFrame::NewWindow(m_hWndMDIClient, m_MDITab, m_AddressBar, false/*true*/, dlCtrlFlag, extededStyleFlags);
		if (pChild == NULL)
			return NULL;

		if ( !strFile.IsEmpty() ) {
			m_OnUserOpenFile_nCmdShow = pChild->ActivateFrame(nCmdShow);
			pChild->Navigate2(strFile);
			pChild->ForceMessageLoop(); 		//+++ 無理やり、この場でメッセージをさばく. もう不要のはずだが、あっても問題ないのなら、念のため残しておく...
		} else {
			m_OnUserOpenFile_nCmdShow = pChild->ActivateFrame(nCmdShow);
		}

		if ( !_check_flag(D_OPENFILE_NOSETFOCUS, openFlag) ) {
			if (MDIGetActive() == pChild->m_hWnd) { // a new window activated, so automatically set focus
				// reset focus
				::SetFocus(NULL);
				MtlSendCommand(pChild->m_hWnd, ID_VIEW_SETFOCUS);
			} else {
				// It's reasonable not to touch a current focus.
			}
		}

		return pChild->m_hWnd;
	}
}



LRESULT CMainFrame::OnOpenWithExProp(_EXPROP_ARGS *pArgs)
{
	if (!pArgs)
		return 0;

	HWND		 hWndNew = OpenUrlWithExProp(pArgs->strUrl, pArgs->dwOpenFlag, pArgs->strIniFile, pArgs->strSection);
	if (!hWndNew)
		hWndNew = MDIGetActive();

	CChildFrame *pChild  = GetChildFrame(hWndNew);

	if (pChild) {
		// 検索オプションを取得
		DWORD		dwStatus;
		{
			CIniFileI pr( g_szIniFileName, _T("Search") );
			dwStatus  = pr.GetValue( _T("Status"), 0 );
		}

		CString 	str 	 = pArgs->strSearchWord;

		// 「全角を半角に置換」がチェックされてたら変換して子ウィンドウに文字列を渡す。
		if (dwStatus & STS_TEXT_FILTER)
			m_SearchBar.FilterString(str);

	  	//+++ 子窓に検索設定を反映 (関数化)
	   #if 1 //defined USE_UNDONUT_G_SRC
		if (dwStatus & STS_AUTOHILIGHT)
			pChild->SetSearchWordAutoHilight( str, 1 );
	   #else
		pChild->SetSearchWordAutoHilight( str, (dwStatus & STS_AUTOHILIGHT) != 0 );
	   #endif
	}

	return (LRESULT) hWndNew;
}


HWND CMainFrame::OpenInternetShortcut(CString strUrlFile, DWORD dwOpenFlag)
{
	CString strUrl = strUrlFile;

	if ( !MTL::ParseInternetShortcutFile(strUrl) )
		return NULL;

	return OpenUrlWithExProp(strUrl, dwOpenFlag, strUrlFile);
}


HWND CMainFrame::OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, DWORD dwExProp, DWORD dwExProp2)
{
	if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) ) {
		return OpenExPropertyActive(strUrl, dwExProp, dwExProp2, dwOpenFlag);		//既存のタブで開く
	} else {
		return OpenExPropertyNew(strUrl, dwExProp, dwExProp2, dwOpenFlag); 		//新規に開く
	}
}


HWND CMainFrame::OpenUrlWithExProp(CString strUrl, DWORD dwOpenFlag, CString strIniFile, CString strSection /*= DONUT_SECTION*/)
{
	DWORD dwExProp = 0xAAAAAA;		//+++ 初期値変更
	DWORD dwExProp2= 0x8;			//+++ 拡張プロパティを増設.

	if ( CExProperty::CheckExPropertyFlag(dwExProp, dwExProp2, strIniFile, strSection) ) {
		if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) ) {
			return OpenExPropertyActive(strUrl, dwExProp, dwExProp2, dwOpenFlag);	//既存のタブで開く
		} else {
			return OpenExPropertyNew(strUrl, dwExProp, dwExProp2, dwOpenFlag); 	//新規に開く
		}
	} else {
	  #if 1	//+++	URL別拡張プロパティのチェック＆処理.
			//		...だったが、CChildFrame::OnBeforeNavigate2側で処理するように変更したので、ここはなし
			//		...にしたかったっが、どうも、dlオプション反映のタイミングにラグがあるようなんで、
			//		なるべく早く反映されるように、速度犠牲で反映.
		if (   (_check_flag(D_OPENFILE_NOCREATE, dwOpenFlag) == 0 || CUrlSecurityOption::activePageToo())
			&& CUrlSecurityOption::FindUrl(strUrl, &dwExProp, &dwExProp2, 0)
		) {
			return OpenUrlWithExProp(strUrl, dwOpenFlag, dwExProp, dwExProp2);
		}
	  #endif
		return OpenExPropertyNot(strUrl, dwOpenFlag);					//標準オプションで開く
	}
}


//既存タブにナビゲートしたのちに拡張設定を適用する.  //+++ dwExProp2増設.
HWND CMainFrame::OpenExPropertyActive(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag)
{
	dwOpenFlag |= D_OPENFILE_NOCREATE;

	//アクティブなタブがナビゲートロックされているかを確認
	HWND		 hWndActive = MDIGetActive();
	if ( hWndActive && ::IsWindow(hWndActive) ) {
		CChildFrame *pChild = GetChildFrame(hWndActive);

		if (pChild) {
			DWORD dwExFlag = pChild->view().m_ViewOption.m_dwExStyle;

			if (dwExFlag & DVS_EX_OPENNEWWIN)
				return OpenExPropertyNew(strUrl, dwExProp, dwExProp2, dwOpenFlag);
		}
	}

	//+++ (dwExProp2増設でUserOpenFileの後にあったのを前に移動.)
	CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);

	//取得したURLをアクティブなタブで開かせる（標準処理に任せる）
	BOOL		 	bOpened		= FALSE;
	HWND 			hWndNew		= UserOpenFile(strUrl, dwOpenFlag, ExProp.GetDLControlFlags(), ExProp.GetExtendedStyleFlags() );

	if (hWndNew && !hWndActive) {
		hWndActive = hWndNew;											//ウィンドウが無かったので新規に開いた
		bOpened    = TRUE;
	}

	//拡張プロパティを適用する
	if (hWndActive == NULL)
		return NULL;
	CChildFrame*	pChild 	= GetChildFrame(hWndActive);
	if (!pChild)
		return NULL;

	pChild->view().PutDLControlFlags( ExProp.GetDLControlFlags() );
	pChild->SetViewExStyle(ExProp.GetExtendedStyleFlags(), TRUE);
	pChild->view().m_ViewOption.SetAutoRefreshStyle( ExProp.GetAutoRefreshFlag() );

	if (bOpened)
		return hWndActive;

	return NULL;
}


//新規タブを開いたのち拡張設定を適用する
HWND CMainFrame::OpenExPropertyNew(CString &strUrl, DWORD dwExProp, DWORD dwExProp2, DWORD dwOpenFlag)
{
  #if 1	//+++
	dwOpenFlag &= ~D_OPENFILE_NOCREATE;

	CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp, dwExProp2);
	int			 dlCtrlFlag        = ExProp.GetDLControlFlags();
	int			 extendedStyleFlag = ExProp.GetExtendedStyleFlags();
	//URLで新規タブを開く
	HWND 			hWndNew		= UserOpenFile(strUrl, dwOpenFlag, dlCtrlFlag, extendedStyleFlag);
	if ( hWndNew == 0 || !::IsWindow(hWndNew) )
		return NULL;

	//拡張プロパティを適用する
	CChildFrame *pChild  = GetChildFrame(hWndNew);
	if (!pChild)
		return NULL;

	pChild->view().PutDLControlFlags( dlCtrlFlag );
	pChild->SetViewExStyle(ExProp.GetExtendedStyleFlags(), TRUE);
	pChild->view().m_ViewOption.SetAutoRefreshStyle( ExProp.GetAutoRefreshFlag() );

	return hWndNew;
  #else
	dwOpenFlag &= ~D_OPENFILE_NOCREATE;

	//URLで新規タブを開く
	HWND 			hWndNew		= UserOpenFile(strUrl, dwOpenFlag);
	if ( hWndNew == 0 || !::IsWindow(hWndNew) )
		return NULL;

	//拡張プロパティを適用する
	CChildFrame *pChild  = GetChildFrame(hWndNew);
	if (!pChild)
		return NULL;
	CExProperty  ExProp(CDLControlOption::s_dwDLControlFlags, CDLControlOption::s_dwExtendedStyleFlags, 0, dwExProp);

	pChild->view().PutDLControlFlags( ExProp.GetDLControlFlags() );
	pChild->SetViewExStyle(ExProp.GetExtendedStyleFlags(), TRUE);
	pChild->view().m_ViewOption.SetAutoRefreshStyle( ExProp.GetAutoRefreshFlag() );

	return hWndNew;
  #endif
}


//タブを開いたのち、標準の設定を適用する
HWND CMainFrame::OpenExPropertyNot(CString &strUrl, DWORD dwOpenFlag)
{
	//アクティブなタブがナビゲートロックされているかを確認
	HWND hWndActive = MDIGetActive();

	if ( ::IsWindow(hWndActive) ) {
		CChildFrame *pChild = GetChildFrame(hWndActive);

		if (pChild) {
			DWORD dwExFlag = pChild->view().m_ViewOption.m_dwExStyle;

			if (dwExFlag & DVS_EX_OPENNEWWIN)
				dwOpenFlag &= ~D_OPENFILE_NOCREATE; 			//新規タブで開くように
		}
	}

	//取得したURLを開かせる
	HWND 			hWndNew		= UserOpenFile(strUrl, dwOpenFlag);
	if (hWndNew) {
		//新規に開いたタブなので何もしなくてよい
		return hWndNew;
	} else {
		//オプションを標準のもので上書き
		CChildFrame *pChild = GetChildFrame( MDIGetActive() );

		if (!pChild)
			return NULL;

		pChild->view().PutDLControlFlags(CDLControlOption::s_dwDLControlFlags);
		pChild->SetViewExStyle(CDLControlOption::s_dwExtendedStyleFlags, TRUE);
		pChild->view().m_ViewOption.SetAutoRefreshStyle(0);
	}

	return NULL;
}



LRESULT CMainFrame::OnFileRecent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	// get file name from the MRU list
	if (ID_FILE_MRU_FIRST <= wID && wID <= ID_FILE_MRU_LAST)						//旧範囲IDから新範囲IDへ変換
		wID = wID - ID_FILE_MRU_FIRST + ID_RECENTDOCUMENT_FIRST;

	CRecentDocumentListFixed::RecentDocEntry Entry;

	if ( CMainOption::s_pMru->GetFromList(wID, &Entry) ) {
		DWORD dwOpenFlag = DonutGetStdOpenCreateFlag(); /*DonutGetStdOpenFlag()*/	// Force New Window // UDT DGSTR
		HWND  hWndNew	 = OnUserOpenFile(Entry.szDocName, dwOpenFlag);

		if ( ::IsWindow(hWndNew) ) {
			if (CMainOption::s_bTravelLogClose) {
				CChildFrame *pChild = GetChildFrame(hWndNew);

				if (pChild) {
					_Load_OptionalData2(pChild, Entry.arrFore, Entry.arrBack);
				}
			}
		}

		CMainOption::s_pMru->RemoveFromList(wID);									// UDT DGSTR
	} else {
		::MessageBeep(MB_ICONERROR);
	}

	return 0;
}



//+++ メモ：複数起動無しで、別のプログラムから引数付きで起動された場合に、ここにくる。
//+++       他の外部からの追加表示はDDE経由で行われるようで、こちらにくることはなさそう...?
void CMainFrame::OnNewInstance(ATOM nAtom)			// WM_NEWINSTANCE
{
	enum { NAME_LEN = 0x4000 };
	TCHAR szBuff[NAME_LEN+2];
	szBuff[0]	= 0;
	bool	bActive = !(CMainOption::s_dwMainExtendedStyle & MAIN_EX_NOACTIVATE);

	if (::GlobalGetAtomName(nAtom, szBuff, NAME_LEN) != 0) {
		//dmfTRACE(_T("CMainFrame::OnNewInstance: %s\n"), szBuff);
		CString					strPath;
		std::vector<CString>	strs;
		Misc::SeptTextToWords(strs, szBuff);
		for (unsigned i = 0; i < strs.size(); ++i) {
			CString&	str = strs[i];
			int 		c   = str[0];
			if (c == _T('-') || c == _T('/')) {
				if (str.CompareNoCase(_T("-tray")) == 0 || str.CompareNoCase(_T("/tray"))) {
					bActive = false;
				}
			} else {
				if (strPath.IsEmpty())
					strPath = str;
				else
					strPath += _T(' ') + str;
			}
		}
		OnUserOpenFile( strPath, DonutGetStdOpenFlag() );
		::GlobalDeleteAtom(nAtom);
	}

	if ( bActive ) {
		MtlSetForegroundWindow(m_hWnd); 						//ウインドウをアクティブにする
		IfTrayRestoreWindow();									//+++ トレイ状態だったら復活.
		if (m_bOldMaximized == 0 && m_bFullScreen == 0) 		//+++
			ShowWindow_Restore(1);	//ShowWindow(SW_RESTORE);	//+++ サイズを戻す.
	}
}



////////////////////////////////////////////////////////////////////////////////
//ファイルメニュー
//　コマンドハンドラ
////////////////////////////////////////////////////////////////////////////////
////新規作成

//ポームページ
void CMainFrame::OnFileNewHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	HWND hWndChild = OnUserOpenFile( CString(), DonutGetStdOpenActivateFlag() );

	if (hWndChild) {
		CGeckoBrowser browser = DonutGetNsIWebBrowser(hWndChild);

		if (browser.m_spBrowser != NULL)
			browser.GoHome();
	}
}


//現在のページ
void CMainFrame::OnFileNewCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	HWND hWndActive = MDIGetActive();

	if (hWndActive) {
		CGeckoBrowser browser = DonutGetNsIWebBrowser(hWndActive);

		if (browser.m_spBrowser != NULL) {
			CString strURL = browser.GetLocationURL();

			if ( !strURL.IsEmpty() )
				OnUserOpenFile( strURL, DonutGetStdOpenActivateFlag() );
		}
	}
}


LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	if (CFileNewOption::s_dwFlags == FILENEW_BLANK) {
		OnFileNewBlank(0, 0, 0);
	} else if (CFileNewOption::s_dwFlags == FILENEW_COPY) {
		if (MDIGetActive() != NULL)
			SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_COPY, 0), 0);
		else
			OnFileNewBlank(0, 0, 0);
	} else if (CFileNewOption::s_dwFlags == FILENEW_HOME) {
		OnFileNewHome(0, 0, 0);
	} else if (CFileNewOption::s_dwFlags == FILENEW_USER) {		//+++ ユーザー指定のページを開く
		//CIniFileI 	pr( g_szIniFileName, _T("Main") );
		//CString		str = pr.GetStringUW( _T("File_New_UsrPage") );
		//pr.Close();
		CString&	str	= CFileNewOption::s_strUsr;
		if ( !str.IsEmpty() )
			OnUserOpenFile(str, 0);
		else
			OnFileNewBlank(0, 0, 0);
	} else {
		ATLASSERT(FALSE);
		OnFileNewBlank(0, 0, 0);
	}

	return 0;
}


//空白ページ
void CMainFrame::OnFileNewBlank(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	OnUserOpenFile( _T("about:blank"), DonutGetStdOpenActivateFlag() );
	SendMessage(WM_COMMAND, ID_SETFOCUS_ADDRESSBAR, 0);
}


//クリップボード
void CMainFrame::OnFileNewClipBoard(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CString strText = MtlGetClipboardText();

	if ( strText.IsEmpty() )
		return;

	OnUserOpenFile( strText, DonutGetStdOpenActivateFlag() );
}


//Default.dfg
// UDT DGSTR
LRESULT CMainFrame::OnFileOpenDef(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString 	strPath = Misc::GetExeDirectory() + _T("Default.dfg");
	ATLTRACE2( atlTraceGeneral, 4, _T("CMainFrame::OnFileOpenDef\n") );
	OnUserOpenFile( strPath, DonutGetStdOpenCreateFlag() );
	return 0;
}
// ENDE



void CMainFrame::OnFileNewClipBoard2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CString strText = MtlGetClipboardText();

	if ( strText.IsEmpty() )
		return;

	OnUserOpenFile( strText, DonutGetStdOpenFlag() );	// allow the option
}



LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	//dmfTRACE( _T("CMainFrame::OnFileOpen\n") );
	COpenURLDlg dlg;

	if ( dlg.DoModal() == IDOK && !dlg.m_strEdit.IsEmpty() ) {
		OnUserOpenFile( dlg.m_strEdit, DonutGetStdOpenFlag() );
	}

	return 0;
}



/// DonutG の配布サイトを開く
LRESULT CMainFrame::OnJumpToWebSite(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	//CString strSite = _T("http://www5.ocn.ne.jp/~minute/tab/"); //unDonut古い方
	//CString strSite = _T("http://undonut.sakura.ne.jp/"); // unDonut新しい方
	//CString strSite = _T("http://tekito.genin.jp/undonut+.html"); //unDonut+
	//CString strSite = _T("http://ichounonakano.sakura.ne.jp/64/undonut/"); //unDonut+mod.	旧
	//CString strSite = _T("http://undonut.undo.jp/"); //unDonut+mod.
	CString strSite = _T("http://donut.tnose.net/"); //DonutG

	OnUserOpenFile( strSite, DonutGetStdOpenFlag() );
	return 0;
}


///+++ .exeのあるフォルダを開く
LRESULT CMainFrame::OnOpenExeDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString		progDir = Misc::GetExeDirectory();
	ShellExecute(NULL, _T("open"), progDir, progDir, NULL, SW_SHOW);
	return 0;
}



#ifndef USE_DIET	//+++ saitama専用だったのを外部ファイル読込にして汎用化.
HWND CMainFrame::_OpenAboutFile(CString strFile)
{

	if (strFile.Left(6) != _T("about:"))
		return NULL;
	strFile     = strFile.Mid(6);		// about:
	bool  bWarn = (strFile == "warning");
	if (bWarn == 0) {
		strFile     = Misc::GetFullPath_ForExe( "help\\about\\" + strFile );	//
		if (Misc::IsExistFile(strFile) == 0)	//ファイルがなかったらかえる
			return NULL;
	}
//plus7: Geckoの場合は別の流儀でaboutプロトコルを提供することになる。よってこれ以降削除。あとで考える。
	return 0;
}
#endif
