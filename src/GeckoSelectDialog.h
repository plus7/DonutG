#pragma once

#include "resource.h"

class CGeckoSelectDlg : public CDialogImpl<CGeckoSelectDlg>, public CWinDataExchange<CGeckoSelectDlg> {
public:
	// Declarations
	enum { IDD = IDD_SELECT_DIALOG };

	// Data members
	CString m_strTitle;
	CString m_strMsg;
	CSimpleArray<CString> m_strList;
	int m_selection;

private:
	CListBox    m_list;
	CButton     m_ok;
	CButton     m_cancel;
	CStatic     m_msg;

public:
	// Constructor
	CGeckoSelectDlg();


	// DDX map
	BEGIN_DDX_MAP(CGeckoSelectDlg)
		DDX_LISTBOX_INDEX(IDC_LIST_SELECT, m_selection)
	END_DDX_MAP()

	// Message map and handlers
	BEGIN_MSG_MAP( CGeckoSelectDlg )
		MESSAGE_HANDLER 	 ( WM_INITDIALOG , OnInitDialog )
		COMMAND_ID_HANDLER	 ( IDOK 		 , OnCloseCmd	)
		COMMAND_ID_HANDLER	 ( IDCANCEL 	 , OnCloseCmd	)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);
};
