#if !defined(AFX_MYWINDOW_H__FD081264_9589_4C1A_9D2E_F3086D9214F5__INCLUDED_)
#define AFX_MYWINDOW_H__FD081264_9589_4C1A_9D2E_F3086D9214F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyWindow.h : ヘッダー ファイル
//
#include "MoveTab.h"

/////////////////////////////////////////////////////////////////////////////
// CMyWindow ウィンドウ

class CMyWindow : public CWnd {
// コンストラクタ
public:
	CMyWindow();

//メンバ変数
public:
	CMoveTab	m_tab;
// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

protected:
	//{{AFX_VIRTUAL(CMyWindow)
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CMyWindow();

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CMyWindow)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MYWINDOW_H__FD081264_9589_4C1A_9D2E_F3086D9214F5__INCLUDED_)
