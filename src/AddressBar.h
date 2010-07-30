/**
 *	@file AddressBar.h
 *	@brief	アドレスバー
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
// Mtlwin.h: Last updated: March 6, 2001
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "HlinkDataObject.h"
#include "SearchBar.h"
#include "Donut.h"


// for debug
#ifdef _DEBUG
	const bool _Mtl_AddressBarCtrl_traceOn = false;
	#define abrTRACE	if (_Mtl_AddressBarCtrl_traceOn) ATLTRACE
#else
	#define abrTRACE
#endif



template < class _OutputIterString >
bool __Mtl_AddFromComboBox(HWND hWndComboEx, _OutputIterString __result)
{
	CComboBoxEx combo(hWndComboEx);

	CString 	str;
	int 		nCount = combo.GetCount();

	if (nCount == 0)
		return false;

	for (int n = 0; n < nCount; ++n) {
		if (MtlGetLBTextFixed(combo, n, str) != CB_ERR)
			*__result++ = str;
	}

	return true;
}



inline bool MtlSaveTypedURLs(HWND hWndComboBoxEx)
{
	Misc::CRegKey 	rk;
	LONG			lRet  = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\TypedURLs") );
	if (lRet != ERROR_SUCCESS)
		return false;

	std::list< CString >	urls;
	__Mtl_AddFromComboBox( hWndComboBoxEx, std::back_inserter(urls) );

	if (urls.size() == 0)
		return false;

	int nSize = 25;
	if (nSize > (int) urls.size())
		nSize = (int) urls.size();

	std::list< CString >::iterator	end   = urls.begin();
	std::advance(end, nSize);
	MtlWriteProfileString(urls.begin(), end, rk, _T("url"), 1);

	return true;
}




////////////////////////////////////////////////////////////////////////////
// CAddressBarCtrl

enum EAbr_Ex {
	ABR_EX_AUTOCOMPLETE 	= 0x00000001L,
	ABR_EX_LOADTYPEDURLS	= 0x00000002L,
	ABR_EX_GOBTNVISIBLE 	= 0x00000004L,
	//UH
	ABR_EX_TEXTVISIBLE		= 0x00000008L,
	ABR_EX_ENTER_CTRL		= 0x00000010L,
	ABR_EX_ENTER_SHIFT		= 0x00000020L,
	//minit
	ABR_EX_SEARCH_REPLACE	= 0x00000040L,
	ABR_EX_USER 			= 0x00010000L,
	ABR_PANE_STYLE			= ( WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN /*| WS_OVERLAPPED | WS_MAXIMIZEBOX*/ | CCS_NODIVIDER | CCS_NOMOVEY),

	// for Donut
	ABR_EX_OPENNEWWIN		= 0x00010000L,
	ABR_EX_NOACTIVATE		= 0x00020000L,
};



class CAddressBarCtrlBase : public CComboBoxEx {
public:
};



template < class T, class TBase = CAddressBarCtrlBase, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE 	CAddressBarCtrlImpl
		: public CWindowImpl < T, TBase, TWinTraits >
		, public IDropTargetImpl < T >
		, public IDropSourceImpl < T >
{
	typedef 	CWindowImpl < T, TBase, TWinTraits >  baseClass;

	DECLARE_WND_SUPERCLASS( NULL, TBase::GetWndClassName() )

	struct CGoBtnInfo {
		CGoBtnInfo(/*+++ UINT nImageBmpID, UINT nHotImageBmpID,*/ int cx, int cy, COLORREF clrMask, UINT nFlags)
			: /*+++ _nImageBmpID(nImageBmpID) , _nHotImageBmpID(nHotImageBmpID), */
			  _cx(cx)
			, _cy(cy)
			, _clrMask(clrMask)
			, _nFlags(nFlags)
		{
		}

		//+++ UINT	_nImageBmpID;
		//+++ UINT	_nHotImageBmpID;
		int 		_cx;
		int 		_cy;
		COLORREF	_clrMask;
		UINT		_nFlags;
	};

	// Constants
	enum { s_kcxGap = 2 };

public:
	DWORD						m_dwAddressBarExtendedStyle;
	CFlatComboBox				m_comboFlat;

protected:
	CToolBarCtrl				m_wndGo;
	CGoBtnInfo *				m_pGoBtnInfo;
	CContainedWindow			m_wndCombo;
	CContainedWindowT< CEdit >	m_wndEdit;

	CFont						m_font;
	HWND						m_hWndParent;		//+++
	//CToolTipCtrl				m_toolTipCtrl;		//+++

	UINT						m_nGoBtnCmdID;
	int 						m_cx;
	int 						m_cxGoBtn;
	SIZE						m_szGoIcon;
	bool						m_bDragFromItself;

	//+++ 手抜きで自身のポインタを控える...
	static CAddressBarCtrlImpl* s_pThis_;

public:
	// Ctor/dtor
	CAddressBarCtrlImpl()
		: m_dwAddressBarExtendedStyle( ABR_EX_LOADTYPEDURLS | ABR_EX_TEXTVISIBLE | ABR_EX_ENTER_CTRL | ABR_EX_ENTER_SHIFT | ABR_EX_SEARCH_REPLACE /*ABR_EX_AUTOCOMPLETE|ABR_EX_GOBTNVISIBLE */ )
		, m_nGoBtnCmdID(0)
		, m_cxGoBtn(0)
		, m_wndCombo(this, 1)
		, m_wndEdit(this, 2)
		, m_bDragFromItself(false)
		, m_cx(0)							//+++
		, m_pGoBtnInfo(0)					//+++
		, m_hWndParent(0)					//+++
	{
		m_szGoIcon.cx = m_szGoIcon.cy = 0;	//+++
		ATLASSERT(s_pThis_ == 0);			//+++
		s_pThis_	  = this;				//+++
	}

	~CAddressBarCtrlImpl() {
		s_pThis_	  = 0;					//+++
	}

	HWND	Create( HWND		hWndParent,
					UINT		nID,
					UINT		nGoBtnCmdID,
					/*UINT		nImageBmpID, UINT		nHotImageBmpID, */
					int 		cx,
					int 		cy,
					COLORREF	clrMask,
					UINT		nFlags = ILC_COLOR24)
	{
		m_nGoBtnCmdID	= nGoBtnCmdID;
		m_pGoBtnInfo	= new CGoBtnInfo(/*+++ nImageBmpID, nHotImageBmpID,*/ cx, cy, clrMask, nFlags);
		m_hWndParent	= hWndParent;		//+++
		return baseClass::Create(hWndParent, CRect(0,0,500,250), NULL, ABR_PANE_STYLE | CBS_DROPDOWN | CBS_AUTOHSCROLL, 0, nID);
	}


	// Attributes
	bool		GetDroppedStateEx() const
	{
		return ( GetDroppedState() || _AutoCompleteWindowVisible() );
	}


	void		ModifyAddressBarExtendedStyle(DWORD dwRemove, DWORD dwAdd)
	{
		DWORD dwOldStyle = m_dwAddressBarExtendedStyle;
		bool  bOldShow	 = _check_flag(dwOldStyle, ABR_EX_GOBTNVISIBLE);

		m_dwAddressBarExtendedStyle = (m_dwAddressBarExtendedStyle & ~dwRemove) | dwAdd;

		bool  bNewShow	 = _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_GOBTNVISIBLE);

		if (bNewShow != bOldShow)
			_ShowGoButton(bNewShow);
	}


	void		SetAddressBarExtendedStyle(DWORD dwStyle)
	{
		DWORD dwOldStyle = m_dwAddressBarExtendedStyle;
		bool  bOldShow	 = _check_flag(dwOldStyle, ABR_EX_GOBTNVISIBLE);
		bool  bOldText	 = _check_flag(dwOldStyle, ABR_EX_TEXTVISIBLE);

		m_dwAddressBarExtendedStyle = dwStyle;

		bool  bNewShow	 = _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_GOBTNVISIBLE);
		bool  bNewText	 = _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_TEXTVISIBLE);

		if (bNewShow != bOldShow)
			_ShowGoButton(bNewShow);

		// U.H
		if (bNewText != bOldText) {
			CWindow wnd( GetParent() );

			SendMessage(wnd.GetParent(), WM_USER_SHOW_TEXT_CHG, bNewText, 0);
		}
	}


	DWORD		GetAddressBarExtendedStyle() const
	{
		return m_dwAddressBarExtendedStyle;
	}


	static CAddressBarCtrlImpl* GetInstance() { return s_pThis_; }	//+++


private:	//public:
	// Overridables

	void		OnItemSelected(const CString & str) {}
	void		OnItemSelectedEx(const CString &str) {}
	void		OnInit() {}
	void		OnTerm() {}
	void		OnGetItem(const CString &str, COMBOBOXEXITEM &item) {}
	void		OnGetDispInfo(COMBOBOXEXITEM &item) {}


public:
	// Methods
	BYTE	PreTranslateMessage(MSG *pMsg)
	{
//m_toolTipCtrl.RelayEvent(pMsg);		//+++ アドレス編集バーでのツールチップ表示... //*これは意味ないかも...

		UINT msg  = pMsg->message;
		int  vKey = (int) pMsg->wParam;

		if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN) {
			if ( !IsWindowVisible() || !IsChild(pMsg->hwnd) )				// ignore
				return _MTL_TRANSLATE_PASS;

			// left or right pressed, check shift and control key.
			if (vKey == VK_UP || vKey == VK_DOWN || vKey == VK_LEFT || vKey == VK_RIGHT || vKey == VK_HOME || vKey == VK_END
				|| vKey == (0x41 + 'C' - 'A') || vKey == (0x41 + 'V' - 'A') || vKey == (0x41 + 'X' - 'A') || vKey == VK_INSERT)
			{
				if (::GetKeyState(VK_SHIFT) < 0 || ::GetKeyState(VK_CONTROL) < 0)
					return _MTL_TRANSLATE_WANT; 			// pass to edit control
			}

			// return key have to be passed
			if (vKey == VK_RETURN) {
				return _MTL_TRANSLATE_WANT;
			}

			// other key have to be passed
			if (VK_LBUTTON <= vKey && vKey <= VK_HELP) {
				BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
				if (!bAlt && ::GetKeyState(VK_SHIFT) >= 0 && ::GetKeyState(VK_CONTROL) >= 0)	// not pressed
					return _MTL_TRANSLATE_WANT; 			// pass to edit control
			}
		}

		return _MTL_TRANSLATE_PASS;
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CAddressBarCtrlBaseImpl)
		MSG_WM_CREATE	( OnCreate	)
		MSG_WM_DESTROY	( OnDestroy )
		MSG_WM_COMMAND	( OnCommand )
		MSG_WM_SIZE 	( OnSize	)
		MSG_WM_SETTEXT	( OnSetText )
		MESSAGE_HANDLER ( WM_ERASEBKGND,		OnEraseBackground		)
		MESSAGE_HANDLER ( WM_WINDOWPOSCHANGING, OnWindowPosChanging 	)
		REFLECTED_COMMAND_CODE_HANDLER_EX( CBN_SELENDOK, OnCbnSelendok	)
		NOTIFY_CODE_HANDLER( TTN_GETDISPINFO,	OnToolTipText			)
		REFLECTED_NOTIFY_CODE_HANDLER_EX( CBEN_GETDISPINFO, OnCbenGetDispInfo )
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER ( WM_WINDOWPOSCHANGING, OnComboWindowPosChanging)
		MESSAGE_HANDLER ( WM_ERASEBKGND,		OnComboEraseBackground	)
		MESSAGE_HANDLER ( WM_LBUTTONDOWN,		OnLButtonDown			)
		MESSAGE_HANDLER ( WM_RBUTTONDOWN,		OnRButtonDown			)
		MESSAGE_HANDLER ( WM_LBUTTONDBLCLK, 	OnLButtonDoubleClick	) // UDT DGSTR
		NOTIFY_CODE_HANDLER( TTN_GETDISPINFO,	OnToolTipText_for_Edit	)	//+++
	ALT_MSG_MAP(2)
		MESSAGE_HANDLER ( WM_ERASEBKGND,		OnEditEraseBackground	)
		MESSAGE_HANDLER ( WM_LBUTTONDBLCLK, 	OnEditLButtonDblClk 	)
		MSG_WM_KEYDOWN	( OnEditKeyDown )
	END_MSG_MAP()


private:
	LRESULT 	OnSetText(LPCTSTR lpszText)
	{
		CString 		strText(lpszText);

		if (MtlGetWindowText( GetEditCtrl() ) == strText)
			return TRUE;

		COMBOBOXEXITEM	item;

		// Strangely, Icon on edit control never be changed... tell me why?
		//		pT->OnGetItem(lpszText, item);
		item.mask	 = CBEIF_TEXT;
		item.iItem	 = -1;							// on edit control
		item.pszText = (LPTSTR) (LPCTSTR) strText;
		MTLVERIFY( SetItem(&item) );

		return TRUE;
	}


	LRESULT 	OnCreate(LPCREATESTRUCT)
	{
		//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		// フォント
		CIniFileI	prFont( g_szIniFileName, _T("Main") );

		MTL::CLogFont lf;
		lf.InitDefault();

		if ( lf.GetProfile(prFont) ) {
			CFontHandle font;

			MTLVERIFY( font.CreateFontIndirect(&lf) );

			if (font.m_hFont) {
				if (m_font.m_hFont != NULL)
					m_font.DeleteObject();

				m_font.Attach(font.m_hFont);
				SetFont(m_font);
			}
		}

		prFont.Close();
		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		_GetProfile();

		_CreateGoButton(/*+++  m_pGoBtnInfo->_nImageBmpID, m_pGoBtnInfo->_nHotImageBmpID, */
						m_pGoBtnInfo->_cx,
						m_pGoBtnInfo->_cy,
						m_pGoBtnInfo->_clrMask,
						m_pGoBtnInfo->_nFlags);

		delete		m_pGoBtnInfo;
		m_pGoBtnInfo	= NULL; 	//+++	delete後は強制クリア.

		LRESULT 	  lRet = DefWindowProc();

		SetExtendedStyle(0, CBES_EX_NOSIZELIMIT);

		if ( _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_AUTOCOMPLETE) )
			MtlAutoComplete( GetEditCtrl() );

		if ( _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_LOADTYPEDURLS) )
			_LoadTypedURLs();

		if ( _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_GOBTNVISIBLE) )
			_ShowGoButton(true);
		else
			_ShowGoButton(false);

		m_wndCombo.SubclassWindow( GetComboCtrl() );
		m_wndEdit.SubclassWindow( GetEditCtrl() );
		m_comboFlat.FlatComboBox_Install( GetComboCtrl() );

		// GetEditCtrl().SetLimitText(0x1000);

	  #if 1	//+++ エディット部分にツールTIPを追加
		{
			CToolTipCtrl	toolTipCtrl;
			toolTipCtrl.Create(m_hWnd/*Parent*/);
			toolTipCtrl.Activate(TRUE);
			//CToolInfo		toolInfo(TTF_SUBCLASS, GetEditCtrl(), 0, NULL, _T("テスト") );
			CToolInfo		toolInfo(TTF_SUBCLASS, GetEditCtrl(), 0, NULL, LPTSTR(-1LL) );
			toolTipCtrl.AddTool(toolInfo);
		}
	  #endif

		T * 	pT	 = static_cast< T * >(this);

		pT->OnInit();

		return lRet;
	}


	void		OnDestroy()
	{
		_WriteProfile();
		MtlDestroyImageLists(m_wndGo);

		if ( _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_LOADTYPEDURLS) )
			MtlSaveTypedURLs(m_hWnd);

		m_wndCombo.UnsubclassWindow();
		m_wndEdit.UnsubclassWindow();

		T * 	pT = static_cast< T * >(this);

		pT->OnTerm();
	}


	void		OnSize(UINT nFlags, CSize size)
	{
		abrTRACE( _T("CAddressBarCtrlImpl::OnSize\n") );

		int   cxGo = _GetGoBtnWidth();
		CRect rc;

		GetClientRect(rc);
		m_wndGo.MoveWindow( rc.right - cxGo + s_kcxGap, 0, cxGo, rc.Height() );
		InvalidateRect( CRect( rc.right - cxGo, 0, rc.right - cxGo + s_kcxGap, rc.Height() ) );
	}


	LRESULT 	OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled */ )
	{
		LPWINDOWPOS lpWP = (LPWINDOWPOS) lParam;

		m_cx = lpWP->cx;

		LRESULT 	lRet = DefWindowProc(uMsg, wParam, lParam);

		if (m_wndGo.m_hWnd) {
			CSize size;

			m_wndGo.GetButtonSize(size);

			if (lpWP->cy < size.cy)
				lpWP->cy = size.cy;
		}

		return lRet;
	}


	void		OnCommand(UINT, int nID, HWND hWndCtrl)
	{
		if (hWndCtrl == m_wndGo.m_hWnd) {
			ATLASSERT(nID == m_nGoBtnCmdID);
			CEdit	edit = GetEditCtrl();
			CString str  = MtlGetWindowText(edit);

			if ( !str.IsEmpty() ) {
				T	*pT = static_cast< T * >(this);
				pT->OnItemSelected(str);
			}
		} else {
			SetMsgHandled(FALSE);
		}
	}


	void		OnCbnSelendok(UINT, int, HWND)
	{
		CString strLB;

		MtlGetLBTextFixed(m_hWnd, GetCurSel(), strLB);
		CEdit	edit	= GetEditCtrl();
		CString strText = MtlGetWindowText(edit);

		if (strText != strLB) {
			T * 	pT = static_cast< T * >(this);
			pT->OnItemSelected(strLB);
			_MoveToTopAddressBox(strLB);
		}
	}


	LRESULT 	OnCbenGetDispInfo(LPNMHDR lpnmhdr)
	{
		PNMCOMBOBOXEX pDispInfo = (PNMCOMBOBOXEX) lpnmhdr;
		T		   *  pT		= static_cast< T * >(this);

		pT->OnGetDispInfo(pDispInfo->ceItem);

		return 0;
	}


	LRESULT 	OnEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled)
	{
		// UDT JOBBY
		HWND	hWnd	= GetParent();
		CPoint	pt(0, 0);

		MapWindowPoints(hWnd, &pt, 1);
		pt = ::OffsetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );
		LRESULT lResult = ::SendMessage(hWnd, WM_ERASEBKGND, wParam, 0L);

		::SetWindowOrgEx( (HDC) wParam, pt.x, pt.y, NULL );

		return lResult;
	}


	LRESULT 	OnComboEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled)
	{
		return 1;			// don't do the default erase
	}


	LRESULT 	OnComboWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & /*bHandled */ )
	{
		abrTRACE( _T("CAddressBarCtrlImpl::OnComboWindowPosChanging\n") );

		LRESULT 	lRet = m_wndCombo.DefWindowProc(uMsg, wParam, lParam);
		LPWINDOWPOS lpWP = (LPWINDOWPOS) lParam;

		lpWP->cx = m_cx - _GetGoBtnWidth();
		return lRet;
	}


	LRESULT 	OnEditEraseBackground(UINT /*uMsg */ , WPARAM wParam, LPARAM /*lParam */ , BOOL &bHandled)
	{
		return 1;	// don't do the default erase
	}


	LRESULT 	OnEditLButtonDblClk(UINT /*uMsg */ , WPARAM /*wParam */ , LPARAM /*lParam */ , BOOL & /*bHandled */ )
	{	// fixed for Win2000 by DOGSTORE
		m_wndEdit.SetSel(0, -1);
		return 0;
	}


	void		OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		// if not dropped, eat VK_DOWN
		if ( !GetDroppedState() && !_AutoCompleteWindowVisible() && (nChar == VK_DOWN || nChar == VK_UP) ) {
			SetMsgHandled(TRUE);
		} else if (nChar == VK_RETURN) {
			if (::GetKeyState(VK_CONTROL) < 0 || ::GetKeyState(VK_SHIFT) < 0)
				_OnEnterKeyDownEx();
			else
				_OnEnterKeyDown();

			SetMsgHandled(TRUE);
		} else {
			SetMsgHandled(FALSE);
		}
	}


	LRESULT 	OnToolTipText(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled */ )
	{
		LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;

		pDispInfo->szText[0] = 0;

		if ( (idCtrl != 0) && !(pDispInfo->uFlags & TTF_IDISHWND) ) {
			CString strHeader = _T('\"');
			CString strHelp   = _T("\" へ移動");
			enum { BUFFSZ = 80 };
			TCHAR	szBuff[BUFFSZ+1];
			szBuff[0] = 0;
			CString strSrc	  = MtlGetWindowText( GetEditCtrl() );

			if ( strSrc.IsEmpty() )
				return 0;

			AtlCompactPathFixed( szBuff, strSrc, BUFFSZ - strHelp.GetLength() - strHeader.GetLength() );
			strHeader += szBuff;
			strHeader += strHelp;
			::lstrcpyn(pDispInfo->szText, strHeader, BUFFSZ);
		}

		return 0;
	}


	///+++ アドレス編集バーでのツールチップ表示.
	LRESULT 	OnToolTipText_for_Edit(int idCtrl, LPNMHDR pnmh, BOOL & /*bHandled */ )
	{
		LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO) pnmh;

		pDispInfo->szText[0] = 0;

		//if ( (idCtrl != 0) )
		{
			CString str = MtlGetWindowText( GetEditCtrl() );
			if ( str.IsEmpty() )
				return 0;
			if (str.Find(_T('%')) >= 0)
				str = Misc::urlstr_decodeJpn(str);
			enum { L = 80 };
			if (str.GetLength() >= L)
				AtlCompactPathFixed( pDispInfo->szText, str, L /* - str.GetLength() */ );
			else
				::lstrcpyn(pDispInfo->szText, str, L);
			pDispInfo->szText[L-1] = 0;
		}

		return 0;
	}


	// Implementation
	void		_GetProfile()
	{
		CString 	strSection = _T("AddressBar#");

		strSection.Append( GetDlgCtrlID() );

		CIniFileI	pr(g_szIniFileName, strSection);
		pr.QueryValue( m_dwAddressBarExtendedStyle, _T("ExtendedStyle") );
		pr.Close();
	}


	void		_WriteProfile()
	{
		CString 	strSection = _T("AddressBar#");
		strSection.Append( GetDlgCtrlID() );
		CIniFileO	pr(g_szIniFileName, strSection);
		pr.SetValue( m_dwAddressBarExtendedStyle, _T("ExtendedStyle") );
	}


	void		_MoveToTopAddressBox(const CString &strURL)
	{
		T		   *   pT	  = static_cast< T * >(this);
		COMBOBOXEXITEM item;

		pT->OnGetItem(strURL, item);
		item.iItem	 = 0;							// add to top
		item.pszText = (LPTSTR) (LPCTSTR) strURL;

		// search the same string
		int 		   nCount = GetCount();

		for (int n = 0; n < nCount; ++n) {
			CString str;

			MtlGetLBTextFixed(m_hWnd, n, str);

			if (strURL == str) {
				DeleteItem(n);
				InsertItem(&item);
				break;
			}
		}
	}


	void		_OnEnterKeyDown()
	{
		CEdit	edit = GetEditCtrl();
		CString str  = MtlGetWindowText(edit);		//some comboboxex control impl often not support GetWindowText

		if ( !str.IsEmpty() ) {
			if ( _AutoCompleteWindowVisible() ) 	// I have to clean auto complete window, but escape means undo...
				::SendMessage(GetEditCtrl(), WM_KEYDOWN, (WPARAM) VK_ESCAPE, 0);

			T		   *pT = static_cast< T * >(this);

			pT->OnItemSelected(str);
			_AddToAddressBoxUnique(str);
		}
	}


	void		_OnEnterKeyDownEx()
	{
		CEdit		edit	= GetEditCtrl();
		CString 	str 	= MtlGetWindowText(edit);									//some comboboxex control impl often not support GetWindowText

		if ( str.IsEmpty() )
			return;

		if ( _check_flag(ABR_EX_SEARCH_REPLACE, m_dwAddressBarExtendedStyle) )			//minit
			str.Replace( _T("　"), _T(" ") );

		CIniFileI	pr( g_szIniFileName, STR_ADDRESS_BAR );
		CString 	strEnterCtrl  = pr.GetStringUW(_T("EnterCtrlEngin"));
		CString 	strEnterShift = pr.GetStringUW(_T("EnterShiftEngin"));
		pr.Close();

		if (::GetKeyState(VK_CONTROL) < 0) {
			if ( !_check_flag(ABR_EX_ENTER_CTRL, m_dwAddressBarExtendedStyle) ) {
				_OnEnterKeyDown();
				return;
			}

			// Ctrl時
			::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) str, (LPARAM) (LPCTSTR) strEnterCtrl);
			return;

		} else if (::GetKeyState(VK_SHIFT) < 0) {
			if ( !_check_flag(ABR_EX_ENTER_SHIFT, m_dwAddressBarExtendedStyle) ) {
				_OnEnterKeyDown();
				return;
			}
			// Shift時
			::SendMessage(GetTopLevelParent(), WM_SEARCH_WEB_SELTEXT, (WPARAM) (LPCTSTR) str, (LPARAM) (LPCTSTR) strEnterShift);
			return;
		}

		if ( _AutoCompleteWindowVisible() ) 		// I have to clean auto complete window, but escape means undo...
			::SendMessage(GetEditCtrl(), WM_KEYDOWN, (WPARAM) VK_ESCAPE, 0);

		T		   *pT = static_cast< T * >(this);

		pT->OnItemSelectedEx(str);

		_AddToAddressBoxUnique(str);
	}


	bool		_AutoCompleteWindowVisible() const
	{
		CRect  rc;

		m_wndEdit.GetWindowRect(&rc);
		CPoint pt(rc.left + 1, rc.bottom + 5);
		HWND   hWndDropDown = ::WindowFromPoint(pt);

		if			( MtlIsFamily(m_hWnd, hWndDropDown) )
			return false;
		else
			return true;
	}



	void	  _AddToAddressBoxUnique(const CString &strURL)
	{
		T		   *   pT	  = static_cast< T * >(this);
		COMBOBOXEXITEM item;

		pT->OnGetItem(strURL, item);
		item.iItem	 = 0;							// ado to top
		item.pszText = (LPTSTR) (LPCTSTR) strURL;

		// search the same string
		int 		   nCount = GetCount();

		for (int n = 0; n < nCount; ++n) {
			CString str;

			MtlGetLBTextFixed(m_hWnd, n, str);

			if (strURL == str) {
				DeleteItem(n);
				break;
			}
		}

		InsertItem(&item);
	}


	void		_ShowGoButton(bool bShow)
	{
		if (bShow)
			m_wndGo.ShowWindow(SW_SHOWNORMAL);
		else
			m_wndGo.ShowWindow(SW_HIDE);

		// generate WM_WINDOWPOSCHANGING, no other way
		CRect rect;

		GetWindowRect(rect);
		CWindow( GetParent() ).ScreenToClient(rect);
		SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOREDRAW);	// | SWP_NOACTIVATE);
		SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left	  , rect.bottom - rect.top, SWP_NOZORDER);					// | SWP_NOACTIVATE);
	}


	int 		_GetGoBtnWidth()
	{
		abrTRACE( _T("CAddressBarCtrlImpl::_GetGoBtnWidth\n") );

		if ( !m_wndGo.m_hWnd || !_check_flag(m_dwAddressBarExtendedStyle, ABR_EX_GOBTNVISIBLE) ) {
			abrTRACE( _T(" return 0\n") );
			return 0;
		}

		return m_cxGoBtn + s_kcxGap;
	}


public:
	/// Strangly GetButtonInfo lies...
	int 		_CalcBtnWidth(int cxIcon, const CString &strText)
	{
		const int cxGap  = 3;

		if ( strText.IsEmpty() )
			return cxGap * 2 + cxIcon;

		int 	  nWidth = 0;
		nWidth += cxGap * 2;
		nWidth += cxIcon;
		nWidth += cxGap * 2 - 1;
		nWidth += MtlComputeWidthOfText( strText, m_wndGo.GetFont() );
		// nWidth += cxGap*2;

		return nWidth;
	}


private:
	int 		_CalcBtnHeight(int cyIcon)
	{
		const int		cyGap = 3;
		MTL::CLogFont	lf;
		CFontHandle( m_wndGo.GetFont() ).GetLogFont(&lf);
		int 	  cy	= lf.lfHeight;
		if (cy < 0)
			cy = -cy;

		return std::max(cy, cyIcon + cyGap * 2);
	}


	bool		_LoadTypedURLs()
	{
		Misc::CRegKey 	rk;
		LONG			lRet = rk.Open( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\TypedURLs") );

		if (lRet != ERROR_SUCCESS)
			return false;

		std::list< CString >			urls;
		if ( !MtlGetProfileString(rk, std::back_inserter(urls), _T("url"), 1, 25) ) 		// get only 25 items
			return false;

		T * 							pT	 = static_cast< T * >(this);
		std::list< CString >::iterator	it;

		for (it = urls.begin(); it != urls.end(); ++it) {
			CString 	   strUrl = *it;
			COMBOBOXEXITEM item;

			pT->OnGetItem(strUrl, item);
			item.iItem		= -1;						// add to tail
			item.pszText	= (LPTSTR) (LPCTSTR)strUrl;
			InsertItem(&item);
		}

		return true;
	}


	CString 	GetSkinGoBtnPath(BOOL bHot)
	{
		LPCTSTR	pTmp;
		if (bHot)
			pTmp = _T("GoHot.bmp");
		else
			pTmp = _T("Go.bmp");

		return _GetSkinDir() + pTmp;
	}


	HWND		_CreateGoButton(/*+++ UINT nImageBmpID, UINT nHotImageBmpID,*/ int cx, int cy, COLORREF clrMask, UINT nFlags = ILC_COLOR24)
	{
		m_szGoIcon.cx = cx;
		m_szGoIcon.cy = cy;

		CImageList imgs;
		MTLVERIFY( imgs.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

		CBitmap    bmp;
		bmp.Attach( AtlLoadBitmapImage(LPCTSTR(GetSkinGoBtnPath(FALSE)), LR_LOADFROMFILE) );
		if (bmp.m_hBitmap == NULL)
			bmp.LoadBitmap(IDB_GOBUTTON/*nImageBmpID*/);		//+++ skin設定がここでやってるので、この場で直接デフォルト設定.
		imgs.Add(bmp, clrMask);

		CImageList imgsHot;
		MTLVERIFY( imgsHot.Create(cx, cy, nFlags | ILC_MASK, 1, 1) );

		CBitmap    bmpHot;
		bmpHot.Attach( AtlLoadBitmapImage(LPCTSTR(GetSkinGoBtnPath(TRUE)), LR_LOADFROMFILE) );
		if (bmpHot.m_hBitmap == NULL)
			bmpHot.LoadBitmap(IDB_GOBUTTON_HOT/*nHotImageBmpID*/);		//+++ skin設定がここでやってるので、この場で直接デフォルト設定.

		imgsHot.Add(bmpHot, clrMask);

		m_wndGo = ::CreateWindowEx(0,
								   TOOLBARCLASSNAME,
								   NULL,
								   ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST | CCS_TOP,
								   0,
								   0,
								   100,
								   100,
								   m_hWnd,
								   (HMENU) 12,
								   _Module.GetModuleInstance(),
								   NULL);

		m_wndGo.SetButtonStructSize( sizeof (TBBUTTON) );

		// フォント
		if (m_font.m_hFont)
			m_wndGo.SetFont(m_font.m_hFont);

		// init button size
		if ( _check_flag(m_dwAddressBarExtendedStyle, ABR_EX_TEXTVISIBLE) )
			m_cxGoBtn = _CalcBtnWidth( cx, _T("移動") );
		else
			m_cxGoBtn = _CalcBtnWidth( cx, _T("") );

		m_wndGo.SetButtonSize( CSize( m_cxGoBtn, _CalcBtnHeight(cy) ) );

		m_wndGo.SetImageList(imgs);
		m_wndGo.SetHotImageList(imgsHot);

		// one button
		TBBUTTON	 btn = { 0, m_nGoBtnCmdID, TBSTATE_ENABLED, TBSTYLE_BUTTON /* | TBSTYLE_AUTOSIZE */ , 0, 0 };
		MTLVERIFY( m_wndGo.InsertButton(-1, &btn) );

		TBBUTTONINFO	bi;
		memset( &bi, 0, sizeof (bi) );
		bi.cbSize  = sizeof (TBBUTTONINFO);
		bi.dwMask  = TBIF_TEXT;
		bi.pszText = _T("移動");
		MTLVERIFY( m_wndGo.SetButtonInfo(m_nGoBtnCmdID, &bi) );

		m_wndGo.AddStrings( _T("NS\0") );			// for proper item height

		return m_wndGo.m_hWnd;
	}


public:
	void		ReloadSkin(int nCmbStyle)
	{
		m_comboFlat.SetDrawStyle(nCmbStyle);

		if (m_wndGo.m_hWnd) {
			CImageList imgs, imgsHot;

			imgs	= m_wndGo.GetImageList();
			imgsHot = m_wndGo.GetHotImageList();
			_ReplaceImageList(GetSkinGoBtnPath(FALSE), imgs);
			_ReplaceImageList(GetSkinGoBtnPath(TRUE), imgsHot);
			m_wndGo.InvalidateRect(NULL, TRUE);
		}

		InvalidateRect(NULL, TRUE);
	}


private:
	// IDropSource
	// Overridables

	HRESULT 	OnGetAddressBarCtrlDataObject(IDataObject **ppDataObject)
	{
		ATLASSERT(ppDataObject != NULL);
		*ppDataObject = NULL;
		return E_NOTIMPL;
	}


	LRESULT 	OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if (wParam & MK_CONTROL) {
			bHandled = FALSE;
			return 0;
		}

		if ( _HitTest(pt) ) {
			_DoDragDrop(pt, (UINT) wParam, true);
		} else {
		  #if 0 //+++ キャプション無しモードの時に、窓を動かせるようにするための処理... やめ
			Donut_FakeCaptionLButtonDown(m_hWnd, GetTopLevelWindow(), lParam);
		  #endif
			bHandled = FALSE;
		}
		return 0;
	}


	LRESULT 	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if (wParam & MK_CONTROL) {
			bHandled = FALSE;
			return 0;
		}

		if ( _HitTest(pt) ) {
			_DoDragDrop(pt, (UINT) wParam, true);
		} else
			bHandled = FALSE;

		return 0;
	}


	// UDT DGSTR
	LRESULT 	OnLButtonDoubleClick(UINT /*uMsg */ , WPARAM /*wParam */ , LPARAM /*lParam */ , BOOL & /*bHandled */ )
	{
		CString strUrl	= MtlGetWindowText(m_wndEdit);

		if ( strUrl.IsEmpty() )
			return 0;

		//+++ IEの代わりに他のexeを実行可能にした.
		CIniFileI	pr( g_szIniFileName, STR_ADDRESS_BAR );
		CString 	strIeExePath = pr.GetStringUW( _T("IeExePath") );
		pr.Close();
		if (strIeExePath.IsEmpty() == 0)	//+++ 何か設定されていたら、フルパス化しておく.
			strIeExePath = Misc::GetFullPath_ForExe( strIeExePath );

		if (strIeExePath.IsEmpty() || Misc::IsExistFile( strIeExePath ) == 0)
		{
			TCHAR	szIEPath[MAX_PATH];
			DWORD	dwCount = MAX_PATH;	//+++ * sizeof (TCHAR);
			Misc::CRegKey 	rk;
			LONG	lRet	= rk.Open( HKEY_LOCAL_MACHINE
									, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE")
									, KEY_QUERY_VALUE);
			if (lRet == ERROR_SUCCESS) {
				szIEPath[0] = 0;				//+++
				rk.QueryStringValue(NULL, szIEPath, &dwCount);
				//ShellExecute(NULL, _T("open"), szIEPath, strUrl, NULL, SW_SHOW);
				strIeExePath = szIEPath;		//+++
				rk.Close();
			}
		}
		if (strIeExePath.IsEmpty() == 0)	//+++
			ShellExecute(NULL, _T("open"), strIeExePath, strUrl, NULL, SW_SHOW);

		return 0;
	}
	// ENDE



	bool		_HitTest(CPoint pt)
	{
		CRect rc;

		m_wndCombo.GetClientRect(rc);

		CRect rcEdit;

		m_wndEdit.GetWindowRect(&rcEdit);
		m_wndCombo.ScreenToClient(&rcEdit);

		rc.right = rcEdit.left;

	  #if 1	//+++
		return rc.PtInRect(pt) != 0;
	  #else
		if ( rc.PtInRect(pt) )
			return true;
		else
			return false;
	  #endif
	}


	void		_DoDragDrop(CPoint pt, UINT nFlags, bool bLeftButton)
	{
		if ( PreDoDragDrop(m_hWnd, NULL, false) ) { 	// now dragging
			CComPtr < IDataObject > spDataObject;
			T * 		pT = static_cast< T * >(this);

			HRESULT 	hr = pT->OnGetAddressBarCtrlDataObject(&spDataObject);

			if ( SUCCEEDED(hr) ) {
				m_bDragFromItself = true;
				DROPEFFECT dropEffect = DoDragDrop(spDataObject, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);

				m_bDragFromItself = false;
			}
		} else {										// canceled
			if (bLeftButton) {
			} else {
				m_wndGo.SendMessage( WM_RBUTTONUP, (WPARAM) nFlags, MAKELPARAM(pt.x, pt.y) );
			}
		}
	}


public: //+++ 手抜きで、ばっちいやりかたで、アドレスバーの文字列を取得する
	static CString GetAddressBarText()
	{
		if (s_pThis_) {
			CEdit	edit = s_pThis_->GetEditCtrl();
			return MtlGetWindowText(edit);		//some comboboxex control impl often not support GetWindowText
		}
		return CString();
	}

};


//+++ 手抜きで自身へのポインタを持つ...
template < class T, class TBase, class TWinTraits >
CAddressBarCtrlImpl<T,TBase,TWinTraits>*	CAddressBarCtrlImpl<T,TBase,TWinTraits>::s_pThis_ = 0;


class CAddressBarCtrl : public CAddressBarCtrlImpl< CAddressBarCtrl > {
public:
	DECLARE_WND_SUPERCLASS( _T("MTL_AddressBar"), GetWndClassName() )
};




//////////////////////
// for Donut


class CDonutAddressBar : public CAddressBarCtrlImpl< CDonutAddressBar >, public CCustomDraw< CDonutAddressBar > {
public:
	DECLARE_WND_SUPERCLASS( _T("Donut_AddressBar"), GetWndClassName() )

private:
	// Constants
	enum { s_kcxHeaderGap = 4 };

	// Data members
	int 				m_cxDefaultHeader;
	CItemIDList 		m_idlHtml;			// used to draw .url icon faster
	bool				m_bDragAccept;


public:
	CDonutAddressBar()
		: m_cxDefaultHeader(0)
		, m_bDragAccept(0)			//+++
	{
		m_idlHtml = MtlGetHtmlFileIDList();
	}


	~CDonutAddressBar() {}


	// Methods
	static CDonutAddressBar* GetInstance() { return (CDonutAddressBar*) CAddressBarCtrlImpl< CDonutAddressBar >::GetInstance(); }

	void		InitReBarBandInfo(CReBarCtrl rebar)
	{
		int 	nIndex = rebar.IdToIndex( GetDlgCtrlID() );

		CVersional < REBARBANDINFO > rbBand;
		rbBand.fMask	  = RBBIM_HEADERSIZE;
		MTLVERIFY( rebar.GetBandInfo(nIndex, &rbBand) );
		m_cxDefaultHeader = rbBand.cxHeader;

		// Calculate the header of the band
		HFONT		hFont = m_font.m_hFont;

		if (hFont == NULL)
			hFont = rebar.GetFont();

		int 	nWidth = MtlComputeWidthOfText(_T("アドレス"), hFont);			// UDT JOBBY (remove (&D))

		if ( !(m_dwAddressBarExtendedStyle & ABR_EX_TEXTVISIBLE) )
			nWidth = 0;

		rbBand.cxHeader   = m_cxDefaultHeader + nWidth + s_kcxHeaderGap;
	}


	// U.H
	void		ShowAddresText(CReBarCtrl rebar, BOOL bShow)
	{
		SIZE	size;

		m_wndGo.GetButtonSize(size);

		if (bShow) {
			m_cxGoBtn = _CalcBtnWidth( m_szGoIcon.cx, _T("移動") );
			m_wndGo.SetButtonSize( CSize(m_cxGoBtn, size.cy) );
		} else {
			m_cxGoBtn = _CalcBtnWidth( m_szGoIcon.cx, _T("") );
			m_wndGo.SetButtonSize( CSize(m_cxGoBtn, size.cy) );
		}

		CRect rect;

		GetWindowRect(rect);
		CWindow( GetParent() ).ScreenToClient(rect);

		SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOREDRAW);		// | SWP_NOACTIVATE);
		SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);							// | SWP_NOACTIVATE);
	}


	// required for stupid OnCustomDraw including pT->SetMsgHandled
	void		SetMsgHandled(BOOL bHandled)
	{
		CAddressBarCtrlImpl< CDonutAddressBar >::SetMsgHandled(bHandled);
	}


	BOOL		IsMsgHandled() const
	{
		return CAddressBarCtrlImpl< CDonutAddressBar >::IsMsgHandled();
	}


	// Custom draw overrides
	DWORD OnPrePaint(int /*idCtrl */ , LPNMCUSTOMDRAW /*lpNMCustomDraw */ )
	{
		return CDRF_NOTIFYITEMDRAW; 	// we need per-item notifications
	}


	// OnItemPostPaint not called
	DWORD		OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		abrTRACE(_T("CDonutAddressBar::OnItemPrePaint : dwItemSpec(%d), uItemState(%d)\n"), lpNMCustomDraw->dwItemSpec, lpNMCustomDraw->uItemState);

		if ( lpNMCustomDraw->dwItemSpec != GetDlgCtrlID() )
			return CDRF_DODEFAULT;

		if (m_cxDefaultHeader == 0) 	// InitReBarBandInfo maybe not called, do nothing.
			return CDRF_DODEFAULT;

		CDCHandle dc = lpNMCustomDraw->hdc;

		RECT &	  rc = lpNMCustomDraw->rc;
		rc.left += m_cxDefaultHeader;

		return CDRF_DODEFAULT;			// continue with the default item painting
	}


	// Methods

public: // Overrides
	void		OnItemSelected(const CString &str)
	{
		DWORD dwOpenFlags = 0;

		if ( !_check_flag( ABR_EX_OPENNEWWIN, GetAddressBarExtendedStyle() ) )
			dwOpenFlags |= D_OPENFILE_NOCREATE;

		if ( !_check_flag( ABR_EX_NOACTIVATE, GetAddressBarExtendedStyle() ) )
			dwOpenFlags |= D_OPENFILE_ACTIVATE;

		DonutToggleOpenFlag(dwOpenFlags);
		HWND  hWndNew	  = DonutOpenFile(m_hWnd, str, dwOpenFlags);
	}


	void		OnItemSelectedEx(const CString &str)
	{
		DWORD dwOpenFlags = 0;

		if ( !_check_flag( ABR_EX_OPENNEWWIN, GetAddressBarExtendedStyle() ) )
			dwOpenFlags |= D_OPENFILE_NOCREATE;

		if ( !_check_flag( ABR_EX_NOACTIVATE, GetAddressBarExtendedStyle() ) )
			dwOpenFlags |= D_OPENFILE_ACTIVATE;

		if ( !_check_flag( ABR_EX_ENTER_CTRL, GetAddressBarExtendedStyle() ) ) {
			if (::GetAsyncKeyState(VK_CONTROL) < 0) {
				if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlags) )
					dwOpenFlags &= ~D_OPENFILE_NOCREATE;
				else
					dwOpenFlags |= D_OPENFILE_NOCREATE;
			}
		}

		if ( !_check_flag( ABR_EX_ENTER_SHIFT, GetAddressBarExtendedStyle() ) ) {
			if (::GetAsyncKeyState(VK_SHIFT) < 0) {
				if ( _check_flag(D_OPENFILE_NOCREATE, dwOpenFlags) )
					dwOpenFlags &= ~D_OPENFILE_NOCREATE;
				else
					dwOpenFlags |= D_OPENFILE_NOCREATE;
			}
		}

		HWND  hWndNew = DonutOpenFile(m_hWnd, str, dwOpenFlags);
	}


	void		OnGetItem(const CString &str, COMBOBOXEXITEM &item)
	{
		item.mask			= CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;	// | CBEIF_OVERLAY;

		int 	nImage; 	// = 2;
		nImage				= I_IMAGECALLBACK;
		item.iImage 		= nImage;
		item.iSelectedImage = nImage;
		item.iOverlay		= nImage;
	}


	void		OnGetDispInfo(COMBOBOXEXITEM &item)
	{
		abrTRACE(_T("OnGetDispInfo(%s)\n"), item.pszText);

		if ( !_check_flag(CBEIF_IMAGE, item.mask) && !_check_flag(CBEIF_SELECTEDIMAGE, item.mask) )
			return;

		CString 	str;

		if ( GetDroppedState() ) {
			MtlGetLBTextFixed(GetComboCtrl(), (int) item.iItem, str);
		} else {
			str = MtlGetWindowText( GetEditCtrl() );
		}

		if ( str.IsEmpty() )
			return;

		CItemIDList idl = str;

		if ( idl.IsNull() ) {		// invalid idl
			int iImage = MtlGetSystemIconIndex(m_idlHtml);

			item.iImage 		= iImage;
			item.iSelectedImage = iImage;
			return;
		}

		if (item.mask & CBEIF_IMAGE) {
			item.iImage = MtlGetNormalIconIndex(idl, m_idlHtml);
		}

		if (item.mask & CBEIF_SELECTEDIMAGE) {
			item.iSelectedImage = MtlGetSelectedIconIndex(idl, true, m_idlHtml);
		}
	}


	void		OnInit()
	{
		_SetSystemImageList();

		COMBOBOXEXITEM item;

		item.mask			= CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		item.iItem			= -1;												// on edit control
		item.iImage 		= I_IMAGECALLBACK;
		item.iSelectedImage = I_IMAGECALLBACK;
		MTLVERIFY( SetItem(&item) );

		RegisterDragDrop();
	}


	void		OnTerm()
	{
		RevokeDragDrop();
	}


	// IDropTargetImpl
	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
	{
		if (m_bDragFromItself)
			return DROPEFFECT_NONE;

		_DrawDragEffect(false);

		m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
		return _MtlStandardDropEffect(dwKeyState);
	}


	DROPEFFECT	OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
	{
		if (m_bDragFromItself || !m_bDragAccept)
			return DROPEFFECT_NONE;

		return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
	}


	void		OnDragLeave()
	{
		if (m_bDragFromItself)
			return;

		_DrawDragEffect(true);
	}


private:
	void		_DrawDragEffect(bool bRemove)
	{
		CClientDC dc(m_wndCombo.m_hWnd);

		CRect	  rect;

		m_wndCombo.GetClientRect(rect);

		if (bRemove)
			MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(0, 0), &rect, CSize(2, 2), NULL, NULL);
		else
			MtlDrawDragRectFixed(dc.m_hDC, &rect, CSize(2, 2), NULL, CSize(2, 2), NULL, NULL);
	}


public:
	DROPEFFECT	OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
	{
		if (m_bDragFromItself)
			return DROPEFFECT_NONE;

		_DrawDragEffect(true);

		CSimpleArray < CString > arrFiles;

		if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
			// one file
			if (arrFiles.GetSize() == 1) {
				OnItemSelected(arrFiles[0]);				// allow to AddressBar style
				return DROPEFFECT_COPY;
			}

			// files
			for (int i = 0; i < arrFiles.GetSize(); ++i) {
				DonutOpenFile(m_hWnd, arrFiles[i]);
			}

			dropEffect = DROPEFFECT_COPY;
			return true;
		}

		CString 				 strText;

		if ( MtlGetHGlobalText(pDataObject, strText) || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) ) {
			OnItemSelected(strText);
			return DROPEFFECT_COPY;
		}

		return DROPEFFECT_NONE;
	}



	HRESULT 	OnGetAddressBarCtrlDataObject(IDataObject **ppDataObject)
	{
		ATLASSERT(ppDataObject != NULL);
		HRESULT hr	= CHlinkDataObject::_CreatorClass::CreateInstance(NULL, IID_IDataObject, (void **) ppDataObject);

		if ( FAILED(hr) ) {
			*ppDataObject = NULL;
			return E_NOTIMPL;
		}

	  #ifdef _ATL_DEBUG_INTERFACES
		ATLASSERT( FALSE && _T("_ATL_DEBUG_INTERFACES crashes the following\n") );
	  #endif
		CHlinkDataObject *pHlinkDataObject = NULL;	// this is hack, no need to release

		hr = (*ppDataObject)->QueryInterface(IID_NULL, (void **) &pHlinkDataObject);

		if ( SUCCEEDED(hr) ) {
			HWND		 hWnd	 = DonutGetActiveWindow( GetTopLevelParent() );

			if (hWnd == NULL)
				return E_NOTIMPL;

			ATLASSERT( ::IsWindow(hWnd) );
			CGeckoBrowser browser = DonutGetNsIWebBrowser(hWnd);

			if ( browser.IsBrowserNull() )
				return E_NOTIMPL;

			CString 	 strName = MtlGetWindowText(hWnd);
			CString 	 strUrl  = browser.GetLocationURL();

			if ( strUrl.IsEmpty() )
				return E_NOTIMPL;

			if ( strUrl.Left(5) == _T("file:") ) {			// Donut, to be explorer or not
				strName.Empty();
				strUrl = strUrl.Right(strUrl.GetLength() - 8);
				strUrl.Replace( _T('/'), _T('\\') );
			}

			pHlinkDataObject->m_arrNameAndUrl.Add( std::make_pair(strName, strUrl) );
		}

		return S_OK;
	}


private:
	void		_SetSystemImageList()
	{
		ATLASSERT( ::IsWindow(m_hWnd) );

		SHFILEINFO	sfi;
		HIMAGELIST	hImgs = (HIMAGELIST) ::SHGetFileInfo(_T("C:\\"), 0, &sfi, sizeof (sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

		ATLASSERT(hImgs != NULL);
		SetImageList(hImgs);
	}
};

