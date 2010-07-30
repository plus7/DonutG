
#ifndef FINDBAR_H
#define FINDBAR_H
#pragma once

#include "FlatComboBox.h"

class CDonutFindBar
	: public CDialogImpl< CDonutFindBar >
{
public:

	enum { IDD = IDD_FINDBAR_FORM };

	enum EEnter_KeyDown {
		ENTER_KEYDOWN_RETURN	=	0,
		ENTER_KEYDOWN_SELCHANGE =	1,
	};

	CDonutFindBar();
	~CDonutFindBar() {}

	HWND		GetHWndToolBar() { return m_wndToolBar; }
	CEdit		GetEditCtrl();
	void	OnKeywordKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BYTE		PreTranslateMessage(MSG *pMsg);
	BEGIN_MSG_MAP(CDonutSearchBarEx)
		MSG_WM_INITDIALOG	(OnInitDialog	)
	ALT_MSG_MAP(1)
		MSG_WM_KEYDOWN		(OnKeywordKeyDown)
    END_MSG_MAP()
	void _InitCombo();
	void	_OnEnterKeyDown(int flag);
private:

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);

	CToolBarCtrl	 m_wndToolBar;	
	CFlatComboBox    m_cmbKeyword;
	CContainedWindow m_wndKeyword;
	CString			 m_strKeyword;
};

#endif