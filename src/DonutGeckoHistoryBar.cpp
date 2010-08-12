/**
 *	@file	DonutGeckoHistoryBar.cpp
 *	@brief	Places‚Ì—š—ð
 */

#include "stdafx.h"
#include "DonutGeckoHistoryBar.h"
#include "Donut.h"
#include "DonutPFunc.h"
#include "option/MainOption.h"

// Ctor
CDonutGeckoHistoryBar::CDonutGeckoHistoryBar()
{
}

// Methods

// Methods
BYTE CDonutGeckoHistoryBar::PreTranslateMessage(MSG *pMsg)
{
	return _MTL_TRANSLATE_PASS;
}

LRESULT CDonutGeckoHistoryBar::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	DlgResize_Init(false, true, WS_CLIPCHILDREN);

	_GetProfile();
	_SetData();
	DoDataExchange(FALSE);

	return TRUE;
}

LRESULT CDonutGeckoHistoryBar::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);
	_GetData();
	_WriteProfile();
	return 0;
}

// Implementation
void CDonutGeckoHistoryBar::_GetData()
{
}

void CDonutGeckoHistoryBar::_SetData()
{
}

void CDonutGeckoHistoryBar::_GetProfile()
{
	CIniFileI	pr( MtlGetChangedExtFromModuleName( _T(".ini") ), _T("GeckoHistoryBar") );
	pr.Close();
}

void CDonutGeckoHistoryBar::_WriteProfile()
{
	CIniFileO	pr( MtlGetChangedExtFromModuleName( _T(".ini") ), _T("GeckoHistoryBar") );
	pr.Close();
}

void CDonutGeckoHistoryBar::_OnItemOpen(int nIndex)
{
}

