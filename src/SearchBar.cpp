/**
 *	@file	SearchBar.cpp
 *	@brief	検索バー
 *	@note
 *		templateで実装されていた SearchBar.h を普通のclassにして .h, .cpp 化したもの.
 */


#include "stdafx.h"
#include "DonutPFunc.h"
#include "DonutViewOption.h"
#include "MtlDragDrop.h"
#include "ExplorerMenu.h"
#include "HlinkDataObject.h"
#include "ExStyle.h"
#include <atlctrls.h>
//#include "StringEncoder.h"			//+++ 不要化
#include <winnls32.h>
#include "ParseInternetShortcutFile.h"
#include "Donut.h"

#include "SearchBar.h"



#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define ENGINENAME_FOR_NO_SEARCH_INI	_T("search.iniが無いのでGoogleのみ")


//+++	ParseInternetShortcutFile()用に手抜きで自身のポインタを用意...
CDonutSearchBar*	CDonutSearchBar::s_pThis_ = 0;




CDonutSearchBar::CDonutSearchBar()
	: m_wndKeyword(this, 1)
	, m_wndEngine  (this, 2)
	, m_wndKeywordCombo(this, 3)
	, m_hThread(0)				//+++
	, m_cxBtnsBtn(0)			//+++ 旧横幅調整用
	, m_clrMask(0)				//+++
	, m_bActiveWindow(0)		//+++
	, m_hWndKeywordList(0)		//+++
	, m_has(0)					//+++
	, m_nDefDlgH(22)			//+++
	, m_nItemFontH(21)			//+++
	, m_nEngineWidth(0)			//+++
	, m_bHilightSw(0)			//+++
	//, m_nHilightBtnNo(0)		//+++
	, m_bExistManifest(IsExistManifestFile())	//+++
   #ifndef USE_DIET
	, m_dwNoWordButton(0)		//+++
	, m_dwUsePageButton(0)		//+++
	, m_dwTinyWordButton(0)		//+++
   #endif
{
	m_hCursor		  = NULL;

	m_bDragAccept	  = false;
	m_bDragFromItself = false;
	m_bShowToolBar	  = FALSE;
	m_bLoadedToolBar  = FALSE;

	m_bFiltering	  = FALSE;

	m_bUseShortcut	  = FALSE;
	m_bActiveWindow   = FALSE;

	//+++ 1つしかインスタンスが作られない、だろうとして、ParseInternetShortcutFile()用に手抜きな自分を指すポインタ.
	ATLASSERT(s_pThis_ == 0);
	s_pThis_ = this;
}


/// AddressBar側で使うコンボ文字列の設定
void CDonutSearchBar::InitComboBox_for_AddressBarPropertyPage(CComboBox& rCmbCtrl, CComboBox& rCmbShift)
{
	int nCount		   = m_cmbEngine.GetCount();
	for (int i = 0; i < nCount; i++) {
		CString strBuf;
		m_cmbEngine.GetLBText(i, strBuf);
		rCmbCtrl.AddString(strBuf);
		rCmbShift.AddString(strBuf);
	}
}



//なにやら描画がおかしくなるバグをfix minit
LRESULT CDonutSearchBar::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	// CAddressBarCtrlImplからコピペ
	HWND	hWnd	= GetParent();
	CPoint	pt(0, 0);

	MapWindowPoints(hWnd, &pt, 1);
	pt = ::OffsetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
	LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, wParam, 0L);
	::SetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
	return 1; //lResult;
}



///+++ 検索文字列を取得する
const CString&	CDonutSearchBar::GetSearchStr()
{
	HWND	hWnd	= GetEditCtrl();

	//+++ なるべく、アロケートが発生しないようにしてみる.
	enum { NAME_LEN = 0x1000 };
	TCHAR	name[ NAME_LEN ];
	int 	nLen	= ::GetWindowTextLength(hWnd);
	if (nLen >= NAME_LEN)
		nLen	 = NAME_LEN - 1;
	name[0]		 = '\0';
	int 	nRetLen = ::GetWindowText(hWnd, name, nLen + 1);
	name[nLen+1] = '\0';
	if (_tcscmp(name, LPCTSTR(m_strEngine)) != 0) {
		m_strKeyword = name;
	}
	return m_strKeyword;
}



LRESULT CDonutSearchBar::OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled*/)
{
	LPNMTTDISPINFO pDispInfo  = (LPNMTTDISPINFO) pnmh;

	pDispInfo->szText[0] = 0;
	if (pDispInfo->uFlags & TTF_IDISHWND)
		return S_OK;

  //#if 1		//+++
  //  CString 	   strKeyword = GetSearchStr();
  //#else
  //  CString 	   strKeyword = MtlGetWindowText(m_wndKeyword);
  //#endif
	CString 	   strHeader  = _T('\"');
	CString 	   strHelp	  = _T("\" を");
	CString 	   strEngine  = GetSearchEngineStr();	// MtlGetWindowText(m_wndEngine);

	TCHAR		   szBuff[80+4];
	szBuff[0]	= 0;
	AtlCompactPathFixed( szBuff, GetSearchStr()/*strKeyword*/, 80 - strHelp.GetLength() - strHeader.GetLength() );

	strHeader	+= szBuff;
	strHeader	+= strHelp;

	switch (idCtrl) {
	case ID_SEARCH_WEB: 		strHeader += _T("\"") + strEngine + _T("\"で検索する"); 				break;
	case ID_SEARCHBAR_HILIGHT:	strHeader += _T("ハイライトする");										break;
	case ID_SEARCH_PAGE:		strHeader += _T("このページで検索する(↑:Shift+Enter ↓:Ctrl+Enter)");	break;
	default:	// ID_SEARCH_WORD00..09
		return S_OK;
	}

	::lstrcpyn(pDispInfo->szText, strHeader, 80);

	return S_OK;
}



void CDonutSearchBar::OnCommand(UINT uFlag, int nID, HWND hWndCtrl)
{
	if (hWndCtrl == m_wndToolBar.m_hWnd) {
	  //#if 1	//+++
	  //	CString str  = GetSearchStr();
	  //#else
	  //	CEdit	edit = GetEditCtrl();
	  //	CString str  = MtlGetWindowText(edit);
	  //#endif
		switch (nID) {
		case ID_SEARCH_WEB:
		  #if 1
			SearchWeb();
		  #else
			_OnCommand_SearchWeb( GetSearchStr() /*str*/ );
		  #endif
			break;
		case ID_SEARCHBAR_HILIGHT:	//ID_SEARCH_HILIGHT:
			//GetHilightBtnFlag();
			//m_bHilightSw^=1;
		  #if 1	//+++
			SearchHilight();
		  #else
			//_OnCommand_SearchHilight( str);
		  #endif
			break;
		case ID_SEARCH_PAGE:
			_OnCommand_SearchPage( (::GetKeyState(VK_SHIFT) >= 0));
			break;
		case ID_SEARCHBAR_WORD00:
		case ID_SEARCHBAR_WORD01:
		case ID_SEARCHBAR_WORD02:
		case ID_SEARCHBAR_WORD03:
		case ID_SEARCHBAR_WORD04:
		case ID_SEARCHBAR_WORD05:
		case ID_SEARCHBAR_WORD06:
		case ID_SEARCHBAR_WORD07:
		case ID_SEARCHBAR_WORD08:
		case ID_SEARCHBAR_WORD09:
		case ID_SEARCHBAR_WORD10:
		case ID_SEARCHBAR_WORD11:
		case ID_SEARCHBAR_WORD12:
		case ID_SEARCHBAR_WORD13:
		case ID_SEARCHBAR_WORD14:
		case ID_SEARCHBAR_WORD15:
		case ID_SEARCHBAR_WORD16:
		case ID_SEARCHBAR_WORD17:
		case ID_SEARCHBAR_WORD18:
		case ID_SEARCHBAR_WORD19:
			_OnCommand_SearchPage((::GetKeyState(VK_SHIFT) >= 0), nID-ID_SEARCHBAR_WORD00);
			break;

		default:
			ATLASSERT(0);
		}

		SetMsgHandled(TRUE);
		return;
	}

	SetMsgHandled(FALSE);
}


//+++ ハイライトボタンのon/off状態を返す
bool CDonutSearchBar::GetHilightBtnFlag()
{
  #if 0
	int		rc1 = m_wndToolBar.IsButtonChecked( ID_SEARCHBAR_HILIGHT );
	int		rc2 = m_wndToolBar.IsButtonPressed( ID_SEARCHBAR_HILIGHT );
	int		rc3 = m_wndToolBar.IsButtonEnabled( ID_SEARCHBAR_HILIGHT );
	int		rc4 = m_wndToolBar.IsButtonHidden ( ID_SEARCHBAR_HILIGHT );
	int		rc5 = m_wndToolBar.IsButtonIndeterminate( ID_SEARCHBAR_HILIGHT );
	int		rc6 = m_wndToolBar.IsButtonHighlighted( ID_SEARCHBAR_HILIGHT );
	if (rc3 < 0)
		return m_bHilightSw != 0;
	if (rc1 < 0)
		rc1     = m_bHilightSw;
	bool	rc  = (rc1 > 0 || rc2 > 0) && /*rc3 > 0 &&*/ (rc4 <= 0 || rc5 <= 0);
	m_bHilightSw= rc;
	return	rc;
  #else
	return m_bHilightSw != 0;
  #endif
}



/** ハイライトボタンを強制的に設定する
 */
bool CDonutSearchBar::ForceSetHilightBtnOn(bool sw)
{
	bool rc = (BOOL(sw) != m_bHilightSw);
	m_bHilightSw = sw;
	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;
	m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_HILIGHT, TBIF_STATE, 0, hilightStat, 0, 0, 0, 0, 0);
	return rc;
}



//+++
void CDonutSearchBar::OnSearchWeb_engineId(UINT code, int id, HWND hWnd)
{
	ATLASSERT(ID_INSERTPOINT_SEARCHENGINE <= id && id <= ID_INSERTPOINT_SEARCHENGINE_END);
	unsigned n = id - ID_INSERTPOINT_SEARCHENGINE;
	CString 	strEngine;
	MtlGetLBTextFixed(m_cmbEngine, n, strEngine);
	CString str = GetSearchStr();
	SearchWeb_str_engine(str, strEngine);
}



void CDonutSearchBar::SearchWeb()
{
  #if 1	//+++
	CString str = GetSearchStr();
	_OnCommand_SearchWeb( str );
  #endif
}


void	CDonutSearchBar::_OnCommand_SearchWeb(CString &str)
{
	int 	nTarCate	 = m_cmbEngine.GetCurSel();
	if (nTarCate == -1)
		return;
	SearchWeb_str_engine(str, GetSearchEngineStr());
}


//+++ エンジンの選択をできるように_OnCommand_SearchWebの実体を分離.
void	CDonutSearchBar::SearchWeb_str_engine(CString &str, const CString&  strSearchEng)
{
  #if 1
	//x CString 	strSearchEng = MtlGetWindowText(m_cmbEngine);
	bool bUrlSearch = false;
	if (::GetFocus() != m_wndKeyword.m_hWnd) {
	   #if 1	//+++ 強引なので、あとで、仕組みを直す
		// 選択範囲があれば、それを優先する.
		CString strSel = Donut_GetActiveSelectedText();
		if (strSel.IsEmpty() == 0) {
			str = strSel;
		}
	   #endif
		if (str.IsEmpty())	//+++ 検索文字列が空の場合.
		{
			if (strSearchEng.IsEmpty())
				return;
			CString 		strSearchPath = GetSearchIniPath();
			CIniFileI		pr(strSearchPath, strSearchEng);
			DWORD			exPropOpt = pr.GetValue(_T("ExPropOpt"), 0);
			pr.Close();
			if (exPropOpt & 1) {	// addressbarの文字を取ってくる場合.
				CString strUrl = GetAddressBarText();
				if (strUrl.IsEmpty() == 0) {
					str 	  = strUrl;
					bUrlSearch = true;
				}
			}
		}
	}
  #endif

	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
	toolBarAddButtons(/*str*/);			//+++ サーチのツールバー部に単語を設定

	if ( !str.IsEmpty() ) {
	  #if 1 //+++	OnItemSelectedを分解したのでOpenSearchを呼ぶように変更.
		BOOL	bFirst		 = TRUE;
		int 	nLoopCtn	 = 0;
		OpenSearch(str, strSearchEng, nLoopCtn, bFirst);
	  #elif 1 //+++	templateをやめたのでthisでいい.
		this->OnItemSelected(str, strSearchEng);
	  #else
		T *pT = static_cast<T *>(this);
		pT->OnItemSelected(str, strSearchEng);
	  #endif
		if (bUrlSearch == false)		//+++ url検索だった場合は、履歴に入れないで置く.
			_AddToSearchBoxUnique(str);
	}

  #if 1	//+++ 不要かもだが...テスト的に...
	//+++ 表示→ツールバー→検索バーボタンをonの時のみ
	ShowToolBarIcon(m_bShowToolBar/*true*/);
  #endif
}




void CDonutSearchBar::_OnCommand_SearchHilight(CString &str)
{
	GetHilightBtnFlag();
	m_bHilightSw ^= 1;

	checkToolBarWords();	//+++
//toolBarAddButtons(false);	//+++ ためし...

	str = RemoveShortcutWord(str);

	if (m_bFiltering)
		FilterString(str);
  #if 0	//+++ ここで、はじくと、ハイライトボタンのonが瞬間でおわってoffに戻るので、メッセージは投げておく
	if (str.IsEmpty())
		return;
  #endif
	SendMessage(GetTopLevelParent(), WM_USER_HILIGHT, (WPARAM) LPCTSTR(str)/*str.GetBuffer(0)*/, 0);
	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
}



void CDonutSearchBar::_OnCommand_SearchPage(BOOL bForward, int no /*=-1*/)
{
	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
	checkToolBarWords();				//+++

	CEdit	edit	= GetEditCtrl();
  #if 1 //+++ カーソル位置の単語だけを選択するようにしてみる.
	CString str;
	if (no >= 0) {
		str 	= _GetSelectText(edit);
		//引用符・ショートカットワードは除外
		str = RemoveShortcutWord(str);
		if (m_bFiltering)
			FilterString(str);
		std::vector<CString> strs;
		strs.reserve(10);
		Misc::SeptTextToWords(strs, str);
		if (size_t(no) < strs.size())
			str = strs[no];
	} else {
		str 	= _GetSelectText_OnCursor(edit);
		//引用符・ショートカットワードは除外
		str = RemoveShortcutWord(str);
	}
  #else
	CString str 	= _GetSelectText(edit);
	//引用符・ショートカットワードは除外
	str = RemoveShortcutWord(str);
  #endif

	CString strExcept = _T(" \t\"\r\n　");
	str.TrimLeft(strExcept);
	str.TrimRight(strExcept);
	if (m_bFiltering)
		FilterString(str);

	SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), (LPARAM) bForward);
}




//public
const CString CDonutSearchBar::RemoveShortcutWord(const CString &str)
{
	if (m_bUseShortcut) {
		if (str.Find( _T("\\") ) == 0 || str.Find( _T("/") ) == 0) {
			int nPos = str.Find( _T(" ") );

			if (nPos != -1)
				return str.Mid(nPos + 1);
		}
	}

	return str;
}



///+++
int CDonutSearchBar::btnWidth() const
{
  #if 1
	int  btnW = ::GetSystemMetrics(SM_CXHTHUMB);
	if (m_bExistManifest == 0)
		btnW += 2 * 2;
	else
		btnW += 2;
	return btnW;
  #else
	return 21;
  #endif
}



///+++ キーワード入力欄が空の時にエンジン名を表示するための設定
void	CDonutSearchBar::SetCmbKeywordEmptyStr()
{
	m_cmbKeyword.setEmptyStr(GetSearchEngineStr(), IDC_EDIT/*1001*/, (m_nEngineWidth <= 8+btnWidth()));
	m_cmbKeyword.redrawEmptyStr();
}



// private:
LRESULT CDonutSearchBar::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  #ifndef USE_DIET
	//+++ 検索単語を項目にするため、検索バーのサイズ調整方法を変更.
	if (m_dwNoWordButton == 0) { //+++ 単語ボタンありのとき
		return ResizeBar(0,0);
	} else {
		//+++ 単語ボタン無しの場合は旧処理.

		//if (!m_wndToolBar.m_hWnd)
		//	return S_FALSE;
		if (!m_wndKeyword.m_hWnd)
			return S_FALSE;

		CRect	rcDlg;
		GetClientRect(&rcDlg);

		CRect	rcToolbar(rcDlg);
		CString str;

		if (rcDlg.right == 0)
			return S_FALSE;

		if ( GetToolIconState() && m_wndToolBar.m_hWnd ) {
			rcToolbar.left = rcDlg.right - m_cxBtnsBtn - 10;
			m_wndToolBar.SetWindowPos(NULL, rcToolbar, SWP_NOZORDER | SWP_NOSENDCHANGING);
		} else {
			//非表示
			rcToolbar.left = rcDlg.right;
		}

		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		int 	nEngineCX = rcEngine.Width();
		int 	nEngineCY = rcEngine.Height();
		if (nEngineCX <= btnWidth())	//+++
			nEngineCX = btnWidth();
		m_nKeywordWidth	 = nEngineCX;
		rcEngine		 = rcToolbar;
		rcEngine.right	 = rcToolbar.left - s_kcxGap;
		rcEngine.left	 = rcEngine.right - nEngineCX;
		rcEngine.top	 = m_nDefEditT; //minit
		rcEngine.bottom  = rcEngine.top + nEngineCY;

		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);

		CRect	rcKeyword(rcEngine);
		rcKeyword.left	= 0;
		rcKeyword.right = rcEngine.left - s_kcxGap;
		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);
	  #if 1	//+++ 暫定対処... 本質的に修正できていないがon_
		m_cmbKeyword.SetEditSel(0,0);	//+++ 無理やり範囲解除
	  #endif
		SetCmbKeywordEmptyStr();	//+++

		m_wndToolBar.InvalidateRect(NULL, TRUE);

		return S_OK;
	}
  #else	// USE_DIET時は 単語ボタンのみ
	return ResizeBar(0,0);
  #endif
}



///+++ バーのサイズ変更. キーワード｜エンジン｜ツールバー  の境目での移動幅を順にdifA,difBとする.
int CDonutSearchBar::ResizeBar(int difA, int difB)
{
	if (!m_wndKeyword.m_hWnd) {
		return S_FALSE;
	}

	CRect	rcDlg;
	GetClientRect(&rcDlg);
	if (rcDlg.right == 0) {
		return S_FALSE;
	}

	//int	h = rcDlg.Height();

	CRect	rcKeyword;
	m_cmbKeyword.GetWindowRect(&rcKeyword);
	m_nKeywordWidth = rcKeyword.Width();
	int 		h	= rcKeyword.Height();
	m_nKeywordWidth += difA;
	if (m_nKeywordWidth <= btnWidth())
		m_nKeywordWidth = btnWidth();
	rcKeyword.left	= 0;
	rcKeyword.top	= m_nDefEditT;
	rcKeyword.right = m_nKeywordWidth;
	rcKeyword.bottom= rcKeyword.top + h;
	m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);


	CRect	rcEngine;
	m_cmbEngine.GetWindowRect(&rcEngine);
	h			  = rcEngine.Height();
	m_nEngineWidth = rcEngine.Width();
	m_nEngineWidth += difB;
	if (m_nEngineWidth <= btnWidth()) {
		m_nEngineWidth = btnWidth();
	}
	rcEngine.left	 = rcKeyword.right + s_kcxGap;
	rcEngine.top	 = m_nDefEditT;
	rcEngine.right	 = rcEngine.left	+ m_nEngineWidth;
	rcEngine.bottom  = rcEngine.top + h;
	m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);

	CRect	rcToolbar(rcDlg);
	rcToolbar.left = rcEngine.right + s_kcxGap;
	if (rcToolbar.left > rcToolbar.right)
		rcToolbar.left = rcToolbar.right;
	if ( GetToolIconState() ) {
	} else {
		//非表示
		rcToolbar.left = rcDlg.right;
	}
	if (m_wndToolBar.m_hWnd)
		m_wndToolBar.SetWindowPos(NULL, rcToolbar, SWP_NOZORDER | SWP_NOSENDCHANGING);

	m_wndToolBar.InvalidateRect(NULL, TRUE);

	return S_OK;
}


#if 1	//test
bool	CDonutSearchBar::checkEngineNameEnable()
{
	if (::IsWindow(m_hWnd) == 0)
		return false;
	if (::IsWindow(m_cmbKeyword.m_hWnd) == 0)
		return false;

	if (::GetFocus() == GetEditCtrl())
		return false;

	CRect	rcEngine;
	m_cmbEngine.GetWindowRect(&rcEngine);
	int engineW		= rcEngine.Width();
	if (engineW > btnWidth())
		return false;

	CRect	rcKeyword;
	m_cmbKeyword.GetWindowRect(&rcKeyword);
	int keywordW	= rcKeyword.Width();
	if (keywordW <= btnWidth())
		return false;

	//CPaintDC dc(m_cmbKeyword.m_hWnd);
	//dc.TextOut(rcKeyword.left, rcKeyword.top, "Test");
	//dc.TextOut(0, 0, "ABCDEF");

	return true;
}
#endif



void CDonutSearchBar::InitToolBar(int cx, int cy, COLORREF clrMask, UINT nFlags /*= ILC_COLOR24*/)
{
	m_clrMask	= clrMask;

	CImageList	imgs;
	MTLVERIFY( imgs.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

	CBitmap 	bmp;
	bmp.Attach( AtlLoadBitmapImage(LPCTSTR(GetSkinSeachBarPath(0)), LR_LOADFROMFILE) );
	if (bmp.m_hBitmap == NULL)
		bmp.LoadBitmap(IDB_SEARCHBUTTON/*nImageBmpID*/);			//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
	imgs.Add(bmp, clrMask);

	CImageList	imgsHot;
	MTLVERIFY( imgsHot.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

	CBitmap 	bmpHot;
	bmpHot.Attach( AtlLoadBitmapImage(LPCTSTR(GetSkinSeachBarPath(1)), LR_LOADFROMFILE) );
	if (bmpHot.m_hBitmap == NULL)
		bmpHot.LoadBitmap(IDB_SEARCHBUTTON_HOT/*nHotImageBmpID*/);		//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
	imgsHot.Add(bmpHot, clrMask);

  #if 1 //+++ Disableも用意する
	CString str = GetSkinSeachBarPath(2);
	int 	dis = 0;
	if (Misc::IsExistFile(str) == 0) {						//+++ 画像ファイルがない時
		if (Misc::IsExistFile(GetSkinSeachBarPath(0))) {	//+++ 通常があれば
			str = GetSkinSeachBarPath(0);					//+++ 通常画で代用
		} else {											//+++ 通常もなければ
			dis = IDB_SEARCHBUTTON_DIS; 					//+++ デフォルトのDisable画を使う.
		}
	}
	CImageList	imgsDis;
	MTLVERIFY( imgsDis.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );
	CBitmap 	bmpDis;
	bmpDis.Attach( AtlLoadBitmapImage(LPCTSTR(str), LR_LOADFROMFILE) );
	if (bmpDis.m_hBitmap == NULL && dis)
		bmpDis.LoadBitmap(dis); 					//+++	skinの処理がここにあるので、デフォルト素材もこの場で設定にしとく...
	imgsDis.Add(bmpDis, clrMask);
  #endif

  #ifndef USE_DIET
	CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
	//DWORD 	jikken = pr.GetValue("Jikken");
	m_dwNoWordButton   = pr.GetValue(_T("NoWordButton"));		//+++ 単語ボタンを使わない場合1 (単語ボタン有の時にもページボタンを出し続けるなら|2)
	m_dwTinyWordButton = pr.GetValue(_T("NumberButton"));		//+++ 単語ボタンでなく、数字だけの5ボタンを使うか? (NoWordButton=1の時のみ有効)
	pr.Close();
	m_dwUsePageButton  = (m_dwNoWordButton >> 1) & 1;		//+++ 単語ボタン有の時にもページボタンを出し続けるか?
	m_dwNoWordButton   &= 1;
  #endif
	DWORD		flags  =  WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
						| CCS_NODIVIDER  /*| CCS_NORESIZE */ | CCS_NOPARENTALIGN | CCS_TOP
						| TBSTYLE_TOOLTIPS | TBSTYLE_FLAT ;

  #if 0	//+++ win9xでは先にこれらを設定してしまうと、後でSetWindowLongしても効かないよう?なので、ここでは設定しない... 関係なかった模様..
   #ifndef USE_DIET
	if (m_dwNoWordButton == 0 || m_dwTinyWordButton)
   #endif
	{
		flags		  |= TBSTYLE_LIST | TBSTYLE_TRANSPARENT;	//+++ 追加 (検索単語の文字列を表示するには必要)
	}
  #endif

	m_wndToolBar	 = ::CreateWindowEx(
							0,
							TOOLBARCLASSNAME,
							NULL,
							// ATL_SIMPLE_TOOLBAR_PANE_STYLE|CCS_TOP,
							flags,
							0,
							0,
							600,	//100, 適当な値
							50,		//100, 適当な値
							m_hWnd,
							(HMENU) NULL,
							_Module.GetModuleInstance(),
							NULL							);

	//+++ どうも古いosでは、CreateWindow時に設定したらまずいらしい?...ので後づけでSetWindowLongする必要がある?... 関係なかった模様.
  #ifndef USE_DIET
	if (m_dwNoWordButton == 0 || m_dwTinyWordButton)
  #endif
	{
		flags	|= (UINT)m_wndToolBar.GetWindowLong(GWL_STYLE);
		flags	|= TBSTYLE_LIST | TBSTYLE_TRANSPARENT;	//+++ 追加 (検索単語の文字列を表示するには必要)
		m_wndToolBar.SetWindowLong(GWL_STYLE, flags);
	}

	m_wndToolBar.SetButtonStructSize( sizeof (TBBUTTON) );
	m_wndToolBar.SetImageList(imgs);
	m_wndToolBar.SetHotImageList(imgsHot);
	m_wndToolBar.SetDisabledImageList(imgsDis); //+++

  #ifndef USE_DIET
	if (m_dwNoWordButton) {
		addDefaultToolBarIcon(0);		//+++ 単語ボタン無しモードでの、ボタン設定.
	} else
  #endif
	{
		toolBarAddButtons();			//+++ ツールバーにボタンを追加.(単語ボタンアリの場合)
	}

	m_bShowToolBar	 = TRUE;
	m_bLoadedToolBar = TRUE;

  #if 0	//+++ この関数抜けたあとで、調整入るのでここでしちゃだめ
	ShowToolBarIcon(true);
  #endif
}


void	CDonutSearchBar::checkToolBarWords()	//+++
{
	if (toolBarAddButtons(true)) {
	  #if 1	//+++ 不要かもだが...テスト的に...
		ShowToolBarIcon(m_bShowToolBar/*true*/);	//+++ trueだとボタン消してるときに不味い
	  #endif
	}
}



//+++ 検索ワードの文字列を分解して、検索ツールバーに追加
bool CDonutSearchBar::toolBarAddButtons(bool chk)
{
  #ifndef USE_DIET
	if (m_dwNoWordButton && m_dwTinyWordButton == 0)	//単語ボタンを表示しない場合は直帰.
		return 0;
  #endif
	CEdit	edit	= GetEditCtrl();

	CString str 	= _GetSelectText(edit);
	//引用符・ショートカットワードは除外
	str = RemoveShortcutWord(str);
	if (m_bFiltering)
		FilterString(str);

	if (chk && m_toolBarAddWordStr == str) {
		return ForceSetHilightBtnOn(m_bHilightSw != 0);	//+++ ハイライトボタンのon/offは変わっている可能性があるので更新.
	}
	m_toolBarAddWordStr = str;

	std::vector<CString> strs;
	strs.reserve(20);
	Misc::SeptTextToWords(strs, str);

	if (m_wndToolBar.m_hWnd == 0)
		return 0;

	//+++ 検索ツールバーの中身を一旦すべて削除
	unsigned num = m_wndToolBar.GetButtonCount();
	for (int i = num; --i >= 0;) {
		m_wndToolBar.DeleteButton(i);
	}

	addDefaultToolBarIcon(strs.size());		//+++ デフォルトのアイコンを追加
  #ifndef USE_DIET
	if (m_dwNoWordButton == 0)				//+++ 数字ボタンの時はワードボタンを登録させない.
  #endif
	{
		toolBarAddWordTbl(strs);
	}

	return true;
}



//+++  デフォルトのアイコンを追加
int  CDonutSearchBar::addDefaultToolBarIcon(unsigned n)
{
	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;
  #ifndef USE_DIET
	if (n == 0 || m_dwNoWordButton || m_dwUsePageButton) {	//+++ 旧来の検索 (単独のページ内検索ボタンあり)
		if (m_dwNoWordButton && m_dwTinyWordButton)
			return addDefaultToolBarIcon_tinyWordButton(n);
		//static
		static const TBBUTTON	btns[3] = {
			{ 0 		, ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },	// Web
			{ 2 		, ID_SEARCH_PAGE,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },	// Page
			{ 1 		, ID_SEARCHBAR_HILIGHT, hilightStat    , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE	, 0, 0 }, // Hilight
		};
		//m_nHilightBtnNo = 2;
	  #if 0	//+++ 本来はこっちでいい
		MTLVERIFY( m_wndToolBar.AddButtons(3, (TBBUTTON *) btns ) );
	  #else	//+++ テスト
		for (unsigned i = 0; i < 3; ++i)
			MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *) &btns[i] ) );
	  #endif
		m_wndToolBar.SetBitmapSize(14,14);
		if (m_dwNoWordButton) {
			m_cxBtnsBtn 	 = (20) * 3 + 1;
		}
		return 3;
	} else
  #endif
	{				//+++ 単語専用ボタン有り
		//static
		static const TBBUTTON	btns[2] = {
			{ 0 		, ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },	// Web
			{ 1 		, ID_SEARCHBAR_HILIGHT, hilightStat    , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE	, 0, 0 },	// Hilight
		};
		//m_nHilightBtnNo = 1;
	  #if 0	//+++ 本来はこっちでいい
		MTLVERIFY( m_wndToolBar.AddButtons(2, (TBBUTTON *) btns ) );
	  #else	//+++ テスト
		for (unsigned i = 0; i < 2; ++i)
			MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *) &btns[i] ) );
	  #endif
		return 2;
	}
}



#ifndef USE_DIET
//+++ 単語ボタンの代わりに1～5の数字ボタンを設定.
//		※単語ボタンが実装されるまでの実験/代用処理だったもの.
//		  単語ボタンを使用しない設定で、かつ、noguiに"NumberButton=1"を設定したときのみ利用可能.
int	CDonutSearchBar::addDefaultToolBarIcon_tinyWordButton(unsigned n)
{
	if (n > 5)
		n = 5;
	int hilightStat = m_bHilightSw ? TBSTATE_PRESSED : TBSTATE_ENABLED;
	//static
	static const TBBUTTON	btns[3] = {
		{ 0 		, ID_SEARCH_WEB,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },	// Web
		{ 2 		, ID_SEARCH_PAGE,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE	, 0, 0 },	// Page
		{ 1 		, ID_SEARCHBAR_HILIGHT, hilightStat	   , TBSTYLE_CHECK	| TBSTYLE_AUTOSIZE	, 0, 0 },	// Hilight
	};
	//m_nHilightBtnNo = 2;
  #if 0	//+++ 本来はこっちでいい
	MTLVERIFY( m_wndToolBar.AddButtons(3, (TBBUTTON *) btns ) );
  #else	//+++ テスト
	for (unsigned i = 0; i < 3; ++i)
		MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *) &btns[i] ) );
  #endif
	m_cxBtnsBtn	 = (20) * (3+5) + 1;

	enum { STYLE = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE }; // | TBSTYLE_LIST | TBSTYLE_TRANSPARENT };
	for (unsigned i = 0; i < 5; ++i) {
		const TBBUTTON	btn = { I_IMAGENONE, ID_SEARCHBAR_WORD00+i, (i < n) ? TBSTATE_ENABLED : 0/*TBSTATE_CHECKED*/, STYLE, 0, 0 };
		MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *)&btn) );

		CVersional<TBBUTTONINFO> bi;
		bi.cbSize	= sizeof(TBBUTTONINFO);
		bi.dwMask	= TBIF_TEXT	/* | TBIF_STYLE*/;
		bi.fsStyle |= TBSTYLE_AUTOSIZE /*| TBBS_NOPREFIX*/ ;
		TCHAR str[4];
		str[0]      = _T('1'+i);
		str[1]      = _T('\0');
		bi.pszText	= str;
		MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_WORD00+i, &bi) );
	}

	// サイズ調整など
	m_wndToolBar.SetMaxTextRows(1);
	CRect rcButton;
	m_wndToolBar.GetItemRect(3, rcButton);
	m_wndToolBar.SetButtonSize(rcButton.Size());
	m_wndToolBar.SetButtonSize(rcButton.Size());
	m_wndToolBar.Invalidate();

	m_wndToolBar.AutoSize();
	return 3;
}
#endif



//+++ vectorで渡された複数の文字列(20個まで)を検索ツールバーに登録.
void CDonutSearchBar::toolBarAddWordTbl(std::vector<CString>& tbl)
{
	CVersional<TBBUTTONINFO> bi;
	bi.cbSize	= sizeof(TBBUTTONINFO);
	bi.dwMask	= TBIF_TEXT/* | TBIF_STYLE*/;
	bi.fsStyle |= TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX;

	unsigned n	= unsigned(tbl.size());
	if (n > 20)
		n = 20;
	for (unsigned i = 0; i < n; ++i) {
		enum { STYLE = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | TBSTYLE_NOPREFIX }; // | TBSTYLE_LIST | TBSTYLE_TRANSPARENT };
		TBBUTTON	btn = { 2/*I_IMAGENONE*/, ID_SEARCHBAR_WORD00+i, TBSTATE_ENABLED, STYLE, 0, 0 };
		MTLVERIFY( m_wndToolBar.AddButton( (TBBUTTON *)&btn) );
		bi.pszText = (LPTSTR) LPCTSTR(tbl[i]);
		MTLVERIFY( m_wndToolBar.SetButtonInfo(ID_SEARCHBAR_WORD00+i, &bi) );

		//m_wndToolBar.AutoSize();
		//CRect rcButton;
		//m_wndToolBar.GetItemRect(TOP+i, rcButton);
		//m_wndToolBar.SetButtonSize(rcButton.Size());
	}
	m_wndToolBar.AutoSize();
	m_wndToolBar.Invalidate();
	//ShowToolBarIcon(true);
}



CString 	CDonutSearchBar::GetSkinSeachBarPath(int mode/*BOOL bHot*/)
{
	ATLASSERT(mode >= 0 && mode <= 2);
	static const TCHAR* tbl[] = {
		_T("SearchBar.bmp"),
		_T("SearchBarHot.bmp"),
		_T("SearchBarDis.bmp"),
	};
	return _GetSkinDir() + tbl[ mode ];
}



LRESULT CDonutSearchBar::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	CRect		rcDlg;
	GetWindowRect(&rcDlg);
	SetWindowPos(NULL, 0, 0, 0, m_nDefDlgH , 0);
	CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
	m_nKeywordWidth 	 = pr.GetValue( _T("KeywordWidth" ), 150 );
	m_nEngineWidth		 = pr.GetValue( _T("EngWidth"	  ), 150 );
	DWORD dwUseShortCut  = pr.GetValue( _T("UseShortcut"  ),   0 );
	pr.Close();

	if (dwUseShortCut)
		m_bUseShortcut = TRUE;

	//コンボボックス初期化
	m_cmbKeyword.FlatComboBox_Install( GetDlgItem(IDC_CMB_KEYWORD) );
	m_cmbEngine.FlatComboBox_Install  ( GetDlgItem(IDC_CMB_ENGIN  ) );
	m_cmbEngine.SetDroppedWidth(150);
	_InitCombo();

	if (m_nKeywordWidth != 0) {
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		int 	h		 = rcKeyword.Height();
		rcKeyword.left	 = 0;
		rcKeyword.right  = m_nKeywordWidth;
		rcKeyword.top	 = 0;
		rcKeyword.bottom = h;
		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);
	}
	if (m_nEngineWidth != 0) {
		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		int 	h		 = rcEngine.Height();
		rcEngine.left	= 0;
		rcEngine.right	= m_nEngineWidth;
		rcEngine.top	= 0;
		rcEngine.bottom = h;
		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER | SWP_NOSENDCHANGING);
	}

	m_wndKeyword.SubclassWindow( m_cmbKeyword.GetDlgItem(IDC_EDIT/*1001*/) );
	m_wndEngine.SubclassWindow(m_cmbEngine.m_hWnd);
	m_wndKeywordCombo.SubclassWindow(m_cmbKeyword.m_hWnd); //minit

	//ツールバー初期化
	DWORD	dwShowToolBar = STB_SHOWTOOLBARICON;
	{
		CIniFileI		pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwShowToolBar, _T("Show_ToolBarIcon") );
	}
	m_bShowToolBar	= (dwShowToolBar & STB_SHOWTOOLBARICON) == STB_SHOWTOOLBARICON;

  #if 1 //+++ 無理やり全部初期化(メモリーの無駄だが安全&見栄え優先)
	BOOL bShowToolBar = m_bShowToolBar;
	InitToolBar(/*+++ IDB_SEARCHBUTTON, IDB_SEARCHBUTTON_HOT,*/ m_nBmpCX, m_nBmpCY, RGB(255, 0, 255) );
	ShowToolBarIcon(bShowToolBar);
  #else //+++ 元の処理。表示しなけりゃメモリー等お得。だが、途中で設定した場合font反映なし.
	if (m_bShowToolBar) {
		InitToolBar(/*+++ IDB_SEARCHBUTTON, IDB_SEARCHBUTTON_HOT,*/ m_nBmpCX, m_nBmpCY, RGB(255, 0, 255) );
	}
  #endif

	//ドラッグドロップ初期化
	RegisterDragDrop();

	//スレッドを利用してコンボボックスにデータを登録(INIからの読み込みに時間がかかるため)
	// Thread Create
	m_ThreadParams.m_hExitEvent  = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_ThreadParams.m_pSearchBar  = this;
	DWORD		wThreadId	   = 0;
	m_hThread	= (HANDLE)_beginthreadex(NULL, 0, _SearchThread, (LPVOID)&m_ThreadParams, 0, (UINT*)&wThreadId);

	::SetThreadPriority(m_hThread , THREAD_PRIORITY_IDLE);

	// SetCmbKeywordEmptyStr();	//+++ どうせまだエンジン名が登録されていないので、このタイミングはなし

	return S_OK;
}




//スレッドを利用してコンボボックスにデータを登録(INIからの読み込みに時間がかかるため)
UINT WINAPI CDonutSearchBar::_SearchThread(void* lpParam)
{
	_ThreadParam *pParam = (_ThreadParam *) lpParam;
	_InitialEngine ( (LPVOID) pParam->m_pSearchBar->m_cmbEngine.m_hWnd  );
	_InitialKeyword( (LPVOID) pParam->m_pSearchBar->m_cmbKeyword.m_hWnd );
	_endthreadex(0);
	return 0;
}


bool CDonutSearchBar::IsAliveSearchThread() const
{
	DWORD dwReturnCode = 0;
	::GetExitCodeThread(m_hThread, &dwReturnCode);
	return (dwReturnCode == STILL_ACTIVE);
}



//private:
void CDonutSearchBar::OnDestroy()
{
	// Thread Remove
	::SetEvent(m_ThreadParams.m_hExitEvent);
	DWORD	dwResult	  = ::WaitForSingleObject(m_hThread, DONUT_THREADWAIT);
	::CloseHandle(m_ThreadParams.m_hExitEvent);
	::CloseHandle(m_hThread);

  #if 0
	CRect			rcKeyword;
	m_wndKeyword.GetWindowRect(rcKeyword);
	DWORD			keywordW = rcKeyword.Width();
	CRect			rcEngine;
	m_wndEngine.GetWindowRect(rcEngine);
	DWORD			enginW	 = rcEngine.Width();
  #else
   #ifndef USE_DIET
	if (m_dwNoWordButton) { //+++ 旧処理のときの辻褄あわせ
		CRect			rcEngine;
		m_wndEngine.GetWindowRect(rcEngine);
		m_nEngineWidth = rcEngine.Width();
	}
   #endif
  #endif

	{
		CIniFileIO	pr( g_szIniFileName, _T("SEARCH") );
		pr.SetValue( (DWORD) m_nKeywordWidth, _T("KeywordWidth") );
		pr.SetValue( (DWORD) m_nEngineWidth  , _T("EngWidth") );

		DWORD	dwShowToolBar = m_bShowToolBar ? STB_SHOWTOOLBARICON : 0;
		pr.SetValue( dwShowToolBar, _T("Show_ToolBarIcon") );

		DWORD	dwStatus	  = 0;
		pr.QueryValue( dwStatus, _T("Status") );
		if (dwStatus & STS_LASTSEL)
			pr.SetValue( m_cmbEngine.GetCurSel(), _T("SelIndex") );
	}

	m_wndKeyword.UnsubclassWindow();
	m_wndEngine.UnsubclassWindow();
	m_wndKeywordCombo.UnsubclassWindow();
	RevokeDragDrop();
	SaveHistory();
}



void CDonutSearchBar::OnEngineKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN) {
		_OnEnterKeyDown(ENTER_KEYDOWN_RETURN);
		SetMsgHandled(TRUE);
	} else if (nChar == VK_TAB) {
		m_cmbKeyword.SetFocus();
		SetMsgHandled(TRUE);
	} else {
		SetMsgHandled(FALSE);
	}
}



void CDonutSearchBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN) {
		SetMsgHandled(TRUE);
	} else {
		SetMsgHandled(FALSE);
	}
	//checkToolBarWords();	//+++
}



LRESULT CDonutSearchBar::OnCtlColorListBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled		  = FALSE;
	m_hWndKeywordList = (HWND) lParam;
	return 0;
}



void CDonutSearchBar::OnKeywordKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// if not dropped, eat VK_DOWN
	if ( !m_cmbKeyword.GetDroppedState() && (nChar == VK_DOWN || nChar == VK_UP) ) {
		int nIndexEngine  = m_cmbEngine.GetCurSel();
		int nIndexKeyword = m_cmbKeyword.GetCurSel();

		SetMsgHandled(TRUE);

		if (nChar == VK_UP) {
			if (::GetKeyState(VK_CONTROL) < 0) {
				if (0 > nIndexEngine - 1)
					m_cmbEngine.SetCurSel(m_cmbEngine.GetCount() - 1);
				else
					m_cmbEngine.SetCurSel(nIndexEngine - 1);
				//SetCmbKeywordEmptyStr();	//+++
			} else {
				if (0 > nIndexKeyword - 1)
					m_cmbKeyword.SetCurSel(m_cmbKeyword.GetCount() - 1);
				else
					m_cmbKeyword.SetCurSel(nIndexKeyword - 1);
			}

		} else if (nChar == VK_DOWN) {
			if (::GetKeyState(VK_CONTROL) < 0) {
				int nIndex = m_cmbEngine.GetCurSel();

				if (m_cmbEngine.GetCount() > nIndexEngine + 1)
					m_cmbEngine.SetCurSel(nIndexEngine + 1);
				else
					m_cmbEngine.SetCurSel(0);
				//SetCmbKeywordEmptyStr();	//+++
			} else {
				if (m_cmbKeyword.GetCount() > nIndexKeyword + 1)
					m_cmbKeyword.SetCurSel(nIndexKeyword + 1);
				else
					m_cmbKeyword.SetCurSel(0);
			}
		}
	} else {
		if (nChar == VK_RETURN) {
			_OnEnterKeyDown(ENTER_KEYDOWN_RETURN);
			SetMsgHandled(TRUE);
		} else if (nChar == VK_DELETE) {
			if ( m_cmbKeyword.GetDroppedState() ) {
				if ( DeleteKeywordHistory() )
					SetMsgHandled(TRUE);
			} else
				SetMsgHandled(FALSE);
		} else if (nChar == VK_TAB) {
			m_cmbEngine.SetFocus();
			SetMsgHandled(TRUE);
		} else {
			SetMsgHandled(FALSE);
		}
	}

	//SetCmbKeywordEmptyStr();	//+++	PreTranseで毎度チェックするからここは無しに.
}



LRESULT CDonutSearchBar::OnKeywordCbnDropDown(LPNMHDR pnmh)
{
	if (m_cmbKeyword.GetCount() == 0)
		::MessageBox(NULL, _T(""), _T(""), MB_OK);

	return FALSE;
}



BOOL CDonutSearchBar::DeleteKeywordHistory()
{
	if ( !::IsWindow(m_hWndKeywordList) )
		return FALSE;

	CListBox List	= m_hWndKeywordList;
	int 	 nIndex = List.GetCurSel();

	if (nIndex == LB_ERR)
		return FALSE;

	m_cmbKeyword.DeleteString(nIndex);
	return TRUE;
}



struct CDonutSearchBar::_Function_MakeSearchFileListMenu {
	CMenuHandle 	menu;
	int 			nIndex;

	_Function_MakeSearchFileListMenu()
	{
		menu.CreatePopupMenu();
		menu.AppendMenu( 0, 1, _T("拡張プロパティ") );
		menu.AppendMenu(MF_SEPARATOR);
		nIndex = 1;
	}

	void operator ()(CString strFile)
	{
		ATLTRACE(_T("%s\n"), strFile);

		nIndex++;
		menu.AppendMenu( 0, nIndex, MtlGetFileName(strFile) );
	}
};



void CDonutSearchBar::OnEngineRButtonUp(UINT nFlags, CPoint point)
{
	_Function_MakeSearchFileListMenu	__f;

	MtlForEachFileSortEx( Misc::GetExeDirectory() + _T("Search\\"), __f, _T("*.ini") );

	CPoint	pos;

	//GetCursorPos(&pos);
	ATLTRACE("mouseposition1 : left=%4d top=%4d", point.x, point.y);
	::ClientToScreen(m_cmbEngine.m_hWnd, &point);
	ATLTRACE("mouseposition2 : left=%4d top=%4d", point.x, point.y);
	CMenu	menu(__f.menu.m_hMenu);

	int 	nRet	 = menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
							point.x,
							point.y,
							m_cmbEngine.m_hWnd);
	if (nRet == 0)
		return;

	if (nRet == 1) {
		if (!m_cmbEngine.IsWindow() || m_cmbEngine.GetCurSel() == CB_ERR)
			return;
		CString 		  strText;
		MtlGetLBTextFixed(m_cmbEngine, m_cmbEngine.GetCurSel(), strText);
		CExPropertyDialog dlg(GetSearchIniPath(), strText, 0);
		dlg.SetTitle(strText);
		dlg.DoModal();
		return;
	}

	CString 		strTitle;
	menu.GetMenuString(nRet, strTitle, MF_BYCOMMAND);
	CString 		strPath = Misc::GetExeDirectory() + _T("Search\\") + strTitle;

	{
		CIniFileO	pr( g_szIniFileName, _T("Search") );
		pr.SetStringUW( strPath, _T("Path") );
	}

	RefreshEngine();
}



void	CDonutSearchBar::OnEngineSetFocus(HWND hWndBefore)
{
	SetMsgHandled(FALSE);
	::WINNLSEnableIME(m_cmbEngine, FALSE);

	SetCmbKeywordEmptyStr();	//+++
}



void	CDonutSearchBar::OnEngineKillFocus(HWND hWndNew)
{
	SetMsgHandled(FALSE);
	::WINNLSEnableIME(m_cmbEngine, TRUE);

	SetCmbKeywordEmptyStr();	//+++
}



void	CDonutSearchBar::OnMouseMove(UINT nFlags, const CPoint& pt)
{
   #ifndef USE_DIET
	if (m_dwNoWordButton) {
		//+++ 単語ボタン無しの場合は旧処理.
		CRect	rcKeyword;

		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);

		if ( abs(rcKeyword.right - pt.x) > 5 && ::GetCapture() != m_hWnd )
			return;

		if (m_hCursor == NULL)
			m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);

		::SetCursor(m_hCursor);

		if ( (nFlags & MK_LBUTTON) ) {
			UpdateLayout(pt);
		}
	} else
  #endif
	{	//+++ 単語ボタンありの場合
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		if ((abs(rcKeyword.right - pt.x) <= 5 || (m_has == 1 && ::GetCapture() == m_hWnd))
			&& (rcKeyword.top <= pt.y && pt.y < rcKeyword.bottom)
		) {
			m_has = 1;
			if (m_hCursor == NULL)
				m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
			::SetCursor(m_hCursor);
			if ( (nFlags & MK_LBUTTON) ) {
				UpdateLayout(pt);
			}
			return;
		}

		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		if ( (abs(rcEngine.right - pt.x) <= 5 || (m_has == 2 && ::GetCapture() == m_hWnd))
			&& (rcEngine.top <= pt.y && pt.y < rcEngine.bottom)
		) {
			m_has = 2;
			if (m_hCursor == NULL)
				m_hCursor = ::LoadCursor(NULL, IDC_SIZEWE);
			::SetCursor(m_hCursor);
			if ( (nFlags & MK_LBUTTON) ) {
				UpdateLayout2(pt);
			}
			return;
		}
		m_has = 0;
		return;

	}
}


#if 0
///+++ エンジン選択の改造実験... 失敗on_.
void CDonutSearchBar::OnSelDblClkForEngine(UINT code, int id, HWND hWnd)
{
	int 	nTarCate	 = m_cmbEngine.GetCurSel();
	if (nTarCate == -1)
		return;

	TCHAR	buf[4096];
	m_cmbEngine.GetLBText(nTarCate, buf);
	CString strSearchEng = buf;
	BOOL	bFirst		 = TRUE;
	int 	nLoopCtn	 = 0;

	CString  str;
	int nIndexCmb = m_cmbKeyword.GetCurSel();
	if (nIndexCmb == -1)
		str = MtlGetWindowText(m_cmbKeyword);
	else
		MtlGetLBTextFixed(m_cmbKeyword.m_hWnd, nIndexCmb, str);
	if ( !str.IsEmpty() )
		OpenSearch(str, strSearchEng, nLoopCtn, bFirst);
	return;
}
#endif



///+++ エンジン選択の改造実験... 失敗on_...だけど、ちょっとだけ有用.
void CDonutSearchBar::OnSelChangeForEngine(UINT code, int id, HWND hWnd)
{
	if (::GetKeyState(VK_RBUTTON) < 0) {
		CString 			strText;
		MtlGetLBTextFixed(m_cmbEngine, m_cmbEngine.GetCurSel(), strText);
		CExPropertyDialog	dlg(GetSearchIniPath(), strText, 0);
		dlg.SetTitle(strText);
		dlg.DoModal();
		SetCmbKeywordEmptyStr();	//+++
		return;
	}

	CIniFileI		pr( g_szIniFileName, _T("SEARCH") );
	DWORD			dwStatus = pr.GetValue( _T("Status"), 0 );
	pr.Close();

	DWORD							 bSts = 0;
	if		(id == IDC_CMB_ENGIN)	 bSts = dwStatus & STS_ENG_CHANGE_GO;

	if (::GetKeyState(VK_SHIFT) < 0) bSts = !bSts;

	if (bSts) {
		//+++ エンジンが切り替わった場合は、検索窓が空でもurl検索の場合があるので、こちら
		//x _OnEnterKeyDown(ENTER_KEYDOWN_SELCHANGE);
		SearchWeb();
	}
}



void CDonutSearchBar::OnSelChange(UINT code, int id, HWND hWnd)
{
	DWORD			dwStatus = 0;
	{
		CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwStatus, _T("Status") );
	}

	DWORD							 bSts = 0;
	//x if	(id == IDC_CMB_ENGIN)	 bSts = dwStatus & STS_ENG_CHANGE_GO; else		//+++ エンジンは別関数に.
	if (id == IDC_CMB_KEYWORD)		 bSts = dwStatus & STS_KEY_CHANGE_GO;
	if (::GetKeyState(VK_SHIFT) < 0) bSts = !bSts;

	if (bSts) {
		_OnEnterKeyDown(ENTER_KEYDOWN_SELCHANGE);
	}
}



void CDonutSearchBar::OnLButtonDown(UINT nFlags, const CPoint& pt)
{
  #ifndef USE_DIET
	if (m_dwNoWordButton) {
		//+++ 単語ボタンなしの場合は旧処理
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);

		if (abs(rcKeyword.right - pt.x) > 5)
			return;

		SetCapture();
		::SetCursor(m_hCursor);
		m_ptDragStart = pt;
		m_ptDragHist  = pt;
	} else
  #endif
	{	//+++ 単語ボタンありの場合
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		if (abs(rcKeyword.right - pt.x) <= 5) {
			m_has = 1;
			SetCapture();
			::SetCursor(m_hCursor);
			m_ptDragStart = pt;
			m_ptDragHist  = pt;
			return;
		}
		CRect	rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		if (abs(rcEngine.right - pt.x) <= 5) {
			m_has = 2;
			SetCapture();
			::SetCursor(m_hCursor);
			m_ptDragStart = pt;
			m_ptDragHist  = pt;
			return;
		}
		m_has = 0;
	}
}



void CDonutSearchBar::OnLButtonUp(UINT nFlags, const CPoint& pt)
{
	if (::GetCapture() != m_hWnd)
		return;

	::ReleaseCapture();

  #ifndef USE_DIET
	if (m_dwNoWordButton) {	//+++ 単語ボタンなしのとき
		UpdateLayout(pt);
	} else
  #endif
	{	//+++ 単語ボタンありの場合
		if (m_has == 1)
			UpdateLayout(pt);
		else if (m_has == 2)
			UpdateLayout2(pt);
	}
}



void CDonutSearchBar::UpdateLayout(const CPoint& pt)
{
  #ifndef USE_DIET
	if (m_dwNoWordButton) { // 単語ボタン無しの場合
		int		btnW   = btnWidth();		//+++
		int 	nMoveX = m_ptDragStart.x - pt.x;
		CRect	rcKeyword;

		m_cmbKeyword.GetWindowRect(&rcKeyword);
		ScreenToClient(&rcKeyword);
		rcKeyword.right -= nMoveX;
	  #if 1	//+++
		if (rcKeyword.right < rcKeyword.left+btnW) {
			nMoveX			= rcKeyword.right - rcKeyword.left+btnW;
			rcKeyword.right = rcKeyword.left  + btnW;
		}
	  #endif

		CRect rcEngine;
		m_cmbEngine.GetWindowRect(&rcEngine);
		ScreenToClient(&rcEngine);
		rcEngine.left	-= nMoveX;

	  #if 1	//+++
		if (rcEngine.left > rcEngine.right - btnW) {
			rcEngine.left   = rcEngine.right - btnW;
			rcKeyword.right = rcEngine.left-2;
		}
		m_nEngineWidth = rcEngine.right - rcEngine.left;
	  #endif

		if (rcEngine.left >= rcEngine.right)
			return;

		if (rcKeyword.left >= rcKeyword.right)
			return;

		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER);
		m_cmbEngine.SetWindowPos(NULL, rcEngine, SWP_NOZORDER);

		m_ptDragStart	 = pt;
		UpdateWindow();
	} else
  #endif
	{
		//+++ 単語ボタンありの場合
		ResizeBar(pt.x - m_ptDragStart.x, 0);
		m_ptDragStart	 = pt;
		UpdateWindow();
	}
}



void CDonutSearchBar::UpdateLayout2(const CPoint& pt)
{
	ResizeBar(0, pt.x - m_ptDragStart.x);
	m_ptDragStart	 = pt;
	UpdateWindow();
}


//public: //+++ あとでprivateに戻すがとりあえずテスト.
void CDonutSearchBar::_AddToSearchBoxUnique(const CString &strURL)
{
	// search the same string
	int nCount = m_cmbKeyword.GetCount();

	for (int n = 0; n < nCount; ++n) {
		CString 	str;
		MtlGetLBTextFixed(m_cmbKeyword, n, str);

		if (strURL == str) {
			m_cmbKeyword.DeleteString(n);
			break;
		}
	}

	m_cmbKeyword.InsertString(0, strURL);
	m_cmbKeyword.SetCurSel(0);
}


//private:
void CDonutSearchBar::_OnEnterKeyDown(int flag)
{
	CString  str;

	int nIndexCmb = m_cmbKeyword.GetCurSel();
	if (nIndexCmb == -1)
		str = MtlGetWindowText(m_cmbKeyword);
	else
		MtlGetLBTextFixed(m_cmbKeyword.m_hWnd, nIndexCmb, str);

	//	m_cmbKeyword.GetLBText(nIndexCmb, str);

	GetHilightBtnFlag();				//+++ ハイライトボタンの状態チェック
	checkToolBarWords();				//+++

	if ( !str.IsEmpty() ) {
		SHORT sShift = ::GetKeyState(VK_SHIFT);
		SHORT sCtrl  = ::GetKeyState(VK_CONTROL);

		if (sShift >= 0 && sCtrl >= 0) {
			_OnCommand_SearchWeb(str);
		} else {
		  #if 1 //+++ カーソル位置の単語だけを選択するようにしてみる.
			str = _GetSelectText_OnCursor( GetEditCtrl() );
		  #else
			str = _GetSelectText( GetEditCtrl() );
		  #endif

			if (sCtrl < 0)
				SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), TRUE );
			else if (sShift < 0)
				SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), FALSE);
		}
	} else {
		m_cmbEngine.ShowDropDown(FALSE); //minit
	}
}



LRESULT CDonutSearchBar::OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;

	if (g_bNoReposition == true)
		return 0;

	CRect	rcDesktop;
	::GetWindowRect(::GetDesktopWindow(), &rcDesktop);

	CRect	rcWnd;
	::GetWindowRect( (HWND) lParam, &rcWnd );

	if (rcDesktop.right <= rcWnd.right) {
		int    nDiff = rcWnd.right - rcDesktop.right;
		rcWnd.left	-= nDiff;
		rcWnd.right -= nDiff;
		::SetWindowPos( (HWND) lParam, NULL, rcWnd.left, rcWnd.top, 0, 0, SWP_NOSIZE );
	}

	return 0;
}



#if 0
void CDonutSearchBar::OnItemSelected(const CString& str, const CString& strSearchEng)
{
	int 	nTarCate	 = m_cmbEngine.GetCurSel();
	if (nTarCate == -1)
		return;

	//x CString strSearchEng = MtlGetWindowText(m_cmbEngine);

	BOOL	bFirst		 = TRUE;
	int 	nLoopCtn	 = 0;
	OpenSearch(str, strSearchEng, nLoopCtn, bFirst);
}
#endif


//public:
CString CDonutSearchBar::GetSearchIniPath()
{
	CIniFileI pr( g_szIniFileName, _T("Search") );
	CString 	strPath = pr.GetStringUW(_T("Path"));
	pr.Close();

	if ( strPath.IsEmpty() ) {
		strPath = _GetFilePath( _T("Search\\Search.ini") );

		if (::GetFileAttributes(strPath) == 0xFFFFFFFF)
			strPath = _GetFilePath( _T("Search.ini") ); 		//以前の仕様でGo
	}

	return strPath;
}


//public:
//+++ ".url"中に記述した拡張情報での検索に対応するため、修正.
void CDonutSearchBar::OpenSearch(CString strWord, CString strSearchEng, int &nLoopCnt, BOOL &bFirst)
{
	if (m_bUseShortcut)
		ShortcutSearch(strWord, strSearchEng);

	if ( strSearchEng.IsEmpty() ) {
		m_cmbEngine.GetLBText(0, strSearchEng);
		if (strSearchEng.IsEmpty())
			return;
	}

	CString 	strSearchPath = GetSearchIniPath();

  #if 1	//+++ 	search.iniがない場合は、Googleで適当にサーチする.
	if (Misc::IsExistFile(strSearchPath) == 0 && strSearchEng == ENGINENAME_FOR_NO_SEARCH_INI) {
		if (m_bFiltering)
			FilterString(strWord);		//全角スペースの置換
		EncodeString(strWord, ENCODE_UTF8);
		DonutOpenFile(m_hWnd, _T("http://www.google.co.jp/search?num=100&q=") + strWord, 0);
		return;
	}
  #endif

	DWORD		dwStatus;
	{
		CIniFileI	pr( g_szIniFileName, _T("Search") );
		dwStatus		= pr.GetValue( _T("Status"), 0 );
		m_bActiveWindow = (dwStatus & STS_ACTIVEWINDOW) != 0;
	}

	CIniFileI		pr(strSearchPath, strSearchEng);
	if (pr.GetValue(_T("Group"), 0 ) != 0) {
		pr.Close();
		OpenSearchGroup(strWord, strSearchEng, nLoopCnt, bFirst);
	} else {
		CString 	strOpenURL;
		if (GetOpenURLstr(strOpenURL, strWord, pr, CString()) == false)
			return;
		pr.Close();
		DWORD		 dwOpenFlags = 0;
		if (bFirst) {
			dwOpenFlags |= D_OPENFILE_ACTIVATE;
			bFirst		 = FALSE;
		}

		if (m_bActiveWindow) {
			dwOpenFlags |= D_OPENFILE_NOCREATE;
		}

		//DonutOpenFile(m_hWnd, strOpenURL, dwOpenFlags);
		_EXPROP_ARGS	args;
		args.strUrl 	   = strOpenURL;
		args.dwOpenFlag    = dwOpenFlags;
		args.strIniFile    = strSearchPath;
		args.strSection    = strSearchEng;
	  #if 1	//+++
		args.strSearchWord = RemoveShortcutWord( GetSearchStr() );
	  #else
		args.strSearchWord = RemoveShortcutWord( MtlGetWindowText( GetEditCtrl() ) );
	  #endif
		::SendMessage(GetTopLevelParent(), WM_OPEN_WITHEXPROP, (WPARAM) &args, 0);
	}
}



bool CDonutSearchBar::GetOpenURLstr(CString& strOpenURL, const CString& strWord0, CIniFileI& pr, const CString& frontURL0)
{
	//検索URLの読み込み
	CString 	 strFrontURL	 = pr.GetString( _T("FrontURL") );
	if ( strFrontURL.IsEmpty() ) {
		if (frontURL0.IsEmpty())
			return false;
		strFrontURL = frontURL0;
	}
	CString 	 strBackURL 	 = pr.GetString( _T("BackURL") );

	//検索付加キーワードの読み込み
	CString 	 strFrontKeyWord = pr.GetString( _T("FrontKeyWord") );
	CString 	 strBackKeyWord  = pr.GetString( _T("BackKeyWord") );

	//検索語の作成
	CString strWord = strFrontKeyWord + strWord0 + strBackKeyWord;

	if (m_bFiltering)
		FilterString(strWord);		//全角スペースの置換

	DWORD		 dwEncode	 = pr.GetValue(_T("Encode"), 0);	//エンコード
	pr.Close();
	if (dwEncode != 0)
		EncodeString(strWord, dwEncode);

	strOpenURL.Format(_T("%s%s%s"), strFrontURL, strWord, strBackURL);
	return true;
}



//private:
void CDonutSearchBar::OpenSearchGroup(const CString& strWord, const CString& strSearchEng, int &nLoopCnt, BOOL &bFirst)
{
	nLoopCnt++;

	if (nLoopCnt > 10)
		return;

	CString 	strSearchPath = GetSearchIniPath();
	CIniFileI	pr(strSearchPath, strSearchEng);

	DWORD		dwListCnt	  = pr.GetValue( _T("ListCount"), 0 );

	for (int ii = 1; ii <= (int) dwListCnt; ii++) {
		CString 	strKey;
		strKey.Format(_T("%02d"), ii);
		CString 	strSearchEng2 = pr.GetStringUW( strKey );
		if ( strSearchEng2.IsEmpty() )
			continue;

		OpenSearch(strWord, strSearchEng2, nLoopCnt, bFirst);
	}
}



void CDonutSearchBar::EncodeString(CString &str, int dwEncode)	//minit
{
  #if 1	//+++ Unicode対応で作り直し
	if	 (dwEncode == ENCODE_SHIFT_JIS)
		str = Misc::urlstr_encode( Misc::tcs_to_sjis(str) );
	else if (dwEncode == ENCODE_EUC)
		str = Misc::urlstr_encode( Misc::tcs_to_eucjp(str) );
	else if (dwEncode == ENCODE_UTF8)
		str = Misc::urlstr_encode( Misc::tcs_to_utf8(str) );
	else
		return;
  #else
	CURLEncoder enc;

	if (dwEncode == 0)
		return;
	else if (dwEncode == ENCODE_SHIFT_JIS)
		enc.URLEncode_SJIS(str);
	else if (dwEncode == ENCODE_EUC)
		enc.URLEncode_EUC(str);
	else if (dwEncode == ENCODE_UTF8)
		enc.URLEncode_UTF8(str);
	else
		return;
	//ATLASSERT(FALSE);
  #endif
}



void CDonutSearchBar::ShortcutSearch(CString &strWord, CString &strSearchEng)
{
	CString 	strSearchPath = GetSearchIniPath();

	if ( strWord.Left(1) == _T("\\") ) {
		DWORD	dwListCount = 0;
		int 	nFind		= strWord.Find(_T(" "));
		CString strShort	= strWord.Mid(1, nFind - 1);

		{
			CIniFileI	pr( strSearchPath, _T("Search-List") );
			pr.QueryValue( dwListCount, _T("ListCount") );
			//x pr.Close();
		}

		if ( strShort.IsEmpty() )
			return;

		strWord = strWord.Mid(nFind + 1);

		CString 	strBuf;
		CString 	strKey;
		int 		nListCount	= dwListCount;

		for (int i = 1; i <= nListCount; i++) {
			//エンジン名を取得
			strKey.Format(_T("%02d"), i);
			CIniFileI		pr( strSearchPath, _T("Search-List") );
			CString 		strEngine = pr.GetStringUW( strKey );
			pr.Close();
			//ショートカットコードを取得
			CString strShortcutWord = GetShortcutWord(strEngine);

			//比較
			if (strShort == strShortcutWord) {
				strSearchEng = strEngine;
				return;
			}
		}
	}
}



CString CDonutSearchBar::GetShortcutWord(const CString& strSearchEng)
{
	CIniFileI	pr(GetSearchIniPath(), strSearchEng);
	return		pr.GetString( _T("ShortCutCode") );
}



void CDonutSearchBar::_InitialEngine(LPVOID lpV)
{
	CComboBox	cmb( (HWND) lpV );

	cmb.ResetContent(); 	//minit
	//::WINNLSEnableIME(cmb,FALSE);

	CString 	strSearchPath 	= GetSearchIniPath();
	DWORD		dwListCnt		= 0;
	if (Misc::IsExistFile(strSearchPath)) {
		CIniFileI	pr( strSearchPath, _T("Search-List") );
		dwListCnt	= pr.GetValue( _T("ListCount"), 0 );
		CString 	strKey;
		for (int ii = 1; ii <= (int) dwListCnt; ii++) {
			strKey.Format(_T("%02d"), ii);
			CString 	strTitle = pr.GetStringUW( strKey );
			if ( strTitle.IsEmpty() )
				continue;

			cmb.AddString(strTitle);
		}

		pr.Close();
	} else {
		cmb.AddString(ENGINENAME_FOR_NO_SEARCH_INI);
	}

	DWORD		dwStatus	  = 0, dwIndex = 0;
	{
		CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwStatus, _T("Status") );
		pr.QueryValue( dwIndex , _T("SelIndex") );
	}

	int 		nSelIndex	  = 0;
	if ((dwStatus & STS_LASTSEL) && dwIndex < dwListCnt) {
		nSelIndex = dwIndex;
	}

	cmb.SetCurSel(nSelIndex);
}



void CDonutSearchBar::_InitialKeyword(LPVOID lpV)
{
	CComboBox	cmb( (HWND) lpV );

	{
		DWORD		dwStatus	 = 0;
		CIniFileI	pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwStatus, _T("Status") );
		//x pr.Close();
		if (dwStatus & STS_NO_REPOSITION)
			g_bNoReposition = true;
		if ( (dwStatus & STS_HISTORY_SAVE) != STS_HISTORY_SAVE )
			return;
	}

	DWORD		dwHistoryCnt = 0;
	CIniFileI	pr( _GetFilePath( _T("WordHistory.ini") ), _T("SEARCH_HISTORY") );
	pr.QueryValue( dwHistoryCnt, _T("HistorySaveCnt") );

	for (int ii = 0; ii < (int) dwHistoryCnt; ii++) {
		CString 	strKey;
		strKey.Format(_T("KEYWORD%d"), ii);
		CString 	strKeyWord = pr.GetStringUW( strKey );
		if ( strKeyWord.IsEmpty() )
			continue;

		cmb.AddString(strKeyWord);
	}
}



// public:
BYTE CDonutSearchBar::PreTranslateMessage(MSG *pMsg)
{
  #if 1	//+++ 手抜きな、描画更新チェック
	SetCmbKeywordEmptyStr();		//+++ キーワード窓にエンジン名を表示するためのフォーカスチェック
  #endif

	UINT msg  = pMsg->message;
	int  vKey =  (int) pMsg->wParam;

	if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
		if ( !IsWindowVisible() || !IsChild(pMsg->hwnd) )									// ignore
			return _MTL_TRANSLATE_PASS;

		// left or right pressed, check shift and control key.
		if (	vKey == VK_UP || vKey == VK_DOWN || vKey == VK_LEFT || vKey == VK_RIGHT
			 || vKey == VK_HOME || vKey == VK_END
			 || vKey == (0x41 + 'C' - 'A')
			 || vKey == (0x41 + 'V' - 'A')
			 || vKey == (0x41 + 'X' - 'A')
			 || vKey == VK_INSERT)
		{
			if (::GetKeyState(VK_SHIFT) < 0 || ::GetKeyState(VK_CONTROL) < 0)
				return _MTL_TRANSLATE_WANT; 												// pass to edit control
		}

		// return key have to be passed
		if (vKey == VK_RETURN) {
			return _MTL_TRANSLATE_WANT;
		}

		// other key have to be passed
		if (VK_LBUTTON <= vKey && vKey <= VK_HELP) {
			BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;

			if (!bAlt && ::GetKeyState(VK_SHIFT) >= 0 && ::GetKeyState(VK_CONTROL) >= 0)	// not pressed
				return _MTL_TRANSLATE_WANT; 												// pass to edit control
		}
	}
  #if 1	//+++	とりあえず、むりやり、右クリックがWEB検索ボタンの範囲内で押された場合、検索エンジンメニューを出す
	else if (msg == WM_RBUTTONUP) {
		CRect		rect;
		if (m_wndToolBar.GetRect(ID_SEARCH_WEB, rect)) {
			m_wndToolBar.ClientToScreen( rect );
			POINT 	pt;
			::GetCursorPos(&pt);
			if (pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top && pt.y < rect.bottom) {
				//MtlSendCommand(m_hWnd, ID_SEARCHENGINE_MENU);
				BOOL	dmy=0;
				OnSearchEngineMenu(0,0,0,dmy);
				return _MTL_TRANSLATE_HANDLE;
			}
		}
	}
  #endif
  #if 1 //*+++	強引対処:検索バーにカーソルがある状態でCTRL+RETURNで頁内次検索をしたとき、
		//		どこかの処理がCTRL+ENTERでエラー音を出しているようなのだが、誰が犯人か
		//		わからないので、ここで強引にキーを食って誤魔化す.
	else if (msg == WM_CHAR && (vKey == VK_RETURN || vKey == 0x0a) && ::GetKeyState(VK_CONTROL) < 0) {
		if ( !IsWindowVisible() || !IsChild(pMsg->hwnd) )									// ignore
			return _MTL_TRANSLATE_PASS;
		return _MTL_TRANSLATE_HANDLE;
	}
  #endif

	return _MTL_TRANSLATE_PASS;
}



#if 0	//+++ WEB検索ボタンで右クリックしたら検索エンジンメニューを表示してみる
void CDonutSearchBar::OnToolBarRButtonUp(UINT nFlags, const CPoint& pt)
{
	CRect		rect;
	m_wndToolBar.GetRect(0, rect);
	if (pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top && pt.y < rect.bottom) {
		//MtlSendCommand(m_hWnd, ID_SEARCHENGINE_MENU);
		BOOL	dmy=0;
		OnSearchEngineMenu(0,0,0,dmy);
	}
}
#endif



CEdit CDonutSearchBar::GetEditCtrl()
{
	return CEdit( m_cmbKeyword.GetDlgItem(IDC_EDIT/*1001*/) );
}



void CDonutSearchBar::SetFocusToEngine()
{
	::SetFocus(m_cmbEngine.m_hWnd);
	m_cmbEngine.ShowDropDown(TRUE);
}



//private:
void CDonutSearchBar::SaveHistory()
{
	DWORD		dwStatus		= 0;
	DWORD		dwHistoryMaxCnt = 10;

	{
		CIniFileI pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwStatus, _T("Status") );
		pr.QueryValue( dwHistoryMaxCnt, _T("HistoryCnt") );
	}

	DWORD		dwItemCount 	= m_cmbKeyword.GetCount();

	if (dwItemCount > dwHistoryMaxCnt)
		dwItemCount = dwHistoryMaxCnt;

	CString 	strFileName 	= _GetFilePath( _T("WordHistory.ini") );
	CString 	strSection		= _T("SEARCH_HISTORY");
	CIniFileO	 pr(strFileName, strSection);
	pr.DeleteSection();
	pr.SetValue( dwItemCount, _T("HistorySaveCnt") );

	if (dwStatus & STS_HISTORY_SAVE) {
		for (int ii = 0; ii < (int) dwItemCount; ii++) {
			CString 	strKeyWord;
			MtlGetLBTextFixed(m_cmbKeyword.m_hWnd, ii, strKeyWord);

			CString 	strKey;
			strKey.Format(_T("KEYWORD%d"), ii);
			pr.SetStringUW(strKeyWord, strKey);
		}
	}
}



//public:
DROPEFFECT CDonutSearchBar::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/)
{
	if (m_bDragFromItself)
		return DROPEFFECT_NONE;

	_DrawDragEffect(false);

	m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
	return _MtlStandardDropEffect(dwKeyState);
}



DROPEFFECT CDonutSearchBar::OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint /*point*/, DROPEFFECT dropOkEffect)
{
	if (m_bDragFromItself || !m_bDragAccept)
		return DROPEFFECT_NONE;

	return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
}



void CDonutSearchBar::OnDragLeave()
{
	if (m_bDragFromItself)
		return;

	_DrawDragEffect(true);
}



//private:
void CDonutSearchBar::_DrawDragEffect(bool bRemove)
{
	CClientDC dc(m_wndKeyword.m_hWnd);

	CRect	  rect;

	m_wndKeyword.GetClientRect(rect);

	if (bRemove)
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(0, 0), &rect, CSize(2, 2), NULL, NULL);
	else
		MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(2, 2), NULL, CSize(2, 2),	NULL, NULL);
}



//public:
DROPEFFECT CDonutSearchBar::OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint /*point*/)
{
	if (m_bDragFromItself)
		return DROPEFFECT_NONE;

	_DrawDragEffect(true);

	DWORD		dwStatus = 0;
	{
		CIniFileI pr( g_szIniFileName, _T("SEARCH") );
		pr.QueryValue( dwStatus, _T("Status") );
		//x pr.Close(); 	//+++
	}

	CString 	strText;

	if (   MtlGetHGlobalText( pDataObject, strText)
		|| MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		CEdit edit = GetEditCtrl();
		edit.SendMessage(WM_CHAR, 'P'); //m_cmbKeyword.GetCurSel() == -1にするための苦肉の策 minit
		edit.SetWindowText(strText);

		BOOL  bSts = dwStatus & STS_DROP_GO;

		if (::GetKeyState(VK_SHIFT) < 0)
			bSts = !bSts;

		if (bSts) {
			_OnCommand_SearchWeb(strText);
		}

		return DROPEFFECT_COPY;
	}

	return DROPEFFECT_NONE;
}



void CDonutSearchBar::SetFont(HFONT hFont, BOOL bRedraw /*= TRUE*/)
{
	int		h		= GetFontHeight( hFont ); 	//+++
	m_nItemFontH	= h > 20 ? h+1 : 20+1;		//+++
	m_nDefDlgH		= h > 20 ? h+2 : 20+2;		//+++
	CDialogImpl<CDonutSearchBar>::SetFont(hFont, bRedraw);
	m_cmbEngine.SetFont(hFont, bRedraw);
	m_cmbKeyword.SetFont(hFont, bRedraw);
	m_wndKeyword.SetFont(hFont, bRedraw);
	if (m_wndToolBar.m_hWnd)
		m_wndToolBar.SetFont(hFont, bRedraw);

	_InitCombo();								//+++
}



//private:
void CDonutSearchBar::_SetVerticalItemCount(CComboBox &cmb, int count)
{
	CRect rc;
	cmb.AddString(_T("DUMMY"));
	int   itemheight = cmb.GetItemHeight(0);
	cmb.DeleteString(0);
	cmb.GetClientRect(&rc);
	int dh	  = (itemheight > m_nDefDlgH) ? itemheight : m_nDefDlgH;
	rc.bottom = rc.top + dh + (itemheight/3) +itemheight * count + 2;	//+++ だいたいの感じになるように、適当に計算
	cmb.MoveWindow(&rc);
}



void CDonutSearchBar::_InitCombo()						//minit
{
	CIniFileI pr( g_szIniFileName, _T("SEARCH")    );
	DWORD	dwStatus	= pr.GetValue( _T("Status")   , 0 );
	DWORD	dwHeightCnt = pr.GetValue( _T("HeightCnt"), DEFAULT_HEIGHTCOUNT );
	pr.Close();

	m_cmbEngine.SetItemHeight(-1, m_nItemFontH);
	m_cmbKeyword.SetItemHeight(-1, m_nItemFontH);

	if (dwStatus & STS_HEIGHTCOUNT) {
		int HeightCount = (int) dwHeightCnt;

		if (HeightCount < 1 || MAXHEIGHTCOUNT < HeightCount)
			HeightCount = DEFAULT_HEIGHTCOUNT;

		_SetVerticalItemCount(m_cmbEngine , HeightCount);
		_SetVerticalItemCount(m_cmbKeyword, HeightCount);
	}
  #if 1	//+++ vista以外でコンボボックスの高さがおかしい件を強制的に回避してみる...
	else {
		_SetVerticalItemCount(m_cmbEngine , MAXHEIGHTCOUNT);
		_SetVerticalItemCount(m_cmbKeyword, DEFAULT_HEIGHTCOUNT/*50*/);
	}
  #endif

	if (dwStatus & STS_TEXT_FILTER)
		m_bFiltering = TRUE;
}



//public:
void CDonutSearchBar::ShowToolBarIcon(BOOL flag)
{
	if (flag) {
		if (m_bLoadedToolBar) {
			if ( ::IsWindow(m_wndToolBar.m_hWnd) )
				m_wndToolBar.ShowWindow(SW_NORMAL);
		} else {
			InitToolBar(/*+++  IDB_SEARCHBUTTON, IDB_SEARCHBUTTON_HOT,*/ m_nBmpCX, m_nBmpCY, RGB(255, 0, 255) );
			m_wndToolBar.ShowWindow(SW_NORMAL);
		}
	} else {
		if (m_bLoadedToolBar) {
			m_wndToolBar.ShowWindow(SW_HIDE);
		}
	}

	m_bShowToolBar = flag;

	//サイズ変更
	CRect	rect;
	GetWindowRect(rect);
	CWindow( GetParent() ).ScreenToClient(rect);
	int 	width	= rect.right  - rect.left - 1;
	int 	height	= rect.bottom - rect.top;
	SetWindowPos(NULL, rect.left, rect.top, width,	 height, SWP_NOZORDER | SWP_NOREDRAW);
	SetWindowPos(NULL, rect.left, rect.top, width+1, height, SWP_NOZORDER);
}



void CDonutSearchBar::SearchHilight()
{
  #if 1 //+++
	CString str  = GetSearchStr();
  #else
	CEdit	edit = GetEditCtrl();
	CString str  = MtlGetWindowText(edit);
  #endif
	str = RemoveShortcutWord(str);
	//if (! str.IsEmpty())
		_OnCommand_SearchHilight(str);
}



void CDonutSearchBar::SearchPage(BOOL bForward)
{
	_OnCommand_SearchPage(bForward);
}



BOOL CDonutSearchBar::GetToolIconState()
{
	return m_bShowToolBar;
}



void CDonutSearchBar::ReloadSkin(int nCmbStyle)
{
	SetComboStyle(nCmbStyle);

	if ( !m_wndToolBar.IsWindow() )
		return;

	m_bExistManifest	= IsExistManifestFile();				//+++

	CImageList	imgs	= m_wndToolBar.GetImageList();
	CImageList	imgsHot = m_wndToolBar.GetHotImageList();
	CImageList	imgsDis = m_wndToolBar.GetDisabledImageList();	//+++

	_ReplaceImageList(GetSkinSeachBarPath(0), imgs	 , IDB_SEARCHBUTTON);
	_ReplaceImageList(GetSkinSeachBarPath(1), imgsHot, IDB_SEARCHBUTTON_HOT);

  #if 1 //+++ Disabled画像の対応.
	CString str = GetSkinSeachBarPath(2);
	int 	dis = 0;
	if (Misc::IsExistFile(str) == 0) {						//+++ 画像ファイルがない時
		if (Misc::IsExistFile(GetSkinSeachBarPath(0))) {	//+++ 通常があれば
			str = GetSkinSeachBarPath(0);					//+++ 通常画で代用
		} else {											//+++ 通常もなければ
			dis = IDB_SEARCHBUTTON_DIS; 					//+++ デフォルトのDisable画を使う.
		}
	}
	_ReplaceImageList(str, imgsDis, dis);					//+++
  #endif

	InvalidateRect(NULL, TRUE);
	m_wndToolBar.InvalidateRect(NULL, TRUE);
}



void	CDonutSearchBar::SetComboStyle(int nCmbStyle)
{
	m_cmbEngine.SetDrawStyle (nCmbStyle);
	m_cmbKeyword.SetDrawStyle(nCmbStyle);
}




//=========================================================================

///+++ エンジン名の取得.
const CString& 	CDonutSearchBar::GetSearchEngineStr()	//+++
{
	HWND	hWnd	= m_cmbEngine;

	//+++ なるべく、アロケートが発生しないようにしてみる.
	enum { NAME_LEN = 0x1000 };
	TCHAR	name[ NAME_LEN ];
	int 	nLen	= ::GetWindowTextLength(hWnd);
	if (nLen >= NAME_LEN)
		nLen	 = NAME_LEN - 1;
	name[0]		 = '\0';
	int 	nRetLen = ::GetWindowText(hWnd, name, nLen + 1);
	name[nLen+1] = '\0';
	if (_tcscmp(name, LPCTSTR(m_strEngine)) != 0) {
		m_strEngine = name;
	}
	return m_strEngine;
}



CMenuHandle CDonutSearchBar::GetSearchEngineMenuHandle()
{
  #if 1	//+++
	CMenu		menu0;
	MakeSearchEngineMenu(menu0);
	CMenuHandle menu = menu0.GetSubMenu(0);
	menu0.RemoveMenu(0, MF_BYPOSITION);
	return 		menu;
  #else
	if (m_engineMenu.m_hMenu == 0)
		MakeSearchEngineMenu(m_engineMenu);
	if (m_engineMenu.m_hMenu == 0)
		return 0;
	CMenuHandle menu = m_engineMenu.GetSubMenu(0);
	return menu;
  #endif
}


//+++ サーチエンジンメニューを作る
bool CDonutSearchBar::MakeSearchEngineMenu(CMenu& menu0)
{
	menu0.LoadMenu(IDR_SEARCHENGINE_MENU);
	if (menu0.m_hMenu == NULL)
		return 0;
	CMenuHandle menu = menu0.GetSubMenu(0);
	if (menu.m_hMenu == NULL)
		return 0;
	menu.DeleteMenu(0, MF_BYPOSITION );
	ATLASSERT( menu.GetMenuItemCount() == 0 );
	unsigned 	num = m_cmbEngine.GetCount();
	if (num > ID_INSERTPOINT_SEARCHENGINE_END+1 - ID_INSERTPOINT_SEARCHENGINE)
		num = ID_INSERTPOINT_SEARCHENGINE_END+1 - ID_INSERTPOINT_SEARCHENGINE;
	for (unsigned i = 0; i < num; ++i) {
		CString 		  strName;
		MtlGetLBTextFixed(m_cmbEngine, i, strName);
		menu.AppendMenu(MF_ENABLED | MF_STRING , ID_INSERTPOINT_SEARCHENGINE + i, LPCTSTR(strName));
	}
	return 1;
}



//+++
bool CDonutSearchBar::OnSearchEngineMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
 #if 1	//+++
	// ポップアップメニューを開く.
	::SetForegroundWindow(m_hWnd);
	POINT 	pt;
	::GetCursorPos(&pt);
	CMenuHandle 	menu = GetSearchEngineMenuHandle();
	HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
	if (hr < ID_INSERTPOINT_SEARCHENGINE || hr > ID_INSERTPOINT_SEARCHENGINE_END)
		return 0;

	// 選択されたものからサーチエンジン名を取得.
	hr -= ID_INSERTPOINT_SEARCHENGINE;
	CString 	strEngine;
	if (menu.GetMenuString(hr, strEngine, MF_BYPOSITION) == 0)
		return 0;

	// 選択されたエンジンで、現在の検索文字列をweb検索.
	CString		strKeyword = GetSearchStr();
	SearchWeb_str_engine(strKeyword, strEngine );
	menu.DestroyMenu();
	return 1;
 #else
	// サーチエンジンメニューを用意.
	GetSearchEngineMenu();

	// ポップアップメニューを開く.
	::SetForegroundWindow(m_hWnd);
	POINT 	pt;
	::GetCursorPos(&pt);
	CMenuHandle 	menu = m_engineMenu.GetSubMenu(0);
	HRESULT hr = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd, NULL);
	if (hr <= 0 || hr > menu.GetMenuItemCount())
		return 0;

	// 選択されたものからサーチエンジン名を取得.
	hr -= 1;
	CString 	strEngine;
	if (menu.GetMenuString(hr, strEngine, MF_BYCOMMAND) == 0)
		return 0;

	// 選択されたエンジンで、現在の検索文字列をweb検索.
	SearchWeb_str_engine( GetSearchStr(), strEngine );
	return 1;
 #endif
}



//=========================================================================
//++++



#if SEARCH_JIKKEN == 1


LRESULT CDonutSearchBar::OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMREBARCHEVRON lpnm = (LPNMREBARCHEVRON) pnmh;

	if ( lpnm->wID != GetDlgCtrlID() ) {
		bHandled = FALSE;
		return 1;
	}

	CMenuHandle 	 menu = PrepareChevronMenu();
	DisplayChevronMenu(menu, lpnm);
	CleanupChevronMenu(menu, lpnm);

	return 0;
}


CMenuHandle CDonutSearchBar::PrepareChevronMenu()
{
#if 0
	CMenuHandle menuCmdBar(m_wndToolBar.m_hMenu);

	// build a menu from hidden items
	CMenuHandle menu;
	BOOL		bRet = menu.CreatePopupMenu();

	ATLASSERT(bRet);
	RECT		rcClient = {0};
	bRet = GetClientRect(&rcClient);
	ATLASSERT(bRet);
	int client_right = rcClient.right;
	unsigned	num = m_arrBtn.GetSize();
	for (unsigned i = 0; i < num; ++i) {
		CCmdBarButton cbb	   = m_arrBtn[i];
		bool		  bEnabled = ( (cbb.m_fsState & CBSTATE_ENABLED) != 0 );

		int cbb_btn_right = cbb.m_rcBtn.right;
		if (cbb_btn_right > client_right) {
			TCHAR			szBuff[100];
			CMenuItemInfo	mii;
			mii.fMask	   = MIIM_TYPE | MIIM_SUBMENU;
			mii.dwTypeData = szBuff;
			mii.cch 	   = sizeof (szBuff) / sizeof (TCHAR);
			bRet		   = menuCmdBar.GetMenuItemInfo(i, TRUE, &mii);
			ATLASSERT(bRet);
			// Note: CmdBar currently supports only drop-down items
			ATLASSERT( ::IsMenu(mii.hSubMenu) );
			bRet		   = menu.AppendMenu( MF_STRING|MF_POPUP|(bEnabled ? MF_ENABLED : MF_GRAYED),
											 (UINT_PTR) mii.hSubMenu,
											 mii.dwTypeData );
			ATLASSERT(bRet);
		}
	}
	if (menu.m_hMenu && menu.GetMenuItemCount() == 0) { // no hidden buttons after all
		menu.DestroyMenu();
		return NULL;
	}

	return menu;
#else
	return NULL;
#endif
}


void CDonutSearchBar::DisplayChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm)
{
	if (menu.m_hMenu == 0)
		return;

	// convert chevron rect to screen coordinates
	CWindow   wndFrom	 = lpnm->hdr.hwndFrom;
	RECT	  rc		 = lpnm->rc;

	wndFrom.ClientToScreen(&rc);
	// set up flags and rect
	UINT		uMenuFlags = TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN
						   | (!AtlIsOldWindows() ? TPM_VERPOSANIMATION : 0);
	TPMPARAMS	TPMParams;
	TPMParams.cbSize	= sizeof (TPMPARAMS);
	TPMParams.rcExclude = rc;
	::TrackPopupMenuEx(menu.m_hMenu, uMenuFlags, rc.left, rc.bottom, m_wndParent, &TPMParams);
}


void CDonutSearchBar::CleanupChevronMenu(CMenuHandle menu, LPNMREBARCHEVRON lpnm)
{
	if (menu.m_hMenu) {
		for (int i = menu.GetMenuItemCount() - 1; i >= 0; i--)
			menu.RemoveMenu(i, MF_BYPOSITION);
	}
	menu.DestroyMenu();
	CWindow wndFrom = lpnm->hdr.hwndFrom;
	RECT	rc		= lpnm->rc;
	wndFrom.ClientToScreen(&rc);
	MtlEatNextLButtonDownOnChevron(m_wndParent, rc);
}




#elif SEARCH_JIKKEN == 2


//+++
LRESULT CDonutSearchBar::OnChevronPushed(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
{
	ATLASSERT( ( (LPNMREBARCHEVRON) pnmh )->wID == GetDlgCtrlID() );

	if ( !PushChevron( pnmh, GetTopLevelParent() ) ) {
		bHandled = FALSE;
		return 1;
	}

	return 0;
}



//+++
HMENU CDonutSearchBar::ChevronHandler_OnGetChevronMenu(int nCmdID, HMENU &hMenuDestroy)
{
	bool		bDestroy = 0;
	bool		bSubMenu = 0;
	CMenuHandle menu = _GetDropDownMenu(nCmdID, bDestroy, bSubMenu);

	if (bDestroy)
		hMenuDestroy = menu.m_hMenu;

	if (bSubMenu)
		return menu.GetSubMenu(0);
	else
		return menu;
}



// Implemantation
HMENU CDonutSearchBar::_GetDropDownMenu(int nCmdID, bool &bDestroy, bool &bSubMenu)
{
	bDestroy = true;
	bSubMenu = false;
#if 0
	CEdit	edit = GetEditCtrl();
	CString str  = MtlGetWindowText(edit);

	switch (nCmdID) {
	case ID_SEARCH_WEB: 		_OnCommand_SearchWeb(str);								break;
	case ID_SEARCHBAR_HILIGHT:	_OnCommand_SearchHilight(str);							break;	//ID_SEARCH_HILIGHT:
	//+++ case ID_SEARCH_PAGE:	_OnCommand_SearchPage( (::GetKeyState(VK_SHIFT) < 0) ? FALSE : TRUE );	break;
	case ID_SEARCH_PAGE:		_OnCommand_SearchPage( (::GetKeyState(VK_SHIFT) >= 0)); break;
	case ID_SEARCHBAR_WORD00:
	case ID_SEARCHBAR_WORD01:
	case ID_SEARCHBAR_WORD02:
	case ID_SEARCHBAR_WORD03:
	case ID_SEARCHBAR_WORD04:
	case ID_SEARCHBAR_WORD05:
	case ID_SEARCHBAR_WORD06:
	case ID_SEARCHBAR_WORD07:
	case ID_SEARCHBAR_WORD08:
	case ID_SEARCHBAR_WORD09:	_OnCommand_SearchPage((::GetKeyState(VK_SHIFT) >= 0), nCmdID-ID_SEARCHBAR_WORD00);	break;
	default:					ATLASSERT(0);
	}
#endif
	return 0;
}


//+++
void	CDonutSearchBar::Chevronhandler_OnCleanupChevronMenu()
{
}
#endif
