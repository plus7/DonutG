// BaseWindow.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "linkextractbar.h"
#include "BaseWindow.h"
#include "MyDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef  THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "Misc.h"		//+++



/////////////////////////////////////////////////////////////////////////////
// CBaseWindow


CBaseWindow::CBaseWindow()
{
}



CBaseWindow::~CBaseWindow()
{
}



BEGIN_MESSAGE_MAP(CBaseWindow, CWnd)
	//{{AFX_MSG_MAP(CBaseWindow)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EXTRACT_LINK      , OnExtractLink)
	ON_COMMAND(ID_SEND_TO_DOWNLOADER, OnSendToDownloader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CBaseWindow メッセージ ハンドラ

int CBaseWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: この位置に固有の作成用コードを追加してください

	//ツールバーを作成し構築する
	m_wndToolBar.Create(TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP | WS_CHILD | WS_VISIBLE | CCS_NORESIZE | CCS_NODIVIDER | CCS_NOPARENTALIGN
						, CRect(0, 0, 100, 24), this, ID_TOOLBAR);

  #if 1	//+++
	CString fname		= Misc::GetFileNameNoExt(Misc::GetExeFileName());
	fname				= fname + _T('\\') + Misc::GetFileBaseName(fname) + _T(".ini");
	bool 	bS_Button   = ::GetPrivateProfileInt(_T("etc"), _T("S_Button"), 0, fname) != 0;
  #endif

	// イメージリストの作成
	CBitmap bitmap;
	HBITMAP hBmp;
	int  	nToolBtnCount = bS_Button ? 2 : 1;
	m_imgList.Create(16, 16, ILC_COLOR24 | ILC_MASK, TOOLBTNCOUNT, 1);


  #if 1	//+++ dllのあるフォルダ基準に変更.
	fname		= Misc::GetDirName(fname) + _T("\\toolbar.bmp");
	hBmp		= (HBITMAP) ::LoadImage(AfxGetResourceHandle(), fname, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE );
  #else
	//+++ まずは32/64別のフォルダを探す.
	hBmp		= (HBITMAP) ::LoadImage(AfxGetResourceHandle(), PLUGIN_DIR "LinkExtractBarRes\\toolbar.bmp",
										 IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE );
	if (hBmp == NULL) {	//+++　なければpluginフォルダから
		hBmp	= (HBITMAP) ::LoadImage(AfxGetResourceHandle(), "Plugin\\LinkExtractBarRes\\toolbar.bmp",
										 IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE );
	}
  #endif
	if (hBmp == NULL) {	//+++ メモ:それでもなければ内蔵リソースのを使う
		bitmap.LoadBitmap(IDB_BITMAP);
		m_imgList.Add( &bitmap, RGB(255, 0, 255) );
	} else {
		m_imgList.Add( bitmap.FromHandle(hBmp), RGB(255, 0, 255) );
	}
	m_wndToolBar.SetImageList(&m_imgList);

	//ボタンの登録
	TBBUTTON	bt[2]	= { {0}, {0} };
	bt[0].iBitmap		= 0;
	bt[0].idCommand		= ID_EXTRACT_LINK;
	bt[0].fsStyle		= BTNS_BUTTON;
	bt[0].fsState		= TBSTATE_ENABLED ;
	bt[0].dwData		= 0;
	bt[0].iString		= NULL;

	if (bS_Button) {
		bt[1].iBitmap	= 1;
		bt[1].idCommand	= ID_SEND_TO_DOWNLOADER;
		bt[1].fsStyle	= BTNS_BUTTON;
		bt[1].fsState	= TBSTATE_ENABLED ;
		bt[1].dwData	= 0;
		bt[1].iString	= NULL;
	}

	m_wndToolBar.AddButtons(nToolBtnCount, bt);
	return 0;
}


BOOL CBaseWindow::OnEraseBkgnd(CDC *pDC)
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	CWnd *	pwndParent = GetParent();
	CPoint	pt(0, 0);

	MapWindowPoints(pwndParent, &pt, 1);
	pt = ::OffsetWindowOrgEx( (HDC) pDC->m_hDC, pt.x, pt.y, NULL );
	LRESULT lResult    = ::SendMessage(pwndParent->m_hWnd, WM_ERASEBKGND, (WPARAM) pDC->m_hDC, 0L);
	::SetWindowOrgEx( (HDC) pDC->m_hDC, pt.x, pt.y, NULL );
	return BOOL(lResult);

	return CWnd::OnEraseBkgnd(pDC);
}


void CBaseWindow::OnExtractLink()
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
  #if 1	//+++
	std::auto_ptr<CMyDialog>	dlg ( new CMyDialog(this) );
	dlg->DoModal();
  #else
	CMyDialog	dlg(this);
	dlg.DoModal();
  #endif
}


BOOL CBaseWindow::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	LPTOOLTIPTEXT pTTT = (LPTOOLTIPTEXT) lParam;

	if (pTTT->hdr.code == TTN_NEEDTEXT) {
		pTTT->hinst = AfxGetResourceHandle();

		switch (pTTT->hdr.idFrom) {
		case ID_EXTRACT_LINK      : pTTT->lpszText = MAKEINTRESOURCE(IDS_EXTRACT_LINK);			break;
		case ID_SEND_TO_DOWNLOADER: pTTT->lpszText = MAKEINTRESOURCE(IDS_SEND_TO_DOWNLOADER);	break;
		}
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}



void CBaseWindow::OnSendToDownloader()
{
	std::auto_ptr<CMyDialog>	dlg ( new CMyDialog(this) );
	dlg->autoSendToDownloader();
	dlg->DoModal();
}
