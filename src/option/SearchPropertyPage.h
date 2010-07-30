/**
 *	@file	SearchPropertyPage.h
 *	@brief	検索バーに関するオプション設定
 *	@note
 *		+++ SearchBar.h より分離.
 */

#ifndef SEARCHPROPETRYPAGE_H
#define SEARCHPROPETRYPAGE_H
#pragma once


#include "SearchBar.h"



class CSearchPropertyPage
	: public CPropertyPageImpl<CSearchPropertyPage>
	, public CWinDataExchange<CSearchPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_SEARCHBAR };

private:
	// Data members
	int 	m_nEngChgGo;
	int 	m_nDropGo;
	int 	m_nHistorySave;
	int 	m_nHistorySaveCnt;
	int 	m_nNoReposition;
	int 	m_nKeyChgGo;
	DWORD	m_dwStatus;
	int 	m_nHeightCount;
	int 	m_nHeightCountCnt;
	int 	m_nFiltering;
	int 	m_nLastSel;
	int 	m_nScrollCenter;
	int 	m_nActiveWindow;
	int 	m_nAutoHilight;
  #ifndef USE_DIET
	int 	m_nNoWordButton;		//+++
	int		m_nUsePageButton;		//+++
  #endif

public:
	// DDX map
	BEGIN_DDX_MAP(CSearchPropertyPage)
		DDX_CHECK( IDC_CHECK_ENG_CHG_GO   , m_nEngChgGo 	)
		DDX_CHECK( IDC_CHECK_KEY_CHG_GO   , m_nKeyChgGo 	)
		DDX_CHECK( IDC_CHECK_DROP_GO	  , m_nDropGo		)
		DDX_CHECK( IDC_CHECK_HISTORY_SAVE , m_nHistorySave	)
		DDX_CHECK( IDC_CHECK_NO_REPOSITION, m_nNoReposition )
		DDX_CHECK( IDC_CHECK_HEIGHTCOUNT  , m_nHeightCount	)
		DDX_CHECK( IDC_CHECK_FILTER 	  , m_nFiltering	)
		DDX_CHECK( IDC_CHECK_LASTSEL	  , m_nLastSel		)
		DDX_CHECK( IDC_CHECK_SCROLLCENTER , m_nScrollCenter )
		DDX_CHECK( IDC_CHECK_ACTIVEWINDOW , m_nActiveWindow )
		DDX_CHECK( IDC_CHECK_AUTOHILIGHT  , m_nAutoHilight	)
	  #ifndef USE_DIET
		DDX_CHECK( IDC_SEARCH_NOWORDBUTTON, m_nNoWordButton )			//+++
		DDX_CHECK( IDC_SEARCH_PAGEBUTTON  , m_nUsePageButton)			//+++
	  #endif

		DDX_INT  ( IDC_ED_HISTORY_CNT	  , m_nHistorySaveCnt )
		DDX_INT_RANGE( IDC_EDIT_HEIGHTCOUNT, m_nHeightCountCnt, 1, int(MAXHEIGHTCOUNT) )
	END_DDX_MAP()


public:
	CSearchPropertyPage()
	{
	  #if 0 //+++ _SetData()で設定されているのでやめにしておく.
		m_dwStatus		  = 0;
		m_nEngChgGo 	  = 0;
		m_nDropGo		  = 0;
		m_nHistorySave	  = 0;
		m_nHistorySaveCnt = 0;
		m_nNoReposition   = 0;
		m_nKeyChgGo 	  = 0;
		m_nHeightCount	  = 0;
		m_nFiltering	  = 0;
		m_nLastSel		  = 0;
		m_nScrollCenter   = 1;
		m_nActiveWindow   = 0;
		m_nAutoHilight	  = 0;
	  #endif
		_SetData();
	}


	// Overrides
	BOOL OnSetActive()
	{
		SetModified(TRUE);

		return DoDataExchange(FALSE);
	}


	BOOL OnKillActive()
	{
		return DoDataExchange(TRUE);
	}


	BOOL OnApply()
	{
		if ( DoDataExchange(TRUE) ) {
			_GetData();
			return TRUE;
		} else {
			return FALSE;
		}
	}


private:
	// Constructor

	/// データを得る
	void _SetData()
	{
		CIniFileI	pr( g_szIniFileName, _T("SEARCH")	  );
		m_dwStatus			= pr.GetValue( _T("Status"), STS_TEXT_FILTER | STS_DROP_GO | STS_HEIGHTCOUNT | STS_HISTORY_SAVE );
		m_nHistorySaveCnt	= pr.GetValue( _T("HistoryCnt"), 10 );
		m_nHeightCountCnt	= pr.GetValue( _T("HeightCnt") , DEFAULT_HEIGHTCOUNT );
	  #ifndef USE_DIET
		m_nNoWordButton 	= pr.GetValue( _T("NoWordButton"), 0 );		//+++
		m_nUsePageButton	= (m_nNoWordButton >> 1) & 1;				//+++ キー名が不適になるが、単独ページボタンのフラグもついでに含める...
		m_nNoWordButton		&= 1;
	  #endif
		pr.Close();

		m_nEngChgGo 		= (m_dwStatus & STS_ENG_CHANGE_GO ) == STS_ENG_CHANGE_GO;
		m_nDropGo			= (m_dwStatus & STS_DROP_GO 	  ) == STS_DROP_GO;
		m_nHistorySave		= (m_dwStatus & STS_HISTORY_SAVE  ) == STS_HISTORY_SAVE;
		m_nNoReposition 	= (m_dwStatus & STS_NO_REPOSITION ) == STS_NO_REPOSITION;
		m_nKeyChgGo 		= (m_dwStatus & STS_KEY_CHANGE_GO ) == STS_KEY_CHANGE_GO;
		m_nHeightCount		= (m_dwStatus & STS_HEIGHTCOUNT   ) == STS_HEIGHTCOUNT;
		m_nFiltering		= (m_dwStatus & STS_TEXT_FILTER   ) == STS_TEXT_FILTER;
		m_nLastSel			= (m_dwStatus & STS_LASTSEL 	  ) == STS_LASTSEL;
		m_nScrollCenter 	= (m_dwStatus & STS_SCROLLCENTER  ) == STS_SCROLLCENTER;
		m_nActiveWindow 	= (m_dwStatus & STS_ACTIVEWINDOW  ) == STS_ACTIVEWINDOW;
		m_nAutoHilight		= (m_dwStatus & STS_AUTOHILIGHT   ) == STS_AUTOHILIGHT;
	}


	/// データを保存
	void _GetData()
	{
		m_dwStatus = 0;
		if (m_nEngChgGo)		m_dwStatus |= STS_ENG_CHANGE_GO;
		if (m_nDropGo)			m_dwStatus |= STS_DROP_GO;
		if (m_nHistorySave) 	m_dwStatus |= STS_HISTORY_SAVE;
		if (m_nNoReposition)	m_dwStatus |= STS_NO_REPOSITION;
		if (m_nKeyChgGo)		m_dwStatus |= STS_KEY_CHANGE_GO;
		if (m_nHeightCount) 	m_dwStatus |= STS_HEIGHTCOUNT;
		if (m_nFiltering)		m_dwStatus |= STS_TEXT_FILTER;
		if (m_nLastSel) 		m_dwStatus |= STS_LASTSEL;
		if (m_nScrollCenter)	m_dwStatus |= STS_SCROLLCENTER;
		if (m_nActiveWindow)	m_dwStatus |= STS_ACTIVEWINDOW;
		if (m_nAutoHilight) 	m_dwStatus |= STS_AUTOHILIGHT;

		CIniFileO	pr( g_szIniFileName, _T("SEARCH") );
		pr.SetValue( m_dwStatus, _T("Status") );
		pr.SetValue( (DWORD) m_nHistorySaveCnt, _T("HistoryCnt") );
		pr.SetValue( (DWORD) m_nHeightCountCnt, _T("HeightCnt") );

	  #ifndef USE_DIET
		DWORD nowordbutton   = m_nNoWordButton | (m_nUsePageButton << 1);	//+++ キー名が不適になるが、単独ページボタンのフラグもついでに含める...
		pr.SetValue( (DWORD) nowordbutton, _T("NoWordButton") );
	  #endif
	}


public:
	// Message map and handlers
	BEGIN_MSG_MAP(CSearchPropertyPage)
		CHAIN_MSG_MAP(CPropertyPageImpl<CSearchPropertyPage>)
	END_MSG_MAP()

};


#endif
