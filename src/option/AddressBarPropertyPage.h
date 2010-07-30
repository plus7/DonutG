/**
 *	@file	AddressBarPropertyPage.h
 *	@brief	アドレスバーのオプション設定.
 *	@note
 *		 +++ AddressBar.hより分離
 */
#pragma once

class CDonutAddressBar;
class CDonutSearchBar;

class CDonutAddressBarPropertyPage
		: public CPropertyPageImpl < CDonutAddressBarPropertyPage >
		, public CWinDataExchange < CDonutAddressBarPropertyPage >
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_ADDRESSBAR };

private:
	// Data members
	int 				m_nAutoComplete;
	int 				m_nNewWin;
	int 				m_nNoActivate;
	int 				m_nLoadTypedUrls;
	int 				m_nGoBtnVisible;
	CDonutAddressBar&	m_AddressBar;
	CDonutSearchBar &	m_SearchBar;	//minit

	// UH
	CString 			m_strEnterCtrl;			//*+++ 未使用かも?
	CString 			m_strEnterShift;		//*+++ 未使用かも?
	int 				m_nUseEnterCtrl;
	int 				m_nUseEnterShift;
	int 				m_nTextVisible;

	//minit
	int 				m_nReplaceSpace;
	BOOL				m_bInit;
	BOOL				m_bAddCtrl;
	BOOL				m_bAddShift;

	//+++
	CString 			m_strIeExePath;
	CEdit				m_edit;				//+++

public:
	// DDX map
	BEGIN_DDX_MAP( CDonutAddressBarPropertyPage )
		DDX_CHECK( IDC_CHECK_ABR_AUTOCOMPLETE,	m_nAutoComplete )
		DDX_CHECK( IDC_CHECK_ABR_NEWWIN,		m_nNewWin		)
		DDX_CHECK( IDC_CHECK_ADB_NOACTIVATE,	m_nNoActivate	)
		DDX_CHECK( IDC_CHECK_ABR_LOADTYPEDURLS, m_nLoadTypedUrls)
		DDX_CHECK( IDC_CHECK_ABR_GOBTNVISIBLE,	m_nGoBtnVisible )
		DDX_CHECK( IDC_CHECK_ABR_TEXTVISIBLE,	m_nTextVisible	)
		// UH
		//DDX_TEXT( IDC_EDIT_CTRL_ENTER,		m_strEnterCtrl	)
		//DDX_TEXT( IDC_EDIT_SHIFT_ENTER,		m_strEnterShift )
		DDX_CHECK( IDC_CHECK_CTRL_ENTER,		m_nUseEnterCtrl )
		DDX_CHECK( IDC_CHECK_SHIFT_ENTER,		m_nUseEnterShift)
		//minit
		DDX_CHECK( IDC_CHECK_REPLACE,			m_nReplaceSpace )
		//+++
		DDX_TEXT ( IDC_ADDRESS_BAR_ICON_EXE,	m_strIeExePath	)
	END_DDX_MAP()


	//+++
	BEGIN_MSG_MAP( CDonutAddressBarPropertyPage )
		COMMAND_ID_HANDLER_EX( IDC_BTN_ADDRESS_BAR_EXE, OnButton )
		CHAIN_MSG_MAP(CPropertyPageImpl<CDonutAddressBarPropertyPage>)
	END_MSG_MAP()


	// Constructor
	CDonutAddressBarPropertyPage(CDonutAddressBar &adBar, CDonutSearchBar &searchBar)
		: m_AddressBar(adBar)
		, m_SearchBar(searchBar)
		, m_bInit(FALSE)
		, m_bAddCtrl(FALSE)
		, m_bAddShift(FALSE)
	  #if 1 //+++
		, m_nAutoComplete(0)
		, m_nNewWin(0)
		, m_nNoActivate(0)
		, m_nLoadTypedUrls(0)
		, m_nGoBtnVisible(0)
		, m_nUseEnterCtrl(0)
		, m_nUseEnterShift(0)
		, m_nTextVisible(0)
		, m_nReplaceSpace(0)
	  #endif
	{
		_SetData();
	}


public: // Overrides

	BOOL OnSetActive()
	{
		if (!m_bInit) {
			InitComboBox();
			m_bInit = TRUE;
		}

	  #if 1	//+++
		if (m_edit.m_hWnd == NULL) {
			m_edit.Attach( GetDlgItem(IDC_ADDRESS_BAR_ICON_EXE) );
			m_edit.SetWindowText(m_strIeExePath);
		}
	  #endif

		SetModified(TRUE);
		return DoDataExchange(FALSE);
	}


	BOOL		OnKillActive()
	{
		return DoDataExchange(TRUE);
	}


	BOOL		OnApply()
	{
		if ( DoDataExchange(TRUE) ) {
			_GetData();
			return TRUE;
		} else
			return FALSE;
	}


	// Implementation
private:

	void		_GetData()
	{
		// update  flags
		DWORD	dwFlags = 0;

		if (m_nAutoComplete ) { dwFlags |= ABR_EX_AUTOCOMPLETE;		}
		if (m_nNewWin		) { dwFlags |= ABR_EX_OPENNEWWIN;		}
		if (m_nNoActivate	) { dwFlags |= ABR_EX_NOACTIVATE;		}
		if (m_nNewWin		) { dwFlags |= ABR_EX_OPENNEWWIN;		}
		if (m_nLoadTypedUrls) { dwFlags |= ABR_EX_LOADTYPEDURLS;	}
		if (m_nGoBtnVisible ) { dwFlags |= ABR_EX_GOBTNVISIBLE;		}
		// U.H
		if (m_nTextVisible	) { dwFlags |= ABR_EX_TEXTVISIBLE; 		}
		if (m_nUseEnterCtrl ) { dwFlags |= ABR_EX_ENTER_CTRL;		}
		if (m_nUseEnterShift) { dwFlags |= ABR_EX_ENTER_SHIFT; 		}
		if (m_nReplaceSpace ) { dwFlags |= ABR_EX_SEARCH_REPLACE;	}

		m_AddressBar.SetAddressBarExtendedStyle(dwFlags);

		//x CIniFileO	pr( g_szIniFileName, STR_ADDRESS_BAR );
		//pr.SetValue(m_strEnterCtrl, _T("EnterCtrl"));
		//pr.SetValue(m_strEnterShift, _T("EnterShift"));

		CComboBox	cmbCtrl  = GetDlgItem(IDC_COMBO_CTRL_ENTER);
		CComboBox	cmbShift = GetDlgItem(IDC_COMBO_SHIFT_ENTER);
		CString 	strBufCtrl,
					strBufShift;
		int 		idx;

		idx = cmbCtrl.GetCurSel();
		if (idx != CB_ERR)
			cmbCtrl.GetLBText(idx, strBufCtrl);

		CIniFileO	pr( g_szIniFileName, STR_ADDRESS_BAR );
		if (strBufCtrl.Find( _T("※") ) != 0) {
			if (m_bAddCtrl)
				idx = idx - 1;

			pr.SetValue ( idx, _T("EnterCtrlIndex") );
			pr.SetStringUW( strBufCtrl, _T("EnterCtrlEngin") );
			pr.SetStringUW( CDonutSearchBar::GetSearchIniPath(), _T("EnterCtrlPath") );
		}

		idx = cmbShift.GetCurSel();
		if (idx != CB_ERR)
			cmbShift.GetLBText(idx, strBufShift);

		if (strBufShift.Find( _T("※") ) != 0) {
			if (m_bAddShift)
				idx = idx - 1;

			pr.SetValue( idx, _T("EnterShiftIndex") );
			pr.SetStringUW( strBufShift, _T("EnterShiftEngin") );
			pr.SetStringUW( CDonutSearchBar::GetSearchIniPath(), _T("EnterShiftPath") );
		}

	  #if 0	//+++
		//m_edit.GetWindowText(m_strIeExePath, MAX_PATH);
	  #endif

		pr.SetStringUW( m_strIeExePath, _T("IeExePath")	);				//+++
	}


	void		_SetData()
	{
		DWORD dwFlags = m_AddressBar.GetAddressBarExtendedStyle();

		m_nAutoComplete  = _check_flag(ABR_EX_AUTOCOMPLETE,   dwFlags); 	//+++ ? 1 : 0;
		m_nNewWin		 = _check_flag(ABR_EX_OPENNEWWIN,	  dwFlags); 	//+++ ? 1 : 0;
		m_nNoActivate	 = _check_flag(ABR_EX_NOACTIVATE,	  dwFlags); 	//+++ ? 1 : 0;
		m_nLoadTypedUrls = _check_flag(ABR_EX_LOADTYPEDURLS,  dwFlags); 	//+++ ? 1 : 0;
		m_nGoBtnVisible  = _check_flag(ABR_EX_GOBTNVISIBLE,   dwFlags); 	//+++ ? 1 : 0;

		// vvv UH vvv
		m_nTextVisible	 = _check_flag(ABR_EX_TEXTVISIBLE,	  dwFlags); 	//+++ ? 1 : 0;
		m_nUseEnterCtrl  = _check_flag(ABR_EX_ENTER_CTRL,	  dwFlags); 	//+++ ? 1 : 0;
		m_nUseEnterShift = _check_flag(ABR_EX_ENTER_SHIFT,	  dwFlags); 	//+++ ? 1 : 0;
		//minit
		m_nReplaceSpace  = _check_flag(ABR_EX_SEARCH_REPLACE, dwFlags); 	//+++ ? 1 : 0;
	}


	void		InitComboBox()
	{
		CComboBox cmbCtrl  = GetDlgItem(IDC_COMBO_CTRL_ENTER);
		CComboBox cmbShift = GetDlgItem(IDC_COMBO_SHIFT_ENTER);
		CString strBuf;

	  #if 1	//+++
		m_SearchBar.InitComboBox_for_AddressBarPropertyPage(cmbCtrl, cmbShift);
	  #else
		int nCount		   = m_SearchBar.m_cmbEngine.GetCount();
		for (int i = 0; i < nCount; i++) {
			m_SearchBar.m_cmbEngine.GetLBText(i, strBuf);
			cmbCtrl.AddString(strBuf);
			cmbShift.AddString(strBuf);
		}
	  #endif

		CIniFileI	pr( g_szIniFileName , STR_ADDRESS_BAR );
		DWORD		idxCtrl 	 = pr.GetValue ( _T("EnterCtrlIndex") , 0 );
		DWORD		idxShift	 = pr.GetValue ( _T("EnterShiftIndex"), 0 );
		CString 	strPathCtrl  = pr.GetStringUW( _T("EnterCtrlPath")   );
		CString 	strPathShift = pr.GetStringUW( _T("EnterShiftPath" ) );
		CString 	strEnginCtrl = pr.GetStringUW( _T("EnterCtrlEngin" ) );
		CString 	strEnginShift= pr.GetStringUW( _T("EnterShiftEngin") );
		m_strIeExePath			 = pr.GetStringUW( _T("IeExePath")   );				//+++
		pr.Close();

		//まず旧仕様の場合（Search.iniオンリー）
		if ( strPathCtrl.IsEmpty() ) {
			//検索ファイルが変更されていたら以前の登録エンジンには※をつける
			if ( CDonutSearchBar::GetSearchIniPath() != _GetFilePath( _T("Search.ini") ) ) {
				CString strText = _T("※") + strEnginCtrl;

				cmbCtrl.InsertString(0, strText);
				strText    = _T("※") + strEnginShift;
				cmbShift.InsertString(0, strText);
				cmbCtrl.SetCurSel(0);
				cmbShift.SetCurSel(0);
				m_bAddCtrl = m_bAddShift = TRUE;
			} else {
				//完全に旧仕様通りに動作
				cmbCtrl.SetCurSel(idxCtrl);
				cmbShift.SetCurSel(idxShift);
			}
		} else {
			//設定時の検索ファイルと今のファイルが異なる場合は以前のエンジンに※をつけて追加
			if (CDonutSearchBar::GetSearchIniPath() != strPathCtrl) {
				CString strText = _T("※") + strEnginCtrl;

				cmbCtrl.InsertString(0, strText);
				cmbCtrl.SetCurSel(0);
				m_bAddCtrl = TRUE;
			} else {
				cmbCtrl.SetCurSel(idxCtrl);
			}

			if (CDonutSearchBar::GetSearchIniPath() != strPathShift) {
				CString strText = _T("※") + strEnginShift;

				cmbShift.InsertString(0, strText);
				cmbShift.SetCurSel(0);
				m_bAddShift = TRUE;
			} else {
				cmbShift.SetCurSel(idxShift);
			}
		}
	}

  #if 1	//+++
	void 	OnButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/) {
		//TCHAR			   szOldPath[MAX_PATH];
		//szOldPath[0]	= 0;
		//::GetCurrentDirectory(MAX_PATH, szOldPath);
		//::SetCurrentDirectory( Misc::GetExeDirectory() );

		static const TCHAR szFilter[] = _T("全ファイル(*.*)\0*.*\0\0");

		CFileDialog 	   fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
		fileDlg.m_ofn.lpstrTitle = _T("アドレスバー・アイコンで起動するアプリ");

		if (fileDlg.DoModal() == IDOK) {
			m_edit.SetWindowText(fileDlg.m_szFileName);
			//m_strIeExePath	= fileDlg.m_szFileName;
		}

		// // restore current directory
		//::SetCurrentDirectory(szOldPath);
	}
  #endif
};

