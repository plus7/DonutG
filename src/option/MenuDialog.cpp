/**
 *	@file	MenuOption.cpp
 *	@brief	donutのオプション : メニュー
 */

#include "stdafx.h"
#include "../MtlCtrlw.h"		//+++
#include "MenuDialog.h"
#include "MainOption.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../ToolTipManager.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace MTL;



CMenuPropertyPage::CMenuPropertyPage(HMENU hMenu, CCommandBarCtrl2& rCmdBar)
	: m_rCmdBar(rCmdBar)
	, m_bFirst(TRUE)
	, m_hMenu(hMenu)
	, m_nREqualL(0)
	, m_nNoButton(0)
	, m_nMenuBarStyle(0)
{
}



CMenuPropertyPage::~CMenuPropertyPage()
{
	int ii;

	for (ii = 0; ii < m_mapFront.GetSize(); ii++) {
		CSimpleArray<int>*pAry = m_mapFront.GetValueAt(ii);
		delete pAry;
	}

}



// Overrides
BOOL CMenuPropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_cmbTar.m_hWnd == NULL) {
		m_strPath	  = _GetFilePath( _T("Menu.ini") );
		InitialCmbbox();
		InitialListbox();

		{
			CIniFileI	pr( g_szIniFileName, _T("ETC") );
			m_nMenuBarStyle = (int)pr.GetValue( _T("MenuBarStyle"), 0 );	//+++ メニューバーの項目の表記(0:日本語 1:1文字 2:英語)
		}

		SetAddMenu( m_cmbTar.GetItemData(0) );

		{
			CIniFileI	pr( m_strPath, _T("Option") );
			m_nREqualL	= pr.GetValue(_T("REqualL"), 0 );
			m_nNoButton = pr.GetValue( _T("NoButton"), 0 );
		}
	}

	return DoDataExchange(FALSE);
}



BOOL CMenuPropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CMenuPropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_SaveData();
		return TRUE;
	} else
		return FALSE;
}



// データを保存
void CMenuPropertyPage::_SaveData()
{

	{
		CIniFileO	  pr( g_szIniFileName, _T("ETC") );
		//+++ メニューバーの項目の表記(0:日本語 1:1文字 2:英語)
		pr.SetValue( m_nMenuBarStyle, _T("MenuBarStyle") );
	}
	m_rCmdBar.setMenuBarStyle();		//+++ MenuBarStyleの反映

	CString 		  strKey;

	// IE Menu カスタム
	static const long nMenuType[] = { CONTEXT_MENU_DEFAULT, CONTEXT_MENU_TEXTSELECT, CONTEXT_MENU_ANCHOR, CONTEXT_MENU_IMAGE, 0xFFFFFFFF };

	CIniFileO	  pr( m_strPath, _T("Type0") );
	for (int ii = 0; ii < ( sizeof (nMenuType) / sizeof (long) ); ii++) {
		if (nMenuType[ii] == 0xFFFFFFFF) {
			//左ボタンを押しながら右クリックメニューだけ別処理
			_SaveExData( m_mapFront.Lookup(nMenuType[ii]) );
			continue;
		}

		strKey.Format(_T("Type%d"), nMenuType[ii]);
		pr.ChangeSectionName(strKey);

		CSimpleArray<int>*		pAryFront = m_mapFront.Lookup(nMenuType[ii]);
		pr.SetValue( pAryFront->GetSize(), _T("FrontCount") );

		for (int jj = 0; jj < pAryFront->GetSize(); jj++) {
			strKey.Format(_T("Front%02d"), jj);
			pr.SetValue( (*pAryFront)[jj], strKey );
		}

	}

	pr.ChangeSectionName(_T("Option"));
	DWORD			  dwFlag = m_nREqualL != 0; 	//+++ ? 1 : 0;
	pr.SetValue( dwFlag, _T("REqualL") );
	dwFlag = m_nNoButton != 0;						//+++ ? 1 : 0;
	pr.SetValue( dwFlag, _T("NoButton") );
}



void CMenuPropertyPage::_SaveExData(CSimpleArray<int>*pAryEx)
{
	CIniFileO	pr( m_strPath, _T("MenuEx") );
	pr.SetValue( pAryEx->GetSize(), _T("Count") );
	CString 	strKey;

	int 		nCount = pAryEx->GetSize();

	for (int ii = 0; ii < nCount; ii++) {
		strKey.Format(_T("%02d"), ii);
		pr.SetValue( (DWORD) (*pAryEx)[ii], strKey );
	}
}



void CMenuPropertyPage::InitialListbox()
{
	m_ltFront.Attach( GetDlgItem(IDC_LIST1) );

	CString 	strKey;
	DWORD		dwCount = 0;

	for (int ii = 0; ii < m_cmbTar.GetCount(); ii++) {
		CSimpleArray<int>*pAryFront = new CSimpleArray<int>;

		int 			  nKind 	= int( m_cmbTar.GetItemData(ii) );
		m_mapFront.Add(nKind, pAryFront);

		if (nKind == 0xFFFFFFFF) {
			_LoadExData(pAryFront);
			continue;
		}

		strKey.Format(_T("Type%d"), nKind);
		CIniFileI	pr(m_strPath, strKey);
		pr.QueryValue( dwCount, _T("FrontCount") );

		for (int jj = 0; jj < (int) dwCount; jj++) {
			strKey.Format(_T("Front%02d"), jj);
			DWORD dwCmd = 0;
			pr.QueryValue(dwCmd, strKey);
			int   nCmd	= dwCmd;
			pAryFront->Add(nCmd);
		}
	}
}



void CMenuPropertyPage::_LoadExData(CSimpleArray<int> *pAryEx)
{
	DWORD		dwCount = 0;
	CIniFileI	pr( m_strPath, _T("MenuEx") );
	pr.QueryValue( dwCount, _T("Count") );
	CString 	strKey;

	for (int ii = 0; ii < (int) dwCount; ii++) {
		strKey.Format(_T("%02d"), ii);
		DWORD	dwCmd = 0;
		pr.QueryValue(dwCmd, strKey);
		int 	nCmd  = dwCmd;
		pAryEx->Add(nCmd);
	}
}



void CMenuPropertyPage::InitialCmbbox()
{
	m_cmbTar.Attach 	( GetDlgItem(IDC_CMB_TAR)		);
	m_cmbCategory.Attach( GetDlgItem(IDC_CMB_CATEGORY)	);
	m_cmbCommand.Attach ( GetDlgItem(IDC_CMB_COMMAND)	);

	m_cmbTar.AddString(_T("通常時"));
	m_cmbTar.AddString(_T("選択時"));
	m_cmbTar.AddString(_T("リンク上時"));
	m_cmbTar.AddString(_T("画像上時"));
	m_cmbTar.AddString(_T("左ボタン押しながら"));
	m_cmbTar.SetItemData(0, CONTEXT_MENU_DEFAULT);
	m_cmbTar.SetItemData(1, CONTEXT_MENU_TEXTSELECT);
	m_cmbTar.SetItemData(2, CONTEXT_MENU_ANCHOR);
	m_cmbTar.SetItemData(3, CONTEXT_MENU_IMAGE);
	m_cmbTar.SetItemData(4, 0xFFFFFFFF);			//minit ちょっと汚いですが

	m_cmbTar.SetCurSel(0);

	// カテゴリセット
	_SetCombboxCategory(m_cmbCategory, m_hMenu);
	OnSelChangeCate(0, IDC_CMB_CATEGORY, 0);
	m_cmbCategory.AddString( _T("グループ・メニュー") );
}



void CMenuPropertyPage::OnSelChangeCate(UINT code, int id, HWND hWnd)
{
	if (id == IDC_CMB_CATEGORY) {
		int nIndex = m_cmbCategory.GetCurSel();

		// コマンド選択
		if ( (nIndex + 1) != m_cmbCategory.GetCount() )
			_PickUpCommand(m_hMenu, nIndex, m_cmbCommand);
		else
			PickUpCommandEx(m_cmbCommand);

		::EnableWindow(GetDlgItem(IDC_BTN_ADD1), FALSE);
		::EnableWindow(GetDlgItem(IDC_BTN_ADD2), FALSE);
		::EnableWindow(GetDlgItem(IDC_BTN_DEL1), FALSE);
		::EnableWindow(GetDlgItem(IDC_BTN_DEL2), FALSE);
	} else if (id == IDC_CMB_CATEGORY2) {
		int nIndex = m_cmbCategory2.GetCurSel();

		// コマンド選択
		if ( (nIndex + 1) != m_cmbCategory2.GetCount() )
			_PickUpCommand(m_hMenu, nIndex, m_cmbCommand2);
		else
			PickUpCommandEx(m_cmbCommand2);

		::EnableWindow(GetDlgItem(IDC_BTN_ADD3), FALSE);
		::EnableWindow(GetDlgItem(IDC_BTN_DEL3), FALSE);
	}
}



// 拡張コマンド
void CMenuPropertyPage::PickUpCommandEx(CComboBox &cmbCmd)
{
	static const UINT uCommandExID[] = {
		ID_FAVORITES_DROPDOWN,
		ID_FAVORITES_GROUP_DROPDOWN,
		ID_SCRIPT,
		ID_DLCTL_CHG_MULTI,
		ID_DLCTL_CHG_SECU,
		ID_VIEW_FONT_SIZE,
		ID_COOKIE,
		ID_HTMLZOOM_MENU,			//+++
		ID_SEARCHENGINE_MENU,		//+++
	};

	cmbCmd.ResetContent();

	for (int ii = 0; ii < sizeof (uCommandExID) / sizeof (UINT); ii++) {
		CString strMenu;
		CToolTipManager::LoadToolTipText(uCommandExID[ii], strMenu);

		if ( strMenu.IsEmpty() )
			continue;

		int 	nIndex = cmbCmd.AddString(strMenu);
		cmbCmd.SetItemData(nIndex, uCommandExID[ii]);
	}
}



void CMenuPropertyPage::OnSelChangeCmd(UINT code, int id, HWND hWnd)
{
	if (id == IDC_CMB_COMMAND) {
		::EnableWindow(GetDlgItem(IDC_BTN_ADD1), TRUE);
		::EnableWindow(GetDlgItem(IDC_BTN_ADD2), TRUE);
	} else if (id == IDC_CMB_COMMAND2) {		//*+++ メモ:ラベルIDC_CMB_COMMAND2を参照してるのはここだけ...ただしIDC_CMB_COMMAND2=IDC_EDIT_SZ_PAIN2なので...
		::EnableWindow(GetDlgItem(IDC_BTN_ADD3), TRUE);
	}
}



void CMenuPropertyPage::OnBtnAdd(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	int 			  nIndexCmd 	= m_cmbCommand.GetCurSel();
	int 			  nCmdID		= int( m_cmbCommand.GetItemData(nIndexCmd) );

	CString 		  strCmd;

	m_cmbCommand.GetLBText(nIndexCmd, strCmd);

	CListBox *		  pListBox		= NULL;

	switch (wID) {
	case IDC_BTN_ADD1:	pListBox = &m_ltFront;		break;
	}

	//x int nIndex =
	pListBox->AddString(strCmd);

	int 			  nType 		= (int) m_cmbTar.GetItemData( m_cmbTar.GetCurSel() );
	CSimpleArray<int>*pAryFrontMenu = m_mapFront.Lookup(nType);
	switch (wID) {
	case IDC_BTN_ADD1:	pAryFrontMenu->Add(nCmdID); break;
	}
}



void CMenuPropertyPage::OnBtnDel(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	CListBox *		  pListBox		= NULL;

	switch (wID) {
	case IDC_BTN_DEL1:	pListBox = &m_ltFront;		break;
	}

	int 			  nIndex		= pListBox->GetCurSel();
	if (nIndex < 0)
		return;

	pListBox->DeleteString(nIndex);

	int 			  nType 		= (int) m_cmbTar.GetItemData( m_cmbTar.GetCurSel() );
	CSimpleArray<int>*pAryFrontMenu = m_mapFront.Lookup(nType);

	switch (wID) {
	case IDC_BTN_DEL1:	pAryFrontMenu->RemoveAt(nIndex);	break;
	}
}



void CMenuPropertyPage::OnListChg(UINT code, int id, HWND hWnd)
{
	int nEnableID = 0;

	switch (id) {
	case IDC_LIST1: nEnableID = IDC_BTN_DEL1;		break;
	case IDC_LIST2: nEnableID = IDC_BTN_DEL2;		break;
	}

	::EnableWindow(GetDlgItem(nEnableID), TRUE);
}



void CMenuPropertyPage::OnSelChangeTarget(UINT code, int id, HWND hWnd)
{
	int nIndex = m_cmbTar.GetCurSel();

	SetAddMenu( m_cmbTar.GetItemData(nIndex) );
}



void CMenuPropertyPage::SetAddMenu(DWORD_PTR nType0)
{
	int 	nType  = int (nType0);

	CSimpleArray<int>*	pAryFrontMenu	= m_mapFront.Lookup(nType);

	m_ltFront.ResetContent();

	CSimpleArray<int>*	pAryMenu		= NULL;
	CListBox *			pListBox		= NULL;

	for (int ii = 0; ii < 1; ii++) {
		if (ii == 0) {
			pAryMenu = pAryFrontMenu;
			pListBox = &m_ltFront;
		}

		for (int jj = 0; jj < pAryMenu->GetSize(); jj++) {
			int 	nCmdID = (*pAryMenu)[jj];
			CString strCmd;

			if (nCmdID == 0)
				strCmd = g_cSeparater;
			else
				CToolTipManager::LoadToolTipText(nCmdID, strCmd);

			//x 		int nIndex =
			pListBox->AddString(strCmd);
		}
	}
}



void CMenuPropertyPage::GetTargetMenuaryAndListbox(int nID, CSimpleArray<int>* &pAryMenu, CListBox * &pListBox)
{
	int nType = (int) m_cmbTar.GetItemData( m_cmbTar.GetCurSel() );

	switch (nID) {
	case IDC_BTN_UP1:
	case IDC_BTN_DOWN1:
		pAryMenu = m_mapFront.Lookup(nType);
		pListBox = &m_ltFront;
		break;

	}
}



void CMenuPropertyPage::OnBtnUp(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	CSimpleArray<int>*	pAryMenu = NULL;
	CListBox *			pListBox = NULL;
	GetTargetMenuaryAndListbox(wID, pAryMenu, pListBox);

	int 	nIndex	 = pListBox->GetCurSel();
	if (nIndex < 1)
		return;

	CString 	strKeep;
	pListBox->GetText(nIndex - 1, strKeep);
	pListBox->DeleteString(nIndex - 1);
	pListBox->InsertString(nIndex, strKeep);

	int 	nTemp			= (*pAryMenu)[nIndex - 1];
	(*pAryMenu)[nIndex - 1] = (*pAryMenu)[nIndex];
	(*pAryMenu)[nIndex] 	= nTemp;
}



void CMenuPropertyPage::OnBtnDown(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	CSimpleArray<int>*pAryMenu = NULL;
	CListBox *		  pListBox = NULL;
	GetTargetMenuaryAndListbox(wID, pAryMenu, pListBox);

	int 	nIndex	 = pListBox->GetCurSel();
	if (nIndex < 0)
		return;

	if ( nIndex >= (pListBox->GetCount() - 1) )
		return;

	CString 	strKeep;
	pListBox->GetText(nIndex + 1, strKeep);
	pListBox->DeleteString(nIndex + 1);
	pListBox->InsertString(nIndex, strKeep);

	int 	nTemp			= (*pAryMenu)[nIndex + 1];
	(*pAryMenu)[nIndex + 1] = (*pAryMenu)[nIndex];
	(*pAryMenu)[nIndex] 	= nTemp;
}
