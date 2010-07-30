#if !defined(AFX_MYTOOLBAR_H__B336C5E6_2EA6_43E0_9D11_08C509DBB5A7__INCLUDED_)
#define AFX_MYTOOLBAR_H__B336C5E6_2EA6_43E0_9D11_08C509DBB5A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyToolBar.h : ヘッダー ファイル
//
#include "MyWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar ウィンドウ

class CMyToolBar : public CToolBarCtrl {
// コンストラクタ
public:
	CMyToolBar();

//メンバ変数
private:
	CMyWindow	m_wnd;
	CImageList	m_ilist;
private:
public:
// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMyToolBar)
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CMyToolBar();

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CMyToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MYTOOLBAR_H__B336C5E6_2EA6_43E0_9D11_08C509DBB5A7__INCLUDED_)
