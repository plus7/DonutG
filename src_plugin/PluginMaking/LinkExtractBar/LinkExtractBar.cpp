// LinkExtractBar.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "LinkExtractBar.h"
#include "Misc.h"			//+++

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CLinkExtractBarApp

BEGIN_MESSAGE_MAP(CLinkExtractBarApp, CWinApp)
	//{{AFX_MSG_MAP(CLinkExtractBarApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CLinkExtractBarApp の構築
//#include 			"MyToolBar.h"
#include 			"BaseWindow.h"
#include 			"PluginInfo.h"

CLinkExtractBarApp 	theApp;
//CMyToolBar		m_wndToolBar;
CBaseWindow 		g_wndBase;

CLinkExtractBarApp::CLinkExtractBarApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance の中の重要な初期化処理をすべて記述してください。
}



int CLinkExtractBarApp::ExitInstance()
{
	g_wndBase.DestroyWindow();
	//m_wndToolBar.DestroyWindow();
	return CWinApp::ExitInstance();
}



//===========================================================================

//+++ exe(dll)のパスを返す
CString 	Misc::GetExeFileName()
{
	TCHAR	buf[MAX_PATH+4];
	buf[0] = 0;
	::GetModuleFileName(theApp.m_hInstance, buf, MAX_PATH);
	return CString(buf);
}



//===========================================================================
// interface

extern "C" HWND WINAPI CreateToolBar(HWND hWndParent, UINT nID)
{
	g_wndBase.Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,CRect(0,0,25,24),CWnd::FromHandle(hWndParent),ID_BASEWINDOW);
	return g_wndBase.m_hWnd;

	//m_wndToolBar.Create(TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|CCS_TOP|WS_CHILD|WS_VISIBLE|
	//	CCS_NORESIZE|CCS_NODIVIDER|CCS_NOPARENTALIGN,
	//	CRect(0,0,100,24),CWnd::FromHandle(hWndParent),nID);

	//return m_wndToolBar.m_hWnd;
}



extern "C" void WINAPI GetPluginInfo(PLUGININFO* pstPluginInfo)
{
	memset( pstPluginInfo, 0, sizeof(PLUGININFO) );
	pstPluginInfo->type = PLT_TOOLBAR;

  #if 1	//+++
	strcpy( pstPluginInfo->name			, "LinkExtractBar" );
	strcpy( pstPluginInfo->version		, "0.3.0 mod.3" );
	strcpy( pstPluginInfo->versionDate	, "2008.07.25" );
	strcpy( pstPluginInfo->authorName	, "ミニット. modified by +mod" );
	strcpy( pstPluginInfo->authorUrl	, "-----" );
	strcpy( pstPluginInfo->authorEmail	, "-----" );
	strcpy( pstPluginInfo->comment		, "リンク抽出機能を備えたツールバー \r\n" );
  #else
	strcpy( pstPluginInfo->name, "LinkExtractBar" );
	strcpy( pstPluginInfo->version, "0.3.0" );
	strcpy( pstPluginInfo->versionDate, "2003.11.02" );
	strcpy( pstPluginInfo->authorName, "ミニット" );
	strcpy( pstPluginInfo->authorUrl, "-----" );
	strcpy( pstPluginInfo->authorEmail, "-----" );
	strcpy( pstPluginInfo->comment, "リンク抽出機能を備えたツールバー \r\n" );
  #endif
}



extern "C" void WINAPI PreTranslateMessage(MSG* pMsg)
{
	theApp.PreTranslateMessage(pMsg);
}
