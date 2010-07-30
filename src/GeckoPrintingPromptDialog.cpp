#include "stdafx.h"
#include "GeckoPrintingPromptDialog.h"

// Constructor
CGeckoPrintingPromptDlg::CGeckoPrintingPromptDlg()
{
}


// Handler
LRESULT CGeckoPrintingPromptDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	CenterWindow( GetParent() );
	DoDataExchange(FALSE);

	m_cancel.Attach( GetDlgItem(IDCANCEL) );
	m_cancel.ShowWindow(SW_HIDE);
	m_progressbar.Attach( GetDlgItem(IDC_PRINTPROGRESS) );
	m_progressbar.SetRange(0,100);

	return 0;
}


LRESULT CGeckoPrintingPromptDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);
	//EndDialog(wID);
	this->DestroyWindow();
	return 0;
}



LRESULT CGeckoPrintingPromptDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	m_cancel.Detach();
	m_progressbar.Detach();
	return 0;
}