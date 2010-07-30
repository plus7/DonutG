/**
 *	@file	MenuOption.h
 *	@brief	donutのオプション : メニュー
 */

#pragma once

#include "../resource.h"

namespace MTL { class CCommandBarCtrl2; }


class CMenuPropertyPage
	: public CPropertyPageImpl<CMenuPropertyPage>
	, public CWinDataExchange<CMenuPropertyPage>
{
public:
	// Constants
	enum { IDD = IDD_PROPPAGE_MENU };

private:
	// Data members
	HMENU								m_hMenu;
	CComboBox							m_cmbTar;
	CComboBox							m_cmbCategory;
	CComboBox							m_cmbCommand;

	CComboBox							m_cmbCategory2;
	CComboBox							m_cmbCommand2;

	CListBox							m_ltFront;


	CSimpleMap<int, CSimpleArray<int>*> m_mapFront;


	BOOL								m_bFirst;
	CString 							m_strPath;



	int 								m_nREqualL;
	int 								m_nNoButton;

	int 								m_nMenuBarStyle;		//+++

	MTL::CCommandBarCtrl2&				m_rCmdBar;				//+++ メニューバー関係用

public:
	// DDX map
	BEGIN_DDX_MAP( CMenuPropertyPage )
		DDX_CHECK( IDC_CHECK_R_EQUAL_L		, m_nREqualL	)
		DDX_CHECK( IDC_CHECK_NOBUTTON		, m_nNoButton	)
		DDX_RADIO( IDC_MENU_BAR_STYLE_JAPAN , m_nMenuBarStyle)
	END_DDX_MAP()

	// Constructor

	CMenuPropertyPage(HMENU hMenu, CCommandBarCtrl2& CmdBar);

	~CMenuPropertyPage();

	// Overrides
	BOOL	OnSetActive();
	BOOL	OnKillActive();
	BOOL	OnApply();


	// Message map and handlers
	BEGIN_MSG_MAP( CMenuPropertyPage )
		CHAIN_MSG_MAP( CPropertyPageImpl<CMenuPropertyPage> )
		COMMAND_HANDLER_EX( IDC_CMB_CATEGORY, CBN_SELCHANGE, OnSelChangeCate	)
		COMMAND_HANDLER_EX( IDC_CMB_COMMAND , CBN_SELCHANGE, OnSelChangeCmd 	)
		COMMAND_HANDLER_EX( IDC_CMB_TAR 	, CBN_SELCHANGE, OnSelChangeTarget	)

		COMMAND_ID_HANDLER_EX( IDC_BTN_ADD1 , OnBtnAdd		)

		COMMAND_HANDLER_EX	 ( IDC_LIST1	, LBN_SELCHANGE, OnListChg )

		COMMAND_ID_HANDLER_EX( IDC_BTN_UP1	, OnBtnUp		)

		COMMAND_ID_HANDLER_EX( IDC_BTN_DOWN1, OnBtnDown 	)

		COMMAND_ID_HANDLER_EX( IDC_BTN_DEL1,  OnBtnDel 	)


	END_MSG_MAP()


private:
	void	OnSelChangeCate(UINT code, int id, HWND hWnd);

	void	OnSelChangeCmd(UINT code, int id, HWND hWnd);

	void	OnBtnAdd(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/);
	void	OnBtnDel(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/);
	void	OnListChg(UINT code, int id, HWND hWnd);

	void	OnSelChangeTarget(UINT code, int id, HWND hWnd);

	void	OnBtnUp(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/);
	void	OnBtnDown(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/);

	// データを保存
	void	_SaveData();
	void	_SaveExData(CSimpleArray<int>*pAryEx);
	void	InitialListbox();
	void	_LoadExData(CSimpleArray<int> *pAryEx);
	void	InitialCmbbox();

	// 拡張コマンド
	void	PickUpCommandEx(CComboBox &cmbCmd);

	void	SetAddMenu(DWORD_PTR /*int*/ nType);
	void	GetTargetMenuaryAndListbox(int nID, CSimpleArray<int>* &pAryMenu, CListBox * &pListBox);

};

