/**
 *	@file	FavoriteMenuDialog.cpp
 *	@brief	donutのオプション : "お気に入りメニュー"
 */

#include "stdafx.h"
#include "../DonutFavoritesMenu.h"
#include "FavoriteMenuDialog.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




LRESULT CDonutFavoritesMenuPropertyPage::OnBtnFavMenuUser(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
	CString strEdit = _BrowseForFolder();

	if ( !strEdit.IsEmpty() )
		m_editUserFolder.SetWindowText(strEdit);

	return 0;
}



// Constructor
CDonutFavoritesMenuPropertyPage::CDonutFavoritesMenuPropertyPage()
{
	m_nAdditionalCheck		 = _check_flag( EMS_ADDITIONALMENUITEM	 , CFavoritesMenuOption::GetStyle() );
	m_nAdditionalCheck2 	 = _check_flag( EMS_ADDITIONAL_SAVEFAV_TO, CFavoritesMenuOption::GetStyle() );
	m_nOrder				 = _check_flag( EMS_ORDER_FAVORITES		 , CFavoritesMenuOption::GetStyle() );
	m_nDrawIcon 			 = _check_flag( EMS_DRAW_ICON			 , CFavoritesMenuOption::GetStyle() );
	m_nSpaceMin 			 = _check_flag( EMS_SPACE_MIN			 , CFavoritesMenuOption::GetStyle() );
	m_nDelNoAsk 			 = _check_flag( EMS_DEL_NOASK			 , CFavoritesMenuOption::GetStyle() );
	m_nMenuOrder			 = _check_flag( EMS_CUSTOMIZE_ORDER		 , CFavoritesMenuOption::GetStyle() );
	m_nAdditionalResetOrder  = _check_flag( EMS_ADDITIONAL_RESETORDER, CFavoritesMenuOption::GetStyle() );

	m_nMaxMenuItemTextLength = CFavoritesMenuOption::GetMaxMenuItemTextLength();
	m_nMaxMenuBreakCount	 = CFavoritesMenuOption::GetMaxMenuBreakCount();

	m_nUserFolder			 = _check_flag( EMS_USER_DEFINED_FOLDER, CFavoritesMenuOption::GetStyle()	);
	m_strFolder 			 = CFavoritesMenuOption::GetUserDirectory();
}



// Overrides
BOOL CDonutFavoritesMenuPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_editUserFolder.m_hWnd == NULL)
		m_editUserFolder.Attach( GetDlgItem(IDC_EDIT_FAVMENU_USER) );

	return DoDataExchange(FALSE);
}



BOOL CDonutFavoritesMenuPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CDonutFavoritesMenuPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		DWORD dwStyle = 0;

		if (m_nAdditionalCheck	/*== 1*/)	dwStyle |= EMS_ADDITIONALMENUITEM;
		if (m_nOrder			/*== 1*/)	dwStyle |= EMS_ORDER_FAVORITES;
		if (m_nUserFolder		/*== 1*/)	dwStyle |= EMS_USER_DEFINED_FOLDER;

		// UH
		if (m_nDrawIcon 		/*== 1*/)	dwStyle |= EMS_DRAW_ICON;
		if (m_nSpaceMin 		/*== 1*/)	dwStyle |= EMS_SPACE_MIN;
		if (m_nDelNoAsk 		/*== 1*/)	dwStyle |= EMS_DEL_NOASK;
		//^^^

		//minit
		if (m_nAdditionalCheck2 /*== 1*/)	dwStyle |= EMS_ADDITIONAL_SAVEFAV_TO;

		if (m_nMenuOrder /*== 1*/ && m_nUserFolder == 0 && m_nOrder /*== 1*/)
			dwStyle |= EMS_CUSTOMIZE_ORDER;

		if (m_nAdditionalResetOrder /*== 1*/)
			dwStyle |= EMS_ADDITIONAL_RESETORDER;

		CFavoritesMenuOption::SetStyle(dwStyle);
		CFavoritesMenuOption::SetMaxMenuItemTextLength(m_nMaxMenuItemTextLength);
		CFavoritesMenuOption::SetMaxMenuBreakCount(m_nMaxMenuBreakCount);
		CFavoritesMenuOption::SetUserDirectory(m_strFolder);
		CFavoritesMenuOption::CallBack();

		return TRUE;
	} else {
		return FALSE;
	}
}



CString CDonutFavoritesMenuPropertyPage::_BrowseForFolder()
{
	TCHAR			szDisplayName[MAX_PATH];
	::ZeroMemory(szDisplayName, sizeof szDisplayName);

	BROWSEINFO		bi = {
		m_hWnd, 			  NULL, szDisplayName, _T("ユーザー定義のお気に入りフォルダ"),
		BIF_RETURNONLYFSDIRS, NULL, 0,				0,
	};
	CItemIDList 	idl;

	idl.Attach( ::SHBrowseForFolder(&bi) );
	return idl.GetPath();
}
