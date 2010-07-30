// MyWindow.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "TabMoveBar.h"
#include "MyWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMyWindow

CMyWindow::CMyWindow()
{
}



CMyWindow::~CMyWindow()
{
}



BEGIN_MESSAGE_MAP(CMyWindow, CWnd)
	//{{AFX_MSG_MAP(CMyWindow)
	// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMyWindow メッセージ ハンドラ


BOOL CMyWindow::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	//ツールチップ表示の通知に対する処理
	LPTOOLTIPTEXT pTTT = (LPTOOLTIPTEXT) lParam;

	if (pTTT->hdr.code == TTN_NEEDTEXT) {
		pTTT->hinst = AfxGetResourceHandle();

		switch (pTTT->hdr.idFrom) {
		case MY_TOOLBTN1: pTTT->lpszText = MAKEINTRESOURCE(IDS_STRING1);	break;
		case MY_TOOLBTN2: pTTT->lpszText = MAKEINTRESOURCE(IDS_STRING2);	break;
		case MY_TOOLBTN3: pTTT->lpszText = MAKEINTRESOURCE(IDS_STRING3);	break;
		case MY_TOOLBTN4: pTTT->lpszText = MAKEINTRESOURCE(IDS_STRING4);	break;
		}
	}
	return CWnd::OnNotify(wParam, lParam, pResult);
}



BOOL CMyWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) ) {
	case MY_TOOLBTN1:	m_tab.MoveTabLeftEnd();		break;
	case MY_TOOLBTN2:	m_tab.MoveTabLeft();		break;
	case MY_TOOLBTN3:	m_tab.MoveTabRight();		break;
	case MY_TOOLBTN4:	m_tab.MoveTabRightEnd();	break;
	}
	return CWnd::OnCommand(wParam, lParam);
}
