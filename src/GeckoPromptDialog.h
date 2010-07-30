#pragma once

#include "resource.h"

class CGeckoPromptDlg : public CDialogImpl<CGeckoPromptDlg>, public CWinDataExchange<CGeckoPromptDlg> {
public:
	// Declarations
	enum { IDD = IDD_PROMPT_DIALOG };

	// Data members
	CString m_strMsg;
	CString m_strEdit;
	CString m_strPwd;
	CString m_strCheck;
	CString m_strTitle;
	bool m_isChecked;
	bool m_showCheck;
	bool m_showPwd;

private:
	CEdit		m_edit;
	CEdit       m_pwd;
	CButton     m_check;
	CButton     m_ok;
	CButton     m_cancel;
	CStatic     m_msg;

public:
	// Constructor
	CGeckoPromptDlg();


	// DDX map
	BEGIN_DDX_MAP(CGeckoPromptDlg)
		DDX_TEXT   ( IDC_EDIT_PROMPT,  m_strEdit )
		DDX_TEXT   ( IDC_EDITPWD_PROMPT,  m_strPwd )
		DDX_CHECK  ( IDC_CHECK_PROMPT, m_isChecked )
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CGeckoPromptDlg )
		MESSAGE_HANDLER 	 ( WM_INITDIALOG , OnInitDialog )
		COMMAND_ID_HANDLER	 ( IDOK 		 , OnCloseCmd	)
		COMMAND_ID_HANDLER	 ( IDCANCEL 	 , OnCloseCmd	)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
};
