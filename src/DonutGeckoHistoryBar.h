#pragma once

#include "resource.h"
#include "Mtl.h"
#include "MtlCtrls.h"
#include "Donut.h"

template <class T>
class CPlacesTreeViewCtrlImpl : public CWindowImpl<T, CTreeViewCtrl> {
public:
	DECLARE_WND_SUPERCLASS( NULL, CTreeViewCtrl::GetWndClassName() )
};

class CDonutGeckoHistoryBar
	: public CDialogImpl<CDonutGeckoHistoryBar>
	, public CDialogResize<CDonutGeckoHistoryBar>
	, public CWinDataExchange<CDonutGeckoHistoryBar>
{
public:
	enum { IDD = IDD_GECKOHISTORY_FORM };

private:

	// Data members

	// Windows

public:
	// Ctor
	CDonutGeckoHistoryBar();

	// Overrides

	// Methods

	BYTE	PreTranslateMessage(MSG *pMsg);

public:
	// Message map and handlers
	BEGIN_MSG_MAP(CDonutGeckoHistoryBar)
		MESSAGE_HANDLER(WM_INITDIALOG				, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY					, OnDestroy)

		CHAIN_MSG_MAP( CDialogResize<CDonutGeckoHistoryBar>	)
	END_MSG_MAP()

public:
	// Dlg resize map
	BEGIN_DLGRESIZE_MAP( CDonutGeckoHistoryBar )
	END_DLGRESIZE_MAP()

	// DDX map
	BEGIN_DDX_MAP( CDonutGeckoHistoryBar )
	END_DDX_MAP()

private:
	LRESULT 	OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT 	OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);

	// Implementation
	void	_GetData();
	void	_SetData();
	void	_GetProfile();
	void	_WriteProfile();

	void	_OnItemOpen(int nIndex);

};