// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。
//

#if !defined(AFX_STDAFX_H__19D07CDD_4159_414E_BCE7_12CDF1A1BEED__INCLUDED_)
#define AFX_STDAFX_H__19D07CDD_4159_414E_BCE7_12CDF1A1BEED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// この位置にヘッダーを挿入してください
#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します

// Change these values to use different versions
#define WINVER			0x0400
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE		0x0400
#define _RICHEDIT_VER	0x0100

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlmisc.h>


// TODO: プログラムで必要なヘッダー参照を追加してください。

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_STDAFX_H__19D07CDD_4159_414E_BCE7_12CDF1A1BEED__INCLUDED_)
