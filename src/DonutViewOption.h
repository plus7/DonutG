/**
 *	@file	DonutViewOption.h
 *	@brief	Viewのオプション設定.
 */

#pragma once

#include "MtlUpdateCmdUI.h"
#include "option/DLControlOption.h"
#include "resource.h"


#if 1 //+++ .dfg セーブ用の情報
#include "MtlProfile.h"
#include "IniFile.h"

#include <nsISHistoryInternal.h>


///+++ .dfg へ書き込むセーブ情報.
struct SDfgSaveInfo {
	CString 			m_section;
	CString 			m_title;
	CString 			m_location_URL;
	DWORD				m_DL_Control_Flags;
	DWORD				m_dwAutoRefreshStyle;
	CWindowPlacement	m_wndpl;
	DWORD				m_dwExStyle;

	//x typedef std::list<std::pair<CString, CString> > 	List;
	typedef std::vector<std::pair<CString, CString> >		List;
	List				m_listFore;
	List				m_listBack;

public:
	SDfgSaveInfo() : m_dwAutoRefreshStyle(0), m_dwExStyle(0) {;}

	void WriteProfile(CIniFileO& pr, bool bSaveTravelLog) {
		pr.ChangeSectionName( m_section );
		pr.SetStringUW( m_title				, _T("Title") ); //minit
		pr.SetString( m_location_URL 		, _T("Location_URL") );
		pr.SetValue ( m_DL_Control_Flags 	, _T("DL_Control_Flags") );
		pr.SetValue ( m_dwAutoRefreshStyle	, _T("Auto_Refresh_Style") );
		m_wndpl.WriteProfile(pr , _T("child."));
		pr.SetValue( m_dwExStyle, _T("Extended_Style") );

		if (bSaveTravelLog) {	// 進む戻る履歴を保存する場合.
			CString 		strTmp;
			CString 		strDir	= "Fore";
			unsigned		i		= 0;
			for (SDfgSaveInfo::List::iterator l = m_listFore.begin();
				l != m_listFore.end();
				++l)
			{
				strTmp.Format(_T("%s_Title%d"), strDir, i);
				pr.SetStringUW(l->first , strTmp);
				strTmp.Format(_T("%s_URL%d")  , strDir, i);
				pr.SetString(l->second, strTmp);
				++i;
			}
			strDir	= "Back";
			i		= 0;
			for (SDfgSaveInfo::List::iterator l = m_listBack.begin();
				l != m_listBack.end();
				++l)
			{
				strTmp.Format(_T("%s_Title%d"), strDir, i);
				pr.SetStringUW(l->first , strTmp);
				strTmp.Format(_T("%s_URL%d")  , strDir, i);
				pr.SetString(l->second, strTmp);
				++i;
			}
		}
	}
};
#endif


enum EDvs_AutoRefresh {
	DVS_AUTOREFRESH_15SEC	= 0x00000001L,
	DVS_AUTOREFRESH_30SEC	= 0x00000002L,
	DVS_AUTOREFRESH_1MIN	= 0x00000004L,
	DVS_AUTOREFRESH_2MIN	= 0x00000008L,
	DVS_AUTOREFRESH_5MIN	= 0x00000010L,
	DVS_AUTOREFRESH_USER	= 0x00000020L,	// UDT DGSTR ( dai

	DVS_AUTOREFRESH_OR		= (    DVS_AUTOREFRESH_15SEC | DVS_AUTOREFRESH_30SEC
								 | DVS_AUTOREFRESH_1MIN  | DVS_AUTOREFRESH_2MIN | DVS_AUTOREFRESH_5MIN
								 | DVS_AUTOREFRESH_USER
							  ),
};


//#define DVS_EX_OPENNEWWIN 	0x00000001L
//#define DVS_EX_MESSAGE_FILTER 0x00000002L



template <class _DonutView>
class CDonutViewOption {
public:
	DWORD		m_dwAutoRefreshStyle;
	DWORD		m_dwExStyle;

private:
	CString 	m_strURL;

	_DonutView *__m_pDonutView;
	UINT_PTR	m_nIDEvent;

public:
	CDonutViewOption(_DonutView *__pDonutView, DWORD dwExStyle);	//+++引数追加.

	void	Init();
	void	Uninit();

	void	SetAutoRefreshStyle(DWORD dwStyle);

public:
	// Message map and handlers
	BEGIN_MSG_MAP(CDonutViewOption)
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_NONE , OnAutoRefreshNone )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_15SEC, OnAutoRefresh15sec)
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_30SEC, OnAutoRefresh30sec)
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_1MIN , OnAutoRefresh1min )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_2MIN , OnAutoRefresh2min )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_5MIN , OnAutoRefresh5min )
		COMMAND_ID_HANDLER_EX( ID_AUTOREFRESH_USER , OnAutoRefreshUser )		// UDT DGSTR ( dai
		// COMMAND_ID_HANDLER_EX(ID_DOCHOSTUI_OPENNEWWIN, OnDocHostUIOpenNewWin)

		// UH
		COMMAND_ID_HANDLER_EX( ID_MESSAGE_FILTER  , OnMessageFilter   )
		COMMAND_ID_HANDLER_EX( ID_MOUSE_GESTURE   , OnMouseGesture	  )
		COMMAND_ID_HANDLER_EX( ID_BLOCK_MAILTO	  , OnBlockMailto	  )
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

private:
	void OnBlockMailto		  (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnMouseGesture 	  (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnMessageFilter	  (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnDocHostUIOpenNewWin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnAutoRefreshNone	  (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnAutoRefresh15sec   (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnAutoRefresh30sec   (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnAutoRefresh1min	  (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnAutoRefresh2min	  (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnAutoRefresh5min	  (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	// UDT DGSTR ( dai
	void OnAutoRefreshUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	// ENDE

	void	OnTimer(UINT_PTR wTimerID, TIMERPROC dmy = 0);
	int 	_GetElapse();
	void	_KillTimer();
	void	_SetTimer();

public:
	void	WriteProfile(const CString &strFileName, int nIndex);	//+++	未使用状態
	void	GetProfile(const CString &strFileName, int nIndex, bool bGetChildFrameState);
	void    LoadSession(CIniFileI& pr, bool bGetChildFrameState);
	void	_GetProfile(const CString &strFileName, const CString &strSection, bool bGetChildFrameState);

public:
	// Update command UI and handlers
	BEGIN_UPDATE_COMMAND_UI_MAP(CDonutViewOption)
		UPDATE_COMMAND_UI(ID_AUTOREFRESH_USER, OnUpdateAutoRefreshUser) 	// UDT DGSTR ( dai
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS(ID_AUTOREFRESH_NONE, m_dwAutoRefreshStyle == 0)
		UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_15SEC, DVS_AUTOREFRESH_15SEC, m_dwAutoRefreshStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_30SEC, DVS_AUTOREFRESH_30SEC, m_dwAutoRefreshStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_1MIN , DVS_AUTOREFRESH_1MIN , m_dwAutoRefreshStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_2MIN , DVS_AUTOREFRESH_2MIN , m_dwAutoRefreshStyle)
		UPDATE_COMMAND_UI_SETCHECK_FLAG(ID_AUTOREFRESH_5MIN , DVS_AUTOREFRESH_5MIN , m_dwAutoRefreshStyle)
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_DOCHOSTUI_OPENNEWWIN, DVS_EX_OPENNEWWIN, m_dwExStyle)

		// UH
		UPDATE_COMMAND_UI_SETCHECK_IF_PASS(ID_AUTO_REFRESH, m_dwAutoRefreshStyle != 0)
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_MESSAGE_FILTER, DVS_EX_MESSAGE_FILTER, m_dwExStyle)
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_MOUSE_GESTURE , DVS_EX_MOUSE_GESTURE , m_dwExStyle)
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG(ID_BLOCK_MAILTO	, DVS_EX_BLOCK_MAILTO  , m_dwExStyle)
	END_UPDATE_COMMAND_UI_MAP()

private:
	// UDT DGSTR ( dai
	void	OnUpdateAutoRefreshUser(CCmdUI *pCmdUI);
	// ENDE

public:
	BOOL _OutPut_TravelLogs(SDfgSaveInfo::List& listFore, SDfgSaveInfo::List& listBack);

	BOOL GetDfgSaveInfo(SDfgSaveInfo & rDfgSaveInfo, bool bSaveFB);

};



#define DONUTVIEWOPTION_CTOR()	 \
	template <class _DonutView>  \
	CDonutViewOption<_DonutView>


#define DONUTVIEWOPTION(rettype) \
	template <class _DonutView>  \
	rettype CDonutViewOption<_DonutView>


DONUTVIEWOPTION_CTOR() ::CDonutViewOption(_DonutView * __pDonutView, DWORD dwExStyle/*+++*/)
	: __m_pDonutView(__pDonutView)
	  , m_nIDEvent(0)
	  , m_dwAutoRefreshStyle(0)
	  , m_dwExStyle(dwExStyle)	//+++引数追加.
	  , m_strURL()			//+++ 抜け対策で書いとく.
{
}



DONUTVIEWOPTION(void) ::Init()
{
	_SetTimer();
}



DONUTVIEWOPTION(void) ::Uninit()
{
	_KillTimer();
}



DONUTVIEWOPTION(void) ::SetAutoRefreshStyle(DWORD dwStyle)
{
	m_dwAutoRefreshStyle = 0;
	DWORD dwCurFlag = DVS_AUTOREFRESH_USER;

	while (dwCurFlag > 0) {
		if (dwCurFlag & dwStyle) {
			m_dwAutoRefreshStyle = dwCurFlag;
			break;
		}

		dwCurFlag >>= 1;
	}

	_KillTimer();
	_SetTimer();
}



DONUTVIEWOPTION(void) ::OnBlockMailto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_dwExStyle & DVS_EX_BLOCK_MAILTO)
		m_dwExStyle &= ~DVS_EX_BLOCK_MAILTO;
	else
		m_dwExStyle |= DVS_EX_BLOCK_MAILTO;
}



DONUTVIEWOPTION(void) ::OnMouseGesture(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_dwExStyle & DVS_EX_MOUSE_GESTURE)
		m_dwExStyle &= ~DVS_EX_MOUSE_GESTURE;
	else
		m_dwExStyle |= DVS_EX_MOUSE_GESTURE;
}



DONUTVIEWOPTION(void) ::OnMessageFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_dwExStyle & DVS_EX_MESSAGE_FILTER)
		m_dwExStyle &= ~DVS_EX_MESSAGE_FILTER;
	else
		m_dwExStyle |= DVS_EX_MESSAGE_FILTER;
}



DONUTVIEWOPTION(void) ::OnDocHostUIOpenNewWin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	if (m_dwExStyle & DVS_EX_OPENNEWWIN)
		m_dwExStyle &= ~DVS_EX_OPENNEWWIN;
	else
		m_dwExStyle |= DVS_EX_OPENNEWWIN;
}



DONUTVIEWOPTION(void) ::OnAutoRefreshNone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle = 0;
	_KillTimer();
}



DONUTVIEWOPTION(void) ::OnAutoRefresh15sec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_15SEC;
	_KillTimer();
	_SetTimer();
}



DONUTVIEWOPTION(void) ::OnAutoRefresh30sec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_30SEC;
	_KillTimer();
	_SetTimer();
}



DONUTVIEWOPTION(void) ::OnAutoRefresh1min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_1MIN;
	_KillTimer();
	_SetTimer();
}



DONUTVIEWOPTION(void) ::OnAutoRefresh2min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_2MIN;
	_KillTimer();
	_SetTimer();
}



DONUTVIEWOPTION(void) ::OnAutoRefresh5min(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_5MIN;
	_KillTimer();
	_SetTimer();
}



// UDT DGSTR ( dai
DONUTVIEWOPTION(void) ::OnAutoRefreshUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	m_dwAutoRefreshStyle  = 0;
	m_dwAutoRefreshStyle |= DVS_AUTOREFRESH_USER;
	_KillTimer();
	_SetTimer();
}
// ENDE



DONUTVIEWOPTION(void) ::OnTimer(UINT_PTR wTimerID, TIMERPROC)
{
	ATLTRACE2( atlTraceGeneral, 4, _T("CChildFrame::OnTimer\n") );

	if (wTimerID == m_nIDEvent)
		::PostMessage(__m_pDonutView->GetParent(), WM_COMMAND, (WPARAM) ID_VIEW_REFRESH, 0);
	//			__m_pDonutView->m_spBrowser->Refresh();
	else
		SetMsgHandled(FALSE);
}



DONUTVIEWOPTION(int) ::_GetElapse()
{
	if (m_dwAutoRefreshStyle == 0)
		return -1;
	else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_15SEC)
		return 15 * 1000;
	else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_30SEC)
		return 30 * 1000;
	else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_1MIN)
		return 1 * 60 * 1000;
	else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_2MIN)
		return 2 * 60 * 1000;
	else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_5MIN)
		return 5 * 60 * 1000;
	// UDT DGSTR ( dai
	else if (m_dwAutoRefreshStyle & DVS_AUTOREFRESH_USER)
		return CMainOption::s_dwAutoRefreshTime * 1000;

	// ENDE

	return -1;
}



DONUTVIEWOPTION(void) ::_KillTimer()
{
	if (m_nIDEvent != 0) {
		MTLVERIFY( ::KillTimer(__m_pDonutView->m_hWnd, m_nIDEvent) );
		m_nIDEvent = 0;
	}
}



DONUTVIEWOPTION(void) ::_SetTimer()
{
	int nElapse = _GetElapse();

	if (nElapse != -1)
		m_nIDEvent = ::SetTimer(__m_pDonutView->m_hWnd, 1, _GetElapse(), NULL);
}



#if 1	//+++ 未使用状態? 念のため残す...
DONUTVIEWOPTION(void) ::WriteProfile(const CString &strFileName, int nIndex)
{
	bool			bSaveFB = false;
	SDfgSaveInfo	dfgSaveInfo;
	dfgSaveInfo.m_section.Format(_T("Window%d"), nIndex);
	GetDfgSaveInfo(dfgSaveInfo, bSaveFB);
	CIniFileO		pr( strFileName, _T("Window") );
	dfgSaveInfo.WriteProfile(pr, bSaveFB);
}
#endif



DONUTVIEWOPTION(void) ::GetProfile(const CString &strFileName, int nIndex, bool bGetChildFrameState)
{
	CString strSection;

	strSection.Format(_T("Window%d"), nIndex);
	_GetProfile(strFileName, strSection, bGetChildFrameState);
}


DONUTVIEWOPTION(void) ::LoadSession(CIniFileI& pr, bool bGetChildFrameState)
//ここで適切なのだろうか
{
	// 戻る・進むの項目を設定する
	/*if (CMainOption::s_bTravelLogGroup) {
		_Load_OptionalData(pChild, strFileName, strSection);
	}*/

	/*TCHAR		szUrl[INTERNET_MAX_PATH_LENGTH];
	szUrl[0]			= 0;
	DWORD		dwBytes = INTERNET_MAX_PATH_LENGTH;

	if (pr.QueryString(szUrl, _T("Location_URL"), &dwBytes) == ERROR_SUCCESS) {
		// avoid endless loop
		CString strURL(szUrl);
		bool	bMaybeEndless = ( strURL == _T("javascript:location.reload()") );

		if (!bMaybeEndless)
			__m_pDonutView->Navigate2(szUrl);
	}*/
	std::vector<std::pair<CString, CString> >	shArr;
	CString 		strKey;
	unsigned index = -1;
	unsigned i;
	if (true/*CMainOption::s_bTravelLogGroup*/) { //???
		for (i = 0; i < 0x7fffffff; ++i) {
			strKey.Format(_T("Back_URL%d"), i);
			CString strURL = pr.GetString(strKey/*, NULL, MAX_PATH*/);
			if (strURL.IsEmpty())
				break;
			strKey.Format(_T("Back_Title%d"), i);
			CString strTitle = pr.GetStringUW(strKey/*, NULL, 1024*/);
			if (strTitle.IsEmpty())
				break;
			shArr.push_back( std::make_pair(strTitle, strURL) );
			index++;
		}
	}

	CString strURL = pr.GetString( _T("Location_URL")/*, NULL, MAX_PATH*/);
	shArr.push_back( std::make_pair(_T(""), strURL) );
	index = index + 1;

	if (true/*CMainOption::s_bTravelLogGroup*/) { //???
		for (unsigned i = 0; i < 0x7fffffff; ++i) {
			strKey.Format(_T("Fore_URL%d"), i);
			CString strURL = pr.GetString(strKey/*, NULL, MAX_PATH*/);
			if (strURL.IsEmpty())
				break;
			strKey.Format(_T("Fore_Title%d"), i);
			CString strTitle = pr.GetStringUW(strKey/*, NULL, 1024*/);
			if (strTitle.IsEmpty())
				break;
			shArr.push_back( std::make_pair(strTitle, strURL) );
		}
	}

	nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(__m_pDonutView->m_spBrowser);
	nsCOMPtr<nsISHistory> sh;
	nav->GetSessionHistory(getter_AddRefs(sh));
	nsCOMPtr<nsISHistoryInternal> shi = do_QueryInterface(sh);

	for (unsigned int i = 0; i < shArr.size(); i++) {
		nsCOMPtr<nsISHEntry> newEntry = do_CreateInstance("@mozilla.org/browser/session-history-entry;1");
		nsCOMPtr<nsIURI> uri;
		NS_NewURI(getter_AddRefs(uri), NS_LossyConvertUTF16toASCII(shArr[i].second));
		newEntry->SetURI(uri);
		newEntry->SetTitle(nsEmbedString(shArr[i].first));
		shi->AddEntry(newEntry, true);
	}
	nav->GotoIndex(index);
}

DONUTVIEWOPTION(void) ::_GetProfile(const CString &strFileName, const CString & strSection, bool bGetChildFrameState)
{
	CString 	strSectionName	 = strSection;

	CIniFileI	pr(strFileName, strSectionName);

	DWORD		dwDLControlFlags = CDLControlOption::DLCTL_DEFAULT;

	if (pr.QueryValue( dwDLControlFlags, _T("DL_Control_Flags") ) == ERROR_SUCCESS)
		__m_pDonutView->PutDLControlFlags(dwDLControlFlags);

	LoadSession(pr, bGetChildFrameState);

	pr.QueryValue( m_dwAutoRefreshStyle, _T("Auto_Refresh_Style") );
	_SetTimer();

	if (bGetChildFrameState)
		MtlGetProfileChildFrameState( pr, __m_pDonutView->GetParent(), SW_SHOWNORMAL, true, _T("child.") );

	// load extended style
	pr.QueryValue( m_dwExStyle, _T("Extended_Style") ); 	// never give up yeah.
	pr.Close();

	if ( _check_flag(DVS_EX_OPENNEWWIN, m_dwExStyle) ) {
		MtlSendOnCommand(__m_pDonutView->GetParent(), ID_DOCHOSTUI_OPENNEWWIN);
	}
}



// UDT DGSTR ( dai
DONUTVIEWOPTION(void) ::OnUpdateAutoRefreshUser(CCmdUI * pCmdUI)
{
	DWORD	dwRefreshTime = CMainOption::s_dwAutoRefreshTime;

	pCmdUI->Enable(!dwRefreshTime == 0 );
	pCmdUI->SetCheck(m_dwAutoRefreshStyle == DVS_AUTOREFRESH_USER/*? 1 : 0*/);
	CString str;

	// NOTE: INUYA
	// if using StrFromTimeInterval , Load Shlwapi.dll...
	// dwRefreshTime is limited to 3599.
	if (dwRefreshTime % 60 == 0)	str.Format(_T("%d分")    , dwRefreshTime / 60);
	else if (dwRefreshTime >= 60)	str.Format(_T("%d分%d秒"), dwRefreshTime / 60, dwRefreshTime % 60);
	else							str.Format(_T("%d秒")    , dwRefreshTime % 60);

	str += "(ユーザー設定)";
	pCmdUI->SetText(str);
}
// ENDE




//+++ CComPtr<ITravelLogStg>pTLStg をフレームのある頁に使うとハングする... でいろいろテスト中
DONUTVIEWOPTION(BOOL) ::_OutPut_TravelLogs(SDfgSaveInfo::List& listFore, SDfgSaveInfo::List& listBack)
{
	nsCOMPtr<nsIWebBrowser> wb = __m_pDonutView->m_spBrowser;
	if(!wb) return FALSE;
	nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(wb);
	nsCOMPtr<nsISHistory> sh;
	nav->GetSessionHistory(getter_AddRefs(sh));
	if(!sh) return FALSE;
	PRInt32 count, index;
	sh->GetCount(&count);
	sh->GetIndex(&index);
	for(int i=0;i<count;i++){
		nsCOMPtr<nsIHistoryEntry> e;
		sh->GetEntryAtIndex(i, PR_FALSE, getter_AddRefs(e));
		nsEmbedString title;
		e->GetTitle(getter_Copies(title));
		nsCOMPtr<nsIURI> uri;
		e->GetURI(getter_AddRefs(uri));
		nsEmbedCString spec;
		uri->GetSpec(spec);
		if(i<index){
			listBack.push_back(std::make_pair( CString(title.get()), CString(spec.get()) ));
		}
		else if(i>index){
			listFore.push_back(std::make_pair( CString(title.get()), CString(spec.get()) ));
		}
	}
	return TRUE;
}

/// 自動バックアップでの、_WrilteProfileの代わり.
DONUTVIEWOPTION(BOOL) ::GetDfgSaveInfo(SDfgSaveInfo & rDfgSaveInfo, bool bSaveFB)
{
	rDfgSaveInfo.m_title				= __m_pDonutView->GetLocationName();
	rDfgSaveInfo.m_location_URL     	= __m_pDonutView->GetLocationURL();
	rDfgSaveInfo.m_DL_Control_Flags 	= __m_pDonutView->GetDLControlFlags();
	rDfgSaveInfo.m_dwAutoRefreshStyle	= m_dwAutoRefreshStyle;
	MTLVERIFY( ::GetWindowPlacement(__m_pDonutView->GetParent(), &rDfgSaveInfo.m_wndpl) );
	rDfgSaveInfo.m_dwExStyle			= m_dwExStyle;

	// 進む・戻る履歴を保存
	if (bSaveFB) {
		_OutPut_TravelLogs( rDfgSaveInfo.m_listFore, rDfgSaveInfo.m_listBack );
	}
	return TRUE;
}

// http://msdn2.microsoft.com/en-us/library/aa768369.aspx
