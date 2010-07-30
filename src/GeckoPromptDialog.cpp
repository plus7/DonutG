#include "stdafx.h"
#include "GeckoPromptDialog.h"

// Constructor
CGeckoPromptDlg::CGeckoPromptDlg()
{
}


// Handler
LRESULT CGeckoPromptDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	CenterWindow( GetParent() );
	DoDataExchange(FALSE);

	m_edit.Attach( GetDlgItem(IDC_EDIT_PROMPT) );
	m_pwd.Attach( GetDlgItem(IDC_EDITPWD_PROMPT) );
	m_ok.Attach( GetDlgItem(IDOK) );
	m_cancel.Attach( GetDlgItem(IDCANCEL) );
	m_check.Attach( GetDlgItem(IDC_CHECK_PROMPT) );
	m_msg.Attach( GetDlgItem(IDC_STATIC_PROMPT) );

	this->SetWindowTextW(m_strTitle);
	if(!m_showCheck) {
		CRect r, wr, cr;
		m_check.GetWindowRect(&r);

		this->GetWindowRect(&wr);
		wr.bottom -= r.Height();
		this->MoveWindow(wr);

		m_ok.GetWindowRect(&cr);
		this->ScreenToClient(cr);
		cr.MoveToY(cr.top - r.Height());
		m_ok.MoveWindow(cr);

		m_cancel.GetWindowRect(&cr);
		this->ScreenToClient(cr);
		cr.MoveToY(cr.top - r.Height());
		m_cancel.MoveWindow(cr);

		m_check.ShowWindow(SW_HIDE);
	}
	if(!m_showPwd){
		CRect r, wr, cr;
		m_pwd.GetWindowRect(&r);

		this->GetWindowRect(&wr);
		wr.bottom -= r.Height();
		this->MoveWindow(wr);

		m_ok.GetWindowRect(&cr);
		this->ScreenToClient(cr);
		cr.MoveToY(cr.top - r.Height());
		m_ok.MoveWindow(cr);

		m_cancel.GetWindowRect(&cr);
		this->ScreenToClient(cr);
		cr.MoveToY(cr.top - r.Height());
		m_cancel.MoveWindow(cr);

		m_pwd.ShowWindow(SW_HIDE);
	}

	m_msg.SetWindowText(m_strMsg);

	return 0;
}


LRESULT CGeckoPromptDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);
	EndDialog(wID);
	return 0;
}


