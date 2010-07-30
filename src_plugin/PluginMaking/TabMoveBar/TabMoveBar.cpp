// TabMoveBar.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "TabMoveBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
/*
	グローバルな変数及び関数を定義・実装するモジュール
*/
//
#include "PluginInfo.h"
#include "MyToolBar.h"
#include "MyWindow.h"

CTabMoveBarApp theApp;		// 唯一の CTabMoveBarApp オブジェクト
CMyToolBar  m_wndToolBar;	// ツールバーオブジェクト






// CTabMoveBarApp

BEGIN_MESSAGE_MAP(CTabMoveBarApp, CWinApp)
	//{{AFX_MSG_MAP(CTabMoveBarApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTabMoveBarApp::CTabMoveBarApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance の中の重要な初期化処理をすべて記述してください。
}

//終了処理・ここでウィンドウなどを削除しておく
int CTabMoveBarApp::ExitInstance() 
{
	m_wndToolBar.DestroyWindow();
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////



//エクスポートする関数
//これらはDonutPから呼ばれる


//ウィンドウを作成してそのハンドルを返す
//そのウィンドウがツールバーとしてDonutP内に取り込まれる
extern "C" HWND WINAPI CreateToolBar(HWND hWndParent, UINT nID)
{
	//ツールバーウィンドウを生成
	m_wndToolBar.Create(TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|CCS_TOP|WS_CHILD|WS_VISIBLE|
		CCS_NORESIZE|CCS_NODIVIDER|CCS_NOPARENTALIGN,
		CRect(0,0,100,24),CWnd::FromHandle(hWndParent),nID);

	return m_wndToolBar.m_hWnd;
}


//プラグインの情報をDonutP側に渡す関数
//これをエクスポートしないとプラグインと認識されない
//構造体や定数についてはPluginInfo.hに記述あり
extern "C" void WINAPI GetPluginInfo(PLUGININFO* pstPluginInfo)
{
	memset( pstPluginInfo, 0, sizeof(PLUGININFO) );
	pstPluginInfo->type = PLT_TOOLBAR;

	strcpy( pstPluginInfo->name, "TabMoveBar" );
	strcpy( pstPluginInfo->version, "1.1.1" );
	strcpy( pstPluginInfo->versionDate, "2003.07.24" );
	strcpy( pstPluginInfo->authorName, "ミニット" );
	strcpy( pstPluginInfo->authorUrl, "-----" );
	strcpy( pstPluginInfo->authorEmail, "-----" );
	strcpy( pstPluginInfo->comment, "タブ移動機能専用ツールバー \r\n" );
}

//これを書いておくとクラスのPreTranslateMessage()が利用できるようになる
//無くてもよいがうまく動かなくなることもある
extern "C" void WINAPI PreTranslateMessage(MSG* pMsg)
{
	theApp.PreTranslateMessage(pMsg);
}



