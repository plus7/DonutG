/**
 *	@file	FavoriteMenuDialog.h
 *	@brief	donutのオプション : "お気に入りメニュー"
 */

#pragma once

#include "../resource.h"


class CDonutFavoritesMenuPropertyPage
	: public CPropertyPageImpl<CDonutFavoritesMenuPropertyPage>
	, public CWinDataExchange<CDonutFavoritesMenuPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_EXPMENU };

private:
	// Data members
	int 		m_nAdditionalCheck;
	int 		m_nOrder;
	int 		m_nDrawIcon;
	int 		m_nSpaceMin;
	int 		m_nDelNoAsk;				//+++ 未使用?
	int 		m_nMaxMenuItemTextLength;
	int 		m_nMaxMenuBreakCount;

	int 		m_nAdditionalCheck2;
	int 		m_nMenuOrder;
	int 		m_nAdditionalResetOrder;

	CString 	m_strFolder;
	int 		m_nUserFolder;

	CEdit		m_editUserFolder;

public:
	// Message map and handlers
	BEGIN_MSG_MAP( COpenURLDlg )
		COMMAND_ID_HANDLER( IDC_BUTTON_FAVMENU_USER, OnBtnFavMenuUser )
		CHAIN_MSG_MAP( CPropertyPageImpl<CDonutFavoritesMenuPropertyPage> )
	END_MSG_MAP()

private:
	LRESULT OnBtnFavMenuUser(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/);

public:
	// DDX map
	BEGIN_DDX_MAP( CDonutFavoritesMenuPropertyPage )
		DDX_CHECK( IDC_CHECK_EXPMENU_ADDITIONAL 	 , m_nAdditionalCheck )
		DDX_CHECK( IDC_CHECK_FAVMENU_ORDER			 , m_nOrder )

		DDX_INT_RANGE( IDC_EDIT_EXPMENU_ITEMTEXTMAX  , m_nMaxMenuItemTextLength, 1,  255 )
		DDX_INT_RANGE( IDC_EDIT_EXPMENU_BREAKCOUNTMAX, m_nMaxMenuBreakCount    , 2, 5000 )

		DDX_TEXT( IDC_EDIT_FAVMENU_USER 			 , m_strFolder )

		DDX_CHECK( IDC_CHECK_FAVMENU_USER			, m_nUserFolder )
		DDX_CHECK( IDC_CHECK_DRAW_ICON				, m_nDrawIcon )
		DDX_CHECK( IDC_CHECK_SPACE_MIN				, m_nSpaceMin )
		DDX_CHECK( IDC_CHECK_EXPMENU_ADDITIONAL2	, m_nAdditionalCheck2 )
		//DDX_CHECK(IDC_CHECK_DEL_NOASK, m_nDelNoAsk )		//*+++ ここ、ほんとにコメントアウトでよいのか?
		DDX_CHECK( IDC_CHECK_MENUORDER				, m_nMenuOrder )
		DDX_CHECK( IDC_CHECK_RESETORDER				, m_nAdditionalResetOrder )
	END_DDX_MAP()

	// Constructor
	CDonutFavoritesMenuPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

private:
	CString _BrowseForFolder();
};
