/**
 *	@file	MtlWin.cpp
 *	@brief	MTL : ウィンドウ関係
 */

#include "stdafx.h"
#include "MtlWin.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




namespace MTL {


// Fatal bug fixed by JOBBY, thanks!
int MtlGetLBTextFixed(HWND hWndCombo, int nIndex, CString &strText)
{
	CComboBox	combo(hWndCombo);
	ATLASSERT( combo.IsWindow() );
	//+++ CString::GetBufferSetLength(0) (GetBuffer(0), AllocBuffer(0))は、メモリ確保せず共通の空を返す...
	//+++ ので、0チェックしないと誤deleteが発生してハングに化ける...
	int 	nRet = combo.GetLBTextLen(nIndex) * 2;
	if (nRet > 0) {
		nRet = combo.GetLBText( nIndex, strText.GetBufferSetLength( nRet+1 ) );
		strText.ReleaseBuffer();
	} else {
		strText.Empty();	//+++ 空にする
	}
	return nRet;
}



int MtlListBoxGetText(HWND hWndBox, int nIndex, CString &strText)
{
	CListBox	box(hWndBox);

	ATLASSERT( box.IsWindow() );
	//+++ CString::GetBufferSetLength(0) (GetBuffer(0), AllocBuffer(0))は、メモリ確保せず共通の空を返す...
	//+++ ので、0チェックしないと誤deleteが発生してハングに化ける...
	int 		nRet = box.GetTextLen(nIndex) * 2;
	if (nRet > 0) {
		nRet = box.GetText( nIndex, strText.GetBufferSetLength( nRet+1 ) );
		strText.ReleaseBuffer();
	} else {
		strText.Empty();	//+++ 空にする
	}
	return nRet;
}


// UDT DGSTR ( add , delete TrayIcon
BOOL TrayMessage(HWND hwnd, DWORD dwMessage, UINT uID, HICON hIcon, LPCTSTR pszTip)
{
	NOTIFYICONDATA	tnd;
	tnd.cbSize				= sizeof (NOTIFYICONDATA);
	tnd.hWnd				= hwnd;
	tnd.uID 				= uID;
	tnd.uFlags				= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage	= MYWM_NOTIFYICON;
	tnd.hIcon				= hIcon;

	//s_bTrayFlag = (dwMessage == NIM_ADD); //+++ トレイに入った時.

	if (pszTip)
		lstrcpyn( tnd.szTip, pszTip, sizeof (tnd.szTip) );
	else
		tnd.szTip[0]		= _T('\0');

	BOOL			res 	= Shell_NotifyIcon(dwMessage, &tnd) != 0;

	if (hIcon)
		DestroyIcon(hIcon);

	return res;
}
// ENDE



//
// CLockRedrawの定義
//
CLockRedraw::CLockRedraw(HWND hWnd)
	: m_wnd(hWnd)
{
	if (m_wnd.m_hWnd) {
		m_wnd.SetRedraw(FALSE);
	}
}



CLockRedraw::~CLockRedraw()
{
	if (m_wnd.m_hWnd) {
		m_wnd.SetRedraw(TRUE);
		m_wnd.Invalidate();
		m_wnd.UpdateWindow();
	}
}



//
// CLockRedrawMDIClientの定義
//
CLockRedrawMDIClient::CLockRedrawMDIClient(HWND hWnd)
	: m_wndMDIClient(hWnd)
{
	m_wndMDIClient.ShowWindow(SW_HIDE);
}



CLockRedrawMDIClient::~CLockRedrawMDIClient()
{
	m_wndMDIClient.ShowWindow(SW_SHOW);
}



bool MtlIsFamily(HWND hWnd1, HWND hWnd2)
{
	if (hWnd1 == NULL || hWnd2 == NULL)
		return false;

	if (hWnd1 == hWnd2)
		return true;

	CWindow 	wnd1(hWnd1);
	CWindow 	wnd2(hWnd2);

	return wnd1.GetTopLevelParent() == wnd2.GetTopLevelParent();
}



//+++ r13testのものに戻す (渡されるnCmdShowの判定を間違ってたからみもあって)
void MtlSetForegroundWindow(HWND hWnd)
{
	::SetWindowPos(hWnd, HWND_TOPMOST  , 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE);
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE);
	::SetForegroundWindow(hWnd);						//ウインドウにフォーカスを移す
}



bool MtlIsApplicationActive(HWND hWnd)
{
	ATLASSERT( ::IsWindow(hWnd) );

	CWindow 	wndFrame = CWindow(hWnd).GetTopLevelParent();

	CVersional<WINDOWPLACEMENT> 	wndpl;
	wndFrame.GetWindowPlacement(&wndpl);

	if (wndpl.showCmd == SW_SHOWMINIMIZED)
		return false;

	CWindow 	wndFore 	  = ::GetForegroundWindow();
	if (wndFore.m_hWnd == NULL)
		return false;

	CWindow 	wndForeParent = wndFore.GetTopLevelParent();
	return wndFrame.m_hWnd == wndForeParent.m_hWnd;
}



//
//CCriticalIdleTimerの定義
//
void CCriticalIdleTimer::InstallCriticalIdleTimer(HWND hWndMainFrame, WORD wCmdID)
{
	s_hWndMainFrame = hWndMainFrame;
	s_wCmdID		= wCmdID;

	if (s_nTimerID == 0)
		s_nTimerID = ::SetTimer(NULL, 0, 800, FocusCheckerTimerProc);
}



void CCriticalIdleTimer::UninstallCriticalIdleTimer()
{
	if (s_nTimerID != 0) {
		::KillTimer(NULL, s_nTimerID);
		s_nTimerID = 0;
	}
}



VOID CALLBACK CCriticalIdleTimer::FocusCheckerTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	FocusChecker();
}



void CCriticalIdleTimer::FocusChecker()
{
	MtlSendCommand(s_hWndMainFrame, s_wCmdID);
}



UINT_PTR	CCriticalIdleTimer::s_nTimerID		= 0;
HWND		CCriticalIdleTimer::s_hWndMainFrame = NULL;
WORD		CCriticalIdleTimer::s_wCmdID		= 0;




////////////////////////////////////////////////////////////////////////////
} // namespace MTL;
