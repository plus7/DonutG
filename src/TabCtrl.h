/**
 *	@file	TabCtrl.h
 *	@brief	MTL : タブ制御
 */
////////////////////////////////////////////////////////////////////////////
// MTL Version 0.10
// Copyright (C) 2001 MB<mb2@geocities.co.jp>
// All rights unreserved.
//
// This file is a part of Mb Template Library.
// The code and information is *NOT* provided "as-is" without
// warranty of any kind, either expressed or implied.
//
// TabCtrl.h: Last updated: February 12, 2001
/////////////////////////////////////////////////////////////////////////////

// April 10, 2004 minit added some

#pragma once

#include <vector>
#include "DropDownButton.h"
//#include "atltheme_d.h"
#include "option/SkinOption.h"
#include "DonutPFunc.h"
//スキン切り替えとかの機能を入れたので移植性とかかなり損なわれてます。
//ごめんなさい(minit)

namespace MTL {


// for debug
#ifdef _DEBUG
const bool _Mtl_TabCtrl2_traceOn = false;
	# define TCTRACE	if (_Mtl_TabCtrl2_traceOn)	ATLTRACE
#else
	# define TCTRACE
#endif


#define TOOLTIP_RELAY_EVENT(tip)																			\
	if (  uMsg == WM_LBUTTONDOWN || uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN	\
	   || uMsg == WM_MBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONUP)							\
	{																										\
		bHandled = FALSE;																					\
		MSG msg = { m_hWnd, uMsg, wParam, lParam }; 														\
		if ( tip.IsWindow() )																				\
			tip.RelayEvent(&msg);																			\
	}



/////////////////////////////////////////////////////////////////////////////
// Tab Bars

// Window Styles:
enum ETab2Ws {
	TAB2WS_TOP				= CCS_TOP,
	TAB2WS_BOTTOM			= CCS_BOTTOM,
	TAB2WS_NORESIZE 		= CCS_NORESIZE,
	TAB2WS_NOPARENTALIGN	= CCS_NOPARENTALIGN,
	TAB2WS_NODIVIDER		= CCS_NODIVIDER,
};


// Extended styles
enum ETab2_Ex {
	TAB2_EX_TRANSPARENT 	= 0x00000001L,	// not supported yet
	TAB2_EX_SHAREIMGLIST	= 0x00000002L,
	TAB2_EX_MULTILINE		= 0x00000004L,
	TAB2_EX_FIXEDSIZE		= 0x00000008L,
	TAB2_EX_SUPPORTREBAR	= 0x00000010L,
	TAB2_EX_SCROLLOPPOSITE	= 0x00000020L,
	TAB2_EX_ANCHORCOLOR 	= 0x00000040L,
	TAB2_EX_MOUSEDOWNSELECT = 0x00000080L,	//minit
};


// Command ID
enum { COMMAND_ID_DROPBUTTON= 0x00000001L, };




/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class T, class TBase, class TWinTraits>
class	CCommandBarCtrlImpl;
class	CTabCtrl2;



/////////////////////////////////////////////////////////////////////////////
// CTabButton

// CTabButton state flags
enum ETcistate {
	TCISTATE_HIDDEN 			= 0x01,
	// standard text state
	TCISTATE_ENABLED			= 0x02,
	TCISTATE_INACTIVE			= 0x04,
	// select or not
	TCISTATE_SELECTED			= 0x08, 	// ordianry selected
	TCISTATE_MSELECTED			= 0x10, 	// multi-selected
	// event state
	TCISTATE_PRESSED			= 0x20, 	// mouse pressed
	TCISTATE_HOT				= 0x40, 	// mouse above item
	TCISTATE_MOUSEAWAYCAPTURED	= 0x80, 	// mouse away but captured
};



class CTabSkin {
	CBitmap 	m_TabSkinCur;
	CBitmap 	m_TabSkinNone;
	CBitmap 	m_TabSkinSel;
	BOOL		m_bVisible;

	friend class CTabCtrlItem;

	COLORREF	m_colText;
	COLORREF	m_colActive,
				m_colInActive;
	COLORREF	m_colDisable,
				m_colDisableHi;
	COLORREF	m_colBtnFace,
				m_colBtnHi;
private:
	int 		m_nDrawStyle;
	CTheme &	m_Theme;


public:
	CTabSkin(CTheme& Theme)
		: m_bVisible(FALSE) , m_nDrawStyle(0) , m_Theme(Theme)
	{
	}


	void LoadTabSkin()
	{
		if (m_nDrawStyle != SKN_TAB_STYLE_DEFAULT) return;

		m_bVisible = TRUE;

		if (m_TabSkinCur.m_hBitmap) 	m_TabSkinCur.DeleteObject();
		if (m_TabSkinNone.m_hBitmap)	m_TabSkinNone.DeleteObject();
		if (m_TabSkinSel.m_hBitmap) 	m_TabSkinSel.DeleteObject();

		CString 	strTabSkinDir = _GetSkinDir();
		CString 	strTabSkinCur,
					strTabSkinNone,
					strTabSkinSel;
		strTabSkinCur.Format (_T("%sTabSkinCur.bmp") , strTabSkinDir);
		strTabSkinNone.Format(_T("%sTabSkinNone.bmp"), strTabSkinDir);
		strTabSkinSel.Format (_T("%sTabSkinSel.bmp") , strTabSkinDir);

		// 文字の色を読み込む minit
		LoadTabTextColors();

		// タブ・スキン
		m_TabSkinCur.Attach ( AtlLoadBitmapImage(strTabSkinCur.GetBuffer (0), LR_LOADFROMFILE) );
		m_TabSkinNone.Attach( AtlLoadBitmapImage(strTabSkinNone.GetBuffer(0), LR_LOADFROMFILE) );
		m_TabSkinSel.Attach ( AtlLoadBitmapImage(strTabSkinSel.GetBuffer (0), LR_LOADFROMFILE) );

		if (m_TabSkinCur.m_hBitmap	== NULL) m_bVisible = FALSE;
		if (m_TabSkinNone.m_hBitmap == NULL) m_bVisible = FALSE;
		if (m_TabSkinSel.m_hBitmap	== NULL) m_bVisible = FALSE;
	}


private:
	CString GetTabSkinDir()
	{
		return _GetSkinDir();
	}


	void LoadTabTextColors()
	{
		CString 	strTabSkinIni = Misc::GetExeDirectory() + _T("TabSkin.ini");

		m_colText	   = ::GetSysColor(COLOR_BTNTEXT);
		m_colActive    = RGB(128, 0, 128);
		m_colInActive  = RGB(  0, 0, 255);
		m_colDisable   = ::GetSysColor(COLOR_3DSHADOW  );
		m_colDisableHi = ::GetSysColor(COLOR_3DHILIGHT );
		m_colBtnFace   = ::GetSysColor(COLOR_BTNFACE   );
		m_colBtnHi	   = ::GetSysColor(COLOR_BTNHILIGHT);


		if (::GetFileAttributes(strTabSkinIni) == 0xFFFFFFFF) {
			strTabSkinIni = _GetSkinDir() + _T("TabSkin.ini");
		}

		CIniFileI	pr( strTabSkinIni, _T("Color") );
		_QueryColorString( pr, m_colText		, _T("Text") );
		_QueryColorString( pr, m_colActive		, _T("Active") );
		_QueryColorString( pr, m_colInActive	, _T("InActive") );
		_QueryColorString( pr, m_colDisable 	, _T("Disable") );
		_QueryColorString( pr, m_colDisableHi	, _T("DisableHilight") );
		_QueryColorString( pr, m_colBtnFace 	, _T("ButtonFace") );
		_QueryColorString( pr, m_colBtnHi		, _T("ButtonHilight") );
	}


public:
	BOOL IsVisible()
	{
		//+++ m_Theme.m_hThemeでなく関数経由に変更.
		return ( m_bVisible || (m_Theme.IsThemeNull() == 0 && m_nDrawStyle == SKN_TAB_STYLE_THEME) ) != 0;
	}


private:
	BOOL CanUseTheme()
	{
		//+++ m_Theme.m_hThemeでなく関数経由に変更.
		return m_Theme.IsThemeNull() == 0;
	}


	void DrawSkinCur(HDC hDC, CRect rcItem)
	{
		if (m_nDrawStyle == SKN_TAB_STYLE_THEME) {
			if ( CanUseTheme() ) {
				DrawThemeCur(hDC, rcItem);
			} else {
				DrawSkin(hDC, rcItem, &m_TabSkinCur);
			}
		} else
			DrawSkin(hDC, rcItem, &m_TabSkinCur);
	}


	void DrawSkinNone(HDC hDC, CRect rcItem)
	{
		if (m_nDrawStyle == SKN_TAB_STYLE_THEME) {
			if ( CanUseTheme() ) {
				DrawThemeNone(hDC, rcItem);
			} else {
				DrawSkin(hDC, rcItem, &m_TabSkinNone);
			}
		} else
			DrawSkin(hDC, rcItem, &m_TabSkinNone);
	}


	void DrawSkinSel(HDC hDC, CRect rcItem)
	{
		if (m_nDrawStyle == SKN_TAB_STYLE_THEME) {
			if ( CanUseTheme() ) {
				DrawThemeSel(hDC, rcItem);
			} else {
				DrawSkin(hDC, rcItem, &m_TabSkinSel);
			}
		} else
			DrawSkin(hDC, rcItem, &m_TabSkinSel);
	}


	void DrawSkin(HDC hDC, CRect rcItem, CBitmap* pBmp)
	{
		CSize		szBmp;
		pBmp->GetSize(szBmp);
		HDC 		hdcCompatible	= ::CreateCompatibleDC(hDC);
		HBITMAP 	hBitmapOld		= (HBITMAP)SelectObject(hdcCompatible, pBmp->m_hBitmap);

		if (rcItem.Width() <= szBmp.cx) {
			// All
			::StretchBlt(hDC, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(),  hdcCompatible, 0, 0, szBmp.cx, szBmp.cy, SRCCOPY);

			int 	nWidthLR = rcItem.Width() / 3;
			if (nWidthLR < szBmp.cx) {
				// Left
				::StretchBlt(hDC, rcItem.left, rcItem.top, nWidthLR, rcItem.Height(),
					hdcCompatible, 0, 0, nWidthLR, szBmp.cy, SRCCOPY);

				// Right
				::StretchBlt(hDC, rcItem.right-nWidthLR, rcItem.top, nWidthLR, rcItem.Height(),
					hdcCompatible, szBmp.cx-(nWidthLR), 0, nWidthLR, szBmp.cy, SRCCOPY);
			}

		} else {
			int 	nWidthLR = szBmp.cx / 3;
			// Left
			::StretchBlt(hDC, rcItem.left, rcItem.top, nWidthLR, rcItem.Height(),
				hdcCompatible, 0, 0, nWidthLR, szBmp.cy, SRCCOPY);

			// Right
			::StretchBlt(hDC, rcItem.right-nWidthLR, rcItem.top, nWidthLR, rcItem.Height(),
				hdcCompatible, szBmp.cx-(nWidthLR), 0, nWidthLR, szBmp.cy, SRCCOPY);

			// Mid
			::StretchBlt(hDC, rcItem.left+nWidthLR, rcItem.top, rcItem.Width()-(nWidthLR*2), rcItem.Height(),
				hdcCompatible, nWidthLR, 0, nWidthLR, szBmp.cy, SRCCOPY);
		}

		::SelectObject(hdcCompatible, hBitmapOld);
		::DeleteDC(hdcCompatible);
	}


public:
	void SetDrawStyle(int nStyle)
	{
		m_nDrawStyle = nStyle;

		if (nStyle == SKN_TAB_STYLE_CLASSIC) {
			m_bVisible = FALSE;
		}
	}


private:
	void SetTheme(CTheme &theme)
	{
		m_Theme = theme;
	}


	void DrawThemeSel(HDC hDC, CRect rcItem)
	{
		if ( m_Theme.IsThemingSupported() ) {
			m_Theme.DrawThemeBackground(hDC, TABP_TABITEM, TIS_HOT, rcItem);
		}
	}


	void DrawThemeNone(HDC hDC, CRect rcItem)
	{
		if ( m_Theme.IsThemingSupported() ) {
			//x if (CThemeDLLLoader::HaveDrawThemeBackground())
			m_Theme.DrawThemeBackground(hDC, TABP_TABITEM, TIS_NORMAL, rcItem);
		}
	}


	void DrawThemeCur(HDC hDC, CRect rcItem)
	{
		if ( m_Theme.IsThemingSupported() ) {
			//x if (CThemeDLLLoader::HaveDrawThemeBackground())
			m_Theme.DrawThemeBackground(hDC, TABP_TABITEM, TIS_SELECTED, rcItem);
		}
	}
};



class CTabCtrlItem {
public:
	// Data members
	BYTE		m_fsState;
	CString 	m_strItem;		// string on button
	CRect		m_rcItem;
	int 		m_nImgIndex;	// image list index
	DWORD_PTR	m_dwUser;		// user's data

private:
	// Constants
	enum { s_kcxIconGap = 2 };

public:
	// Constructor/destructor
	CTabCtrlItem(const CString &strBtn = CString(), int nImgIndex = -1, DWORD_PTR dwUser = 0, BYTE fsState = TCISTATE_ENABLED)
		: m_fsState(fsState)
		, m_strItem(strBtn)
		, m_nImgIndex(nImgIndex)
		, m_dwUser(dwUser)
	{
		m_rcItem.SetRectEmpty();
	}


	// Attributes
	bool ModifyState(BYTE fsRemove, BYTE fsAdd)
	{
		BYTE fsStateOld = m_fsState;

		m_fsState = (m_fsState & ~fsRemove) | fsAdd;

		if ( (fsStateOld & TCISTATE_SELECTED || fsStateOld & TCISTATE_MSELECTED)
		   && (fsRemove == TCISTATE_HOT || fsAdd == TCISTATE_HOT) ) 	// selected, no need to update
			return false;
		else
			return m_fsState != fsStateOld;
	}


	// Methods
	void Update(CDCHandle dc, HIMAGELIST hImgList, bool bAnchorColor, CTabSkin *pTabSkin)
	{
		if (m_fsState & TCISTATE_HIDDEN)
			return;

		int    cxIcon		= 0, cyIcon = 0;
		int    cxIconOffset = 0;

		if (m_nImgIndex != -1) {
			::ImageList_GetIconSize(hImgList, &cxIcon, &cyIcon);
			cxIconOffset += cxIcon + s_kcxIconGap * 2;
		}

		bool   bHot 		= (m_fsState & TCISTATE_HOT) != 0;
		bool   bPressed 	= (m_fsState & TCISTATE_PRESSED) != 0;

		CPoint ptOffset(0, 0);

		if (m_fsState & TCISTATE_SELECTED) {							// now selected
			Update_Selected(dc, pTabSkin, bHot, bPressed);
			ptOffset += CPoint(2, 2);
		} else if (m_fsState & TCISTATE_MSELECTED) {					// multi-selected
			Update_MultiSel(dc, pTabSkin, bHot, bPressed);
			ptOffset += CPoint(2, 2);
		} else {														// not selected
			Update_NotSel(dc, pTabSkin, bHot, bPressed, ptOffset);
		}

		_DrawText(dc, ptOffset + CPoint(cxIconOffset, 0), bAnchorColor, pTabSkin);

		if (m_nImgIndex != -1) {
			::ImageList_Draw(hImgList, m_nImgIndex, dc, m_rcItem.left + ptOffset.x + s_kcxIconGap,
							 m_rcItem.top  + (m_rcItem.Height() - cyIcon) / 2 + ptOffset.y / 2, ILD_TRANSPARENT);
		}
	}


private:
	void Update_Selected(CDCHandle dc, CTabSkin *pTabSkin, BOOL bHot, BOOL bPressed)
	{
		if (bHot && bPressed) {
			//選択・スキンなし
			if (pTabSkin->IsVisible() == FALSE) {
				COLORREF crTxt = dc.SetTextColor(pTabSkin->m_colBtnFace);
				COLORREF crBk  = dc.SetBkColor(pTabSkin->m_colBtnHi);
				CBrush	 hbr( CDCHandle::GetHalftoneBrush() );
				dc.SetBrushOrg(m_rcItem.left, m_rcItem.top);
				dc.FillRect(m_rcItem, hbr);
				dc.SetTextColor(crTxt);
				dc.SetBkColor(crBk);
				dc.DrawEdge(m_rcItem, BDR_RAISEDINNER, BF_RECT);
			} else {
				pTabSkin->DrawSkinCur(dc.m_hDC, m_rcItem);
			}
		} else {														// ignored hot
			if (pTabSkin->IsVisible() == FALSE) {
				TCTRACE( _T(" draw selected(checked) button\n") );
				COLORREF crTxt = dc.SetTextColor(pTabSkin->m_colBtnFace);
				COLORREF crBk  = dc.SetBkColor(pTabSkin->m_colBtnHi);
				CBrush	 hbr( CDCHandle::GetHalftoneBrush() );
				dc.SetBrushOrg(m_rcItem.left, m_rcItem.top);
				dc.FillRect(m_rcItem, hbr);
				dc.SetTextColor(crTxt);
				dc.SetBkColor(crBk);
				dc.DrawEdge(m_rcItem, EDGE_SUNKEN, BF_RECT);
			} else {
				pTabSkin->DrawSkinCur(dc.m_hDC, m_rcItem);
			}
		}
	}


	void Update_MultiSel(CDCHandle dc, CTabSkin *pTabSkin, BOOL bHot, BOOL bPressed)
	{
		if (pTabSkin->IsVisible() == FALSE) {
			if (bHot && bPressed)
				dc.DrawEdge(m_rcItem, BDR_RAISEDINNER, BF_RECT);
			else
				dc.DrawEdge(m_rcItem, EDGE_SUNKEN, BF_RECT);
		} else {
			pTabSkin->DrawSkinSel(dc.m_hDC, m_rcItem);
		}
	}


	void Update_NotSel(CDCHandle dc, CTabSkin *pTabSkin, BOOL bHot, BOOL bPressed, CPoint &ptOffset)
	{
		if (pTabSkin->IsVisible() == FALSE) {
			if (bHot && bPressed) {
				dc.DrawEdge(m_rcItem, BDR_RAISEDINNER, BF_RECT);		// hot
				ptOffset += CPoint(2, 2);								// sunk a little
			} else if (bHot && !bPressed) {
				dc.DrawEdge(m_rcItem, BDR_RAISEDINNER, BF_RECT);		// hot
			}
		} else {
			if (bHot && !bPressed) {
				pTabSkin->DrawSkinSel(dc.m_hDC, m_rcItem);
				//				ptOffset += CPoint(2, 2);
			} else {
				pTabSkin->DrawSkinNone(dc.m_hDC, m_rcItem);
			}
		}
	}


	// Implementation
	void _DrawText(CDCHandle dc, CPoint ptOffset, bool bAnchorColor, CTabSkin *pTabSkin)
	{
		COLORREF clr;

		if ( !(m_fsState & TCISTATE_ENABLED) )
			clr = pTabSkin->m_colDisable;
		else if (m_fsState & TCISTATE_INACTIVE)
			clr = bAnchorColor ? pTabSkin->m_colInActive : pTabSkin->m_colText;
		else
			clr = bAnchorColor ? pTabSkin->m_colActive : pTabSkin->m_colText;

		COLORREF clrOld = dc.SetTextColor(clr);

		CRect	 rcBtn(m_rcItem.left + ptOffset.x, m_rcItem.top + ptOffset.y, m_rcItem.right, m_rcItem.bottom);
		rcBtn.DeflateRect(2, 0);

		UINT	 uFortmat;
		int 	 nWidth = MtlComputeWidthOfText( m_strItem, dc.GetCurrentFont() );

		if ( nWidth > rcBtn.Width() )
			uFortmat = DT_LEFT | DT_END_ELLIPSIS;
		else
			uFortmat = DT_CENTER | DT_NOCLIP;

		if ( !(m_fsState & TCISTATE_ENABLED) ) {
			// disabled - draw shadow text shifted down and right 1 pixel (unles selected)
			CRect	 rcDisabled = rcBtn + CPoint(1, 1);
			COLORREF clrOld2	= dc.SetTextColor(pTabSkin->m_colDisableHi);
			dc.DrawText(m_strItem, -1, rcDisabled, DT_SINGLELINE | uFortmat | DT_VCENTER | DT_NOPREFIX);
			dc.SetTextColor(clrOld2);
		}

		dc.DrawText(m_strItem, -1, rcBtn, DT_SINGLELINE | uFortmat | DT_VCENTER | DT_NOPREFIX);
		dc.SetTextColor(clrOld);
	}


	void DrawHot(CDCHandle dc)
	{
		dc.DrawEdge(m_rcItem, BDR_RAISEDINNER, BF_RECT);
	}


	void DrawPressed(CDCHandle dc)
	{
		dc.DrawEdge(m_rcItem, EDGE_SUNKEN, BF_MIDDLE);
	}


	void DrawSelected(CDCHandle dc)
	{
		dc.DrawEdge(m_rcItem, EDGE_SUNKEN, BF_RECT);
	}
};



/////////////////////////////////////////////////////////////////////////////
// CTabCtrl2Base - base class for the Tab Ctrl2 implementation

class CTabCtrl2Base : public CWindow {
public:

	CTabCtrl2Base()
	{
	}
};



/////////////////////////////////////////////////////////////////////////////
// CTabCtrl2 - MTL implementation of Tab Ctrl2

template <class T, class TBase = CTabCtrl2Base, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CTabCtrl2Impl
	: public CWindowImpl< T, TBase, TWinTraits >
	, public CTrackMouseLeave<CTabCtrl2Impl< T, TBase, TWinTraits > >
	, public CThemeImpl<CTabCtrl2Impl< T, TBase, TWinTraits > >
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, -1)

private:
	typedef CTabCtrl2Impl< T, TBase, TWinTraits >	thisClass;

public:
	// Constants
	enum _TabCtrl2DrawConstants {
		s_kcxTextMargin = 7,
		s_kcyTextMargin = 3,
		s_kcxGap		= 2,
		s_kcyGap		= 2,
		s_kcxSeparator	= 2,

		s_kcxUpDown 	= 28,
		s_kcyUpDown 	= 14
	};

private:
	enum {
		_nMaxMenuItemTextLength = 100,
	};

	enum _CurrentState {
		none = 0,
		pressed,
		captured,
		hot_by_mouse
	};

	// Data members
public:
	std::vector<CTabCtrlItem> m_items;
	CImageList				  m_imgs;
private:
	_CurrentState			  m_nCurrentState;
public:
	CFont					  m_font;
	DWORD					  m_dwTabCtrl2ExtendedStyle;	// Tab Ctrl2 specific extended styles
private:
	int 					  m_nHotIndex;					// Current hot index
	int 					  m_nPressedIndex;				// Current capturing index
	CSimpleArray<CPoint>	  m_arrSeparators;				// Point of separator
public:
	int 					  m_nFirstIndexOnSingleLine;	// for single line
	CUpDownCtrl 			  m_wndUpDown;					//
private:
	CSize					  m_sizeItem;					// for fixed size
	bool					  m_bLockRefreshBandInfo;

	CTabSkin				  m_tabSkin;
	CToolTipCtrl			  m_tip;

public:
	CDropDownButtonCtrl 	  m_wndDropBtn;
private:
	int 					  m_nDrawStyle;

public:
	// Constructor/destructor
	CTabCtrl2Impl()
		: m_dwTabCtrl2ExtendedStyle(TAB2_EX_TRANSPARENT | TAB2_EX_SUPPORTREBAR | TAB2_EX_SCROLLOPPOSITE)
		, m_nCurrentState(none)
		, m_nFirstIndexOnSingleLine(0)
		, m_sizeItem(200, 50)
		, m_nHotIndex(-1)
		, m_nPressedIndex(-1)
		, m_bLockRefreshBandInfo(false)
		, m_nDrawStyle(0)
		, m_tabSkin( static_cast<CTheme &>(*this) )
	{
		m_tabSkin.LoadTabSkin();
		SetThemeClassList(L"TAB");
	}

	~CTabCtrl2Impl()
	{
		if (m_imgs.m_hImageList != NULL && (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SHAREIMGLIST) == 0)
			m_imgs.Destroy();
	}


private:
	// Overridables
	void OnSetCurSel(int nIndex)
	{
	}


	CString OnGetToolTipText(int nIndex)
	{
		CString strItem;

		GetItemText(nIndex, strItem);
		return strItem;
	}


	void OnDropDownButton()
	{
	}


public:
	// Attributes
	DWORD GetTabCtrl2ExtendedStyle() const
	{
		return m_dwTabCtrl2ExtendedStyle;
	}


	void SetItemSize(const CSize &size)
	{
		CSize sizePrev = m_sizeItem;

		m_sizeItem = size;

		if (sizePrev != m_sizeItem
		   && m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
		{
			_UpdateLayout();
		}
	}


	CSize GetItemSize()
	{
		return m_sizeItem;
	}


	void ModifyTabCtrl2ExtendedStyle(DWORD dwRemove, DWORD dwAdd)
	{
		DWORD dwOldStyle = m_dwTabCtrl2ExtendedStyle;

		m_dwTabCtrl2ExtendedStyle = (m_dwTabCtrl2ExtendedStyle & ~dwRemove) | dwAdd;

		if (dwOldStyle != m_dwTabCtrl2ExtendedStyle) {
			m_nFirstIndexOnSingleLine = 0;
			_UpdateLayout();
		}

		if ( (dwOldStyle & TAB2_EX_ANCHORCOLOR) != (m_dwTabCtrl2ExtendedStyle & TAB2_EX_ANCHORCOLOR) )
			Invalidate();
	}


	int GetItemCount() const
	{
		return (int) m_items.size();
	}


	int GetCurSel()
	{
		for (int i = 0; i < (int) m_items.size(); ++i) {
			if (m_items[i].m_fsState & TCISTATE_SELECTED)
				return i;
		}

		return -1;
	}


	int HitTest(const CPoint& point)
	{
		CRect	rc;
		GetClientRect(&rc);
		if ( !rc.PtInRect(point) )
			return -1;

		int   i = (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) ? 0 : m_nFirstIndexOnSingleLine;

		for (; i < (int) m_items.size(); ++i) {
			if (m_items[i].m_rcItem.PtInRect(point)/* && m_items[i].m_fsState & TCISTATE_ENABLED*/) {
				return i;
			}
		}

		return -1;
	}


	bool InsertItem(int nIndex, const CTabCtrlItem &item)
	{		// if nDestIndex is invalid, added to tail
		_HotItem();

	  #if 1 //+++
		int size = int (m_items.size());	// = GetItemCount();
		if (size == 0) {
			m_items.push_back(item);
		} else {
			if ( nIndex < 0 || nIndex > size ) {
				nIndex = size;
			}
			m_items.insert(m_items.begin() + nIndex, item);
		}
		//int size2 = int(m_items.size());	//*+++ debug
	  #else
		if (m_items.size() == 0) {
			m_items.push_back(item);
		} else {
			if ( nIndex < 0 || nIndex > (int) m_items.size() ) {
				nIndex = (int) m_items.size();
			}

			m_items.insert(m_items.begin() + nIndex, item);
		}
	  #endif

		_UpdateLayout();

		return true;
	}


	void DeleteItems(CSimpleArray<int> &arrSrcs)
	{
		for (int i = arrSrcs.GetSize() - 1; i >= 0; --i) {
			int nIndex = arrSrcs[i];

			if ( !_IsValidIndex(nIndex) )
				continue;

			DeleteItem(nIndex);
		}
	}


	bool MoveItems(int nDestIndex, CSimpleArray<int> &arrSrcs)
	{		// arrSrcs has to be sorted! if nDestIndex is invalid, added to tail
		TCTRACE( _T("CTabCtrlImpl::MoveItems\n") );

		if (arrSrcs.GetSize() <= 0)
			return false;

		int 					  i = 0;

		if (arrSrcs.GetSize() == 1) {
			if (nDestIndex == arrSrcs[0] || nDestIndex == arrSrcs[0] + 1)
				return true;		// no need

			if (!_IsValidIndex(nDestIndex) && arrSrcs[0] == m_items.size() - 1)
				return true;		// no need
		}

		CLockRedraw 			  lock(m_hWnd);
		m_bLockRefreshBandInfo = true;

		std::vector<CTabCtrlItem> temp;

		TCTRACE( _T(" save src indexs - ") );

		for (i = 0; i < arrSrcs.GetSize(); ++i) {
			int nIndex = arrSrcs[i];

			if ( !_IsValidIndex(nIndex) )
				continue;

			TCTRACE(_T(" %d"), nIndex);
			temp.push_back(m_items[nIndex]);
		}

		TCTRACE( _T("\n") );

		TCTRACE( _T(" delete item indexs - ") );

		for (i = arrSrcs.GetSize() - 1; i >= 0; --i) {
			int nIndex = arrSrcs[i];

			if ( !_IsValidIndex(nIndex) )
				continue;

			if (nDestIndex > nIndex)
				--nDestIndex;

			TCTRACE(_T(" %d"), nIndex);
			DeleteItem(nIndex);
		}

		TCTRACE( _T("\n") );

		// add all
		if ( !_IsValidIndex(nDestIndex) ) {
			nDestIndex = GetItemCount();
		}

		for (i = 0; i < (int) temp.size(); ++i) {
			InsertItem(nDestIndex, temp[i]);
			++nDestIndex;
		}

		m_bLockRefreshBandInfo = false;
		RefreshBandInfo();

		return true;
	}


	bool SetItemText(int nIndex, const CString &str)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		if (m_items[nIndex].m_strItem == str)
			return true;

		m_items[nIndex].m_strItem = str;

		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE) {
			InvalidateRect(m_items[nIndex].m_rcItem);
			UpdateWindow();
		} else {
			InvalidateRect(m_items[nIndex].m_rcItem);	// even if layout will not be changed
			_UpdateLayout();							//_UpdateItems(nIndex);
		}

		return true;
	}


	bool SetItemImageIndex(int nIndex, int nImgIndex)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		if (m_items[nIndex].m_nImgIndex == nImgIndex)
			return true;

		m_items[nIndex].m_nImgIndex = nImgIndex;

		// imagelist does'nt change item rect
		InvalidateRect(m_items[nIndex].m_rcItem);
		UpdateWindow();
		return true;
	}


	bool SetItemInactive(int nIndex)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		if ( m_items[nIndex].ModifyState(0, TCISTATE_INACTIVE) ) {
			InvalidateRect(m_items[nIndex].m_rcItem, FALSE);
			return true;
		}

		return false;
	}


	bool SetItemActive(int nIndex)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		if ( m_items[nIndex].ModifyState(TCISTATE_INACTIVE, 0) ) {
			InvalidateRect(m_items[nIndex].m_rcItem, FALSE);
			return true;
		}

		return false;
	}


	bool SetItemDisabled(int nIndex)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		if ( m_items[nIndex].ModifyState(TCISTATE_ENABLED, 0) ) {
			InvalidateRect(m_items[nIndex].m_rcItem);
			return true;
		}

		return false;
	}


	bool SetItemEnabled(int nIndex)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		if ( m_items[nIndex].ModifyState(0, TCISTATE_ENABLED) ) {
			InvalidateRect(m_items[nIndex].m_rcItem);
			return true;
		}

		return false;
	}


private:
	bool SetItemUserData(int nIndex, DWORD_PTR dwUser)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		m_items[nIndex].m_dwUser = m_dwUser;
		return true;
	}


	bool GetItem(int nIndex, CTabCtrlItem &item)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		item = m_items[nIndex];
		return true;
	}


public:
	bool GetItemText(int nIndex, CString &str)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		str = m_items[nIndex].m_strItem;
		return true;
	}


	bool GetItemImageIndex(int nIndex, int &nImgIndex)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		nImgIndex = m_items[nIndex].m_nImgIndex;
		return true;
	}


	bool GetItemUserData(int nIndex, DWORD_PTR &dwData)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		dwData = m_items[nIndex].m_dwUser;
		return true;
	}


	bool GetItemState(int nIndex, BYTE &bytData)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		bytData = m_items[nIndex].m_fsState;
		return true;
	}


private:
	bool GetItemRect(int nIndex, CRect &rc)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		if (!(m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) && nIndex < m_nFirstIndexOnSingleLine)
			return false;

		rc = m_items[nIndex].m_rcItem;
		return true;
	}


public:
	// added by minit
	int GetFirstVisibleIndex()
	{
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
			return 0;
		} else {
			CRect rc;
			GetClientRect(&rc);

			for (int i = 0; i < (int) m_items.size(); ++i) {
				if ( MtlIsCrossRect(m_items[i].m_rcItem, &rc) ) {
					return i;
				}
			}
		}

		return -1;
	}


	int GetLastVisibleIndex()
	{
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
			return m_items.size() - 1;
		} else {
			CRect rc;
			GetClientRect(&rc);

			for (int i = int(m_items.size()) - 1; i >= 0; i--) {
				if ( MtlIsCrossRect(m_items[i].m_rcItem, &rc) ) {
					return i;
				}
			}
		}

		return -1;
	}


	bool AddItem(const CTabCtrlItem &item)
	{
		_HotItem();

		m_items.push_back(item);
		_UpdateLayout();	// _UpdateItems(m_items.size() - 1);
		return true;
	}


	bool DeleteItem(int nIndex)
	{
		if ( !_IsValidIndex(nIndex) )
			return false;

		_HotItem();

		// first, post it
		InvalidateRect( _InflateGapWidth(m_items[nIndex].m_rcItem) );
		//		_InvalidateItemsRect(nIndex, m_items.size());

		if ( nIndex < m_nFirstIndexOnSingleLine || nIndex + 1 == GetItemCount() ) {
			--m_nFirstIndexOnSingleLine;

			if (m_nFirstIndexOnSingleLine < 0)
				m_nFirstIndexOnSingleLine = 0;
		}

		m_items.erase(m_items.begin() + nIndex);
		_UpdateLayout();
		return true;
	}


	void GetCurMultiSel(CSimpleArray<int> &arrDest, bool bIncludeCurSel = true)
	{						// have to be sorted!
		arrDest.RemoveAll();

		for (int i = 0; i < (int) m_items.size(); ++i) {
			if (bIncludeCurSel) {
				if (m_items[i].m_fsState & TCISTATE_SELECTED) {
					arrDest.Add(i);
					continue;
				}
			}

			if (m_items[i].m_fsState & TCISTATE_MSELECTED)
				arrDest.Add(i);
		}
	}


	bool SetCurSel(int nIndex, bool bClicked = false)
	{
		TCTRACE( _T("SetCurSel\n") );

		if ( !_IsValidIndex(nIndex) ) {
			TCTRACE( _T(" invalid index\n") );
			return false;
		}

		_ResetMultiSelectedItems();
		int nCurSel = GetCurSel();

		if (nCurSel != -1 && nCurSel == nIndex) {
			TCTRACE( _T(" no need\n") );
			return true;
		}

		// clean up
		if ( _IsValidIndex(nCurSel) ) {
			if ( m_items[nCurSel].ModifyState(TCISTATE_SELECTED | TCISTATE_HOT | TCISTATE_PRESSED | TCISTATE_MSELECTED, 0) )
				InvalidateRect(m_items[nCurSel].m_rcItem);
		}

		// new selected item
		if ( m_items[nIndex].ModifyState(TCISTATE_HOT | TCISTATE_PRESSED | TCISTATE_MSELECTED, TCISTATE_SELECTED) )
			InvalidateRect(m_items[nIndex].m_rcItem);

		_ScrollOpposite(nIndex, bClicked);

		T * pT		= static_cast<T *>(this);
		pT->OnSetCurSel(nIndex);

		UpdateWindow();
		return true;
	}


private:
	void _ScrollOpposite(int nNewSel, bool bClicked)
	{
		ATLASSERT( _IsValidIndex(nNewSel) );

		if (GetItemCount() < 2)
			return;

		if ( !(m_dwTabCtrl2ExtendedStyle & TAB2_EX_SCROLLOPPOSITE) )
			return;

		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE)
			return;

		if ( !m_wndUpDown.IsWindowVisible() )
			return;

		const CRect &rcItem = m_items[nNewSel].m_rcItem;

		CRect		 rcClient;
		GetClientRect(rcClient);
		if (bClicked) {
			if (m_nFirstIndexOnSingleLine != nNewSel) {
				CRect rcUpDown;
				m_wndUpDown.GetWindowRect(rcUpDown);
				ScreenToClient(rcUpDown);

				if ( MtlIsCrossRect(rcItem, rcUpDown) )
					ScrollItem();
			}
		} else if (rcItem.IsRectEmpty() || (rcClient & rcItem) != rcItem) { // outside or crossed
			//+++ m_nFirstIndexOnSingleLine = MtlMax(0, nNewSel - 1);			//nNewSel;
			m_nFirstIndexOnSingleLine = std::max(0, nNewSel - 1);			//nNewSel;
			_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
			Invalidate();
			_ShowOrHideUpDownCtrl(rcClient);
		}
	}


	void RefreshBandInfo()
	{
		ATLASSERT(m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR);

		if (m_bLockRefreshBandInfo)
			return;

		HWND		  hWndReBar = GetParent();
		CReBarCtrl	  rebar(hWndReBar);
		REBARBANDINFO rbBand;
		rbBand.cbSize = sizeof (REBARBANDINFO);
		rbBand.fMask  = RBBIM_CHILDSIZE;

		int 		  nIndex	= rebar.IdToIndex( GetDlgCtrlID() );
		rebar.GetBandInfo(nIndex, &rbBand);

		if ( rbBand.cyMinChild != _GetRequiredHeight() ) {
			// Calculate the size of the band
			rbBand.cxMinChild = 0;
			rbBand.cyMinChild = _GetRequiredHeight();

			rebar.SetBandInfo(nIndex, &rbBand);
		}
	}


public:
	int GetItemHeight() const
	{
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE) {
			return m_sizeItem.cy;
		} else {
			int 	cxIcon = 0, cyIcon = 0;

			if (m_imgs.m_hImageList != NULL)
				m_imgs.GetIconSize(cxIcon, cyIcon);

			LOGFONT lf;
			m_font.GetLogFont(lf);
			int 	cy	   = lf.lfHeight;

			if (cy < 0)
				cy = -cy;

			cy += 2 * s_kcyTextMargin;
			return std::max(cy, cxIcon);
		}
	}


	bool CanScrollItem(bool bRight = true) const
	{
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE)
			return false;			// can't
		if (bRight) {
			int 	nCount = GetItemCount();
			return	m_nFirstIndexOnSingleLine + 1 < nCount;
		} else {
			return	m_nFirstIndexOnSingleLine - 1 >= 0;
		}
	}


	bool ScrollItem(bool bRight = true)
	{
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE)
			return false;		// do nothing

		if (bRight) {
			int 	nCount = GetItemCount();
			if ( m_nFirstIndexOnSingleLine + 1 < nCount ) {
				++m_nFirstIndexOnSingleLine;
				_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
				Invalidate();
			} else {
				return false;
			}
		} else {
			if (m_nFirstIndexOnSingleLine - 1 >= 0) {
				--m_nFirstIndexOnSingleLine;
				_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
				Invalidate();
			} else {
				return false;
			}
		}

		CRect	rcClient;
		GetClientRect(&rcClient);

		_ShowOrHideUpDownCtrl(rcClient);
		return true;
	}


	// Overrides
	void OnTrackMouseMove(UINT nFlags, CPoint pt)
	{
		int nIndex = HitTest(pt);

		if ( _IsValidIndex(nIndex) ) {
			if (m_nHotIndex == -1 || m_nHotIndex != nIndex) {							// if other button made hot or first hot
				m_tip.Activate(FALSE);
				m_tip.Activate(TRUE);
				PostMessage( WM_MOUSEMOVE, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );	// make sure

				_HotItem(nIndex);														// button made hot with mouse
			}
		} else {
			m_tip.Activate(FALSE);
			_HotItem();
		}
	}


	void OnTrackMouseLeave()
	{
		m_tip.SetDelayTime(TTDT_INITIAL, 500);
		_HotItem();
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP( CTabCtrl2Impl )
		CHAIN_MSG_MAP( CThemeImpl<CTabCtrl2Impl> )											//minit
		COMMAND_ID_HANDLER( COMMAND_ID_DROPBUTTON,OnPushDropButton	)						//minit
		MESSAGE_HANDLER( WM_CREATE			, OnCreate			)
		MESSAGE_HANDLER( WM_PAINT			, OnPaint			)
		MESSAGE_HANDLER( WM_WINDOWPOSCHANGING,OnWindowPosChanging )
		MESSAGE_HANDLER( WM_LBUTTONDOWN 	, OnLButtonDown 	)
		MESSAGE_HANDLER( WM_LBUTTONUP		, OnLButtonUp		)
		MESSAGE_HANDLER( WM_ERASEBKGND		, OnEraseBackground )
		NOTIFY_CODE_HANDLER( UDN_DELTAPOS	, OnDeltaPos		)
		NOTIFY_CODE_HANDLER( TTN_GETDISPINFO, OnGetDispInfo 	)
		CHAIN_MSG_MAP( CTrackMouseLeave<CTabCtrl2Impl> )
		REFLECT_NOTIFICATIONS( )														//minit
	END_MSG_MAP( )


private:
	LRESULT OnPushDropButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)	//minit
	{
		T *pT = static_cast<T *>(this);

		pT->OnDropDownButton();
		return 0;
	}


	LRESULT OnDeltaPos(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMUPDOWN lpnmud = (LPNMUPDOWN) pnmh;

		TCTRACE(_T("OnDeltaPos iPos(%d), iDelta(%d)\n"), lpnmud->iPos, lpnmud->iDelta);
		ScrollItem(lpnmud->iDelta > 0);

		return 0;
	}


	LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL &bHandled)
	{
		LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;

		TCTRACE( _T("OnGetDispInfo\n") );
		CPoint		   ptCursor;
		::GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);
		int 		   nIndex	 = HitTest(ptCursor);

		if ( _IsValidIndex(nIndex) ) {
			T * 	pT		   = static_cast<T *>(this);
			CString strToolTip = pT->OnGetToolTipText(nIndex);
			strToolTip = MtlCompactString( strToolTip, _countof(pDispInfo->szText) );
			::lstrcpyn( pDispInfo->szText, strToolTip, _countof(pDispInfo->szText) );
		} else {
			pDispInfo->szText[0] = _T('\0');
		}

		m_tip.SetDelayTime( TTDT_INITIAL, m_tip.GetDelayTime(TTDT_RESHOW) );

		return 0;
	}


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		// maybe nothing done
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

		// get and use system settings
		GetSystemSettings();

		m_wndUpDown.Create(m_hWnd, CRect(0, 0, s_kcxUpDown, s_kcyUpDown), NULL, WS_CHILD | WS_OVERLAPPED | UDS_HORZ);
		m_wndDropBtn.Create(m_hWnd, CRect(0, 0, s_kcxUpDown, s_kcxUpDown), COMMAND_ID_DROPBUTTON);
		m_wndDropBtn.SetWindowPos(m_wndUpDown.m_hWnd, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		_InitToolTip();

		// default size
		MoveWindow(0, 0, 200, _GetRequiredHeight(), FALSE);
		return lRet;
	}


	LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		LPWINDOWPOS lpwndpos = (LPWINDOWPOS) lParam;

		{
			CSize size(lpwndpos->cx, lpwndpos->cy);
			m_wndUpDown.MoveWindow(size.cx - s_kcyUpDown * 3, size.cy - s_kcyUpDown, s_kcxUpDown, s_kcyUpDown);
			m_wndDropBtn.MoveWindow(size.cx - s_kcyUpDown, size.cy - s_kcyUpDown, s_kcyUpDown, s_kcyUpDown);
			//m_wndUpDown.MoveWindow(size.cx - s_kcxUpDown, size.cy - s_kcyUpDown, s_kcxUpDown, s_kcyUpDown);
			CRect rc(0, 0, s_kcyUpDown, s_kcyUpDown);
			m_wndDropBtn.InvalidateRect(&rc, TRUE);
			_ShowOrHideUpDownCtrl( CRect(0, 0, size.cx, size.cy) );

			if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
				_UpdateMultiLineLayout(size.cx);

				if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR) {
					RefreshBandInfo();
				}
			}
		}
		return 0;
	}


	bool _MustBeInvalidateOnMultiLine(CSize size)
	{
		CRect rc;  GetClientRect(rc);		//*+++

		if (rc.Width() < size.cx)	// expanding
			return true;

		int   cxLeft = 0;

		for (int i = 0; i < m_items.size(); ++i) {
			cxLeft = max(m_items[i].m_rcItem.right, cxLeft);
		}

		if (cxLeft != 0 && cxLeft < size.cx) {
			return false;
		} else {
			return true;
		}
	}


	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
	{
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_TRANSPARENT) {
			bHandled = FALSE;
			return 0;
		}

		RECT rect;
		GetClientRect(&rect);
		::FillRect( (HDC) wParam, &rect, (HBRUSH) LongToPtr(COLOR_BTNFACE + 1) );

		return 1;					// don't do the default erase
	}


	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (wParam != NULL) {
			_DoPaint( (HDC) wParam );
		} else {
			CPaintDC dc(m_hWnd);
			_DoPaint(dc.m_hDC, &dc.m_ps.rcPaint);
		}

		return 0;
	}


	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		TCTRACE( _T("OnLButtonDown\n") );
		POINT pt	 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		int   nIndex = HitTest(pt);

		if (nIndex != -1) {
			ATLASSERT( _IsValidIndex(nIndex) );

			if ( (wParam & MK_CONTROL) && GetCurSel() != nIndex ) {
				if ( !_FindIndexFromCurMultiSelected(nIndex) ) {
					if ( m_items[nIndex].ModifyState(TCISTATE_SELECTED, TCISTATE_MSELECTED) )
						InvalidateRect(m_items[nIndex].m_rcItem);
				} else {
					if ( m_items[nIndex].ModifyState(TCISTATE_MSELECTED, 0) )
						InvalidateRect(m_items[nIndex].m_rcItem);
				}
			} else {
				_PressItem(nIndex);
				SetCapture();
			}
		}

		return 0;
	}


	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		TCTRACE( _T("OnLButtonUp\n") );
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if (GetCapture() == m_hWnd) {
			ReleaseCapture();

			int nIndex = HitTest(pt);

			if (nIndex != -1 && nIndex == m_nPressedIndex) {
				TCTRACE( _T(" change current selelected item\n") );
				ATLASSERT( _IsValidIndex(nIndex) );
				_PressItem();	// always clean up pressed flag
				SetCurSel(nIndex, true);
				NMHDR nmhdr = { m_hWnd, GetDlgCtrlID(), TCN_SELCHANGE };
				::SendMessage(GetParent(), WM_NOTIFY, (WPARAM) GetDlgCtrlID(), (LPARAM) &nmhdr);
			} else {
				_PressItem();	// always clean up pressed flag
			}
		}

		return 0;
	}


	// Implementation
	bool _FindIndexFromCurMultiSelected(int nIndex)
	{
		CSimpleArray<int> arrCurMultiSel;
		GetCurMultiSel(arrCurMultiSel, false);

		if (arrCurMultiSel.Find(nIndex) != -1)
			return true;

		return false;
	}


	void _UpdateLayout()
	{
		if (m_items.size() == 0) {
			m_arrSeparators.RemoveAll();
			Invalidate();

			if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR)
				RefreshBandInfo();

			return;
		}

		CRect		rc;
		GetClientRect(&rc);

		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) {
			_UpdateMultiLineLayout( rc.Width() );
		} else {
			_UpdateSingleLineLayout(m_nFirstIndexOnSingleLine);
		}

		_ShowOrHideUpDownCtrl(rc);

		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_SUPPORTREBAR)
			RefreshBandInfo();

		UpdateWindow();
	}


public:
	void _HotItem(int nNewHotIndex = -1)
	{
		TCTRACE( _T("_HotItem\n") );

		// clean up
		if ( _IsValidIndex(m_nHotIndex) ) {
			TCTRACE(_T(" clean up - %d\n"), m_nHotIndex);
			CTabCtrlItem &item = m_items[m_nHotIndex];

			if ( item.ModifyState(TCISTATE_HOT, 0) )
				InvalidateRect(item.m_rcItem);
		}

		m_nHotIndex = nNewHotIndex;

		if ( _IsValidIndex(m_nHotIndex) ) {
			TCTRACE(_T(" hot - %d\n"), m_nHotIndex);
			CTabCtrlItem &item = m_items[m_nHotIndex];

			if ( item.ModifyState(0, TCISTATE_HOT) )
				InvalidateRect(item.m_rcItem);
		}
	}


private:
	void _PressItem(int nPressedIndex = -1)
	{
		TCTRACE( _T("_PressItem\n") );

		// clean up prev
		if ( _IsValidIndex(m_nPressedIndex) ) {
			TCTRACE(_T(" clean up - %d\n"), m_nPressedIndex);
			CTabCtrlItem &item = m_items[m_nPressedIndex];

			if ( item.ModifyState(TCISTATE_PRESSED, 0) )
				InvalidateRect(item.m_rcItem);
		}

		m_nPressedIndex = nPressedIndex;

		if ( _IsValidIndex(m_nPressedIndex) ) {
			TCTRACE(_T(" press - %d\n"), m_nPressedIndex);
			CTabCtrlItem &item = m_items[m_nPressedIndex];

			if ( item.ModifyState(0, TCISTATE_PRESSED) )
				InvalidateRect(item.m_rcItem);
		}
	}


	void _ResetMultiSelectedItems()
	{
		for (int i = 0; i < (int) m_items.size(); ++i) {
			if ( m_items[i].ModifyState(TCISTATE_MSELECTED, 0) )
				InvalidateRect(m_items[i].m_rcItem);
		}
	}


	void _DoPaint(CDCHandle dc, LPCRECT lpRect = NULL)
	{
		CFontHandle fontOld = dc.SelectFont(m_font.m_hFont);
		int 		modeOld = dc.SetBkMode(TRANSPARENT);

		int 		i		= m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE ? 0 : m_nFirstIndexOnSingleLine;

		for (; i < (int) m_items.size(); ++i) {
			if ( lpRect == NULL || MtlIsCrossRect(m_items[i].m_rcItem, lpRect) ) {
				m_items[i].Update(dc.m_hDC, m_imgs.m_hImageList, _check_flag(TAB2_EX_ANCHORCOLOR, m_dwTabCtrl2ExtendedStyle), &m_tabSkin);
			}
		}

		if (m_tabSkin.IsVisible() == FALSE)
			_DrawSeparators(dc, lpRect);

		dc.SelectFont(fontOld);
		dc.SetBkMode(modeOld);
	}


	void _UpdateSingleLineLayout(int nFirstIndex, bool bForce = false)
	{
		TCTRACE(_T("_UpdateSingleLineLayout index(%d)\n"), nFirstIndex);
		ATLASSERT( _IsValidIndex(nFirstIndex) );
		m_arrSeparators.RemoveAll();

		// clean invisible items
		int i;

		for (i = 0; i < nFirstIndex && i < (int) m_items.size(); ++i) {
			m_items[i].m_rcItem.SetRectEmpty();
		}

		int cxOffset = 0;	//s_kcxGap;

		for (i = nFirstIndex; i < (int) m_items.size(); ++i) {
			CTabCtrlItem &item	= m_items[i];
			CRect		  rcSrc = item.m_rcItem;

			// update the rect
			if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
				item.m_rcItem = CRect(cxOffset, 0, cxOffset + m_sizeItem.cx, m_sizeItem.cy);
			else
				item.m_rcItem = _MeasureItem(m_items[i].m_strItem) + CPoint(cxOffset, 0);

			cxOffset = cxOffset + item.m_rcItem.Width();

			if (m_tabSkin.IsVisible() == FALSE)
				cxOffset += s_kcxGap;

			m_arrSeparators.Add( CPoint(cxOffset, 0) );

			if (m_tabSkin.IsVisible() == FALSE)
				cxOffset += s_kcxSeparator + s_kcxGap;

			//
			if (bForce || rcSrc != item.m_rcItem) {
				InvalidateRect( _InflateGapWidth(rcSrc) );
				InvalidateRect( _InflateGapWidth(item.m_rcItem) );
			}
		}
	}


	void _UpdateMultiLineLayout(int nWidth)
	{
		TCTRACE( _T("_UpdateMultiLineLayout\n") );

		m_arrSeparators.RemoveAll();

		int cxOffset = 0;
		int cyOffset = 0;

		for (int i = 0; i < (int) m_items.size(); ++i) {
			CTabCtrlItem &item	= m_items[i];
			CRect		  rcSrc = item.m_rcItem;

			// update the rect
			if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
				item.m_rcItem = CRect(cxOffset, cyOffset, cxOffset + m_sizeItem.cx, cyOffset + m_sizeItem.cy);
			else
				item.m_rcItem = _MeasureItem(m_items[i].m_strItem) + CPoint(cxOffset, cyOffset);

			if (i != 0 && item.m_rcItem.right > nWidth) { // do wrap
				cxOffset  = 0;
				cyOffset += GetItemHeight() + s_kcyGap;

				if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
					item.m_rcItem = CRect(cxOffset, cyOffset, cxOffset + m_sizeItem.cx, cyOffset + m_sizeItem.cy);
				else
					item.m_rcItem = _MeasureItem(m_items[i].m_strItem) + CPoint(cxOffset, cyOffset);
			}

			cxOffset = cxOffset + m_items[i].m_rcItem.Width();

			if (m_tabSkin.IsVisible() == FALSE)
				cxOffset += s_kcxGap;

			m_arrSeparators.Add( CPoint(cxOffset, cyOffset) );

			if (m_tabSkin.IsVisible() == FALSE)
				cxOffset += s_kcxSeparator + s_kcxGap;

			//

			if (rcSrc != item.m_rcItem) {
				InvalidateRect( _InflateGapWidth(rcSrc) );
				InvalidateRect( _InflateGapWidth(item.m_rcItem) );
			}
		}
	}


	void _ShowOrHideUpDownCtrl(const CRect &rcClient)
	{
		TCTRACE( _T("_ShowOrHideUpDownCtrl\n") );

		int 	nCount = GetItemCount();	// = m_items.size();	//+++
		if ( (m_dwTabCtrl2ExtendedStyle & TAB2_EX_MULTILINE) || nCount/*m_items.size()*/ < 1 ) {
			m_wndDropBtn.ShowWindow(SW_HIDE);
			m_wndUpDown.ShowWindow(SW_HIDE);
			return;
		}

		m_wndUpDown.SetRange( 0, nCount );
		ATLASSERT(nCount == GetItemCount());	//+++
		nCount = GetItemCount();				//+++

		if (m_nFirstIndexOnSingleLine != 0) {
			m_wndDropBtn.ShowWindow(SW_SHOWNORMAL);
			m_wndUpDown.ShowWindow(SW_SHOWNORMAL);
			return;
		} else if (m_items[nCount/*m_items.size()*/ - 1].m_rcItem.right > rcClient.right) {
			TCTRACE( _T(" show!\n") );
			m_wndDropBtn.ShowWindow(SW_SHOWNORMAL);
			m_wndUpDown.ShowWindow(SW_SHOWNORMAL);
			return;
		}

		m_wndDropBtn.ShowWindow(SW_HIDE);
		m_wndUpDown.ShowWindow(SW_HIDE);
	}


	CRect _MeasureItem(const CString &strText)
	{
		if (m_dwTabCtrl2ExtendedStyle & TAB2_EX_FIXEDSIZE)
			return CRect(0, 0, m_sizeItem.cx, m_sizeItem.cy);

		// compute size of text - use DrawText with DT_CALCRECT
		int 	cx	   = MtlComputeWidthOfText(strText, m_font);

		int 	cxIcon = 0, cyIcon = 0;

		if (m_imgs.m_hImageList != NULL)
			m_imgs.GetIconSize(cxIcon, cyIcon);

		LOGFONT lf;
		m_font.GetLogFont(lf);
		int 	cy	   = lf.lfHeight;

		if (cy < 0)
			cy = -cy;

		cy += 2 * s_kcyTextMargin;

		// height of item is the bigger of these two
		cy	= std::max(cy, cyIcon);

		// width is width of text plus a bunch of stuff
		cx += 2 * s_kcxTextMargin;	// L/R margin for readability

		//		ATLTRACE(_T("(cx, cy) = (%d, %d)\n"), cx, cy);
		return CRect(0, 0, cx, cy);
	}


	int _GetRequiredHeight()
	{
		size_t size = m_items.size();	// = GetItemCount();
		if (size == 0) {
			return GetItemHeight();
		} else {
			return m_items[size - 1].m_rcItem.bottom;
		}

		return 0;
	}


public:
	bool _IsValidIndex(int nIndex) const
	{
		size_t	size	= m_items.size();	// = GetItemCount();
		bool	rc		= ((size_t)nIndex < size);
		return rc;
	}


private:
	void _ClearAll()
	{
		m_items.clear();
	}


	const CRect _InflateGapWidth(const CRect &rc) const
	{
		int cxSeparatorOffset = s_kcxGap * 2 + s_kcxSeparator;

		return CRect(rc.left, rc.top, rc.right + cxSeparatorOffset, rc.bottom);
	}


	void _DrawSeparators(CDCHandle dc, LPCRECT lpRect = NULL)
	{
		int   cy = GetItemHeight();
		CRect rect(lpRect);

		for (int i = 0; i < m_arrSeparators.GetSize(); ++i) {
			CPoint &pt = m_arrSeparators[i];
			CRect	rc(pt.x, pt.y + 2, pt.x + 2, pt.y + cy - 2);

			if ( lpRect == NULL || MtlIsCrossRect(rc, lpRect) ) {
				dc.DrawEdge(rc, EDGE_ETCHED, BF_LEFT);	// draw separator line
			}
		}
	}


	void GetSystemSettings()
	{
		// refresh our font
		NONCLIENTMETRICS info = {0};

		info.cbSize = sizeof (info);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof (info), &info, 0);
		LOGFONT 		 logfont;
		memset( &logfont, 0, sizeof (LOGFONT) );

		if (m_font.m_hFont != NULL)
			m_font.GetLogFont(logfont);

		if (  logfont.lfHeight			!= info.lfMenuFont.lfHeight
		   || logfont.lfWidth			!= info.lfMenuFont.lfWidth
		   || logfont.lfEscapement		!= info.lfMenuFont.lfEscapement
		   || logfont.lfOrientation 	!= info.lfMenuFont.lfOrientation
		   || logfont.lfWeight			!= info.lfMenuFont.lfWeight
		   || logfont.lfItalic			!= info.lfMenuFont.lfItalic
		   || logfont.lfUnderline		!= info.lfMenuFont.lfUnderline
		   || logfont.lfStrikeOut		!= info.lfMenuFont.lfStrikeOut
		   || logfont.lfCharSet 		!= info.lfMenuFont.lfCharSet
		   || logfont.lfOutPrecision	!= info.lfMenuFont.lfOutPrecision
		   || logfont.lfClipPrecision	!= info.lfMenuFont.lfClipPrecision
		   || logfont.lfQuality 		!= info.lfMenuFont.lfQuality
		   || logfont.lfPitchAndFamily	!= info.lfMenuFont.lfPitchAndFamily
		   || lstrcmp(logfont.lfFaceName, info.lfMenuFont.lfFaceName) != 0)
		{
			HFONT hFontMenu = ::CreateFontIndirect(&info.lfMenuFont);
			ATLASSERT(hFontMenu != NULL);

			if (hFontMenu != NULL) {
				if (m_font.m_hFont != NULL)
					m_font.DeleteObject();

				m_font.Attach(hFontMenu);
				SetFont(m_font);
			}
		}
	}


	void _InitToolTip()
	{
		// create a tool tip
		m_tip.Create(m_hWnd);
		ATLASSERT( m_tip.IsWindow() );
		CToolInfo tinfo(TTF_SUBCLASS, m_hWnd);
		tinfo.hwnd = m_hWnd;		// WTL always sucks...
		m_tip.AddTool(tinfo);
		m_tip.SetMaxTipWidth(SHRT_MAX);
	}


public:
	void SetDrawStyle(int nStyle)
	{
		m_tabSkin.SetDrawStyle(nStyle);
		m_wndDropBtn.SetDrawStyle(nStyle);
	  #if 1	//+++ uxtheme.dll の関数の呼び出し方を変更.
		if (nStyle == SKN_TAB_STYLE_CLASSIC)
			UxTheme_Wrap::SetWindowTheme(m_wndUpDown.m_hWnd, L" ", L" ");
		else
			UxTheme_Wrap::SetWindowTheme(m_wndUpDown.m_hWnd, NULL, L"SPIN");
	  #else
		CTheme		theme;
		theme.Attach(m_hTheme);
		if (nStyle == SKN_TAB_STYLE_CLASSIC) {
			theme.SetWindowTheme(m_wndUpDown.m_hWnd, L" ", L" ");
		} else {
			theme.SetWindowTheme(m_wndUpDown.m_hWnd, NULL, L"SPIN");
		}
	  #endif
	}


	void ReloadSkinData()
	{
		m_tabSkin.LoadTabSkin();
		InvalidateRect(NULL, TRUE);
	}


};



class CTabCtrl2 : public CTabCtrl2Impl<CTabCtrl2> {
public:
	DECLARE_WND_CLASS_EX(_T("MTL_TabCtrl2"), CS_DBLCLKS, COLOR_BTNFACE)
};



////////////////////////////////////////////////////////////////////////////



}	//namespace MTL



#ifndef _MTL_NO_AUTOMATIC_NAMESPACE
using namespace MTL;
#endif	//!_MTL_NO_AUTOMATIC_NAMESPACE
