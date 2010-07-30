/**
 *	@file	UrlSecurityExPropDialog.h
 *	@brief	Url別セキュリティの(拡張プロパティ)設定
 */

#pragma once

#include "../ExStyle.h"


//拡張プロパティデータ設定用ダイアログ
class CExPropertyDialog2 : public CDialogImpl<CExPropertyDialog2> , public CWinDataExchange<CExPropertyDialog2> {
public:
	enum { IDD = IDD_DIALOG_EXPROPERTY2 };

private:
  #ifndef USE_DIET
	BOOL		m_bRegex;
  #endif
	//DWORD		m_dwExProp;		//+++ 未使用.
	int 		m_nImage;
	int 		m_nFrame;
	int 		m_nPlugin;
	int 		m_nScript;
	int 		m_nRedir;
	int 		m_nDNS;
	int 		m_nNaviLock;
	int 		m_nFlatView;	//+++ 名前変更.
	int 		m_nBlockMailTo;
	int 		m_nGesture;
	int 		m_nFilter;
	int 		m_nReload;

	int			m_urlEditBtnSw;			//+++ url編集のon,off
	CString&	m_rRsltUrl;			//+++ エディットチェック用
	unsigned&	m_rRsltFlags;
	unsigned&	m_rRsltExopts;
	unsigned&	m_rRsltExopts2;		//+++

public:
	CExPropertyDialog2(CString &strUrlFile, unsigned& rFlags, unsigned& rExopts, unsigned& rExopts2)
		: m_rRsltUrl(strUrlFile)
		, m_rRsltFlags(rFlags)
		, m_rRsltExopts(rExopts)
		, m_rRsltExopts2(rExopts2)	//+++ 引数追加.
		, m_urlEditBtnSw(0)
	  #ifndef USE_DIET
		, m_bRegex((rFlags & 1) != 0)
	  #endif
	{
		DWORD		dwExProp  = rExopts;

		m_nImage			= exProp2btn(dwExProp, EXPROP_IMAGE		);
		m_nFrame			= exProp2btn(dwExProp, EXPROP_FRAME		);
		m_nPlugin			= exProp2btn(dwExProp, EXPROP_PLUGIN	);
		m_nScript			= exProp2btn(dwExProp, EXPROP_SCRIPT 	);
		m_nRedir			= exProp2btn(dwExProp, EXPROP_REDIR		);
		m_nDNS				= exProp2btn(dwExProp, EXPROP_DNS		);

		m_nNaviLock 		= exProp2btn(dwExProp, EXPROP_NAVI		);
		m_nFilter			= exProp2btn(dwExProp, EXPROP_FILTER 	);
		m_nGesture			= exProp2btn(dwExProp, EXPROP_GETSTURE	);
		m_nBlockMailTo		= exProp2btn(dwExProp, EXPROP_MAILTO 	);

		DWORD	  dwExProp2 = rExopts2;										//+++
		m_nFlatView 		= exProp2btn(dwExProp2, EXPROPOPT_FLATVIEW  );	//+++

		//+++ m_nNoCloseNaviLock =	拡張プロパティでは設定できないとにする.

		int   nFlag 		= dwExProp / EXPROP_REFRESH_NONE;
		m_nReload			= 0;

		while ( (nFlag >>= 1) > 0 )
			m_nReload++;
	}


	BEGIN_DDX_MAP(CExPropertyDialogT<IDD_DLG> )
		DDX_CHECK(IDC_CHECK_IMAGE			, m_nImage		)
		DDX_CHECK(IDC_CHECK_FRAME  			, m_nFrame	 	)
		DDX_CHECK(IDC_CHECK_PLUGIN  		, m_nPlugin 	)
		DDX_CHECK(IDC_CHECK_SCRIPT  		, m_nScript 	)
		DDX_CHECK(IDC_CHECK_REDIR	  		, m_nRedir 		)
		DDX_CHECK(IDC_CHECK_DNS		  		, m_nDNS	 	)
		DDX_CHECK(IDC_CHECK_NAVILOCK		, m_nNaviLock	)
		DDX_CHECK(IDC_CHECK_SCROLLBAR		, m_nFlatView	)	//+++
		DDX_CHECK(IDC_CHK_BLOCK_MAILTO		, m_nBlockMailTo)
		DDX_CHECK(IDC_CHK_MOUSE_GESTURE 	, m_nGesture	)
		DDX_CHECK(IDC_CHK_MSG_FILTER		, m_nFilter 	)
		DDX_RADIO(IDC_RADIO_RELOAD_NONE 	, m_nReload 	)
	  #ifndef USE_DIET
		DDX_CHECK(IDC_CHK_URLSEC_REGEX		, m_bRegex		)
	  #endif
	END_DDX_MAP()


	BEGIN_MSG_MAP(CExPropertyDialogT<IDD_DLG> )
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK				, OnOK		)
		COMMAND_ID_HANDLER_EX(IDCANCEL			, OnCancel	)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DELETE , OnDelete	)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DEFAULT, OnDefault )
		COMMAND_ID_HANDLER_EX(IDC_BTN_DFLTBASE  , OnDfltBase )
		COMMAND_ID_HANDLER_EX(IDC_BTN_EXPROP_URL, OnBtnExpropUrl )
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam)
	{
		CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetWindowText(m_rRsltUrl);
		CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetReadOnly(1);
		m_urlEditBtnSw = 0;
		CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).EnableWindow(1);
		CButton( GetDlgItem(IDC_BTN_EXPROP_URL) ).EnableWindow(1);

		DoDataExchange(FALSE);
		return 1;
	}

	//+++ 拡張プロパティ情報をボタン状態に変換.
	static int		exProp2btn(DWORD dwExProp, DWORD flag) {
		return (dwExProp & flag) / (flag / 3);
	}


	//+++ 拡張プロパティ情報を設定
	static DWORD	btn2ExProp(int stat, DWORD flag) {
	  #if 1
		if (stat == BST_UNCHECKED)
			return 0;					//+++ チェックされていない場合
		if (stat == BST_INDETERMINATE)
			return 2 * flag / 3;		//+++ デフォルト設定を用いる場合
		return flag / 3;				//+++ チェックされている場合
	  #else
		return stat * (flag / 3);
	  #endif
	}


	void OnOK(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		DoDataExchange(TRUE);

		DWORD		dwExProp  = 0;
		dwExProp  |= btn2ExProp(m_nImage		, EXPROP_IMAGE	);
		dwExProp  |= btn2ExProp(m_nFrame		, EXPROP_FRAME	);
		dwExProp  |= btn2ExProp(m_nPlugin		, EXPROP_PLUGIN	);
		dwExProp  |= btn2ExProp(m_nScript		, EXPROP_SCRIPT	);
		dwExProp  |= btn2ExProp(m_nRedir		, EXPROP_REDIR	);
		dwExProp  |= btn2ExProp(m_nDNS			, EXPROP_DNS	);

		dwExProp  |= btn2ExProp(m_nNaviLock		, EXPROP_NAVI		);
		dwExProp  |= btn2ExProp(m_nFilter		, EXPROP_FILTER	 	);
		dwExProp  |= btn2ExProp(m_nGesture 		, EXPROP_GETSTURE	);
		dwExProp  |= btn2ExProp(m_nBlockMailTo	, EXPROP_MAILTO		);

		if (m_nReload != -1)
			dwExProp |= (EXPROP_REFRESH_NONE << m_nReload);

		//+++	m_dwExProp = dwExProp;

		DWORD		dwExProp2 = m_rRsltExopts2 & 3;							//+++
		dwExProp2  |= btn2ExProp(m_nFlatView	, EXPROPOPT_FLATVIEW	);	//+++

		// セーブ
	  #ifndef USE_DIET
		m_rRsltFlags  = int(m_bRegex);
	  #endif
		m_rRsltExopts = dwExProp;
		m_rRsltExopts2= dwExProp2;	//+++

		//+++ URL の編集を行った場合
		CString 	strUrl = MtlGetWindowText( GetDlgItem(IDC_EDIT_EXPROP_URL) );
		if (m_rRsltUrl != strUrl) {
			m_rRsltUrl   = strUrl;
		}

		EndDialog(nID);
	}


	void OnCancel(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		EndDialog(nID);
	}


	void OnDelete(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		m_nImage		= 0;
		m_nFrame		= 0;
		m_nPlugin		= 0;
		m_nScript		= 0;
		m_nRedir		= 0;
		m_nDNS			= 0;
		m_nNaviLock 	= 0;
		m_nFlatView 	= 0;	//+++
		m_nFilter		= 0;
		m_nGesture		= 0;
		m_nBlockMailTo	= 0;
		m_nReload		= 0;

	  #ifndef USE_DIET
		m_bRegex		= 0;
	  #endif
		DoDataExchange(FALSE);
	}


	void OnDefault(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		DWORD dwDLFlag	= CDLControlOption::s_dwDLControlFlags;
		DWORD dwExStyle = CDLControlOption::s_dwExtendedStyleFlags;

		//m_bRegex		= 0;
		m_nReload	   	= 0;

		m_nImage	   	=  (dwDLFlag & GECKO_DLCTL_IMAGES		 )	!= 0;
		m_nFrame	   	=  (dwDLFlag & GECKO_DLCTL_FRAMES		 )	!= 0;
		m_nPlugin	   	=  (dwDLFlag & GECKO_DLCTL_PLUGINS		 )	!= 0;
		m_nScript	   	=  (dwDLFlag & GECKO_DLCTL_SCRIPTS		 )	!= 0;
		m_nRedir	   	=  (dwDLFlag & GECKO_DLCTL_REDIR		 )	!= 0;
		m_nDNS		   	=  (dwDLFlag & GECKO_DLCTL_DNS			 )	!= 0;

		m_nNaviLock    	=  (dwExStyle & DVS_EX_OPENNEWWIN	 )	!= 0;
		m_nFlatView    	=  (dwExStyle & DVS_EX_FLATVIEW		 )	!= 0;	//+++
		m_nFilter	   	=  (dwExStyle & DVS_EX_MESSAGE_FILTER )	!= 0;
		m_nGesture	   	=  (dwExStyle & DVS_EX_MOUSE_GESTURE  )	!= 0;
		m_nBlockMailTo 	=  (dwExStyle & DVS_EX_BLOCK_MAILTO	 )	!= 0;

		DoDataExchange(FALSE);
	}


	void OnDfltBase(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		m_nReload	    = 0;

		m_nImage		= BST_INDETERMINATE;
		m_nFrame		= BST_INDETERMINATE;
		m_nPlugin		= BST_INDETERMINATE;
		m_nScript		= BST_INDETERMINATE;
		m_nRedir		= BST_INDETERMINATE;
		m_nDNS			= BST_INDETERMINATE;
		m_nNaviLock 	= BST_INDETERMINATE;
		m_nFlatView		= BST_INDETERMINATE;	//+++
		m_nFilter		= BST_INDETERMINATE;
		m_nGesture		= BST_INDETERMINATE;
		m_nBlockMailTo	= BST_INDETERMINATE;

		DoDataExchange(FALSE);
	}


  #if 1	//+++ URL を編集可能に変更. URLを表示させない場合は、URLボタンもなしに.
	void OnBtnExpropUrl(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetReadOnly( m_urlEditBtnSw );
		m_urlEditBtnSw ^= 1;
		//DoDataExchange(FALSE);
	}
  #endif

};

