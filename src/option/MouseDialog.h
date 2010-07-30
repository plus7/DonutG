/**
 *	@file	MouseDialog.h
 *	@brief	donutのオプション : マウス
 */

#pragma once

#include "../resource.h"



class CMousePropertyPage
	: public CPropertyPageImpl<CMousePropertyPage>
	, public CWinDataExchange<CMousePropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MOUSE };

private:
	// Data members
	CComboBox					m_cmbLinkM;
	CComboBox					m_cmbDragDrop;
	CSimpleMap<CString, DWORD>	m_mapMouseCmd;
	HMENU						m_hMenu;

	CString 					m_strPath;

public:
	// DDX map
	BEGIN_DDX_MAP( CMousePropertyPage )
	END_DDX_MAP()

	// Constructor
	CMousePropertyPage(HMENU hMenu);

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();

private:
	void	OnInitSetting();
	void	OnInitCmb();

	// データを得る
	void	_SetData();

	// データを保存
	void	_GetData();

	void	SetCmdString();
	void	AddMenuItem(CMenu &menu, CComboBox &cmb);
	void	AddDragDropItem(CComboBox &cmb);

public:
	// Message map and handlers
	BEGIN_MSG_MAP( CMousePropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMousePropertyPage> )

		COMMAND_HANDLER_EX( IDC_CMB_LINK_M, CBN_SELCHANGE, OnSelChange )

		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RL		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RM		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RX1		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RX2		, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RW_UP 	, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_RW_DOWN	, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_SIDE1 	, OnSetBtn )
		COMMAND_ID_HANDLER_EX( IDC_SETBTN_SIDE2 	, OnSetBtn )

		COMMAND_ID_HANDLER( IDC_STC_RL			, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RM			, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RX1 		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RX2 		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RW_UP		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_RW_DOWN 	, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_SIDE1		, OnSetStatic )
		COMMAND_ID_HANDLER( IDC_STC_SIDE2		, OnSetStatic )
	END_MSG_MAP()

private:
	LRESULT OnSetStatic(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled); // UDT DGSTR

	void	OnSetBtn(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/);
	void	OnSelChange(UINT code, int id, HWND hWnd);

	CString GetTarString(int id);
	DWORD	GetTarStaticID(int id);
};

