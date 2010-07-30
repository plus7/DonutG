// Memo.cpp : DLL アプリケーション用のエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "PluginInfo.h"
#include "Memo.h"


CMemo      	g_Memo;
CAppModule 	_Module;



BOOL APIENTRY DllMain( HANDLE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID /*lpReserved*/
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		//DLLがロードされた
		HRESULT hRes = ::CoInitialize(NULL);

		if ( FAILED(hRes) )
			return FALSE;

		hRes = _Module.Init(NULL, (HINSTANCE) hModule);

		if ( FAILED(hRes) ) {
			::CoUninitialize();
			return FALSE;
		}

		return TRUE;
	} else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		//DLLがアンロードされた
		if (::IsWindow(g_Memo.m_hWnd))	//+++ どうもここにくるまでに窓なくなってる場合があるようなんでチェック追加
			g_Memo.DestroyWindow(); //ウィンドウを削除する
		_Module.Term();
		::CoUninitialize();
		return TRUE;
	}

	return TRUE;
}


//ウィンドウを作成してそのハンドルを返す
//そのウィンドウがツールバーとしてunDonut内に取り込まれる
extern "C" HWND WINAPI CreateExplorerPane(HWND hWndParent, UINT nID)
{
	//ウィンドウの初期化
	RECT rc = { 0, 0, 1, 1 };   //サイズはunDonut側によって後で調整される

	return g_Memo.Create(hWndParent, rc, nID);
}


//プラグインの情報をunDonut側に渡す関数
//これをエクスポートしないとプラグインと認識されない
//構造体や定数についてはPluginInfo.hに記述あり
extern "C" void WINAPI GetPluginInfo(PLUGININFO *pstPluginInfo)
{
	memset( pstPluginInfo, 0, sizeof (PLUGININFO) );
	pstPluginInfo->type = PLT_EXPLORERBAR;

	strcpy( pstPluginInfo->name			, "メモ帳-1" );
	//strcpy( pstPluginInfo->version	, "1.0.0" );
	strcpy( pstPluginInfo->version		, "1.0.0 +mod.0.2" );
	//strcpy( pstPluginInfo->versionDate, "2004/07/06" );
	strcpy( pstPluginInfo->versionDate	, "2008/08/10" );
	//strcpy( pstPluginInfo->authorName	, "ミニット" );
	strcpy( pstPluginInfo->authorName	, "ミニット (微修正 +mod)" );
	strcpy( pstPluginInfo->authorUrl	, "http://www5.ocn.ne.jp/minute/" );
	//strcpy( pstPluginInfo->authorEmail, "sakurabe@violin.ocn.ne.jp" );		//+++ このアドレスはすでにない?はず
	strcpy( pstPluginInfo->authorEmail	, "" );									//+++
	strcpy( pstPluginInfo->comment		, "メモ帳プラグイン \r\n" );
}



//DonutPからキー入力に関するメッセージが送られてくる
extern "C" BOOL WINAPI PreTranslateMessage(MSG *pMsg)
{
	//DLLにはメッセージループがないので自分で必要なところに配送する
	LONG_PTR lRet  = 0;
	//BOOL	 bPass = TRUE;

	//ウィンドウのメッセージを処理する
	BOOL     bRet  = g_Memo.ProcessWindowMessage(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam, lRet, 0);

	if (!bRet) {
		UINT	msg  = pMsg->message;
		//int	vKey = pMsg->wParam;

		if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
			if (g_Memo.m_hWnd && pMsg->hwnd == g_Memo.m_Edit.m_hWnd) {
				//エディットコントロールのキー入力を処理
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
			}

			if (g_Memo.m_hWnd)	//+++ NULLチェック追加
				bRet = g_Memo.IsDialogMessage(pMsg);
		}
	}

	return bRet;
}



//プラグイン定義のメニューを表示する
//本体エクスプローラバーの「メニュー」をドロップダウン時に呼ばれる
extern "C" void WINAPI ShowExplorerMenu(int x, int y)
{
	HMENU hMenu    = ::LoadMenu( _Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU) );
	if ( !::IsMenu(hMenu) )
		return;

	HMENU hMenuSub = ::GetSubMenu(hMenu, 0);
	if ( !::IsMenu(hMenuSub) )
		return;

	::TrackPopupMenu(hMenuSub, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, x, y, 0, g_Memo.m_hWnd, NULL);

	::DestroyMenu(hMenu);
}
