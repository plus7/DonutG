/**
 *	@file	DebugWindow.cpp
 *	@brief	とりあえず的な適当デバッグ用ダイアログを生成するクラス
 */
#include "stdafx.h"
#include "DebugWindow.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifndef NDEBUG

CDebugWindow *		CDebugWindow::s_pThis = NULL;



void CDebugWindow::Create()
{
	if (g_cnt_b_use_debug_window) {
		RECT rc = { 0, 0, 300, 600 };
		baseclass::Create(m_hWnd, rc, _T("DebugWindow"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0);
		s_pThis = this;

		::DeleteFile(_T("log.txt"));
	}
}



void CDebugWindow::Destroy()
{
	if (g_cnt_b_use_debug_window) {
		if ( IsWindow() )
			baseclass::DestroyWindow();

		s_pThis = NULL;
	}
}



void CDebugWindow::OutPutString(CString strText, bool bReturn)
{
	if (!s_pThis)
		return;

	if ( !s_pThis->m_wndEdit.IsWindow() )
		return;

	if (!g_cnt_b_use_debug_window)
		return;

	FILE *fp = fopen("log.txt", "a");

	if (fp) {
		fprintf(fp, "%s\n", strText);
		fclose(fp);
	}

	if (bReturn)
		strText += _T("\r\n");

	s_pThis->m_wndEdit.ReplaceSel(strText);
}



LRESULT CDebugWindow::OnCreate(LPCREATESTRUCT lpcs)
{
	SetMsgHandled(FALSE);

	CRect rc;
	GetClientRect(&rc);
	::InflateRect(&rc, -4, -4);
	m_wndEdit.Create(m_hWnd, rc, NULL, ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL);
	return 0;
}



LRESULT CDebugWindow::OnSize(UINT, CSize size)
{
	SetMsgHandled(FALSE);

	CRect rc;
	GetClientRect(&rc);
	::InflateRect(&rc, -4, -4);
	m_wndEdit.MoveWindow(rc);
	return 0;
}


#endif
