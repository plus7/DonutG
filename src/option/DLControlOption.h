/**
 *	@file	DLControlOption.h
 *	brief	donutのオプション : "ブラウザ"
 */

#pragma once


#include "../resource.h"
#include "../DonutDefine.h"




/////////////////////////////////////////////////////////////////////////////
// CDLControlOption

class CDLControlOption {
	friend class CDLControlPropertyPage;
public:
	// DL Control Flags
	enum { DLCTL_DEFAULT	= (GECKO_DLCTL_IMAGES |
                               GECKO_DLCTL_FRAMES | 
                               GECKO_DLCTL_PLUGINS | 
                               GECKO_DLCTL_SCRIPTS | 
                               GECKO_DLCTL_REDIR) };

	static DWORD	s_dwDLControlFlags; 			// default flags		//+++ ref by MainFrame.cpp
	static DWORD	s_dwExtendedStyleFlags; 								//+++ ref by DonutView.cpp

	static const TCHAR*	userAgent();					//+++ 追加

private:
	static TCHAR	s_szUserAgent[MAX_PATH];			// UDT DGSTR			//+++ ref by MainFrame.cpp
	static TCHAR	s_szUserAgent_cur[MAX_PATH];		//+++

public:
	static void 	GetProfile();
	static void 	WriteProfile();
};




/////////////////////////////////////////////////////////////////////////////
// CDLControlPropertyPage


class CDLControlPropertyPage
	: public CPropertyPageImpl<CDLControlPropertyPage>
	, public CWinDataExchange<CDLControlPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_DLCONTROL };

private:
	// Data members
	int 		m_nImage;
	int 		m_nFrame;
	int 		m_nPlugin;
	int 		m_nScript;
	int 		m_nRedir;
	int 		m_nDNS;
	int 		m_nNaviLock;
	int 		m_nNoCloseNL;			//+++
	int 		m_nFlatView;			// UDT DGSTR ( dai
	int			m_bUserAgent;			//+++
	CString 	m_strUserAgent; 		// UDT DGSTR
	CEdit		m_edit; 				// UDT DGSTR

	// UH
	HWND		m_hMainWnd;
	BOOL		m_bMsgFilter;
	BOOL		m_bMouseGesture;
	BOOL		m_bBlockMailto;
public:

	// DDX map
	BEGIN_DDX_MAP( CDLControlPropertyPage )
		DDX_CHECK( IDC_CHECK_DLCTL_IMAGE 		, m_nImage		)
		DDX_CHECK( IDC_CHECK_DLCTL_FRAME		, m_nFrame		)
		DDX_CHECK( IDC_CHECK_DLCTL_PLUGIN		, m_nPlugin		)
		DDX_CHECK( IDC_CHECK_DLCTL_SCRIPT		, m_nScript		)
		DDX_CHECK( IDC_CHECK_DLCTL_REDIR		, m_nRedir		)
		DDX_CHECK( IDC_CHECK_DLCTL_DNS		, m_nDNS		)
		DDX_CHECK( IDC_CHECK_NAVILOCK			, m_nNaviLock		)
		DDX_CHECK( IDC_CHECK_NOCLOSE_NAVILOCK	, m_nNoCloseNL		)	//+++
		DDX_CHECK( IDC_CHECK_SCROLLBAR			, m_nFlatView		)	// UDT DGSTR ( dai

		// UH
		DDX_CHECK( IDC_CHK_MSG_FILTER			, m_bMsgFilter		)
		DDX_CHECK( IDC_CHK_MOUSE_GESTURE		, m_bMouseGesture	)
		DDX_CHECK( IDC_CHK_BLOCK_MAILTO 		, m_bBlockMailto	)
		DDX_CHECK( IDC_CHK_USER_AGENT	 		, m_bUserAgent		)	//+++

		DDX_TEXT_LEN( IDC_EDIT_USER_AGENT, m_strUserAgent, MAX_PATH )	// UDT DGSTR
	END_DDX_MAP()

	// Constructor
	CDLControlPropertyPage( HWND  hMainWnd );

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

	// Message map and handlers
	BEGIN_MSG_MAP( CDLControlPropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CDLControlPropertyPage> )
	END_MSG_MAP()

	// Implementation
private:
	void	_GetData();
	void	_SetData();
};

