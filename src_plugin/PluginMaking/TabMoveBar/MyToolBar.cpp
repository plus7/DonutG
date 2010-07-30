// MyToolBar.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "TabMoveBar.h"
#include "MyToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//+++ 32/64ビット版別のプラグインフォルダ
#ifdef WIN64
#define PLUGIN_DIR		"plugin64\\"
#else
#define PLUGIN_DIR		"plugin32\\"
#endif



/////////////////////////////////////////////////////////////////////////////
// CMyToolBar

CMyToolBar::CMyToolBar()
{
}


CMyToolBar::~CMyToolBar()
{
}


BEGIN_MESSAGE_MAP(CMyToolBar, CToolBarCtrl)
	//{{AFX_MSG_MAP(CMyToolBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMyToolBar メッセージ ハンドラ



int CMyToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBarCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	//代替親ウィンドウを設定
	//ツールバーの通知メッセージを受けるだけの非表示ウィンドウ
	m_wnd.Create(NULL, NULL, WS_CHILD, CRect(0, 0, 1, 1), this, MY_WINDOW);
	SetOwner(&m_wnd);

	// イメージリストの作成
	CBitmap  bitmap;
	HBITMAP  hBmp;
	m_ilist.Create(16, 16, ILC_COLOR24 | ILC_MASK, TOOLBTNCOUNT, 1);
	//+++ plugin32/64フォルダをまずみる
	hBmp = (HBITMAP) ::LoadImage(AfxGetResourceHandle(), PLUGIN_DIR "TabMoveBarRes\\toolbar.bmp",
									 IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE );
	if (hBmp == NULL) {	//+++ なければ、plugin/ をみる
		hBmp = (HBITMAP) ::LoadImage(AfxGetResourceHandle(), "Plugin\\TabMoveBarRes\\toolbar.bmp",
									 IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE );
	}
	if (hBmp == NULL) {
		bitmap.LoadBitmap(IDB_BITMAP);
		m_ilist.Add( &bitmap, RGB(255, 0, 255) );
	} else {
		m_ilist.Add( bitmap.FromHandle(hBmp), RGB(255, 0, 255) );
	}
	SetImageList(&m_ilist);

	//ボタンの登録
	TBBUTTON 	bt[TOOLBTNCOUNT];

	for (int i = 0; i < TOOLBTNCOUNT; i++) {
		bt[i].iBitmap	= i;
		bt[i].idCommand = MY_TOOLBTN1 + i;
		bt[i].fsStyle	= BTNS_BUTTON;
		bt[i].fsState	= TBSTATE_ENABLED;
		bt[i].dwData	= 0;
		bt[i].iString	= NULL;
	}
	AddButtons(TOOLBTNCOUNT, bt);

	return 0;
}
