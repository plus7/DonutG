/**
 *	@file	ExStyle.h
 *	@brief	拡張プロパティ
 */

#pragma once

#define DONUT_SECTION			_T("DonutG")
#define EXPROP_KEY_ENABLED		_T("EnabledG")
#define EXPROP_KEY				_T("ExPropertyG")
#define EXPROP_OPTION			_T("ExPropOptG") 	//+++	追加.


enum EExProp {
	EXPROP_DEFAULT			= 2,
	EXPROP_OFF				= 0,
	EXPROP_ON				= 1,

	EXPROP_IMAGE			= 0x00000003,
	EXPROP_FRAME			= 0x0000000C,
	EXPROP_PLUGIN			= 0x00000030,
	EXPROP_SCRIPT			= 0x000000C0,
	EXPROP_REDIR			= 0x00000300,
	EXPROP_DNS				= 0x00000C00,
//	EXPROP_JAVA 			= 0x00003000, //plus7: どうしたものか
	EXPROP_NAVI 			= 0x00030000,
	EXPROP_FILTER			= 0x000C0000,
	EXPROP_GETSTURE 		= 0x00300000,
	EXPROP_MAILTO			= 0x00C00000,

	EXPROP_REFRESH_NONE 	= 0x01000000,
	EXPROP_REFRESH_15SEC	= 0x02000000,
	EXPROP_REFRESH_30SEC	= 0x04000000,
	EXPROP_REFRESH_1MIN 	= 0x08000000,
	EXPROP_REFRESH_2MIN 	= 0x10000000,
	EXPROP_REFRESH_5MIN 	= 0x20000000,
	EXPROP_REFRESH_USER 	= 0x40000000,
	EXPROP_REFRESH_DEFAULT	= 0x80000000,
};


enum {
	EXPROPOPT_ADDRESSBAR	=	0x00000001,	//+++ 検索でアドレスバーの文字列を対象にする.
	EXPROPOPT_FLATVIEW		=   0x0000000C,	//+++ そのページを平面的な表示にする
};



/*
	拡張プロパティ形式データとDonut標準のデータフラグとの相互変換を行うクラス

	拡張プロパティ形式データはフラグごとにOn,Off,Defaultの三態をとる
	Defaultはコンストラクタで指定したデフォルトの値を利用する

	release12時点ではDefault値は利用されていない（On,Offのみ設定可）
 */

class CExProperty {
private:
	DWORD		m_dwExProp;
	DWORD		m_dwExPropOpt;		//+++

public:
	DWORD		m_dwDefDLFlag;
	DWORD		m_dwDefExStyle;
	DWORD		m_dwDefReloadFlag;


public:
	CExProperty(DWORD dwDefaultDLFlag, DWORD dwDefaultExStyle, DWORD dwDefaultReloadFlag, DWORD dwExProp, DWORD dwExPropOpt)
		: m_dwDefDLFlag(dwDefaultDLFlag)
		, m_dwDefExStyle(dwDefaultExStyle)
		, m_dwDefReloadFlag(dwDefaultReloadFlag)
		, m_dwExProp(dwExProp)
		, m_dwExPropOpt(dwExPropOpt)		//+++引数追加.
	{
	}


	int GetPropFlag(DWORD dwExProp, DWORD dwFlag) const
	{
		return (dwExProp & dwFlag) / (dwFlag / 3);
	}


	//セキュリティ関連のフラグ変数を取得する
	int GetDLControlFlags() const
	{
		DWORD	dwDLFlags = 0;
		DWORD	dwExProp	= m_dwExProp;

		switch ( GetPropFlag(dwExProp, EXPROP_IMAGE) ) {
		case EXPROP_DEFAULT:	dwDLFlags |= (m_dwDefDLFlag & GECKO_DLCTL_IMAGES); 		break;
		case EXPROP_ON: 		dwDLFlags |= GECKO_DLCTL_IMAGES;						break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_FRAME) ) {
		case EXPROP_DEFAULT:	dwDLFlags |= (m_dwDefDLFlag & GECKO_DLCTL_FRAMES); 		break;
		case EXPROP_ON: 		dwDLFlags |= GECKO_DLCTL_FRAMES;						break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_PLUGIN) ) {
		case EXPROP_DEFAULT:	dwDLFlags |= (m_dwDefDLFlag & GECKO_DLCTL_PLUGINS);    	break;
		case EXPROP_ON: 		dwDLFlags |= GECKO_DLCTL_PLUGINS;						break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_SCRIPT) ) {
		case EXPROP_DEFAULT:	dwDLFlags |= (m_dwDefDLFlag & GECKO_DLCTL_SCRIPTS);		break;
		case EXPROP_OFF:		dwDLFlags |= GECKO_DLCTL_SCRIPTS;						break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_REDIR) ) {
		case EXPROP_DEFAULT:	dwDLFlags |= (m_dwDefDLFlag & GECKO_DLCTL_REDIR); 		break;
		case EXPROP_ON: 		dwDLFlags |= GECKO_DLCTL_REDIR;							break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_DNS) ) {
		case EXPROP_DEFAULT:	dwDLFlags |= (m_dwDefDLFlag & GECKO_DLCTL_DNS);			break;
		case EXPROP_ON: 		dwDLFlags |= GECKO_DLCTL_DNS;							break;
		}

		return dwDLFlags;
	}


	//タブの拡張スタイルを取得する
	int GetExtendedStyleFlags() const
	{
		DWORD	dwExFlag = 0;
		DWORD	dwExProp = m_dwExProp;

		switch ( GetPropFlag(dwExProp, EXPROP_NAVI) ) {
		case EXPROP_DEFAULT:	dwExFlag |= (m_dwDefExStyle & DVS_EX_OPENNEWWIN);		break;
		case EXPROP_ON: 		dwExFlag |= DVS_EX_OPENNEWWIN;							break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_FILTER) ) {
		case EXPROP_DEFAULT:	dwExFlag |= (m_dwDefExStyle & DVS_EX_MESSAGE_FILTER);	break;
		case EXPROP_ON: 		dwExFlag |= DVS_EX_MESSAGE_FILTER;						break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_GETSTURE) ) {
		case EXPROP_DEFAULT:	dwExFlag |= (m_dwDefExStyle & DVS_EX_MOUSE_GESTURE);	break;
		case EXPROP_ON: 		dwExFlag |= DVS_EX_MOUSE_GESTURE;						break;
		}

		switch ( GetPropFlag(dwExProp, EXPROP_MAILTO) ) {
		case EXPROP_DEFAULT:	dwExFlag |= (m_dwDefExStyle & DVS_EX_BLOCK_MAILTO); 	break;
		case EXPROP_ON: 		dwExFlag |= DVS_EX_BLOCK_MAILTO;						break;
		}

	  #if 1	//+++
		switch ( GetPropFlag(m_dwExPropOpt, EXPROPOPT_FLATVIEW) ) {
		case EXPROP_DEFAULT:	dwExFlag |= (m_dwDefExStyle & DVS_EX_FLATVIEW);	 	break;
		case EXPROP_ON: 		dwExFlag |= DVS_EX_FLATVIEW;						break;
		}
	  #endif
		return dwExFlag;
	}


	//自動更新のフラグ変数を取得する
	int GetAutoRefreshFlag() const
	{
		DWORD dwRefreshFlag = m_dwExProp / (EXPROP_REFRESH_15SEC / DVS_AUTOREFRESH_15SEC);
		DWORD dwCurFlag 	= DVS_AUTOREFRESH_USER;

		while (dwCurFlag > 0) {
			if (dwCurFlag & dwRefreshFlag) {
				return dwCurFlag;
			}

			dwCurFlag >>= 1;
		}

		return 0;
	}


	int GetAutoRefreshNo() const
	{
		DWORD dwRefreshFlag = m_dwExProp / (EXPROP_REFRESH_15SEC / DVS_AUTOREFRESH_15SEC);
		DWORD dwCurFlag 	= DVS_AUTOREFRESH_USER;

		DWORD n				= 6;
		while (dwCurFlag > 0) {
			if (dwCurFlag & dwRefreshFlag) {
				return n;
			}

			dwCurFlag >>= 1;
			--n;
		}

		return 0;
	}


	DWORD GetExProperty() const
	{
		return 	m_dwExProp;
	}


	void SetExProperty(DWORD dwExProp)
	{
		m_dwExProp = dwExProp;
	}


	BOOL LoadExProperty(const CString &strIniFile, const CString &strSection = DONUT_SECTION)
	{
		CIniFileI	pr(strIniFile, strSection);
		m_dwExProp	  = pr.GetValue(EXPROP_KEY	 , 0xAAAAAA);		//+++ 初期値がデフォルト設定になるように.
		m_dwExPropOpt = pr.GetValue(EXPROP_OPTION, 8|0);			//+++
	}


	static BOOL CheckExPropertyFlag(DWORD &dwFlags, DWORD& dwFlags2, CString &strIniFile, CString strSection = DONUT_SECTION)
	{
		CIniFileI	pr(strIniFile, strSection);
		DWORD	dwEnabled = pr.GetValue(EXPROP_KEY_ENABLED, 0);
		DWORD	dwExProp  = pr.GetValue(EXPROP_KEY        , 0xAAAAAA);		//+++ 初期値変更.
		DWORD	dwExProp2 = pr.GetValue(EXPROP_OPTION     , 0x8);			//+++ 追加.
		pr.Close();
		if (dwEnabled) {
			dwFlags  = dwExProp;
			dwFlags2 = dwExProp2;	//+++
		}
		return dwEnabled != 0;
	}
};



//拡張プロパティデータ設定用ダイアログ
template<int IDD_DLG>					//+++ 通常のダイアログとは別に、URLセキュリティ向けに別ダイアログを用意するためにテンプレート化... だったが、やっぱりやめ
class CExPropertyDialogT : public CDialogImpl<CExPropertyDialogT<IDD_DLG> > , public CWinDataExchange<CExPropertyDialogT<IDD_DLG> > {
public:
	enum { IDD = IDD_DLG/*IDD_DIALOG_EXPROPERTY*/ };

private:
	DWORD		m_dwExProp;
	DWORD		m_dwExPropOpt;		//+++
	BOOL		m_bEnabled;

	CString 	m_strUrlFile;
	CString 	m_strSection;

	CString 	m_strTitle;


	int 		m_nImage;
	int 		m_nFrame;
	int 		m_nPlugin;
	int 		m_nScript;
	int 		m_nRedir;
	int 		m_nDNS;
	int 		m_nNaviLock;
	int 		m_nScrollBar;
	int 		m_nBlockMailTo;
	int 		m_nGesture;
	int 		m_nFilter;
	int 		m_nReload;
	int			m_nFlatView;				//+++

  #if 1 //+++ サーチ関係追加
	CString 	m_exs_frontURL;
	CString 	m_exs_backURL;
	CString 	m_exs_frontKeyword;
	CString 	m_exs_backKeyword;
	int 		m_exs_adrBar;
	int 		m_exs_encode;
  #endif
  #if 1	//+++ url編集
	int			m_urlMode;				//+++ 0:url無  1:url有.
	int			m_urlEditBtnSw;			//+++ url編集のon,off
	CString		m_strUrlBase;			//+++ エディットチェック用
  #endif

public:
	CExPropertyDialogT(const CString &strUrlFile, const CString strSection = DONUT_SECTION, int urlMode=1)
		: m_strUrlFile(strUrlFile)
		, m_strSection(strSection)
		, m_urlMode(urlMode)			//+++ 追加
		, m_urlEditBtnSw(0)				//+++ 追加
	{
		DWORD			dwExProp    = 0xAAAAAA;	//+++ 初期値がデフォルト設定になるように.
		DWORD			dwExPropOpt = 0x8;		//+++
		DWORD			dwEnabled   = 0;

		CIniFileI		pr(strUrlFile, strSection);
		pr.QueryValue(dwExProp   , EXPROP_KEY);
		pr.QueryValue(dwExPropOpt, EXPROP_OPTION);			//+++
		pr.QueryValue(dwEnabled  , EXPROP_KEY_ENABLED);

		m_bEnabled			= dwEnabled != 0;

		m_nImage			= exProp2btn(dwExProp, EXPROP_IMAGE	);
		m_nFrame			= exProp2btn(dwExProp, EXPROP_FRAME	);
		m_nPlugin			= exProp2btn(dwExProp, EXPROP_PLUGIN);
		m_nScript			= exProp2btn(dwExProp, EXPROP_SCRIPT);
		m_nRedir			= exProp2btn(dwExProp, EXPROP_REDIR	);
		m_nDNS				= exProp2btn(dwExProp, EXPROP_DNS	);

		m_nNaviLock 		= exProp2btn(dwExProp, EXPROP_NAVI		);
		m_nFilter			= exProp2btn(dwExProp, EXPROP_FILTER 	);
		m_nGesture			= exProp2btn(dwExProp, EXPROP_GETSTURE	);
		m_nBlockMailTo		= exProp2btn(dwExProp, EXPROP_MAILTO 	);

		m_nFlatView 		= exProp2btn(dwExPropOpt, EXPROPOPT_FLATVIEW  );	//+++

		//+++ m_nNoCloseNaviLock =	拡張プロパティでは設定できないとする.

		int   nFlag 		= dwExProp / EXPROP_REFRESH_NONE;
		m_nReload			= 0;

		while ( (nFlag >>= 1) > 0 )
			m_nReload++;

	  #if 1 //+++ 検索関係追加
		if (IDD == IDD_DIALOG_EXPROPERTY) {
			//DWORD	dwExPropOpt= pr.GetValue(EXPROP_OPTION);
			m_exs_adrBar	   = dwExPropOpt & EXPROPOPT_ADDRESSBAR;
			//x m_exs_adrBar   = pr.GetValue(  _T("AddressBar") 	);
			m_exs_frontURL	   = pr.GetString( _T("FrontURL")		);
			m_exs_backURL	   = pr.GetString( _T("BackURL")		);
			m_exs_frontKeyword = pr.GetString( _T("FrontKeyWord")	);
			m_exs_backKeyword  = pr.GetString( _T("BackKeyWord")	);
			m_exs_encode	   = pr.GetValue ( _T("Encode") 		);
		}
	  #endif
	}


	void SetTitle(CString strTitle) {
		m_strTitle = strTitle;
	}


	BEGIN_DDX_MAP(CExPropertyDialogT<IDD_DLG> )
		DDX_CHECK( IDC_CHECK_IMAGE			, m_nImage		)
		DDX_CHECK( IDC_CHECK_FRAME			, m_nFrame		)
		DDX_CHECK( IDC_CHECK_PLUGIN			, m_nPlugin		)
		DDX_CHECK( IDC_CHECK_SCRIPT 		, m_nScript 	)
		DDX_CHECK( IDC_CHECK_REDIR			, m_nRedir		)
		DDX_CHECK( IDC_CHECK_DNS			, m_nDNS		)
		DDX_CHECK( IDC_CHECK_NAVILOCK		, m_nNaviLock	)
		DDX_CHECK( IDC_CHECK_SCROLLBAR		, m_nFlatView	)	//+++
		DDX_CHECK( IDC_CHK_BLOCK_MAILTO		, m_nBlockMailTo)
		DDX_CHECK( IDC_CHK_MOUSE_GESTURE 	, m_nGesture	)
		DDX_CHECK( IDC_CHK_MSG_FILTER		, m_nFilter 	)
		DDX_RADIO( IDC_RADIO_RELOAD_NONE 	, m_nReload 	)
		DDX_CHECK( IDC_CHECK_ENABLE			, m_bEnabled	)
		//+++ サーチ関係追加
		if (IDD == IDD_DIALOG_EXPROPERTY) {
			DDX_TEXT ( IDC_EDIT_EXSEARCH_FRONTURL	, m_exs_frontURL	)
			DDX_TEXT ( IDC_EDIT_EXSEARCH_BACKURL 	, m_exs_backURL 	)
			DDX_TEXT ( IDC_EDIT_EXSEARCH_FRONTKEYWORD,m_exs_frontKeyword)
			DDX_TEXT ( IDC_EDIT_EXSEARCH_BACKKEYWORD, m_exs_backKeyword )
			DDX_CHECK( IDC_CHECK_EXSEARCH_ADDRESSBAR, m_exs_adrBar		)
			DDX_RADIO( IDC_RADIO_EXSEARCH_NONE		, m_exs_encode		)
		}
	END_DDX_MAP()


	BEGIN_MSG_MAP(CExPropertyDialogT<IDD_DLG> )
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK				, OnOK			)
		COMMAND_ID_HANDLER_EX(IDCANCEL			, OnCancel		)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DELETE , OnDelete		)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DEFAULT, OnDefault 	)
		COMMAND_ID_HANDLER_EX(IDC_BTN_DFLTBASE  , OnDfltBase 	)
		COMMAND_ID_HANDLER_EX(IDC_BTN_EXPROP_URL, OnBtnExpropUrl)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam)
	{
		CIniFileI	pr( m_strUrlFile, _T("InternetShortcut") );
		CString 	strUrl	 = pr.GetString( _T("URL")/*, NULL, 1024*/ );
		DWORD		dwEnable = pr.GetValue( _T("Enable"), 0 );
		pr.Close();

	  #if 1	//+++ URL を編集可能に変更. URLを表示させない場合は、URLボタンもなしに.
		m_strUrlBase = strUrl;
		if (m_urlMode > 0) {
			CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetWindowText(strUrl);
			CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).SetReadOnly(1);
			m_urlEditBtnSw = 0;
			CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).EnableWindow(1);
			CButton( GetDlgItem(IDC_BTN_EXPROP_URL) ).EnableWindow(1);
		} else {
			CButton( GetDlgItem(IDC_BTN_EXPROP_URL) ).EnableWindow(0);
			CEdit( GetDlgItem(IDC_EDIT_EXPROP_URL) ).EnableWindow(0);
		}
	  #else
		CStatic( GetDlgItem(IDC_STATIC_URL) ).SetWindowText(strUrl);
	  #endif

		if ( m_strTitle.IsEmpty() ) {
			CString strTitle = MtlGetFileName(m_strUrlFile);
			strTitle = strTitle.Left( strTitle.ReverseFind('.') );
			CStatic( GetDlgItem(IDC_STATIC_TITLE) ).SetWindowText(strTitle);
		} else {
			CStatic( GetDlgItem(IDC_STATIC_TITLE) ).SetWindowText(m_strTitle);
		}

		DoDataExchange(FALSE);
		return 1;
	}


	//+++ 拡張プロパティ情報をボタン状態に変換.
	static int		exProp2btn(DWORD dwExProp, DWORD flag) {
		return (dwExProp & flag) / (flag / 3);
	}


	//+++ ボタン状態を拡張プロパティ情報に変換
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

		// TODO: この位置にその他の検証用のコードを追加してください
		DWORD		dwExProp  = 0;
		DWORD		dwEnabled = (m_bEnabled != 0);	//+++	? 1 : 0;

		dwExProp  |= btn2ExProp( m_nImage		, EXPROP_IMAGE		);
		dwExProp  |= btn2ExProp( m_nFrame		, EXPROP_FRAME	 	);
		dwExProp  |= btn2ExProp( m_nPlugin		, EXPROP_PLUGIN	 	);
		dwExProp  |= btn2ExProp( m_nScript		, EXPROP_SCRIPT	 	);
		dwExProp  |= btn2ExProp( m_nRedir		, EXPROP_REDIR	 	);
		dwExProp  |= btn2ExProp( m_nDNS			, EXPROP_DNS	 	);

		dwExProp  |= btn2ExProp( m_nNaviLock	, EXPROP_NAVI		);
		dwExProp  |= btn2ExProp( m_nFilter		, EXPROP_FILTER	 	);
		dwExProp  |= btn2ExProp( m_nGesture 	, EXPROP_GETSTURE	);
		dwExProp  |= btn2ExProp( m_nBlockMailTo	, EXPROP_MAILTO		);

		if (dwExProp || dwEnabled || m_nReload > 0) {
			if (m_nReload != -1)
				dwExProp |= (EXPROP_REFRESH_NONE << m_nReload);
		}

		m_dwExProp = dwExProp;

		//+++ 追加
		DWORD		dwExPropOpt = 0;
		if (m_exs_adrBar) dwExPropOpt |= EXPROPOPT_ADDRESSBAR;
		dwExPropOpt   |= btn2ExProp( m_nFlatView, EXPROPOPT_FLATVIEW);	//+++
		m_dwExPropOpt = dwExPropOpt;

		// セーブ
		CIniFileIO	pr(m_strUrlFile, m_strSection);
		if (dwEnabled || dwExProp || pr.GetValue(EXPROP_KEY_ENABLED, 0       ))	pr.SetValue(dwEnabled	, EXPROP_KEY_ENABLED);
		if (dwEnabled || dwExProp || pr.GetValue(EXPROP_KEY 	   , 0xAAAAAA))	pr.SetValue(dwExProp	, EXPROP_KEY); 	//+++ 初期値がデフォルト設定になるように.

		//+++ 検索関係追加
		if (IDD == IDD_DIALOG_EXPROPERTY) {
			if (dwExPropOpt 	  || pr.GetValue(EXPROP_OPTION	   , 8))	pr.SetValue(dwExPropOpt , EXPROP_OPTION);

			if ( m_exs_frontURL.IsEmpty() == 0 || pr.GetString(_T("FrontURL")).IsEmpty() == 0
			  || m_exs_backURL.IsEmpty()  == 0 || pr.GetString(_T("BackURL")).IsEmpty()  == 0 )
			{
				pr.SetString(m_exs_frontURL		, _T("FrontURL")	);
				pr.SetString(m_exs_backURL		, _T("BackURL") 	);
			}

			if ( m_exs_frontKeyword.IsEmpty() == 0 || pr.GetString(_T("FrontKeyword")).IsEmpty() == 0
			  || m_exs_backKeyword.IsEmpty()  == 0 || pr.GetString(_T("BackKeyword")).IsEmpty()  == 0)
			{
				pr.SetString(m_exs_frontKeyword	, _T("FrontKeyword"));
				pr.SetString(m_exs_backKeyword	, _T("BackKeyword") );
			}

			if (m_exs_encode || pr.GetValue(_T("Encode"), 0) != 0)
				pr.SetValue(m_exs_encode	, _T("Encode")		);
		}

		//+++ URL の編集を行った場合
		if (m_urlMode > 0) {
			CString 	strUrl = MtlGetWindowText( GetDlgItem(IDC_EDIT_EXPROP_URL) );
			if (m_strUrlBase != strUrl) {
				pr.ChangeSectionName( _T("InternetShortcut") );
				pr.SetString( strUrl, _T("URL") );
			}
		}

		pr.Close();

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
		m_nFlatView		= 0;	//+++
		m_nFilter		= 0;
		m_nGesture		= 0;
		m_nBlockMailTo	= 0;
		m_nReload		= 0;

		if (m_strSection == DONUT_SECTION) {
			//x MtlIniDeleteSection(m_strUrlFile, m_strSection);	//+++ メンバーに置き換え.
			CIniFileO 	pr(m_strUrlFile, m_strSection);
			pr.DeleteSection();
		} else {
			CIniFileO 	pr(m_strUrlFile, m_strSection);
			pr.DeleteValue(EXPROP_KEY);
			pr.DeleteValue(EXPROP_KEY_ENABLED);
		}

		m_bEnabled	= FALSE;

		//+++ 検索関係追加.
		m_exs_frontURL	   = "";
		m_exs_backURL	   = "";
		m_exs_frontKeyword = "";
		m_exs_backKeyword  = "";
		m_exs_adrBar	   = 0;
		m_exs_encode	   = 0;

		DoDataExchange(FALSE);
	}


	void OnDefault(UINT uNotifyCode, int nID, HWND hWndCtl)
	{
		DWORD 	dwDLFlag	= CDLControlOption::s_dwDLControlFlags;
		DWORD 	dwExStyle	= CDLControlOption::s_dwExtendedStyleFlags; //_dwViewStyle;

		m_nReload	   = 0;

		m_nImage	   =  (dwDLFlag & GECKO_DLCTL_IMAGES		 )	!= 0;	//+++	? 1 : 0;
		m_nFrame	   =  (dwDLFlag & GECKO_DLCTL_FRAMES		 )	!= 0;
		m_nPlugin	   =  (dwDLFlag & GECKO_DLCTL_PLUGINS		 )	!= 0;
		m_nScript	   =  (dwDLFlag & GECKO_DLCTL_SCRIPTS		 )	!= 0;
		m_nRedir	   =  (dwDLFlag & GECKO_DLCTL_REDIR			 )	!= 0;
		m_nDNS		   =  (dwDLFlag & GECKO_DLCTL_DNS			 )	!= 0;

		m_nNaviLock    =  (dwExStyle & DVS_EX_OPENNEWWIN	 )	!= 0;	//+++	? 1 : 0;
		m_nFlatView    =  (dwExStyle & DVS_EX_FLATVIEW	 	 )	!= 0;	//+++
		m_nFilter	   =  (dwExStyle & DVS_EX_MESSAGE_FILTER )	!= 0;	//+++	? 1 : 0;
		m_nGesture	   =  (dwExStyle & DVS_EX_MOUSE_GESTURE  )	!= 0;	//+++	? 1 : 0;
		m_nBlockMailTo =  (dwExStyle & DVS_EX_BLOCK_MAILTO	 )	!= 0;	//+++	? 1 : 0;

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
		m_nNaviLock 	= BST_INDETERMINATE;	//+++
		m_nFlatView 	= BST_INDETERMINATE;	//+++
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
		DoDataExchange(FALSE);
	}
  #endif

};

typedef CExPropertyDialogT<IDD_DIALOG_EXPROPERTY>		CExPropertyDialog;
//typedef CExPropertyDialogT<IDD_DIALOG_EXPROPERTY2>	CExPropertyDialog2;

