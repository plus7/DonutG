/**
 *	@file	FavoriteMenu.h
 *	@brief	お気に入りメニュー.
 */

#pragma once

#include "stdafx.h"
#include "ExplorerMenu.h"
#include "FavoritesMenu.h"
#include "dialog/RenameFileDialog.h"



inline BOOL SaveInternetShortcutFile(const CString &strFileTitle, const CString &strPath, const CString &strURL)
{
	CString strFileName 	= strFileTitle;

	if ( CDonutRenameFileDialog::IsContainBadCharacterForName(strFileName) ) {
		MessageBox(NULL, _T("タイトルに名前として使用できない文字が入っています。別の名前をつけてください。"), _T("注意"), MB_OK);
		CDonutRenameFileDialog::ReplaceBadCharacterForName(strFileName);

		CDonutRenameFileDialog	dlg;
		dlg.m_strName = strFileName;

		if ( dlg.DoModal() == IDOK && !dlg.m_strName.IsEmpty() ) {
			strFileName = dlg.m_strName;
		} else {
			return FALSE;
		}
	}

	CString strShortcutPath = strPath + strFileName + _T(".url");

	if (::GetFileAttributes(strShortcutPath) != 0xFFFFFFFF) {
		CString strMsg;
		strMsg.Format(_T("%s\nはすでに存在しています。上書きしますか？"), strShortcutPath);
		int 	nRet = MessageBox(NULL, strMsg, _T("確認"), MB_YESNO);

		if (nRet == IDYES)
			::DeleteFile(strShortcutPath);
		else
			return FALSE;
	}

	MtlCreateInternetShortcutFile(strShortcutPath, strURL);
	return TRUE;
}



// Function object
template <class _MainFrame>
struct __Function_OpenAllFiles : public std::unary_function<const CString &, void> {
private:
	_MainFrame *			__pMainFrame;

public:
	__Function_OpenAllFiles(_MainFrame &_m) : __pMainFrame(&_m) { }

	void	operator ()(const CString &strFileName)
	{
		DonutOpenFile(__pMainFrame->m_hWnd, strFileName);
	}
};



// Helper
template <class _MainFrame>
void _The_OpenAllFiles(const CString &strFilePath, _MainFrame *__pMainFrame)
{
	CString strCheck = strFilePath.Left(2);
	CString strPath  = strFilePath;

	if ( strPath.Mid(1, 1) == _T(" ") )
		strPath = strPath.Mid(2);


	if ( strCheck == _T("s ") ) {
		HWND		 hActiveWnd  = __pMainFrame->MDIGetActive();
		CChildFrame *child		 = __pMainFrame->GetChildFrame(hActiveWnd);

		if (!child)
			return;

		CString 	 strFileName = MtlGetWindowText(hActiveWnd);
		SaveInternetShortcutFile( strFileName, strPath, child->GetLocationURL() );
		return;

	} else if ( strCheck == _T("a ") && MtlIsDirectoryPath(strPath) ) {
		CLockRedrawMDIClient	 lock(__pMainFrame->m_hWndMDIClient);
		CMDITabCtrl::CLockRedraw lock2(__pMainFrame->mdiTab());
		MtlForEachFile( strPath, __Function_OpenAllFiles<_MainFrame>(*__pMainFrame) );

	} else if ( strCheck == _T("r ") && MtlIsDirectoryPath(strPath) ) {
		CFavoriteOrderHandler	order;
		CString 				strRegKey = order.GetOrderKeyName(strPath);

		if ( strRegKey.IsEmpty() )
			return;

		Misc::CRegKey 	rkOrder;

		if (rkOrder.Open(HKEY_CURRENT_USER, strRegKey) != ERROR_SUCCESS) {
			CString strMsg;
			strMsg.Format(_T("レジストリキー\n%s\nのオープンに失敗しました。キーが存在しない可能性があります。"), strRegKey);
			MessageBox(NULL, strMsg, _T("エラー"), MB_OK);
			return;
		}

		rkOrder.DeleteValue( _T("Order") );
		rkOrder.Close();
	} else {
		DonutOpenFile( __pMainFrame->m_hWnd, strPath, DonutGetStdOpenFlag() );
	}
}



template <class _MainFrame>
class	CFavoriteGroupMenu : public CExplorerMenuImpl<CFavoriteGroupMenu<_MainFrame> > {
	_MainFrame*		__m_pMainFrame;

public:
	CFavoriteGroupMenu(_MainFrame *__pMainFrame, int nInsertPointMenuItemID)
		: CExplorerMenuImpl<CFavoriteGroupMenu>(nInsertPointMenuItemID, _T("(なし)"), FAVGROUP_MENU_ID_MIN, FAVGROUP_MENU_ID_MAX)
		, __m_pMainFrame(__pMainFrame)
	{
		RefreshMenu();
	}


	void OnExecute(const CString &strFilePath)
	{
		ATLTRACE2(atlTraceGeneral, 4, _T("CFavoriteMenu::OnExecute: %s\n"), strFilePath);

		DonutOpenFile(__m_pMainFrame->m_hWnd, strFilePath);
	}


	void RefreshMenu()
	{
		// set up option
		SetStyle(0);

		m_nMaxMenuItemTextLength = CFavoritesMenuOption::GetMaxMenuItemTextLength();
		m_nMaxMenuBreakCount	 = CFavoritesMenuOption::GetMaxMenuBreakCount();

		SetRootDirectoryPath( DonutGetFavoriteGroupFolder() );
	}


private:
	//Overrides
	void OnMenuPrepareContext(CItemIDList &idlFolder)
	{
		CString strFolder;

		strFolder = DonutGetFavoriteGroupFolder();
		idlFolder = strFolder;
	}
};



template <class _ChildFrame>
class	CChildFavoriteMenu : public CFavoritesMenu<CChildFavoriteMenu<_ChildFrame> > {
	_ChildFrame *__m_pChildFrame;

public:
	CChildFavoriteMenu(_ChildFrame *__pChildFrame, int nInsertPointMenuItemID)
		: CFavoritesMenu<CChildFavoriteMenu>(nInsertPointMenuItemID, _T("(なし)"), 0x012C, 0x7530)
		, __m_pChildFrame(__pChildFrame)
	{
		RefreshMenu();
	}


	// Overrides
	void OnExecute(const CString &strFilePath)
	{
		ATLTRACE2(atlTraceGeneral, 4, _T("CFavoriteMenu::OnExecute: %s\n"), strFilePath);
		CString strCheck = strFilePath.Left(2);
		CString strFile;

		if (strFilePath[1] == ' ')
			strFile = strFilePath.Mid(2);
		else
			strFile = strFilePath;

		if ( strCheck == _T("s ") ) {
			CString strFileTitle = MtlGetWindowText(__m_pChildFrame->m_hWnd);
			SaveInternetShortcutFile( strFileTitle, strFile, __m_pChildFrame->GetLocationURL() );
			return;

		} else if ( strCheck == _T("a ") ) {

		} else if ( strCheck == _T("r ") ) {
			ATLASSERT(FALSE);
		}

		if (  !MtlIsProtocol( strFile, _T("http") )
		   && !MtlIsProtocol( strFile, _T("https") ) )
		{
			if ( MtlPreOpenFile(strFile) )
				return;
			// handled
		}

		MTL::ParseInternetShortcutFile(strFile);

		if ( strFile == _T("javascript:location.reload()") )
			return;

		__m_pChildFrame->Navigate2(strFile);
	}


	void RefreshMenu()
	{
		// set up option
		DWORD dwStyle = CFavoritesMenuOption::GetStyle();
		dwStyle 				&= ~EMS_ADDITIONALMENUITEM;
		SetStyle(dwStyle);
		m_nMaxMenuItemTextLength = CFavoritesMenuOption::GetMaxMenuItemTextLength();
		m_nMaxMenuBreakCount	 = CFavoritesMenuOption::GetMaxMenuBreakCount();

		SetRootDirectoryPath( DonutGetFavoritesFolder() );
	}

};
