// TmpFavGroup.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "TmpFavGroup.h"
#include "BaseWindow.h"
#include "PluginInfo.h"
#include "Misc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	メモ!
//
//		この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされた
//		どの関数も関数の最初に追加される AFX_MANAGE_STATE
//		マクロを含んでいなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな
//		らないことを意味します、コンストラクタが MFC
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

/////////////////////////////////////////////////////////////////////////////
// CTmpFavGroupApp

BEGIN_MESSAGE_MAP(CTmpFavGroupApp, CWinApp)
	//{{AFX_MSG_MAP(CTmpFavGroupApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CTmpFavGroupApp の構築

CTmpFavGroupApp::CTmpFavGroupApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance の中の重要な初期化処理をすべて記述してください。
}



/////////////////////////////////////////////////////////////////////////////

//グローバル・オブジェクト
CTmpFavGroupApp theApp;		// 唯一の CTmpFavGroupApp オブジェクト
CBaseWindow 	g_Base;		// プラグインの最上位ウィンドウ



int CTmpFavGroupApp::ExitInstance()
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	// ウィンドウを削除する
	g_Base.DestroyWindow();
	return CWinApp::ExitInstance();
}



//エクスポートする関数
//これらはDonutPから呼ばれる

//ウィンドウが必要になった際に呼び出される
//ウィンドウを作成してそのハンドルを返す
//そのウィンドウがエクスプローラバーの一部としてDonutP内に取り込まれる
extern "C" HWND WINAPI CreateExplorerPane(HWND hWndParent, UINT nID)
{
	//エクスプローラバー・ペインを生成
	RECT rc = { 0,0,1,1 }; //大きさはunDonut側が勝手に調整するので適当でいい
	CWnd wnd; wnd.Attach(hWndParent);

	LPCTSTR lpstrClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,0,0,0);
	g_Base.Create(lpstrClassName,_T(""),WS_CHILD|WS_VISIBLE,rc,&wnd,nID);
	wnd.Detach();

	return g_Base.m_hWnd;
}



//プラグインの情報が必要になったときに呼び出される
//これをエクスポートしないとプラグインと認識されない
//構造体や定数についてはPluginInfo.hに記述あり
extern "C" void WINAPI GetPluginInfo(PLUGININFO* pstPluginInfo)
{
	memset( pstPluginInfo, 0, sizeof(PLUGININFO) );
	pstPluginInfo->type = PLT_EXPLORERBAR; //エクスプローラバー・プラグインを作る

	strcpy( pstPluginInfo->name			, "スナップショット" );
	strcpy( pstPluginInfo->version		, "1.0 +mod.5" );
	//strcpy( pstPluginInfo->versionDate, "2004.7.31" );
	strcpy( pstPluginInfo->versionDate	, "2009.01.11" );
	strcpy( pstPluginInfo->authorName	, "ミニット. modified by +mod" );
	strcpy( pstPluginInfo->authorUrl	, "-----" );
	strcpy( pstPluginInfo->authorEmail	, "-----" );
	strcpy( pstPluginInfo->comment		, "スナップショットプラグイン \r\n"
									 	  "お気に入りグループの一時ファイルを管理するプラグインです。\r\n"
										  "unDonut+/+mod と本家unDonutとでデータに非互換があるため、\r\n"
										  "+mod用に再コンパイルしたモノです。\r\n"
										  "(+mod.1.34でのunicode対策(utf8→%HH化文字列)に対応)"
									      );
}



//キー入力に関するメッセージがあるときに呼び出される
//MFCにおいてはtheAppの同名関数を呼び出すことで問題なく処理される
extern "C" void WINAPI PreTranslateMessage(MSG* pMsg)
{
	theApp.PreTranslateMessage(pMsg);
}



//プラグイン定義のメニューを表示する
//本体エクスプローラバーの「メニュー」をドロップダウン時に呼ばれる
extern "C" void WINAPI ShowExplorerMenu(int x, int y)
{
	HMENU hMenu = g_Base.LoadPluginMenu();
	if (!hMenu)
		return;

	HMENU hMenuSub = ::GetSubMenu(hMenu,0);
	if (!::IsMenu(hMenuSub))
		return;

	::TrackPopupMenu(hMenuSub,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, x, y, 0, g_Base.m_hWnd, NULL);

	::DestroyMenu(hMenu);
}



//===========================================================================

//+++ exe(dll)のパスを返す
const CString 	Misc::GetExeFileName()
{
	TCHAR	buf[MAX_PATH+4];
	buf[0] = 0;
	::GetModuleFileName(theApp.m_hInstance, buf, MAX_PATH);
	return CString(buf);
}

