#if !defined(AFX_TMPFAVTREEVIEW_H__6AA346DD_4049_45A2_A700_72A436EC67FF__INCLUDED_)
#define AFX_TMPFAVTREEVIEW_H__6AA346DD_4049_45A2_A700_72A436EC67FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmpFavTreeView.h : ヘッダー ファイル
//


/////////////////////////////////////////////////////////////////////////////
// Struct

struct FavGroupInfo {
	CString strIniFile;		//ファイル名(ここで扱うのはiniではなくdfgだけど)
	CString strSection;		//参照するセクション名
	BOOL	bUseIniStyle;	//常にTRUEで

	CString strLocation;	//URL
	CString	strTitle;		//タイトル
};

struct _ExInfo {
	LPTSTR	lpstrIniFile;
	LPTSTR	lpstrSection;
	BOOL	bUseIniStyle;
};

// ExStyle Option

#define TMPFAV_CLOSE		0x00000001
#define TMPFAV_ACTIVE		0x00000002
#define TMPFAV_DBLCLICK		0x00000004
#define TMPFAV_SAVENAME		0x00000008		//+++ セーブ時に名前をつける場合


#define PREFIXSECTION _T("window")
#define PREFIXSIZE	  6

/////////////////////////////////////////////////////////////////////////////
// CTmpFavTreeView ウィンドウ

class CTmpFavTreeView : public CTreeCtrl
{
// コンストラクション
public:
	CTmpFavTreeView();
	virtual ~CTmpFavTreeView();

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTmpFavTreeView)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	BOOL DeleteItem(HTREEITEM hItem);
	BOOL DeleteAllItems();

// インプリメンテーション
public:
	void RenameGroup(HTREEITEM hItem);
	void RenameGroup();
	void DeleteGroupItem(HTREEITEM hItem);
	void DeleteGroupItem();
	BOOL LoadFavGroups(const CString& strFolder);

	BOOL OpenFavGroup();
	BOOL OpenFavGroup(HTREEITEM hItem);
	BOOL OpenFavGroup(const CString& strFileName);

	void SaveGroup();

	void DeleteGroup();
	void DeleteGroup(HTREEITEM hItem);

	void AddItemToGroup(bool bAll=false);

	BOOL OpenGroupItem(HTREEITEM hItem);

	DWORD GetExtendedStyle();
	void SetExtendedStyle(DWORD dwStyle);

	void SetItemOption();
	void SetItemOption(HTREEITEM hItem);

	void setRenameEndFunc( void (*func)() ) { m_renameEndFunc = func; }
	

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CTmpFavTreeView)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	//プライベート関数
	void			FreeDataStruct(FavGroupInfo *ptr);
	FavGroupInfo*	AllocDataStruct();
	BOOL			DeleteChildItems(HTREEITEM hParent);
	void			GetGroupItemData(const CString& strDfgFile, int nSection, CString& strLocation, CString &strTitle);
	FavGroupInfo**	ReadFavGroupFile(const CString& strFilePath, int *pnCount);
	BOOL			InsertFavGroup(const CString& strGroupTitle, const CString& strFilePath);
	BOOL			InsertFavGroupItem(const CString& strFilePath, HTREEITEM hParent);
	BOOL			FileOperation(const CString& strFromFilePath, const CString& strToFilePath, int nOperate, int nFlag);

private:
	//プライベート変数
	CString		m_strDataFolder;
	DWORD		m_dwExStyle;
	int			m_nNewCount;
	void		(*m_renameEndFunc)();
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TMPFAVTREEVIEW_H__6AA346DD_4049_45A2_A700_72A436EC67FF__INCLUDED_)
