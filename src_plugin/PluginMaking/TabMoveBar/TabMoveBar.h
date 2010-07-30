// TabMoveBar.h : TABMOVEBAR アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_TABMOVEBAR_H__93BE99C7_0041_4126_8BFD_08FE58A3B403__INCLUDED_)
#define AFX_TABMOVEBAR_H__93BE99C7_0041_4126_8BFD_08FE58A3B403__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CTabMoveBarApp
// このクラスの動作の定義に関しては TabMoveBar.cpp ファイルを参照してください。
//

class CTabMoveBarApp : public CWinApp
{
public:
	CTabMoveBarApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTabMoveBarApp)
	public:
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTabMoveBarApp)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TABMOVEBAR_H__93BE99C7_0041_4126_8BFD_08FE58A3B403__INCLUDED_)



