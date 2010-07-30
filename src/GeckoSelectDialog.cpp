#include "stdafx.h"
#include "GeckoSelectDialog.h"

// Constructor
CGeckoSelectDlg::CGeckoSelectDlg()
{
}


// Handler
LRESULT CGeckoSelectDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	CenterWindow( GetParent() );
	DoDataExchange(FALSE);

	m_ok.Attach( GetDlgItem(IDOK) );
	m_cancel.Attach( GetDlgItem(IDCANCEL) );
	m_list.Attach( GetDlgItem(IDC_LIST_SELECT) );
	m_msg.Attach( GetDlgItem(IDC_STATIC_PROMPT) );

	this->SetWindowTextW(m_strTitle);

	for(int i=0;i<m_strList.GetSize(); i++){
		m_list.AddString(m_strList[i]);
	}

	m_msg.SetWindowText(m_strMsg);

	return 0;
}


LRESULT CGeckoSelectDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);
	EndDialog(wID);
	return 0;
}


