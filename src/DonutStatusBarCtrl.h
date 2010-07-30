/**
 *	@file	DonutStatusBarCtrl.h
 *	@brief	ステータス・バー
 */

#pragma once



class CDonutStatusBarCtrl : public CMultiPaneStatusBarCtrl, public COwnerDraw<CDonutStatusBarCtrl> {
public:
	DECLARE_WND_SUPERCLASS( _T("DonutStatusBar"), GetWndClassName() )

	CDonutStatusBarCtrl()
		: CMultiPaneStatusBarCtrl()
		, m_bUseBackColor(FALSE)
		, m_bOwnerdraw(FALSE)
	  #if 1 //+++
		, m_nStatusStyle(0)
		, m_colText(0)
		, m_colBack(-1)
	  #endif
	{
	}


	~CDonutStatusBarCtrl()
	{
		std::map<int, HICON>::iterator	itIcon;

		for (itIcon = m_mapIcon.begin(); itIcon != m_mapIcon.end(); ++itIcon) {
			if (itIcon->second)
				DestroyIcon( itIcon->second );
		}
	}


	void SetIcon(int nIdPane, int nIndexIcon)
	{
		HICON hIcon = NULL;

		if (nIndexIcon != USHRT_MAX) {
			if (m_mapIcon[nIndexIcon] == NULL) {
				m_mapIcon[nIndexIcon] = m_imgList.ExtractIcon(nIndexIcon);
			}

			hIcon = m_mapIcon[nIndexIcon];
		}

		SetPaneIcon(nIdPane, hIcon);
	}


	void SetCommand(int nPaneID, int nCommand)
	{
		m_mapCmdID.Add(nPaneID, nCommand);
	}


	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
		int nIndex = lpdis->itemID;

		CDC dc;

		dc.CreateCompatibleDC(lpdis->hDC);

		if (m_bmpBg.m_hBitmap) {
			HBITMAP hbmpOld = dc.SelectBitmap(m_bmpBg);

			SetBkMode(lpdis->hDC, TRANSPARENT);
			CSize	size;
			m_bmpBg.GetSize(size);
			DrawBackGround(&lpdis->rcItem, dc.m_hDC, lpdis->hDC);
			//BitBlt(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,size.cx,size.cy, dc.m_hDC,0,0,SRCCOPY);
			dc.SelectBitmap(hbmpOld);
		}

		if (nIndex == 0) {
			SIZE sizeText;
			int  Height = lpdis->rcItem.bottom - lpdis->rcItem.top + 1;

			::SetTextAlign(lpdis->hDC, TA_TOP | TA_LEFT);
			::SetTextColor(lpdis->hDC, m_colText);
			GetTextExtentPoint32(lpdis->hDC, _T("h"), 1, &sizeText);
			::TextOut( lpdis->hDC, lpdis->rcItem.left + 2, lpdis->rcItem.top + (Height - sizeText.cy) / 2,
					  m_strText, m_strText.GetLength() );
		}

		SetMsgHandled(TRUE);
	}


	void SetOwnerDraw(BOOL bOwnerdraw)
	{
		if (bOwnerdraw) {
			CString 	strBuf;
			int nCount = GetPaneCount();
			for (int i = 0; i < nCount; i++) {
				GetText(i, strBuf);
				SetText(i, strBuf, SBT_OWNERDRAW);
			}
		}

		m_bOwnerdraw = bOwnerdraw;
	}


	bool IsValidBmp() const
	{
		return m_bmpBg.m_hBitmap != 0;
	}


	void ReloadSkin(int nStyle, int colText /*=0*/, int colBack /*=-1*/)
	{
		CString 	strPath = _GetSkinDir() + _T("secure.bmp");
		_ReplaceImageList(strPath, m_imgList, IDB_SECURE);
		OnImageListUpdated();

		strPath 	   = _GetSkinDir() + _T("status.bmp");
		m_bmpBg.Attach( AtlLoadBitmapImage(strPath.GetBuffer(0), LR_LOADFROMFILE) );

		m_nStatusStyle = nStyle;

		if (m_nStatusStyle == SKN_STATUS_STYLE_THEME || m_bmpBg.m_hBitmap == NULL) {
			SetOwnerDraw(FALSE);
		} else {
			SetOwnerDraw(TRUE);
		}

	   	//+++ 呼び元から渡すようにした.
		m_colText	= COLORREF( colText <= 0 ? 0 : colText );
		if (colBack >= 0) {
			m_colBack		= colBack; // | 0xFF000000;
			m_bUseBackColor = TRUE;
			SetBkColor(m_colBack);
		} else {
			m_colBack		= COLORREF( 0xFFFFFFFF );
			m_bUseBackColor = FALSE;
			SetBkColor(CLR_DEFAULT);
		}
	}


public:
	BEGIN_MSG_MAP(CDonutStatusBarCtrl)
		USER_MSG_WM_GET_OWNERDRAWMODE (OnGetOwnerDrawMode)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MESSAGE_HANDLER 	(WM_CREATE			, OnCreate		)
		MESSAGE_HANDLER 	(WM_STATUS_SETICON	, OnSetIcon 	)
		MESSAGE_HANDLER 	(WM_STATUS_SETTIPTEXT,OnSetTipText	)
		MESSAGE_HANDLER 	(WM_SETTEXT 		, OnSetText 	)
		MESSAGE_HANDLER 	(SB_SETTEXT 		, OnSetStatusText)
		MESSAGE_HANDLER 	(SB_SIMPLE			, OnSimple		)
		CHAIN_MSG_MAP		(CMultiPaneStatusBarCtrl)
		CHAIN_MSG_MAP_ALT	(COwnerDraw<CDonutStatusBarCtrl>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


private:
	void SetText(int nPane, LPCTSTR lpszText, int nType = 0)
	{
		m_strText = lpszText;
		CMultiPaneStatusBarCtrl::SetText(nPane, lpszText, nType);
	}


	inline int GetPaneCount()
	{
		return GetParts(-1, NULL);
	}


	void DrawBackGround(LPRECT lpRect, HDC hDCSrc, HDC hDCDest)
	{
		SIZE size;
		m_bmpBg.GetSize(size);
		int  BmpWidth  = size.cx;
		int  BmpHeight = size.cy;

		if (m_nStatusStyle == SKN_STATUS_STYLE_TILE) {
			//タイル表示　面倒なのでクリッピングはしない
			for (int y = 0; y < lpRect->bottom; y += BmpHeight) {
				for (int x = 0; x < lpRect->right; x += BmpWidth) {
					::BitBlt(hDCDest, x, y, BmpWidth, BmpHeight, hDCSrc, 0, 0, SRCCOPY);
				}
			}

		} else if (m_nStatusStyle == SKN_STATUS_STYLE_STRETCH) {
			//拡大縮小表示
			RECT rcMin, rcMax;
			GetRect(0, &rcMin);
			GetRect(GetPaneCount() - 1, &rcMax);

			int  PaneWidth	= lpRect->right  - lpRect->left + 1;
			int  PaneHeight = lpRect->bottom - lpRect->top	+ 1;
			int  BarWidth	= rcMax.right	 - rcMin.left	+ 1;

			::StretchBlt(hDCDest, lpRect->left, lpRect->top,
						 PaneWidth,
						 PaneHeight,
						 hDCSrc,
						 BmpWidth * lpRect->left / BarWidth ,
						 0,
						 BmpWidth * PaneWidth / BarWidth ,
						 BmpHeight > PaneHeight ? PaneHeight : BmpHeight,
						 SRCCOPY);
		}
	}


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		DefWindowProc(uMsg, wParam, lParam);

		CString 	strPath = _GetSkinDir() + _T("secure.bmp");
		CBitmap 	bmp;
		bmp.Attach( AtlLoadBitmapImage(strPath.GetBuffer(0), LR_LOADFROMFILE) );
		if (bmp.m_hBitmap == NULL)			//+++
			bmp.LoadBitmap(IDB_SECURE); 	//+++

		if (bmp.m_hBitmap) {
			CSize szIcon;
			bmp.GetSize(szIcon);
			m_imgList.Create(szIcon.cy, szIcon.cy, ILC_COLOR24 | ILC_MASK, 1, 1);
			m_imgList.Add( bmp, RGB(255, 0, 255) );
		} else {
			m_imgList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 1);
		}

		strPath   = _GetSkinDir() + _T("status.bmp");
		m_bmpBg.Attach( AtlLoadBitmapImage(LPCTSTR(strPath), LR_LOADFROMFILE) );

		m_strText = _T("レディ");

		return 0;
	}


	LRESULT OnSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		int nIdPane    = LOWORD(wParam);
		int nIndexIcon = HIWORD(wParam);

		SetIcon(nIdPane, nIndexIcon);
		return S_OK;
	}


	LRESULT OnSetTipText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		SetPaneTipText( (int) wParam, (LPCTSTR) lParam );
		return S_OK;
	}


	LRESULT OnSimple(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		BOOL bSimple = (BOOL) wParam;
		if (bSimple) {
			m_strText = _T("");
		} else {
			m_strText = _T("レディ");
		}

		if ( !IsValidBmp() ) {
			return DefWindowProc(uMsg, wParam, lParam);
		} else {
			bHandled = FALSE;
			InvalidateRect(NULL);
			return 0;
		}
	}


	LRESULT OnGetOwnerDrawMode()
	{
		return IsValidBmp();	//+++ ? 1 : 0;
	}


	LRESULT OnSetStatusText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		int nStyle = (int) wParam;

		if ( (nStyle & 255) && IsValidBmp() ) {
			nStyle	 |= SBT_OWNERDRAW;
			m_strText = (LPCTSTR) lParam;
		}

		return DefWindowProc(uMsg, (WPARAM) nStyle, lParam);
	}


	LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = TRUE;
		if (!m_bOwnerdraw) {
			bHandled = FALSE;
			return 0;
		}

		CString 		 strNewText = (LPCTSTR) lParam;
		if (m_strText != strNewText) {
			SetText( 0,  strNewText, SBT_OWNERDRAW);
			m_strText =  strNewText;
		}

		return 0;
	}


	void OnImageListUpdated()
	{
		std::map<int, HICON>::iterator	itIcon;

		for (itIcon = m_mapIcon.begin(); itIcon != m_mapIcon.end(); ++itIcon) {
			if (itIcon->second)
				DestroyIcon( itIcon->second );

			itIcon->second = m_imgList.ExtractIcon(itIcon->first);
		}

		InvalidateRect(NULL, TRUE);
	}


	void OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		SetMsgHandled(FALSE);

		for (int nIndex = 0; nIndex < m_nPanes; nIndex++) {
			CRect rcPane;
			GetRect(nIndex, &rcPane);

			if ( !rcPane.PtInRect(point) )
				continue;

			int   nCommand = 0;
			nCommand = m_mapCmdID.Lookup(m_pPane[nIndex]);

			if (nCommand == 0)
				continue;

			::PostMessage(GetTopLevelParent(), WM_COMMAND, nCommand, 0);
		}
	}


private:
  #if 0
	struct _IconInfo {
		int 	nIconIndex;
		int 	nIDPane;
		HICON	hIcon;
	};
  #endif

	std::map<int, HICON>	m_mapIcon;
	CBitmap 				m_bmpBg;
	CImageList				m_imgList;
	CCrasyMap<int, int> 	m_mapCmdID; 		// CmdID
	CString 				m_strText;
	BOOL					m_bOwnerdraw;
	int 					m_nStatusStyle;
	COLORREF				m_colText;
	COLORREF				m_colBack;
	BOOL					m_bUseBackColor;
};
