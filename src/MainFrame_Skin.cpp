/**
 *	@file	MainFrame_Skin.cpp
 *	@brief	CMainFrame の スキン関係の処理.
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




// ===========================================================================
// スキン関係

void CMainFrame::InitSkin()
{
	setMainFrameBg(CSkinOption::s_nMainFrameBgColor);			//+++ メインフレームのBg設定.
	initCurrentIcon();											//+++ アイコン
	m_CmdBar.setMenuBarStyle(m_hWndToolBar, false); 			//+++ メニュー (FEVATWHの短名にするか否か)

	m_MDITab.SetDrawStyle(CSkinOption::s_nTabStyle);			//タブ
	m_ReBar.RefreshSkinState(); 								//ReBar
	m_wndStatusBar.ReloadSkin( CSkinOption::s_nStatusStyle		//ステータスバー
							 , CSkinOption::s_nStatusTextColor
							 , CSkinOption::s_nStatusBackColor);

	setMainFrameCaptionSw(CSkinOption::s_nMainFrameCaption);	//+++ メインフレームのキャプションの有無.
}


HRESULT CMainFrame::OnSkinChange()
{
	InvalidateRect(NULL, TRUE);

	//メッセージをブロードキャストするべきか
	CSkinOption::GetProfile();

	setMainFrameBg(CSkinOption::s_nMainFrameBgColor);			//+++ メインフレームのBg設定.

	initCurrentIcon();											//+++ アイコン
	m_CmdBar.setMenuBarStyle(m_hWndToolBar, true);				//+++ メニュー
	m_ReBar.RefreshSkinState(); 								//ReBar
	m_CmdBar.InvalidateRect(NULL, TRUE);						//メニューバー
	m_MDITab.ReloadSkin(CSkinOption::s_nTabStyle);				//タブ
	m_ToolBar.ReloadSkin(); 									//ツールバー
	m_AddressBar.ReloadSkin(CSkinOption::s_nComboStyle);		//アドレスバー
	m_SearchBar.ReloadSkin(CSkinOption::s_nComboStyle); 		//検索バー
	m_LinkBar.InvalidateRect(NULL, TRUE);						//リンクバー
	m_ExplorerBar.ReloadSkin(); 								//エクスプローラバー
	m_ExplorerBar.m_PanelBar.ReloadSkin();						//パネルバー
	m_ExplorerBar.m_PluginBar.ReloadSkin(); 					//プラグインバー
	m_wndStatusBar.ReloadSkin(CSkinOption::s_nStatusStyle		//ステータスバー
							, CSkinOption::s_nStatusTextColor
							, CSkinOption::s_nStatusBackColor);

	setMainFrameCaptionSw(CSkinOption::s_nMainFrameCaption);	//+++ キャプション

	//リフレッシュ
	InvalidateRect(NULL, TRUE);

	return 0;
}


#if 1 //+++	メインフレームのCaption on/off
void CMainFrame::setMainFrameCaptionSw(int sw)
{
	if (sw == 0) {	//+++ キャプションを外す場合
		ModifyStyle(WS_CAPTION, 0);
	} else {
		ModifyStyle(0, WS_CAPTION); 			//+++ キャプションをつける場合.
	}
	m_mcCmdBar.SetExMode(sw ? 0/*有*/ : m_hWnd/*無*/);

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
}
#endif


///+++ 現在のスキンのアイコンを(再)設定.
///    ※ HICON の開放ってしなくてよい? 駄目でリークしてる?...winのリソース管理ってようわかんね....
void CMainFrame::initCurrentIcon()
{
	//+++ xp ビジュアルスタイルを一時的にoff
  #if 1	//+++ uxtheme.dll の関数の呼び出し方を変更.
	UxTheme_Wrap::SetWindowTheme(m_hWnd, L" ", L" ");
  #else
	CTheme		theme;
	theme.SetWindowTheme(m_hWnd, L" ", L" ");
  #endif

	/*static*/ HICON hIcon	= 0;
	//if (hIcon)
	//	::CloseHandle(hIcon);
	//hIcon 		= 0;
	CString strDir	= _GetSkinDir();
	m_strIcon		= strDir + "MainFrameBig.ico";
	if (Misc::IsExistFile(m_strIcon) == 0)
		m_strIcon	= strDir + "icon.ico";
	if (Misc::IsExistFile(m_strIcon))
		hIcon = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), m_strIcon, IMAGE_ICON, 32, 32, LR_SHARED|LR_LOADFROMFILE );
	if (hIcon == 0) {
		m_strIcon.Empty();
		hIcon = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	}
	if (hIcon)
		::SetClassLongPtr(m_hWnd, GCLP_HICON  , (LONG_PTR)hIcon );
	//::CloseHandle(hIcon);

	/*static*/ HICON hIconSm	= 0;
	//if (hIconSm)
	//	::CloseHandle(hIconSm);
	//hIconSm			= 0;
	m_strIconSm 		= strDir + "MainFrameSmall.ico";
	if (Misc::IsExistFile(m_strIconSm) == 0)
		m_strIconSm 	= strDir + "icon.ico";
	if (Misc::IsExistFile(m_strIconSm))
		hIconSm 	= (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), m_strIconSm, IMAGE_ICON, 16, 16, LR_SHARED|LR_LOADFROMFILE);
	if (hIconSm == 0) {
		m_strIconSm.Empty();
		hIconSm   = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	}
	if (hIconSm)
		::SetClassLongPtr(m_hWnd, GCLP_HICONSM, (LONG_PTR)hIconSm );
	//::CloseHandle(hIconSm);

	//+++ XPのビジュアルスタイルに戻す...これはいらないかも
  #if 1	//+++ uxtheme.dll の関数の呼び出し方を変更.
	UxTheme_Wrap::SetWindowTheme(m_hWnd, 0, 0);
  #else
	theme.SetWindowTheme(m_hWnd, 0, 0);
  #endif
}


//+++ メインフレームのBg設定.
void CMainFrame::setMainFrameBg(int bgColor)
{
	m_nBgColor = bgColor;

	CString 	strPath = _GetSkinDir() + _T("bg.bmp");
	m_bmpBg.Attach( AtlLoadBitmapImage(strPath.GetBuffer(0), LR_LOADFROMFILE) );
}


///+++ bg描画
bool CMainFrame::drawMainFrameBg(HDC hDC)
{
  #if 1 //+++ bgタイルのみ
	if (m_bmpBg.m_hBitmap) {		// bg画像を敷き詰めて表示
		DrawBmpBg_Tile(hDC);
		return 1;
	} else if (m_nBgColor >= 0) {	// 色指定があれば、その色でべた塗り
		HBRUSH	hBrushBg = CreateSolidBrush(COLORREF(m_nBgColor));
		RECT	rect;
		GetClientRect(&rect);
		::FillRect( hDC, &rect, hBrushBg );
		DeleteObject(hBrushBg);
		return 1;
	}
	return 0;
  #else //+++ 背景色とbgが同時に表示される必要があるとき
	if (m_nBgColor < 0 || m_bmpBg.m_hBitmap == 0)
		return 0;

	if (m_nBgColor >= 0) {
		HBRUSH	hBrushBg = CreateSolidBrush(COLORREF(m_nBgColor));
		RECT	rect;
		GetClientRect(&rect);
		::FillRect( hDC, &rect, hBrushBg );
		DeleteObject(hBrushBg);
	}
	if (m_bmpBg.m_hBitmap) {
		//if (m_nBgStyle == SKIN_BG_STYLE_TILE)
		DrawBmpBg_Tile(hDC);
		//else if (m_nBGStyle == SKN_BG_STYLE_STRETCH)
		//DrawBmpBg_Stretch(hDC);
	}
	return 1;
  #endif
}


//+++ bg描画(敷き詰めるタイプ)
void CMainFrame::DrawBmpBg_Tile(HDC hDC)
{
	CRect	rc;
	GetClientRect(&rc);

	CDC 	dcSrc;
	dcSrc.CreateCompatibleDC(hDC);

	HBITMAP hOldbmpSrc	= dcSrc.SelectBitmap(m_bmpBg.m_hBitmap);
	SIZE	size;
	m_bmpBg.GetSize(size);
	DWORD	srcW = size.cx;
	DWORD	srcH = size.cy;
	DWORD	dstW = rc.Width();
	DWORD	dstH = rc.Height();
	for (unsigned y = 0; y < dstH; y += srcH) {
		for (unsigned x = 0; x < dstW; x += srcW) {
			::BitBlt(hDC, x, y, srcW, srcH, dcSrc, 0, 0, SRCCOPY);
		}
	}

	dcSrc.SelectBitmap(hOldbmpSrc);
}


#if 0 //+++ 画面の更新がようわからないので、拡縮は止め
//+++ 元画像を拡大して表示.
void CMainFrame::DrawBmpBg_Stretch(HDC hDC)
{
	CRect	rc;
	GetClientRect(&rc);
  #if 0
	InvalidateRect(rc, TRUE);
	UpdateWindow();
  #endif

	CDC 	dcSrc;
	dcSrc.CreateCompatibleDC(hDC);

	HBITMAP hOldbmpSrc	= dcSrc.SelectBitmap(m_bmpBg.m_hBitmap);
	SIZE	size;
	m_bmpBg.GetSize(size);
	DWORD	srcW = size.cx;
	DWORD	srcH = size.cy;
	double	srcR = double(srcW) / double(srcH);
	DWORD	dstW = rc.Width();
	DWORD	dstH = rc.Height();
	double	dstR = double(dstW) / double(dstH);
	if (dstR > srcR) {			//+++ 描画範囲のほうが、元画像よりも横長
		srcH = DWORD( srcH * srcR / dstR );
	} else if (dstR < srcR) {	//+++ 描画範囲のほうが、元画像よりも縦長
		srcW = DWORD( srcW * dstR / srcR );
	}
	::StretchBlt(hDC, 0, 0, dstW, dstH, dcSrc , 0, 0, srcW, srcH, SRCCOPY);
	dcSrc.SelectBitmap(hOldbmpSrc);
	//ValidateRect(NULL);
}
#endif


//public:
#if 0 //+++ あとで
HICON	CMainFrame::LoadIcon4AboutDialog()
{
	int 	w = ::GetSystemMetrics(SM_CXICON);
	int 	h = ::GetSystemMetrics(SM_CYICON);
	HICON	hIcon = 0;
  #if 1
	if (IsExistFile(m_strIcon))
		hIcon = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), m_strIcon, IMAGE_ICON, w, h, LR_SHARED|LR_LOADFROMFILE);
	if (hIcon == 0)
  #endif
		hIcon = (HICON)::LoadImage(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, w, h, LR_DEFAULTCOLOR );
	return hIcon;
}
#endif

