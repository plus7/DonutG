// LinkExtractBar.h : LINKEXTRACTBAR アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_LINKEXTRACTBAR_H__4033A31B_4E8A_4949_8301_908341E6E13F__INCLUDED_)
#define AFX_LINKEXTRACTBAR_H__4033A31B_4E8A_4949_8301_908341E6E13F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CLinkExtractBarApp
// このクラスの動作の定義に関しては LinkExtractBar.cpp ファイルを参照してください。
//

class CLinkExtractBarApp : public CWinApp
{
public:
	CLinkExtractBarApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CLinkExtractBarApp)
	public:
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLinkExtractBarApp)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_LINKEXTRACTBAR_H__4033A31B_4E8A_4949_8301_908341E6E13F__INCLUDED_)
