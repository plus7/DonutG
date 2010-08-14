#pragma once

#include "resource.h"
#include "Mtl.h"
#include "MtlCtrls.h"
#include "Donut.h"
#include <nsINavHistoryService.h>

template <class T>
class CPlacesTreeViewCtrlImpl : 
	public CWindowImpl<T, CTreeViewCtrl>, 
	public nsINavHistoryResultViewer 
{
public:
	enum SortingType {
		FIRST_VISIT, LAST_VISIT, DATE_AND_SITE, SITE, DATE
	};
	DECLARE_WND_SUPERCLASS( NULL, CTreeViewCtrl::GetWndClassName() )

	CPlacesTreeViewCtrlImpl()
		:m_sortingType(DATE)
	{
		m_historySvc = do_GetService("@mozilla.org/browser/nav-history-service;1");
		NS_ADDREF(this);
	}
	~CPlacesTreeViewCtrlImpl()
	{
		m_result->SetViewer(NULL);
	}
public:
	// Message map and handlers
	BEGIN_MSG_MAP(CPlacesTreeViewCtrlImpl)
		MESSAGE_HANDLER (	WM_CREATE		, OnCreate			)
	END_MSG_MAP()

	NS_DECL_ISUPPORTS
	NS_DECL_NSINAVHISTORYRESULTVIEWER


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		DoSearch(_T(""));
		bHandled = FALSE;
		return 1;
	}

	void MakeChild(HTREEITEM parent, nsINavHistoryContainerResultNode * parentNode){
		PRUint32 cnt;
		parentNode->GetChildCount(&cnt);
		for(unsigned int i = 0;i<cnt;i++){
			nsCOMPtr<nsINavHistoryResultNode> child;
			parentNode->GetChild(i, getter_AddRefs(child));
			nsEmbedCString title;
			child->GetTitle(title);
			HTREEITEM item = 
				InsertItem(NS_ConvertUTF8toUTF16(title).get(), parent, TVI_LAST);
			nsCOMPtr<nsINavHistoryContainerResultNode> container = do_QueryInterface(child);
			if(container){ 
				container->SetContainerOpen(PR_TRUE);
				MakeChild(item, container);
			}
		}
	}

	void DoSearch(const CString& str){
		nsCOMPtr<nsINavHistoryQuery> query;
		nsCOMPtr<nsINavHistoryQueryOptions> options;
		m_historySvc->GetNewQuery(getter_AddRefs(query));
		m_historySvc->GetNewQueryOptions(getter_AddRefs(options));
		options->SetResultType(nsINavHistoryQueryOptions::RESULTS_AS_DATE_SITE_QUERY);//RESULTS_AS_DATE_QUERY);
		m_historySvc->ExecuteQuery(query, options, getter_AddRefs(m_result));
		m_result->SetViewer(this);
		nsCOMPtr<nsINavHistoryContainerResultNode> root;
		m_result->GetRoot(getter_AddRefs(root));
		root->SetContainerOpen(PR_TRUE);
		MakeChild(TVI_ROOT, root);
	}

	void SetSortingType(int type){
		m_sortingType = type;
	}

	int GetSortingType(){
		return m_sortingType;
	}
private:
	nsCOMPtr<nsINavHistoryService> m_historySvc;
	nsCOMPtr<nsINavHistoryResult> m_result;
	int m_sortingType;
	//CString m_keyword;
};

class CPlacesTreeViewCtrl : public CPlacesTreeViewCtrlImpl<CPlacesTreeViewCtrl> {
public:
	DECLARE_WND_SUPERCLASS( _T("PlacesTreeViewCtrl"), GetWndClassName() )
};

#define PLACESTREEVIEW_CTOR()	 \
	template <class T>  \
	CPlacesTreeViewCtrlImpl<T>

#define PLACESTREEVIEW(rettype) \
	template <class T>  \
	rettype CPlacesTreeViewCtrlImpl<T>


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
		DLGRESIZE_CONTROL( IDC_PLACES_TREEVIEW,	DLSZ_SIZE_X | DLSZ_SIZE_Y )
	END_DLGRESIZE_MAP()

	// DDX map
	BEGIN_DDX_MAP( CDonutGeckoHistoryBar )
	END_DDX_MAP()

private:
	LRESULT 	OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT 	OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);

	CPlacesTreeViewCtrl m_treeview;

	// Implementation
	void	_GetData();
	void	_SetData();
	void	_GetProfile();
	void	_WriteProfile();

	void	_OnItemOpen(int nIndex);

};