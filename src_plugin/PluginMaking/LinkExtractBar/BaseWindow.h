#if !defined(AFX_BASEWINDOW_H__87A925A8_5646_44C3_8CD9_D328421DBE25__INCLUDED_)
#define AFX_BASEWINDOW_H__87A925A8_5646_44C3_8CD9_D328421DBE25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseWindow.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CBaseWindow ウィンドウ

class CBaseWindow : public CWnd
{
// コンストラクション
public:
	CBaseWindow();

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

	//{{AFX_VIRTUAL(CBaseWindow)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	CImageList m_imgList;
	CToolBarCtrl m_wndToolBar;
	virtual ~CBaseWindow();

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CBaseWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnExtractLink();
	afx_msg void OnSendToDownloader();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_BASEWINDOW_H__87A925A8_5646_44C3_8CD9_D328421DBE25__INCLUDED_)
