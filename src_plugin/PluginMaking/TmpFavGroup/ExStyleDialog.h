#if !defined(AFX_EXSTYLEDIALOG_H__EE1BA1EE_8DB6_4A42_AFE8_8F1A0A5B74E4__INCLUDED_)
#define AFX_EXSTYLEDIALOG_H__EE1BA1EE_8DB6_4A42_AFE8_8F1A0A5B74E4__INCLUDED_

#include "ExStyle.h"	// ClassView によって追加されました。
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExStyleDialog.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CExStyleDialog ダイアログ

class CExStyleDialog : public CDialog
{
// コンストラクション
public:
	CExStyleDialog(CApiExStyle& style, CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CExStyleDialog)
	enum { IDD = IDD_STYLE };

	BOOL	m_bDLActiveX;
	BOOL	m_bImage;
	BOOL	m_bJava;
	BOOL	m_bNaviLock;
	BOOL	m_bRunActiveX;
	BOOL	m_bScript;
	BOOL	m_bScrollBar;
	BOOL	m_bSound;
	BOOL	m_bVideo;
	BOOL	m_bBlockMailTo;
	BOOL	m_bGesture;
	BOOL	m_bFilter;
	int		m_nReload;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CExStyleDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	CApiExStyle& m_ExStyle;

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CExStyleDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_EXSTYLEDIALOG_H__EE1BA1EE_8DB6_4A42_AFE8_8F1A0A5B74E4__INCLUDED_)
