/**
 *	@file	ReBarVerticalBorder.h
 *	@brief	ReBar ÇÃècÇÃã´
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
// Mtlbase.h: Last updated: February 12, 2001
////////////////////////////////////////////////////////////////////////////

#pragma once


namespace MTL {



////////////////////////////////////////////////////////////////////////////
// Add WS_BORDER style to rebar

//+++ #define MTLTRACERC(rc)	ATLTRACE(_T("(%d, %d, %d, %d)\n"), rc.left, rc.top, rc.right, rc.bottom)


enum {
	MTL_SIMPLE_REBAR_STYLE	=	ATL_SIMPLE_REBAR_STYLE | CCS_NOPARENTALIGN | CCS_NODIVIDER,
};


template <class T>
class CReBarVerticalBorder {
public:
	// Constants
	enum _ReBarBorderConstants {
		s_kcxBorder = 2
	};


private:
	CRect	m_rcNew;


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CReBarVerticalBorder<T>)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()


private:
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
	{
		T * 	pT	= static_cast<T *>(this);
		CClientDC	dc(pT->m_hWnd);
		_DoPaintReBarBorders(dc.m_hDC);
		bHandled = FALSE;
		return 0;
	}


public:
	void _DoPaintReBarBorders(CDCHandle dc)
	{
		T *   pT = static_cast<T *>(this);
		CRect rc;

		::GetWindowRect(pT->m_hWndToolBar, &rc);
		pT->ScreenToClient(&rc);

		rc.InflateRect(s_kcxBorder, s_kcxBorder);
		dc.DrawEdge(rc, EDGE_ETCHED, BF_RECT);
	}


	void _UpdateReBarPosition(const CRect &rect)
	{
		T * 	   pT = static_cast<T *>(this);
		//		MTLTRACERC(rect);
		CReBarCtrl rebar(pT->m_hWndToolBar);

		if (!rebar.m_hWnd)
			return;

		CRect	   rcSrc;
		rebar.GetClientRect(&rcSrc);

		CRect	   rcNew(s_kcxBorder, s_kcxBorder, rect.right - s_kcxBorder, rcSrc.Height() + s_kcxBorder);
		rebar.MoveWindow(rcNew, FALSE);
		CRect	   rcEraseBkgnd(rcSrc.right, rcSrc.top, rcNew.right, rcNew.bottom);

		if (rcEraseBkgnd.Width() > 0 && rcEraseBkgnd.Height() > 0) {
			rebar.InvalidateRect(rcEraseBkgnd, TRUE);
			rebar.UpdateWindow();
		}

		rebar.Invalidate(FALSE);
	}


	void _InflateReBarBordersRect(RECT &rc)
	{
		rc.top += s_kcxBorder * 2;
		//		MTLTRACERC(rc);
	}
};



#define DECLARE_MTL_REBAR_UPDATELAYOUT()									\
	void UpdateLayout(BOOL bResizeBars = TRUE)								\
	{																		\
		RECT rect;															\
		GetClientRect(&rect);												\
		if (bResizeBars) {													\
			_UpdateReBarPosition(rect); 									\
		}																	\
		{																	\
			CClientDC	dc(m_hWnd); 										\
			_DoPaintReBarBorders(dc.m_hDC); 								\
		}																	\
		UpdateBarsPosition(rect, bResizeBars);								\
		_InflateReBarBordersRect(rect); 									\
		if (m_hWndClient != NULL)											\
			::SetWindowPos( m_hWndClient, NULL, rect.left, rect.top,		\
							rect.right - rect.left, rect.bottom - rect.top, \
							SWP_NOZORDER | SWP_NOACTIVATE );				\
	}



////////////////////////////////////////////////////////////////////////////

}		// namespace MTL;



#ifndef _MTL_NO_AUTOMATIC_NAMESPACE
using namespace MTL;
#endif	//!_MTL_NO_AUTOMATIC_NAMESPACE
