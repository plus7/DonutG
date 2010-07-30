#if !defined(AFX_BASEWINDOW_H__F470B660_7783_4EFC_B38C_F01A303FDCAB__INCLUDED_)
#define AFX_BASEWINDOW_H__F470B660_7783_4EFC_B38C_F01A303FDCAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseWindow.h : ヘッダー ファイル
//
#include "TmpFavTreeView.h"


/////////////////////////////////////////////////////////////////////////////
// CBaseWindow ウィンドウ

class CBaseWindow : public CWnd {
public:
	CBaseWindow();

	void		SetCheckOfMenu(CMenu& menu, UINT uItemID, DWORD dwFlag);
	void		ToggleOptionFlag(DWORD dwFlag);
	HMENU		LoadPluginMenu();
	void		WriteProfile();
	void		ReadProfile();
	CString		GetBaseDataFolderPath();
	CString		GetSelfPath();
	CString		GetSelfDir();
	void		SetImageList();
	virtual		~CBaseWindow();


protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CBaseWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOpenActive();
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnOpenDblclick();
	afx_msg void OnOpenandclose();
	afx_msg void OnAddItemToGroup();
	afx_msg void OnAddItemToGroup_all();	//+++
	afx_msg void OnAddTmpgroup();
	afx_msg void OnGroupDelete();
	afx_msg void OnReloadFolder();
	afx_msg void OnGroupOpen();
	afx_msg void OnItemProperty();
	afx_msg void OnItemDelete();
	afx_msg void OnRenameGroup();
	afx_msg void OnSaveNameSw();
	afx_msg void OnSetDataDir();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static int CALLBACK BaseDir_BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData);

	static void		ReloadFolder();	//+++

public:
	CTmpFavTreeView		m_Tree;
	CImageList			m_imgs;


private:
	CString				m_userDataPath;		//+++
	CString				m_baseDataPath;		//+++

	static CBaseWindow*	s_pThis_;
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_BASEWINDOW_H__F470B660_7783_4EFC_B38C_F01A303FDCAB__INCLUDED_)
