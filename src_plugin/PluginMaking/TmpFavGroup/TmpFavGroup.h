// TmpFavGroup.h : TMPFAVGROUP アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_TMPFAVGROUP_H__22DC3708_8055_4550_8892_96A210526428__INCLUDED_)
#define AFX_TMPFAVGROUP_H__22DC3708_8055_4550_8892_96A210526428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル



/////////////////////////////////////////////////////////////////////////////
// CTmpFavGroupApp
// このクラスの動作の定義に関しては TmpFavGroup.cpp ファイルを参照してください。
//

class CTmpFavGroupApp : public CWinApp {
public:
	CTmpFavGroupApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTmpFavGroupApp)
	public:
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTmpFavGroupApp)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TMPFAVGROUP_H__22DC3708_8055_4550_8892_96A210526428__INCLUDED_)
