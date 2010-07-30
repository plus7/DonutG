#pragma once

#include "resource.h"

class CGeckoPrintingPromptDlg : public CDialogImpl<CGeckoPrintingPromptDlg>, public CWinDataExchange<CGeckoPrintingPromptDlg> {
public:
	// Declarations
	enum { IDD = IDD_DIALOG_PRINTPROGRESS };

	// Data members
	int m_progress;

	CProgressBarCtrl m_progressbar;

private:
	CButton     m_cancel;

public:
	// Constructor
	CGeckoPrintingPromptDlg();


	// DDX map
	BEGIN_DDX_MAP(CGeckoPrintingPromptDlg)
		/*DDX_TEXT   ( IDC_EDIT_PROMPT,  m_strEdit )
		DDX_TEXT   ( IDC_EDITPWD_PROMPT,  m_strPwd )
		DDX_CHECK  ( IDC_CHECK_PROMPT, m_isChecked )*/
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CGeckoPrintingPromptDlg )
		MESSAGE_HANDLER 	 ( WM_INITDIALOG , OnInitDialog )
		MESSAGE_HANDLER      ( WM_DESTROY,     OnDestroy )
		COMMAND_ID_HANDLER	 ( IDCANCEL 	 , OnCloseCmd	)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
};
