/**
 *	@file	MouseDialog.cpp
 *	@brief	donutのオプション : マウス
 */

#include "stdafx.h"
#include "MouseDialog.h"
#include "../IniFile.h"
#include "../DonutPFunc.h"
#include "../ToolTipManager.h"
#include "../dialog/CommandSelectDialog.h"


#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace MTL;



CMousePropertyPage::CMousePropertyPage(HMENU hMenu)
{
	m_hMenu   = hMenu;
	m_strPath = _GetFilePath( _T("MouseEdit.ini") );
}



// Overrides
BOOL CMousePropertyPage::OnSetActive()
{
	SetModified(TRUE);

	if (m_cmbLinkM.m_hWnd == NULL) {
		OnInitSetting();
		OnInitCmb();
		SetCmdString();
	}

	_SetData();
	return DoDataExchange(FALSE);
}



BOOL CMousePropertyPage::OnKillActive()
{
	return DoDataExchange(TRUE);
}



BOOL CMousePropertyPage::OnApply()
{
	if ( DoDataExchange(TRUE) ) {
		_GetData();
		return TRUE;
	} else
		return FALSE;
}



// Constructor
// データを得る
void CMousePropertyPage::_SetData()
{
}



// データを保存
void CMousePropertyPage::_GetData()
{
	CIniFileO	pr( m_strPath, _T("MouseCtrl") );

	for (int ii = 0; ii < m_mapMouseCmd.GetSize(); ii++) {
		CString strKey	  = m_mapMouseCmd.GetKeyAt(ii);
		DWORD	dwCommand = m_mapMouseCmd.GetValueAt(ii);

		pr.SetValue(dwCommand, strKey);
	}

	DWORD		dwCommandD = DWORD( m_cmbDragDrop.GetItemData( m_cmbDragDrop.GetCurSel() ) );
	pr.SetValue( dwCommandD, _T("DragDrop") );

	//x pr.Close(); 	//+++
}



void CMousePropertyPage::OnInitSetting()
{
	static LPCTSTR lpKeyStr[] = {
		_T("WHEEL_UP"),		_T("WHEEL_DOWN"),
		_T("LButtonUp"),	_T("MButtonUp"),
		_T("XButtonUp1"),	_T("XButtonUp2"),
		_T("LinkOpenBtnM"),
		_T("Side1"),		_T("Side2")
	};
	int 		nKeyCnt   = 9;

	CIniFileI	pr( m_strPath, _T("MouseCtrl") );

	for (int ii = 0; ii < nKeyCnt; ii++) {
		CString		strKey;
		strKey.Format(_T("%s"), lpKeyStr[ii]);

		DWORD	dwCommand = 0;
		pr.QueryValue(dwCommand, strKey);

		m_mapMouseCmd.Add(strKey, dwCommand);
	}
}



void CMousePropertyPage::SetCmdString()
{
	static const DWORD dwWndID[] = {
		IDC_SETBTN_RL,	  IDC_SETBTN_RM,
		IDC_SETBTN_RX1,   IDC_SETBTN_RX2,
		IDC_SETBTN_RW_UP, IDC_SETBTN_RW_DOWN,
		IDC_SETBTN_SIDE1, IDC_SETBTN_SIDE2
	};

	int 			   ii;

	for (ii = 0; ii < ( sizeof (dwWndID) / sizeof (DWORD) ); ii++) {
		CString strTar	  = GetTarString(dwWndID[ii]);
		DWORD	dwCommand = m_mapMouseCmd.Lookup(strTar);

		CString strCmdName;

		if (dwCommand != 0)
			CToolTipManager::LoadToolTipText(dwCommand, strCmdName);
		else
			strCmdName = _T("デフォルト");

		DWORD	dwStatic  = GetTarStaticID(dwWndID[ii]);
		::SetWindowText(::GetDlgItem(m_hWnd, dwStatic), strCmdName);
	}

	CString 		   strTar	 = GetTarString(IDC_CMB_LINK_M);
	DWORD			   dwCommand = m_mapMouseCmd.Lookup(strTar);

	for (ii = 0; ii < m_cmbLinkM.GetCount(); ii++) {
		DWORD_PTR dwVal = m_cmbLinkM.GetItemData(ii);

		if (dwVal != dwCommand)
			continue;

		m_cmbLinkM.SetCurSel(ii);
		break;
	}
}



void CMousePropertyPage::OnInitCmb()
{
	CMenu menuSel;

	menuSel.LoadMenu(IDR_MOUSE_EX_SEL);

	m_cmbLinkM.Attach( GetDlgItem(IDC_CMB_LINK_M) );
	AddMenuItem(menuSel, m_cmbLinkM);

	m_cmbDragDrop.Attach( GetDlgItem(IDC_COMBO_DRAGDROP) );
	AddDragDropItem(m_cmbDragDrop);
}



void CMousePropertyPage::AddMenuItem(CMenu &menu, CComboBox &cmb)
{
	cmb.SetDroppedWidth(250);
	int nIndex = cmb.AddString( _T("デフォルト") );
	cmb.SetItemData(nIndex, 0);

	for (int ii = 0; ii < menu.GetMenuItemCount(); ii++) {
		DWORD	dwCommand = menu.GetMenuItemID(ii);

		if (dwCommand == 0)
			continue;

		CString strMenu;
		menu.GetMenuString(ii, strMenu, MF_BYPOSITION);

		nIndex = cmb.AddString(strMenu);
		cmb.SetItemData(nIndex, dwCommand);
	}
}



void CMousePropertyPage::AddDragDropItem(CComboBox &cmb)
{
	struct _CmbItemData {
		int 		nCommand;
		LPCTSTR 	strDesc;
	};
	static const _CmbItemData	ccd[]	= {
		{ 0 				, _T("デフォルト")	  },
		{ ID_SEARCH_DIRECT	, _T("即検索")		  },
		{ ID_OPENLINK_DIRECT, _T("リンクを開く")  },
	};

	CIniFileI	pr( m_strPath, _T("MouseCtrl") );
	DWORD		dwCommand = pr.GetValue( _T("DragDrop"), ID_SEARCH_DIRECT /*0*/ );		//+++ デフォルト変更.
	pr.Close();

	int 		nCommand	= dwCommand;
	int 		nIndex		= 0;
	int 		nCount		= 3;

	for (int i = 0; i < nCount; i++) {
		nIndex = cmb.AddString(ccd[i].strDesc);
		cmb.SetItemData(nIndex, ccd[i].nCommand);

		if (ccd[i].nCommand == nCommand)
			cmb.SetCurSel(nIndex);
	}
}



LRESULT CMousePropertyPage::OnSetStatic(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/) // UDT DGSTR
{
	switch (wID) {
	case IDC_STC_RL:		OnSetBtn(0, IDC_SETBTN_RL		, 0);	break;
	case IDC_STC_RM:		OnSetBtn(0, IDC_SETBTN_RM		, 0);	break;
	case IDC_STC_RX1:		OnSetBtn(0, IDC_SETBTN_RX1		, 0);	break;
	case IDC_STC_RX2:		OnSetBtn(0, IDC_SETBTN_RX2		, 0);	break;
	case IDC_STC_RW_UP: 	OnSetBtn(0, IDC_SETBTN_RW_UP	, 0);	break;
	case IDC_STC_RW_DOWN:	OnSetBtn(0, IDC_SETBTN_RW_DOWN	, 0);	break;
	case IDC_STC_SIDE1: 	OnSetBtn(0, IDC_SETBTN_SIDE1	, 0);	break;
	case IDC_STC_SIDE2: 	OnSetBtn(0, IDC_SETBTN_SIDE2	, 0);	break;
	}
	return 0;
}



void CMousePropertyPage::OnSetBtn(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	CCommandSelectDialog dlg(m_hMenu);

	if (dlg.DoModal() != IDOK)
		return;

	UINT				 nCommand  = (UINT) dlg.GetCommandID();
	CString 			 strTar    = GetTarString(wID);

	m_mapMouseCmd.SetAt(strTar, nCommand);

	CString 			 strCmdName;

	if (nCommand != 0)
		CToolTipManager::LoadToolTipText(nCommand, strCmdName);
	else
		strCmdName = _T("デフォルト");

	DWORD				 dwStatic  = GetTarStaticID(wID);
	::SetWindowText(::GetDlgItem(m_hWnd, dwStatic), strCmdName);
}



CString CMousePropertyPage::GetTarString(int id)
{
	CString strTar;

	switch (id) {
	case IDC_SETBTN_RL: 		strTar = _T("LButtonUp");		break;
	case IDC_SETBTN_RM: 		strTar = _T("MButtonUp");		break;
	case IDC_SETBTN_RW_UP:		strTar = _T("WHEEL_UP");		break;
	case IDC_SETBTN_RW_DOWN:	strTar = _T("WHEEL_DOWN");		break;
	case IDC_SETBTN_RX1:		strTar = _T("XButtonUp1");		break;
	case IDC_SETBTN_RX2:		strTar = _T("XButtonUp2");		break;
	case IDC_SETBTN_SIDE1:		strTar = _T("Side1");			break;
	case IDC_SETBTN_SIDE2:		strTar = _T("Side2");			break;
	case IDC_CMB_LINK_M:		strTar = _T("LinkOpenBtnM");	break;
	}

	return strTar;
}



DWORD CMousePropertyPage::GetTarStaticID(int id)
{
	DWORD dwWndID = 0; //value should initialized : minit

	switch (id) {
	case IDC_SETBTN_RL: 		dwWndID = IDC_STC_RL;		break;
	case IDC_SETBTN_RM: 		dwWndID = IDC_STC_RM;		break;
	case IDC_SETBTN_RX1:		dwWndID = IDC_STC_RX1;		break;
	case IDC_SETBTN_RX2:		dwWndID = IDC_STC_RX2;		break;
	case IDC_SETBTN_RW_UP:		dwWndID = IDC_STC_RW_UP;	break;
	case IDC_SETBTN_RW_DOWN:	dwWndID = IDC_STC_RW_DOWN;	break;
	case IDC_SETBTN_SIDE1:		dwWndID = IDC_STC_SIDE1;	break;
	case IDC_SETBTN_SIDE2:		dwWndID = IDC_STC_SIDE2;	break;
	}

	return dwWndID;
}



void CMousePropertyPage::OnSelChange(UINT code, int id, HWND hWnd)
{
	CString 	strTar		= GetTarString(id);

	CComboBox	cmb(hWnd);
	int 		nIndex		= cmb.GetCurSel();
	DWORD		dwCommand	= DWORD( cmb.GetItemData(nIndex) );
	m_mapMouseCmd.SetAt(strTar, dwCommand);
}
