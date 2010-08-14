/**
 *	@file	DonutGeckoHistoryBar.cpp
 *	@brief	Places‚Ì—š—ð
 */

#include "stdafx.h"
#include "DonutGeckoHistoryBar.h"
#include "Donut.h"
#include "DonutPFunc.h"
#include "option/MainOption.h"

template <class T>  \
NS_IMPL_ADDREF(CPlacesTreeViewCtrlImpl<T>)
template <class T>  \
NS_IMPL_RELEASE(CPlacesTreeViewCtrlImpl<T>)
template <class T>  \
NS_IMPL_QUERY_INTERFACE1(CPlacesTreeViewCtrlImpl<T>, nsINavHistoryResultViewer)

/* void nodeInserted (in nsINavHistoryContainerResultNode aParent, in nsINavHistoryResultNode aNode, in unsigned long aNewIndex); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeInserted(nsINavHistoryContainerResultNode *aParent, nsINavHistoryResultNode *aNode, PRUint32 aNewIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeRemoved (in nsINavHistoryContainerResultNode aParent, in nsINavHistoryResultNode aItem, in unsigned long aOldIndex); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeRemoved(nsINavHistoryContainerResultNode *aParent, nsINavHistoryResultNode *aItem, PRUint32 aOldIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeMoved (in nsINavHistoryResultNode aNode, in nsINavHistoryContainerResultNode aOldParent, in unsigned long aOldIndex, in nsINavHistoryContainerResultNode aNewParent, in unsigned long aNewIndex); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeMoved(nsINavHistoryResultNode *aNode, nsINavHistoryContainerResultNode *aOldParent, PRUint32 aOldIndex, nsINavHistoryContainerResultNode *aNewParent, PRUint32 aNewIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeTitleChanged (in nsINavHistoryResultNode aNode, in AUTF8String aNewTitle); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeTitleChanged(nsINavHistoryResultNode *aNode, const nsACString & aNewTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeURIChanged (in nsINavHistoryResultNode aNode, in AUTF8String aNewURI); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeURIChanged(nsINavHistoryResultNode *aNode, const nsACString & aNewURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeIconChanged (in nsINavHistoryResultNode aNode); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeIconChanged(nsINavHistoryResultNode *aNode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeHistoryDetailsChanged (in nsINavHistoryResultNode aNode, in PRTime aNewVisitDate, in unsigned long aNewAccessCount); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeHistoryDetailsChanged(nsINavHistoryResultNode *aNode, PRTime aNewVisitDate, PRUint32 aNewAccessCount)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeTagsChanged (in nsINavHistoryResultNode aNode); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeTagsChanged(nsINavHistoryResultNode *aNode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeKeywordChanged (in nsINavHistoryResultNode aNode, in AUTF8String aNewKeyword); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeKeywordChanged(nsINavHistoryResultNode *aNode, const nsACString & aNewKeyword)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeAnnotationChanged (in nsINavHistoryResultNode aNode, in AUTF8String aAnnoName); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeAnnotationChanged(nsINavHistoryResultNode *aNode, const nsACString & aAnnoName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeDateAddedChanged (in nsINavHistoryResultNode aNode, in PRTime aNewValue); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeDateAddedChanged(nsINavHistoryResultNode *aNode, PRTime aNewValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeLastModifiedChanged (in nsINavHistoryResultNode aNode, in PRTime aNewValue); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeLastModifiedChanged(nsINavHistoryResultNode *aNode, PRTime aNewValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void nodeReplaced (in nsINavHistoryContainerResultNode aParentNode, in nsINavHistoryResultNode aOldNode, in nsINavHistoryResultNode aNewNode, in unsigned long aIndex); */
PLACESTREEVIEW(NS_IMETHODIMP) ::NodeReplaced(nsINavHistoryContainerResultNode *aParentNode, nsINavHistoryResultNode *aOldNode, nsINavHistoryResultNode *aNewNode, PRUint32 aIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void containerOpened (in nsINavHistoryContainerResultNode aContainerNode); */
PLACESTREEVIEW(NS_IMETHODIMP) ::ContainerOpened(nsINavHistoryContainerResultNode *aContainerNode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void containerClosed (in nsINavHistoryContainerResultNode aContainerNode); */
PLACESTREEVIEW(NS_IMETHODIMP) ::ContainerClosed(nsINavHistoryContainerResultNode *aContainerNode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void invalidateContainer (in nsINavHistoryContainerResultNode aContainerNode); */
PLACESTREEVIEW(NS_IMETHODIMP) ::InvalidateContainer(nsINavHistoryContainerResultNode *aContainerNode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void sortingChanged (in unsigned short sortingMode); */
PLACESTREEVIEW(NS_IMETHODIMP) ::SortingChanged(PRUint16 sortingMode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsINavHistoryResult result; */
PLACESTREEVIEW(NS_IMETHODIMP) ::GetResult(nsINavHistoryResult * *aResult)
{
	*aResult = m_result;
    return NS_OK;
}
PLACESTREEVIEW(NS_IMETHODIMP) ::SetResult(nsINavHistoryResult * aResult)
{
	m_result = aResult;
    return NS_OK;
}


// Ctor
CDonutGeckoHistoryBar::CDonutGeckoHistoryBar()
{

}

// Methods

// Methods
BYTE CDonutGeckoHistoryBar::PreTranslateMessage(MSG *pMsg)
{
	return _MTL_TRANSLATE_PASS;
}

LRESULT CDonutGeckoHistoryBar::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{

	m_treeview.SubclassWindow(GetDlgItem(IDC_PLACES_TREEVIEW));
	m_treeview.ModifyStyle(0, TVS_SINGLEEXPAND | TVS_TRACKSELECT | TVS_FULLROWSELECT);
	m_treeview.DoSearch(_T(""));

	DlgResize_Init(false, true, WS_CLIPCHILDREN);

	_GetProfile();
	_SetData();
	DoDataExchange(FALSE);

	return TRUE;
}

LRESULT CDonutGeckoHistoryBar::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	DoDataExchange(TRUE);
	_GetData();
	_WriteProfile();
	return 0;
}

// Implementation
void CDonutGeckoHistoryBar::_GetData()
{
}

void CDonutGeckoHistoryBar::_SetData()
{
}

void CDonutGeckoHistoryBar::_GetProfile()
{
	CIniFileI	pr( MtlGetChangedExtFromModuleName( _T(".ini") ), _T("GeckoHistoryBar") );
	pr.Close();
}

void CDonutGeckoHistoryBar::_WriteProfile()
{
	CIniFileO	pr( MtlGetChangedExtFromModuleName( _T(".ini") ), _T("GeckoHistoryBar") );
	pr.Close();
}

void CDonutGeckoHistoryBar::_OnItemOpen(int nIndex)
{
}

