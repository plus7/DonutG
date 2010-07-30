/**
 *	@file	MainFrame_PreTrnMsg.cpp
 *	@brief	CMainFrame の PreTranslateMessage関係の処理.
 *	@note
 *		+++ MainFrame.cpp から分離.
 */

#include "stdafx.h"
#include "MainFrame.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#ifndef WM_XBUTTONDOWN
 #define WM_XBUTTONDOWN 				0x020B
 #define WM_XBUTTONUP					0x020C
 #define GET_KEYSTATE_WPARAM(wParam)	( LOWORD(wParam) )
 #define GET_XBUTTON_WPARAM(wParam) 	( HIWORD(wParam) )
 #define MK_XBUTTON1					0x0020
 #define MK_XBUTTON2					0x0040
 #define XBUTTON1						0x0001
 #define XBUTTON2						0x0002
#endif



//各ウィンドウへ(主にキー)メッセージを転送する
BOOL CMainFrame::PreTranslateMessage(MSG *pMsg)
{
  #if 0	//+++ 実験
	if (g_pMainWnd && g_pMainWnd->m_hWnd) {
		static CString sav;
		CString tmp = MtlGetWindowText(g_pMainWnd->m_hWnd);
		//if (sav != tmp && tmp != "")
		{
			ErrorLogPrintf(_T("%s\n"), LPCTSTR(tmp));
			sav = tmp;
			MtlSetWindowText(g_pMainWnd->m_hWnd, tmp+"@");
		}
	}
  #endif

	//コマンドバー(メニュー)
	if ( m_CmdBar.PreTranslateMessage(pMsg) )
		return TRUE;

	//アドレスバー
	BYTE ptFlag 	= m_AddressBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE)
		return TRUE;
	else if (ptFlag == _MTL_TRANSLATE_WANT)
		return FALSE;

	//検索バー
	ptFlag = m_SearchBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE)
		return TRUE;
	else if (ptFlag == _MTL_TRANSLATE_WANT)
		return FALSE;

	//ページ内検索バー
	ptFlag = m_fayt.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE)
		return TRUE;
	else if (ptFlag == _MTL_TRANSLATE_WANT)
		return FALSE;

	//エクスプローラバー
	//if (m_ExplorerBar.PreTranslateMessage(pMsg)) return TRUE;
	ptFlag = m_ExplorerBar.PreTranslateMessage(pMsg);
	if (ptFlag == _MTL_TRANSLATE_HANDLE)
		return TRUE;
	else if (ptFlag == _MTL_TRANSLATE_WANT)
		return FALSE;

  #if 1
	//+++ アドレスバーorサーチバーにフォーカスが当たっている時、他の処理で余計なこと(キー入力)させないようにガードしてみる.
	int	bFocus = false;
	if (::IsWindow(m_AddressBar.m_hWnd))
		bFocus	|= (::GetFocus() == m_AddressBar.GetEditCtrl());
	if (::IsWindow(m_SearchBar.m_hWnd))
		bFocus	|= (::GetFocus() == m_SearchBar.GetEditCtrl());
  #endif

	// 基底クラス
	if (/*bFocus == 0 &&*/ baseClass::PreTranslateMessage(pMsg) )
		return TRUE;

	//アクティブ・チャイルド・ウィンドウ
	HWND hWnd = 0;
	if (m_hWndMDIClient && ::IsWindow(m_hWndMDIClient))
		hWnd = MDIGetActive();

  #if 0	//+++ 左ボタンクリックのテスト
	if ( pMsg->message == WM_LBUTTONDOWN) {
		printf("%d,%d\n",pMsg->wParam, pMsg->lParam);
	}
  #endif

	// 中ボタンクリックのチェック
	if ( pMsg->message == WM_MBUTTONDOWN && hWnd != NULL && ::IsChild(hWnd, pMsg->hwnd) && OnMButtonHook(pMsg) )
		return TRUE;

	//マウスジェスチャーへ
	if ( pMsg->message == WM_RBUTTONDOWN && OnRButtonHook(pMsg) )
		return TRUE;

	//サイドボタン
	if (pMsg->message == WM_XBUTTONUP) {
		if ( OnXButtonUp( GET_KEYSTATE_WPARAM(pMsg->wParam), GET_XBUTTON_WPARAM(pMsg->wParam),
						 CPoint( GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) ) ) )
			return TRUE;
	}

	// エクスプローラバーを自動で表示
	if ( (CMainOption::s_dwExplorerBarStyle & MAIN_EXPLORER_AUTOSHOW) == MAIN_EXPLORER_AUTOSHOW )
		ExplorerBarAutoShow(pMsg);

	//BHO
	if (bFocus == 0 && TranslateMessageToBHO(pMsg) )											//+++ bFocusチェック
		return TRUE;

	// hWnd = MDIGetActive();	//+++ 意味ないと思うが、念のため取り直す...やめ

	if (bFocus == 0 && hWnd != NULL && ::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM) pMsg) )	//+++ bFocusチェック
		return TRUE;

	return FALSE; // IsDialogMessage(pMsg);
}


void CMainFrame::UpdateLayout(BOOL bResizeBars /*= TRUE*/)
{
	CRect	  rc;
	GetClientRect(&rc);

	if (bResizeBars) {
		_UpdateReBarPosition(rc);
	}

	{
		CClientDC dc(m_hWnd);
		_DoPaintReBarBorders(dc.m_hDC);
	}
	UpdateBarsPosition(rc, bResizeBars);
	_InflateReBarBordersRect(rc);

	if (m_hWndClient != NULL) {
		::SetWindowPos( m_hWndClient, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE );
	}
}



BOOL CMainFrame::OnXButtonUp(WORD wKeys, WORD wButton, CPoint point)
{
	CString 	strKey;

	switch (wButton) {
	case XBUTTON1: strKey = _T("Side1"); break;
	case XBUTTON2: strKey = _T("Side2"); break;
	}

	DWORD		dwCommand = 0;
	{
		CIniFileI pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
		pr.QueryValue(dwCommand, strKey);
	}

	if (dwCommand == 0)
		return FALSE;

	::SendMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
	return TRUE;
}



void CMainFrame::ExplorerBarAutoShow(MSG *pMsg)
{
	if (pMsg->message != WM_MOUSEMOVE)
		return;

	if (pMsg->wParam != 0)
		return;

	if (  m_ExplorerBar.IsFavBarVisible()		== false
	   && m_ExplorerBar.IsClipboardBarVisible() == false
	   && m_ExplorerBar.IsPanelBarVisible() 	== false
	   && m_ExplorerBar.IsPluginBarVisible()	== false)
		return;

	CPoint point;
	::GetCursorPos(&point);

	CRect  rcWndCmd;
	::GetWindowRect(m_hWndToolBar, &rcWndCmd);

	if ( rcWndCmd.PtInRect(point) )
		return;

	ScreenToClient(&point);

	CRect  rcWnd;
	GetClientRect(&rcWnd);

	if (m_ExplorerBar.IsExplorerBarHidden() == false) {
		CRect rcWndExp;
		m_ExplorerBar.GetClientRect(&rcWndExp);
		rcWnd.left += rcWndExp.Width() + 5;

		if ( rcWnd.PtInRect(point) )
			m_wndSplit.SetSinglePaneMode(SPLIT_PANE_RIGHT);
	} else {
		if (point.x < 10 && point.x >= 0) {
			m_ExplorerBar.ShowBar(m_ExplorerBar.m_dwExStyle, true);
		}
	}
}


BOOL CMainFrame::OnMButtonHook(MSG* pMsg)
{
	DWORD	dwLinkOpenBtnM;
	{
		CIniFileI	 pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
		dwLinkOpenBtnM = pr.GetValue(_T("LinkOpenBtnM"), 0);
	}

	// ボタンが設定されていなかったら、終わり.
	if (dwLinkOpenBtnM == 0)
		return FALSE;
	//+++ メモ：デフォルトでは dwLinkOpenBtmM = IDM_FOLLOWLINKN
	// ATLASSERT(dwLinkOpenBtnM == IDM_FOLLOWLINKN);

  #if 0
	//+++ メモ:この部分の処理は、DonutPやDonutRAPT,unDonutでさほどソースに違いがあるわけでなさそう.
	::SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, 0, pMsg->lParam); 	//+++ おまじない.
	int 	nTabCnt = m_MDITab.GetItemCount();						//+++ リンクをクリックしたかどうかのチェック用.
	::SendMessage(pMsg->hwnd, WM_COMMAND, dwLinkOpenBtnM, 0);		//+++ リンクを新しい窓にひらく
	if (m_MDITab.GetItemCount() != nTabCnt) 						//+++ タブが増えていたら、リンクをクリックしたのだろう
		return TRUE;												//+++ trueを返して中ボタンクリックの処理をやったことにする.
	::SendMessage(pMsg->hwnd, WM_LBUTTONUP, 0, pMsg->lParam);		//+++ リンク以外をクリックした場合おまじないの左クリック押しを終了しておく.
	return FALSE;													//+++ falseを返すことで、IEコンポーネントにウィールクリックの処理を任せる.

	//+++ メモ: おそらく上記の右に書いたコメントのような動作を想定したものだと思われる.
	//		"おまじない"、は、WM_COMMAND+IDM_FOLLOWLINKNのみを送った場合、
	//		なぜか開く挙動が1クリックずれるようで、それの辻褄あわせとして
	//		左クリックを押したことにしているようだ(...よくわからないけれど、たぶん).
	//		ただ、リンク以外の箇所をクリックしたときは、範囲選択の開始として機能してしまうため、
	//		その場合は、速攻左ボタンクリックを終了させる処理をしている。
	//
	//		問題は、クリックしたかどうかの判定で、タブの個数がIDM_FOLLOWLINKNによって
	//		増えた(変動した)かどうかを見てるのだが...
	//		ミニット氏以前の開発者の環境では有効のようだが、undonut+や+modの開発環境では、
	//		リンクの有無にかかわらずこの時点ではタブ個数が変わらない模様... で
	//		FALSEを返す処理に流れ、左クリックキャンセルの処理が窓開クリックと同様に作用して
	//		しまい、結果的に同じページを２つ開けてしまう、というバグに化けてしまっている.
	//
	//		状況的には::SendMessage(pMsg->hwnd, WM_COMMAND, dwLinkOpenBtnM, 0);の実行が
	//		新しいタブができてから復帰してたモノが、リクエストだけ発行してすぐ戻ってくる処理
	//		に変わってしまった、ということに思われる。
	//
	//		生成されるCChildFrame側では メンバーのActivateFrame()の処理の中でタブへの追加が
	//		発生するのだが、WM_COMMAND+dwLinkOpenBtnMでCChildFrameが作られる場合、
	//		CChildFrame::OnNewWindow2() が呼ばれ、このルートではActivateFrame()は直には呼ばれず、
	//		あとでActivateFrame()が呼ばれるようにしている。この状態でOnNewWindow2は終わるので、
	//		SendMessageが終わってもタブの数は変わらない、という今の状態は、
	//		ソース的には当然に思えるのだが... (このへんの大筋はdonutPにさかのぼっても同じ)
	//
	//		で、ActivateFrameをあとで誰が呼ぶかといえば CChildFrame::OnBeforeNavigate2() で、
	//		これは MtlBrawser.hのIWebBrowserEvents2Impl::OnBeforeNavigate2から呼ばれていて
	//		つまり、うまく動いている開発環境では、Donutソース中で明示的にこの関数を呼んでいないけれど、
	//		SendMessageが終了するまでにこれも実行される、ということなのだろう.
	//		(※ ひょっとすると、そもそも OnNewWindow2以外でChildFrame作られていた可能性もあるかもだけど)
	//
	//		OS,IE,PSDK/ATL,WTL, unDonut のどこかの処理がそうなったのだろうが、正直よくわからない
	//		(2007-11-23)
	//		どうやら AtlHost(Ex)でのCreateControlLicEx 中の,Navigate2()を含むelse文が残っているのが
	//		まずかった模様。このelseを削除したら直る. (2008-01-01)
  #else //+++ リンクを押したかどうかの判定にステータスバーにリンク先が設定されているかどうか、を用いるようにした.
	HWND			hWnd	= MDIGetActive();
	if ( ::IsWindow(hWnd) == 0 )
		return FALSE;
	CChildFrame*	pChild	= GetChildFrame(hWnd);
	bool			bLink	= 0;
	if (pChild) {																//+++ カーソルがリンクをさしていたらstatusBar用のメッセージがあることを利用.
		const CString& str = pChild->strStatusBar();
		if (str.IsEmpty() == 0 && str != _T("ページが表示されました"))			//+++ リンクのないページでは"ページが表示されました"というメッセージが設定されているので除外.
			bLink = true;
	}
	::SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, 0, pMsg->lParam); 				//+++ おまじない.
	int 	nTabCnt = m_MDITab.GetItemCount();									//+++ リンクをクリックしたかどうかのチェック用.
	::SendMessage(pMsg->hwnd, WM_COMMAND, dwLinkOpenBtnM, 0);					//+++ リンクを新しい窓にひらく
	int 	nTabCnt2 = m_MDITab.GetItemCount();
	if (nTabCnt != nTabCnt2 || bLink)											//+++ リンクメッセージがあるか、タブが増えていたら、リンクをクリックしたとする.
		return TRUE;															//+++ trueを返して中ボタンクリックの処理をやったことにする.
	::SendMessage(pMsg->hwnd, WM_LBUTTONUP, 0, pMsg->lParam);					//+++ リンク以外をクリックした場合おまじないの左クリック押しを終了しておく.
	return FALSE;																//+++ falseを返すことで、IEコンポーネントにウィールクリックの処理を任せる.
  #endif
}


// Mouse Gesture
BOOL CMainFrame::OnRButtonHook(MSG *pMsg)
{
	HWND		hChildWnd = MDIGetActive();

	if (hChildWnd) {
		BOOL	bUsedMouseGesture = ::SendMessage(hChildWnd, WM_USER_USED_MOUSE_GESTURE, 0, 0) != 0;
		if (bUsedMouseGesture == FALSE)
			return FALSE;
	}

	::SetCapture(m_hWnd);

	CPoint		ptDown;
	ptDown.x = GET_X_LPARAM(pMsg->lParam);
	ptDown.y = GET_Y_LPARAM(pMsg->lParam);
	::ClientToScreen(pMsg->hwnd, &ptDown);

	CPoint		ptBefor   = ptDown;
	CPoint		ptUp	  = ptDown;
	int 		nAng1	  = -1;
	CString 	strMove;
	CString 	strMark1;
	CString 	strMark2;
	DWORD		dwTime	  = -1;
	HWND		hWndHist  = m_hWnd;
	HWND		hWnd	  = NULL;
	BOOL		bNoting   = TRUE;
	MSG 		msg;
	ZeroMemory(&msg, sizeof msg);

	do {
		if (::GetCapture() != hWndHist)
			break;

		if (m_hWnd != hWndHist)
			break;

		if (::GetMessage(&msg, NULL, 0, 0) == FALSE)
			continue;

		int 	nDiff		= 0;
		DWORD	dwCommand	= 0;
		BOOL	bNeedFresh	= FALSE,
				bBreak		= FALSE;

		switch (msg.message) {
		case WM_MOUSEWHEEL:
			dwCommand = GetMouseWheelCmd( (short) HIWORD(msg.wParam) );
			break;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
			dwCommand = GetMouseButtonUpCmd(msg.message);
			break;

		case WM_XBUTTONUP:
			dwCommand = GetMouseButtonUpCmd( msg.message, GET_XBUTTON_WPARAM(msg.wParam) );
			break;

		case WM_MOUSEMOVE:
			ptUp.x	  = GET_X_LPARAM(msg.lParam);
			ptUp.y	  = GET_Y_LPARAM(msg.lParam);
			::ClientToScreen(msg.hwnd, &ptUp);

			nDiff	  = (int) ( sqrt( pow(float (ptBefor.x - ptUp.x), 2.0f) + pow(float (ptBefor.y - ptUp.y), 2.0f) ) );
			if (nDiff < 10)
				break;

			nAng1	  = (int) _GetAngle(ptBefor, ptUp);
			ptBefor   = ptUp;

			if		(nAng1 <  45 || nAng1 >  315)
				strMark1 = _T("→");
			else if (nAng1 >= 45 && nAng1 <= 135)
				strMark1 = _T("↑");
			else if (nAng1 > 135 && nAng1 <  225)
				strMark1 = _T("←");
			else if (nAng1 >= 225 && nAng1 <= 315)
				strMark1 = _T("↓");

			if (strMark1 == strMark2) {
				DWORD dwTimeNow = ::GetTickCount();

				if ( (dwTimeNow - dwTime) > 300 ) {
					strMark2 = _T("");
					dwTime	 = dwTimeNow;
				}
			}

			if (strMark1 != strMark2) {
				strMove += strMark1;
				strMark2 = strMark1;

				DWORD	dwCommand = 0;
				CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
				pr.QueryValue(dwCommand, strMove);
				pr.Close();
				CString strCmdName;

				if (dwCommand != 0) {
					CString strTemp;
					CToolTipManager::LoadToolTipText(dwCommand, strTemp);
					strCmdName.Format(_T("[ %s ]"), strTemp);
				}

				CString 	strMsg;
				strMsg.Format(_T("ジェスチャー : %s %s"), strMove, strCmdName);
				::SetWindowText(m_hWndStatusBar, strMsg);
			}

			dwTime = ::GetTickCount();
			break;

		case WM_KEYDOWN:
			hWnd   = MDIGetActive();
			if (hWnd != NULL)
				SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM) &msg);

			break;

		default:
			DispatchMessage(&msg);
			break;
		}

		switch (dwCommand) {
		case ID_FILE_CLOSE:
			hWnd	   = MDIGetActive();
			if (hWnd == NULL)
				break;

			::PostMessage(hWnd, WM_COMMAND, ID_FILE_CLOSE, 0);
			//::PostMessage(hWnd, WM_CLOSE, 0, 0);
			bNoting    = FALSE;
			bNeedFresh = TRUE;
			//goto NEXT;
			break;

		case ID_GET_OUT:				// 退避
		case ID_VIEW_FULLSCREEN:		// 全体表示
		case ID_VIEW_UP:				// 上へ
		case ID_VIEW_BACK:				// 前に戻る
		case ID_VIEW_FORWARD:			// 次に進む
		case ID_VIEW_STOP_ALL:			// すべて中止
		case ID_VIEW_REFRESH_ALL:		// すべて更新
		case ID_WINDOW_CLOSE_ALL:		// すべて閉じる
		case ID_WINDOW_CLOSE_EXCEPT:	// これ以外閉じる
			::PostMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting    = FALSE;
			bBreak	   = TRUE;
			break;

		case 0:
			break;

		default:
			::PostMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting    = FALSE;
			bNeedFresh = TRUE;
			break;
		}

		if (bNeedFresh) {
			hWnd = MDIGetActive();
			if (hWnd)
				::RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW);
		}

		if (!(GetAsyncKeyState(VK_RBUTTON) & 0x80000000) && msg.message != WM_RBUTTONUP) {
			MSG msgR;
			if (::PeekMessage(&msgR, NULL, 0, 0, PM_NOREMOVE) == 0) {
				break;
			}
		}
	} while (msg.message != WM_RBUTTONUP);

	::ReleaseCapture();

	if (bNoting) {
		ptUp.x = GET_X_LPARAM(msg.lParam);
		ptUp.y = GET_Y_LPARAM(msg.lParam);
		::ClientToScreen(msg.hwnd, &ptUp);

		::SetWindowText( m_hWndStatusBar, _T("") );

		DWORD dwCommand = 0;
		CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
		pr.QueryValue(dwCommand, strMove);
		pr.Close();

		if (dwCommand != 0) {
			::SendMessage(m_hWnd, WM_COMMAND, dwCommand, 0);
			bNoting = FALSE;
		} else if (dwCommand == -1)
			return TRUE;
	}

	if ( bNoting && strMove.IsEmpty() ) {
		::ScreenToClient(pMsg->hwnd, &ptUp);
		pMsg->lParam = MAKELONG(ptUp.x, ptUp.y);

		::PostMessage(pMsg->hwnd, WM_RBUTTONUP, pMsg->wParam, pMsg->lParam);
	}

	return !bNoting;
}


DWORD CMainFrame::GetMouseWheelCmd(short nWheel)
{
	CString 	strKey;
	if (nWheel > 0)
		strKey = _T("WHEEL_UP");
	else
		strKey = _T("WHEEL_DOWN");

	CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	DWORD		dwCommand = pr.GetValue(strKey, 0);
	return dwCommand;
}


DWORD CMainFrame::GetMouseButtonUpCmd(UINT uMsg, UINT nXButton/* = 0*/)
{
	CString 	strKey;
	switch (uMsg) {
	case WM_LBUTTONUP:	strKey = _T("LButtonUp");					break;
	case WM_MBUTTONUP:	strKey = _T("MButtonUp");					break;
	case WM_XBUTTONUP:	strKey.Format(_T("XButtonUp%d"), nXButton); break;
	}

	CIniFileI	pr( _GetFilePath( _T("MouseEdit.ini") ), _T("MouseCtrl") );
	DWORD		dwCommand = pr.GetValue(strKey, 0);
	return dwCommand;
}



BOOL CMainFrame::TranslateMessageToBHO(MSG *pMsg)
{
  #if 1	//+++ お試しで、そもそもif文をやめる...やめれない?
	if (  (WM_KEYFIRST		 <= pMsg->message) && (pMsg->message <= WM_KEYLAST	)
	   || (WM_IME_SETCONTEXT <= pMsg->message) && (pMsg->message <= WM_IME_KEYUP)
	  #if 0	//+++ お試し...
	   || (WM_MOUSEFIRST     <= pMsg->message) && (pMsg->message <= WM_MOUSELAST)
	   || (WM_NCMOUSEMOVE    <= pMsg->message) && (pMsg->message <= 0xAD/*WM_NCXBUTTONDBLCLK*/)
	  #endif
	)
  #endif 
	{
		int nCount = CPluginManager::GetCount(PLT_TOOLBAR);

		for (int i = 0; i < nCount; i++) {
			if ( CPluginManager::Call_PreTranslateMessage(PLT_TOOLBAR, i, pMsg) )
				return TRUE;
		}
	}

	return FALSE;
}

