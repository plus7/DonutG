
#include "stdafx.h"
#include <atlctrls.h>
#include "FindBar.h"

CDonutFindBar::CDonutFindBar()
	: m_wndKeyword(this, 1)
{
}

// public:
BYTE CDonutFindBar::PreTranslateMessage(MSG *pMsg)
{
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

	return _MTL_TRANSLATE_PASS;
}

LRESULT CDonutFindBar::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	CRect		rcDlg;
	GetWindowRect(&rcDlg);
	SetWindowPos(NULL, 0, 0, 0, 25/*height*/ , 0);


	//コンボボックス初期化
	m_cmbKeyword.FlatComboBox_Install( GetDlgItem(IDC_CMB_FIND ) );
	_InitCombo();
	m_wndKeyword.SubclassWindow( m_cmbKeyword.GetDlgItem(IDC_EDIT/*1001*/) );

	//if (m_nKeywordWidth != 0) {
		CRect	rcKeyword;
		m_cmbKeyword.GetWindowRect(&rcKeyword);
		int 	h		 = rcKeyword.Height();
		rcKeyword.left	 = 0;
		rcKeyword.right  = 200;//m_nKeywordWidth;
		rcKeyword.top	 = 0;
		rcKeyword.bottom = h;
		m_cmbKeyword.SetWindowPos(NULL, rcKeyword, SWP_NOZORDER | SWP_NOSENDCHANGING);
	//}

	//m_wndKeyword.SubclassWindow( m_cmbKeyword.GetDlgItem(IDC_EDIT/*1001*/) );

	//ツールバー初期化

	//ドラッグドロップ初期化
	//TODO:RegisterDragDrop();
		ShowWindow(SW_SHOW);

	return S_OK;
}

void CDonutFindBar::_InitCombo()
{
	m_cmbKeyword.SetItemHeight(-1, 9);
}

CEdit CDonutFindBar::GetEditCtrl()
{
	return CEdit( m_cmbKeyword.GetDlgItem(IDC_EDIT/*1001*/) );
}


void CDonutFindBar::_OnEnterKeyDown(int flag)
{
	CString  str;
	str = MtlGetWindowText(m_cmbKeyword);

	if ( !str.IsEmpty() ) {
		SHORT sShift = ::GetKeyState(VK_SHIFT);
		if (sShift < 0)
			SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), FALSE);
		else 
			SendMessage(GetTopLevelParent(), WM_USER_FIND_KEYWORD, (WPARAM) str.GetBuffer(0), TRUE );
	}
}

void CDonutFindBar::OnKeywordKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// if not dropped, eat VK_DOWN
	if ( !m_cmbKeyword.GetDroppedState() && (nChar == VK_DOWN || nChar == VK_UP) ) {

		SetMsgHandled(TRUE);

	} else {
		if (nChar == VK_RETURN) {
			_OnEnterKeyDown(ENTER_KEYDOWN_RETURN);
			SetMsgHandled(TRUE);
		} else {
			SetMsgHandled(FALSE);
		}
	}
}