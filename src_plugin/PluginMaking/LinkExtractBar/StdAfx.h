// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。
//

#if !defined(AFX_STDAFX_H__1584A881_2A92_44BD_9ABA_7D0E9D2B9D58__INCLUDED_)
#define AFX_STDAFX_H__1584A881_2A92_44BD_9ABA_7D0E9D2B9D58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//+++ 現状基本はwin2k以降だけど、ちょっとだけwin9xを気にしてみる.
#ifndef WINVER
//#define WINVER		0x0500	//+++ unDonut+modはwin2k以降としているので(というかvs2005以降でのコンパイルのため)
#define WINVER					0x0400
#define _WIN32_WINNT			0x0400
#define _WIN32_IE				0x0501
//#define _RICHEDIT_VER			0x0100
#endif


#define VC_EXTRALEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します。

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC の OLE クラス
#include <afxodlgs.h>       // MFC の OLE ダイアログ クラス
#include <afxdisp.h>        // MFC のオートメーション クラス
#endif // _AFX_NO_OLE_SUPPORT

#include <afxdtctl.h>		// MFC の Internet Explorer 4 コモン コントロール サポート
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT


//+++ 32/64ビット版別のプラグインフォルダ
#ifdef WIN64
#define PLUGIN_DIR		"Plugin64\\"
#else
#define PLUGIN_DIR		"Plugin32\\"
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。


#endif // !defined(AFX_STDAFX_H__1584A881_2A92_44BD_9ABA_7D0E9D2B9D58__INCLUDED_)
