/**
 *	@file	DonutPFunc.cpp
 *	@brief	DonutP の汎用関数.
 */

#include "stdafx.h"
#include "DonutPFunc.h"
#include "MtlBase.h"
#include "MtlWin.h"
#include "ToolTipManager.h"
#include "resource.h"

#if defined USE_ATLDBGMEM
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace ATL;
using namespace WTL;
using namespace MTL;



extern TCHAR		g_szIniFileName[MAX_PATH];

extern const TCHAR	g_cSeparater[]	= _T("────────────────────────────");


static __inline double RADIAN_TO_ANGLE(double a) {
	const double PI	= 3.141592653589793238462643383279;
	return (a) / PI * 180.0;
}



double _GetAngle(CPoint pt1, CPoint pt2)
{
	double xlen  = pt2.x - pt1.x;
	double ylen  = pt1.y - pt2.y;

	if (fabs( xlen ) < 1.0)
		xlen = 0.0;

	if (fabs( ylen ) < 1.0)
		ylen = 0.0;

	if ( (xlen == 0.0) && (ylen == 0.0) )
		return ( 0.0 );

	if (xlen == 0.0) {
		if (pt1.y < pt2.y)
			return ( 270.0 );
		else
			return (  90.0 );
	} else if (ylen == 0.0) {
		if (pt1.x < pt2.x)
			return (  0.0);
		else
			return (180.0);
	}

	double angle = RADIAN_TO_ANGLE( atan2(ylen, xlen) );

	if (angle < 0.0)
		angle += 360.0;

	return ( angle );
}



BOOL _CheckOsVersion_98Later()
{
	OSVERSIONINFO osi = { sizeof (OSVERSIONINFO) };

	//BOOL bRet =
	::GetVersionEx(&osi);

	//windows98 == 4.10
	return ( (osi.dwMajorVersion >= 5) || (osi.dwMajorVersion == 4 && osi.dwMinorVersion >= 10) );
}



BOOL _CheckOsVersion_MELater()
{
	OSVERSIONINFO osi = { sizeof (OSVERSIONINFO) };

	//BOOL bRet =
	::GetVersionEx(&osi);

	//windowsME == 4.90
	return ( (osi.dwMajorVersion >= 5) || (osi.dwMajorVersion == 4 && osi.dwMinorVersion >= 90) );
}



BOOL _CheckOsVersion_2000Later()
{
	OSVERSIONINFO osi = { sizeof (OSVERSIONINFO) };

	//BOOL bRet =
	::GetVersionEx(&osi);

	//windows2000 5.0
	return ( (osi.dwMajorVersion >= 5) );
}



BOOL _CheckOsVersion_XPLater()
{
	OSVERSIONINFO osi = { sizeof (OSVERSIONINFO) };

	//BOOL bRet =
	::GetVersionEx(&osi);

	//windowsXP 5.1
	return ( (osi.dwMajorVersion == 5) && (osi.dwMinorVersion >= 1) || (osi.dwMajorVersion > 5) );
}



BOOL _CheckOsVersion_VistaLater()
{
	OSVERSIONINFO osi  = { sizeof (OSVERSIONINFO) };

	//BOOL		  bRet =
	::GetVersionEx(&osi);

	//windowsVista 6.0
	return (osi.dwMajorVersion >= 6);
}



CString _GetSkinDir()
{
	CString 	strPath    = Misc::GetExeDirectory() + _T("Skin\\");
	CString 	strDefPath = strPath + _T("default");

	CIniFileI	pr( g_szIniFileName, _T("Skin") );
	CString 	strSkinPath = pr.GetStringUW( _T("SkinFolder") );
	pr.Close();

	if ( strSkinPath.IsEmpty() )
		strSkinPath = strDefPath;
	else
		strSkinPath = strPath + strSkinPath;

	if (::GetFileAttributes(strSkinPath) == 0xFFFFFFFF) {
		if (GetFileAttributes(strDefPath) == 0xFFFFFFFF) {
			strSkinPath = strPath;
		} else {
			strSkinPath = strDefPath;
		}
	}

	if ( !strSkinPath.IsEmpty() && strSkinPath.Right(1) != _T("\\") )
		strSkinPath += _T("\\");

	return strSkinPath;
}



bool _LoadToolTipText(int nCmdID, CString &strText)
{
	enum { SIZE = 1024/*256*/ };
	TCHAR	szBuff[SIZE+2];
	szBuff[0] = 0;
	int   nRet = ::LoadString(_Module.GetResourceInstance(), nCmdID, szBuff, SIZE);

	for (int i = 0; i < nRet; i++) {
		if ( szBuff[i] == _T('\n') ) {
			TCHAR	szText[SIZE];
			lstrcpyn(szText, &szBuff[i + 1], SIZE);
			strText = szText;
			return true;
		}
	}

	return false;
}



#if 1	//+++ DonutRAPT よりパクらしてもらったルーチン.

// WinNT3.5+: メモリの予約領域を一時的に最小化。ウィンドウを最小化した場合と同等
void RtlSetMinProcWorkingSetSize()
{
  #if 1
	HINSTANCE hLib = ::LoadLibrary( _T( "kernel32.dll" ) );
	if ( hLib ) {
		typedef BOOL ( WINAPI *LPFN )( HANDLE, SIZE_T, SIZE_T );
		LPFN fnSetProcessWorkingSetSize = reinterpret_cast< LPFN >( ::GetProcAddress( hLib, "SetProcessWorkingSetSize" ) );
		if ( fnSetProcessWorkingSetSize ) {
			HANDLE hCurrentProcess = ::GetCurrentProcess();
		  #ifdef WIN64
			MTLVERIFY( fnSetProcessWorkingSetSize(hCurrentProcess, 0xFFFFFFFFFFFFFFFFll, 0xFFFFFFFFFFFFFFFFll ) );
		  #else
			MTLVERIFY( fnSetProcessWorkingSetSize(hCurrentProcess, 0xFFFFFFFF, 0xFFFFFFFF ) );
		  #endif
		}
		::FreeLibrary( hLib );
	}

	Misc::mallocHeapCompact();	//+++
  #endif
}

#endif


/*+++ メモ:
	Xp(ビジュアルスタイル)でManifestファイルを使用した場合、ANSI版のEditコントロール関係
	EM_GETSEL,EM_SETSEL,EM_LIMITTEXT で、それ以前のWinOSではバイト数で指定していたものが、
	文字数単位にかわってしまうらしい。(この動作は Unicode 版 API と同様の動作らしい)
		http://support.microsoft.com/default.aspx?scid=kb;ja;418099
			WindowsXP で Manifest ファイルを使用したアプリケーションで Common Control
			バージョン 6 を使用した場合、Edit コントロールへの EM_LIMITTEXT メッセージ
			による入力量制限を設定する際の単位が、バイト数単位ではなく文字数単位に変更
			されました。この動作は仕様です。

	……でManifestファイルを使用っていうのは、組み込みmanifestの場合は当てはまらない、と
	いうことなのか? vistaやxpでも通常はバイト数が返される. 下記ルーチンでは、.manifest
	ファイルの存在チェックで解釈を切り替えているので...
 */



// 1文字進め、進めたバイトを返す。但しNULLのときはポインタを進めず、0を返す
int RtlMbbNext(LPTSTR &r_lp)
{
	LPCTSTR lpBegin = r_lp;

	if (r_lp) {
		++r_lp;
	  #ifndef UNICODE
		if ( _ismbblead( *(r_lp - 1) ) && *r_lp /*_ismbbtrail(*r_lp)*/ ) {
			// 2バイト文字の時はもう1バイト進める
			++r_lp;
		}
	  #endif
	}

	return int (r_lp - lpBegin);
}



// strText の nFirst 文字目から、nCount 文字数を取得して返す。マルチバイト文字対応版
CString RtlMbbStrMid(CString strText, const int &nFirst, const int &nCount)
{
	ATLASSERT( nFirst >= 0 );

	if (nCount < 1 || strText.GetLength() <= nFirst)
		return CString();

	LPTSTR lp = strText.GetBuffer(0);
	int    i, p1 = 0, p2 = 0;

	for (i = 0; *lp && p1 < nFirst; ++i) {
		p1 += RtlMbbNext(lp);
	}

	for (i = 0; *lp && p2 < nCount; ++i) {
		p2 += RtlMbbNext(lp);
	}

	strText.ReleaseBuffer();
	return strText.Mid(p1, p2);
}


CString RtlGetSelectedText(const CEdit &edit)
{
	CString str    = MtlGetWindowText(edit);

	if ( str.IsEmpty() )
		return CString();

	int 	nStart = -1;
	int 	nEnd   = -1;
	edit.GetSel(nStart, nEnd);									// 選択範囲を「文字数」で返す

	if ( nStart != nEnd && str.GetLength() != (nEnd - nStart) && !(nStart == 0 && nEnd == -1) ) {
		str = RtlMbbStrMid(str, nStart, nEnd - nStart); 		// 文字数をバイト数に変換する必要がある
	}

	return str;
}


#if defined(UNICODE) == 0		//+++ UNICODE コンパイル時は、余計な手間は不要

/// EM_GETSELがVisualStyle適用時にUNICODE的動作をする(UNICODEを定義しない場合でも)のでなんとかする関数
CString _GetSelectTextWtoA(CEdit &edit)
{
	int 	nStart = -1;
	int 	nEnd   = -1;
	edit.GetSel(nStart, nEnd);									// 選択範囲を「文字数」で返す

	//これはUNICODEでの値(文字数)
	if (nStart == nEnd)
		return MtlGetWindowText(edit);							// +++ メモ:範囲指定がない場合は、全体を返す.

	int 	nTextLen  = ::GetWindowTextLength(edit.m_hWnd);
	int 	nSelCount = nEnd - nStart;

	LPSTR	lpAll	  = new CHAR [nTextLen	+ 1];
	LPWSTR	lpwAll	  = new WCHAR[nTextLen	+ 1];
	LPWSTR	lpwSel	  = new WCHAR[nSelCount + 1];

	::GetWindowText(edit.m_hWnd, (LPTSTR) lpAll, nTextLen + 1); 		//これはANSIなテキストを返す
	::MultiByteToWideChar(CP_ACP, 0, lpAll, -1, lpwAll, nTextLen + 1);	// +++ メモ: 一旦 UTF16にして

	memset( lpwSel, 0, (nSelCount + 1) * sizeof (WCHAR) );
	CopyMemory( lpwSel, lpwAll + nStart, nSelCount * sizeof (WCHAR) );	// +++ メモ:

	CString str 	  = lpwSel; 		// +++ メモ:このコピーコンストラクタで UTF16から MBC に変換.

	delete[] lpAll;
	delete[] lpwAll;
	delete[] lpwSel;
	return str;
}

#endif


CString _GetSelectText(CEdit &edit)
{
	//VisualStyle が適用されている場合は EM_GETSEL は UNICODE で返す
  #if defined(UNICODE)		//+++ UNICODE コンパイル時は、余計な手間は不要
	CString str    = MtlGetWindowText(edit);

	if ( str.IsEmpty() )
		return CString();

	int 	nStart = -1;
	int 	nEnd   = -1;
	edit.GetSel(nStart, nEnd);									// 選択範囲を「文字数」で返す
	if (nStart == nEnd || (nStart == 0 && nEnd == -1) || str.GetLength() == (nEnd - nStart))
		return str;
	return str.Mid(nStart, nEnd - nStart);
  #else
	// +++ メモ:Xp以降のWinOS で、exeと同じフォルダに .manifestファイルがある場合.
	//			※ exeファイルと同じところにおく.manifestファイルって、障害時対策ようなのかな.
	//x if ( CThemeDLLLoader::IsExistManifest() && _CheckOsVersion_XPLater() )	//+++ CThemeDLLLoader を使わないようにする.
	if ( IsExistManifestFile() && _CheckOsVersion_XPLater() ) {
		//UNICODEからANSIへ変換
		return _GetSelectTextWtoA(edit);
	} else
	{
		//ANSIの場合はRAPT氏のコードを流用
		return RtlGetSelectedText(edit);
	}
  #endif
}



//+++ あとで作り直す.
static CString GetCurPosWord(const TCHAR* buf, unsigned len, unsigned pos);
static CString GetCurPosWordB(const TCHAR* buf, unsigned len, unsigned pos);

///*+++ 実験品: GetSelの返り値の扱いが適当なので、誤動作(違う単語を検索)がありえる.
///+++ エディットの、選択テキストの取得. 未選択の場合は、カーソル位置の単語を取得.(ページ内検索用)
CString _GetSelectText_OnCursor(CEdit &edit)
{
	int 	len = ::GetWindowTextLength( edit.m_hWnd ); 	// バイト数. (キャラ数)
	int 	nStart = -1, nEnd = -1;
	edit.GetSel(nStart, nEnd);								// 選択範囲を返す(文字数かバイト数か状況しだいのよう)

	if (len == 0 || nStart != nEnd || nStart < 0)			// 範囲指定だったり、文字がなかったりヘンな状態なら元の処理まかせ.
		return _GetSelectText( edit );

	std::vector<TCHAR>	buf( len + 2 );
	::GetWindowText(edit.m_hWnd, (LPTSTR)&buf[0], len + 1); // 文字列取得.

  #ifdef UNICODE	//+++ UNICODEコンパイルなら、余計なことをする必要なし
	return GetCurPosWord( &buf[0], len, nStart );			// 単語取得. nStartが文字数版.
  #else
	// +++ メモ:Xp以降のWinOS で、exeと同じフォルダに .manifestファイルがある場合.
	if ( IsExistManifestFile() && _CheckOsVersion_XPLater() ) {
		return GetCurPosWord( &buf[0], len, nStart );			// 単語取得. nStartが文字数版.
	} else {
		for (int i = 0; i < len; ++i) { 						// 全角空白は半角２つに置き換える.
			unsigned c = *(unsigned char*)&buf[i];
			if (c == 0x81 && buf[i+1] == 0x40) {
				buf[i  ]  = ' ';
				buf[++i]  = ' ';
			}
		}
		return GetCurPosWordB( &buf[0], len, nStart );			// 単語取得. nStartがバイト数版.
	}
  #endif
}



///+++ posの位置(文字数)にある空白で区切られた単語を CStringにして返す. '"'等は未考慮.
static CString GetCurPosWord(const TCHAR* src, unsigned len, unsigned pos)
{
	#define IS_SPACE_W(c)		((c) == L' ' || (c) == L'\t' || (c) == L'\n' || (c) == L'\r' || (c) == L'　')
	ATLASSERT(src != 0 && len > 0 && pos <= len);
	std::vector<WCHAR>	wbuf( len + 2 );
  #ifdef UNICODE
	::lstrcpyn(&wbuf[0], src, len+1);
  #else
	::MultiByteToWideChar(CP_ACP, 0, &src[0], -1, &wbuf[0], len + 1);	// 一旦wchar化.
  #endif
	unsigned	top = 0;
	unsigned	end = len;

  #if 0 	// まず、カーソル位置に文字があるかチェック. 空白なら、前の単語の最後へ移動.
	unsigned c = wbuf[ pos ];
	if (c == 0)
		return CString( src );
	if (IS_SPACE_W(c)) {
		if (pos > 0) {
			do {
				c = wbuf[--pos];
			} while (pos > top && IS_SPACE_W(c));
		}
		if (IS_SPACE_W(c)) {	// カーソルより前に単語がないなら、後ろに移動.
			do {
				c = wbuf[++pos];
			} while (pos < end && IS_SPACE_W(c));
		}
	}
  #else 	// まず、カーソル位置に文字があるかチェック. 空白なら、次の単語の先頭へ移動.
	unsigned c = wbuf[ pos ];
	if (IS_SPACE_W(c)) {
		do {
			c = wbuf[++pos];
		} while (pos < end && IS_SPACE_W(c));
	}
	if (c == 0)
		return CString( src );
  #endif

	// カーソル位置から単語の先頭を探す.
	for (int i = int(pos); --i >= 0;) {
		c = wbuf[i];
		if (IS_SPACE_W(c) || c == 0) {
			top = i;
			break;
		}
	}

	// カーソル位置から単語の最後を探す.
	for (unsigned i = pos; i < end; ++i) {
		c = wbuf[i];
		if (IS_SPACE_W(c) || c == 0) {
			end = i;
			break;
		}
	}

	// 終端を設定.
	wbuf[end] = 0;

	if (top >= end) { // 文字が無かったら全体を返す.
		return CString( src );
	} else {
		return CString ( &wbuf[top] );	// topからendまでのwchar文字をCString化(コピーコンストラクタに任す)
	}

	#undef IS_SPACE_W
}


#if defined(UNICODE) == 0	//+++ MultiByte版の時

///+++ posの位置(バイト数)にある空白で区切られた単語を CStringにして返す. '"'等は未考慮.
/// とりあえず、面倒なんで、全角空白未対応.→予備元であらかじめ置換するようにした.(ヘンなxpはまだ...)
static CString GetCurPosWordB(const TCHAR* src, unsigned len, unsigned pos)
{
	#define IS_SPACE(c) 	((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
	ATLASSERT(src != 0 && len > 0 && pos <= len);
	std::vector<char>		bbuf(len + 4);
	memcpy(&bbuf[0], src, len+1);
	unsigned	top = 0;
	unsigned	end = len;

  #if 0 	// まず、カーソル位置に文字があるかチェック. 空白なら、前の単語の最後へ移動.
	unsigned c = bbuf[ pos ];
	if (c == 0)
		return CString( src );
	if (IS_SPACE(c) || c == 0) {
		if (pos > 0) {
			do {
				c = bbuf[--pos];
			} while (pos > top && IS_SPACE(c));
		}
		if (IS_SPACE(c)) {	// カーソルより前に単語がないなら、後ろに移動.
			do {
				c = bbuf[++pos];
			} while (pos < end && IS_SPACE(c));
		}
	}
  #else 	// まず、カーソル位置に文字があるかチェック. 空白なら、次の単語の先頭へ移動.
	unsigned c = bbuf[ pos ];
	if (IS_SPACE(c)) {
		do {
			c = bbuf[++pos];
		} while (pos < end && IS_SPACE(c));
	}
	if (c == 0)
		return CString( src );
  #endif

	// カーソル位置から単語の先頭を探す.
	for (int i = int(pos); --i >= 0;) {
		c = bbuf[i];
		if (IS_SPACE(c) || c == 0) {
			top = i;
			break;
		}
	}

	// カーソル位置から単語の最後を探す.
	for (unsigned i = pos; i < end; ++i) {
		c = bbuf[i];
		if (IS_SPACE(c) || c == 0) {
			end = i;
			break;
		}
	}

	// 終端を設定.
	bbuf[end] = 0;

	if (top >= end) { // 文字が無かったら全体を返す.
		return CString( src );
	} else {
		return CString ( &bbuf[top] );	// topからendまでのwchar文字をCString化(コピーコンストラクタに任す)
	}

	#undef IS_SPACE
}

#endif




BOOL _AddSimpleReBarBandCtrl(
		HWND	hWndReBar,
		HWND	hWndBand,
		int 	nID,
		LPTSTR	lpstrTitle,
		BOOL	bNewRow,
		int 	cxWidth,
		BOOL	bFullWidthAlways)
{
	ATLASSERT( ::IsWindow(hWndReBar) ); 	// must be already created
  #ifdef _DEBUG
	// block - check if this is really a rebar
	{
		TCHAR lpszClassName[sizeof (REBARCLASSNAME)];
		lpszClassName[0] = 0;	//+++
		::GetClassName( hWndReBar, lpszClassName, sizeof (REBARCLASSNAME) );
		ATLASSERT(lstrcmp(lpszClassName, REBARCLASSNAME) == 0);
	}
  #endif	//_DEBUG
	ATLASSERT( ::IsWindow(hWndBand) );		// must be already created

	// Get number of buttons on the toolbar
	int 		  nBtnCount = (int) ::SendMessage(hWndBand, TB_BUTTONCOUNT, 0, 0L);
	// Set band info structure
	REBARBANDINFO rbBand;
	rbBand.cbSize	 = sizeof (REBARBANDINFO);
  #if (_WIN32_IE >= 0x0400)
	rbBand.fMask	 = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE | RBBIM_IDEALSIZE;
  #else
	rbBand.fMask	 = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE;
  #endif	//!(_WIN32_IE >= 0x0400)
	if (lpstrTitle != NULL)
		rbBand.fMask |= RBBIM_TEXT;

	rbBand.fStyle	 = 0;
  #if (_WIN32_IE >= 0x0500)
	if (nBtnCount > 0)											// add chevron style for toolbar with buttons
		rbBand.fStyle |= RBBS_USECHEVRON;
  #endif	//(_WIN32_IE >= 0x0500)

	if (bNewRow)
		rbBand.fStyle |= RBBS_BREAK;

	rbBand.lpText	 = lpstrTitle;
	rbBand.hwndChild = hWndBand;

	if (nID == 0)	// calc band ID
		nID = ATL_IDW_BAND_FIRST + (int) ::SendMessage(hWndReBar, RB_GETBANDCOUNT, 0, 0L);

	rbBand.wID		 = nID;

	// Calculate the size of the band
	BOOL	bRet;
	RECT	rcTmp;

	if (nBtnCount > 0) {
		bRet			  = ::SendMessage(hWndBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM) &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cx		  = (cxWidth != 0) ? cxWidth : rcTmp.right;
		rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
		rbBand.cxMinChild = 0;
	} else {	// no buttons, either not a toolbar or really has no buttons
		bRet			  = ::GetWindowRect(hWndBand, &rcTmp) != 0;
		ATLASSERT(bRet);
		rbBand.cx		  = (cxWidth != 0) ? cxWidth : (rcTmp.right - rcTmp.left);
		rbBand.cxMinChild = (bFullWidthAlways) ? rbBand.cx : 0;
		rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
	}

  #if (_WIN32_IE >= 0x0400)
	rbBand.cxIdeal = rbBand.cx;
  #endif	//(_WIN32_IE >= 0x0400)

	// Add the band
	LRESULT lRes = ::SendMessage(hWndReBar, RB_INSERTBAND, (WPARAM) -1, (LPARAM) &rbBand);

	if (lRes == 0) {
		ATLTRACE2( atlTraceUI, 0, _T("Failed to add a band to the rebar.\n") );
		return FALSE;
	}

	return TRUE;
}



bool FileWriteString(const CString& strFile, std::list<CString>* pString)
{
	FILE *	fp;
	if ( ( fp = _tfopen(strFile, _T("w")) ) == NULL ) {
		return false;
	}

  #if 1	//+++ UNICODE 対応で書き換え
	for (std::list<CString>::iterator  str = pString->begin();
		 str != pString->end();
		 ++str)
	{
	  #ifdef UNICODE
		fprintf(fp, "%s\n", &Misc::tcs_to_sjis(*str)[0]);
	  #else
		fprintf(fp, "%s\n", LPCTSTR(*str));
	  #endif
	}
  #else
	std::list<CString>::iterator str;
	CString 					 strOut;

	for (str = pString->begin(); str != pString->end(); ++str) {
		fprintf(fp, "%s\n", *str);
	}
  #endif

	fclose(fp);

	return true;
}



bool FileReadString(const CString& strFile, std::list<CString>* pString)
{
	FILE *	fp;

	if ( ( fp = _tfopen(strFile, _T("r")) ) == NULL ) {
		return false;
	}

	enum { L = 4096 };
	char		cBuff[L];
	CString		strBuff;

	while (fgets(cBuff, L, fp) != NULL) {
		unsigned	nLen = unsigned( strlen(cBuff) );

		if (nLen != 0 && cBuff[nLen - 1] == '\n')
			cBuff[nLen - 1] = '\0';

		pString->insert(pString->end(), CString(cBuff) );
	}

	fclose(fp);

	return true;
}



BOOL _SetCombboxCategory(CComboBox &cmb, HMENU hMenu)
{
	if (cmb.m_hWnd == NULL)
		return FALSE;

	if (cmb.GetCount() != 0)
		return FALSE;

	for (int ii = 0; ii < ::GetMenuItemCount(hMenu); ii++) {
		TCHAR	cBuff[MAX_PATH];
		cBuff[0]	= 0;
		if (::GetMenuString(hMenu, ii, cBuff, MAX_PATH, MF_BYPOSITION) == 0)
			continue;

		CString strMenu(cBuff);

		if (strMenu.Find(_T("(")) != -1)
			strMenu = strMenu.Left( strMenu.Find(_T("(")) );
		else if (strMenu.Find(_T("\t")) != -1)
			strMenu = strMenu.Left( strMenu.Find(_T("\t")) );

		cmb.AddString(strMenu);
	}

	cmb.SetCurSel(0);
	return TRUE;
}



BOOL _DontUseID(UINT uID)
{
	// UINT uDontUseID[] = {ID_FILE_MRU_FILE1, ID_INSERTPOINT_GROUPMENU, ID_INSERTPOINT_FAVORITEMENU};
	static const UINT uDontUseID[] = { ID_INSERTPOINT_GROUPMENU, ID_INSERTPOINT_FAVORITEMENU };

	for (int ii = 0; ii < sizeof (uDontUseID) / sizeof (UINT); ii++) {
		if (uID == uDontUseID[ii])
			return TRUE;
	}

	return FALSE;
}



void _PickUpCommandSub(HMENU hMenuSub, CComboBox &cmbCmd)
{
	CMenu	menuSub(hMenuSub);
	int nPopStartIndex = cmbCmd.AddString(g_cSeparater);
	int nAddCnt 	   = 0;

	for (int ii = 0; ii < menuSub.GetMenuItemCount(); ii++) {
		HMENU	hMenuSub2 = menuSub.GetSubMenu(ii);

		if (hMenuSub2)
			_PickUpCommandSub(hMenuSub2, cmbCmd);

		UINT	nCmdID	  = menuSub.GetMenuItemID(ii);

		if ( _DontUseID(nCmdID) )
			break;

		CString strMenu;
		CToolTipManager::LoadToolTipText(nCmdID, strMenu);

		if ( strMenu.IsEmpty() )
			continue;

		int 	nIndex	  = cmbCmd.AddString(strMenu);
		cmbCmd.SetItemData(nIndex, nCmdID);
		nAddCnt++;
	}

	if (nAddCnt != 0)
		cmbCmd.AddString(g_cSeparater);
	else
		cmbCmd.DeleteString(nPopStartIndex);

	menuSub.Detach();
}



void _PickUpCommand(HMENU hMenu, int nPopup, CComboBox &cmbCmd)
{
	HMENU	hMenuSub = ::GetSubMenu(hMenu, nPopup);
	CMenu	menu(hMenuSub);
	cmbCmd.ResetContent();

	for (int ii = 0; ii < ::GetMenuItemCount(hMenuSub); ii++) {
		HMENU	hMenuSub = menu.GetSubMenu(ii);

		if (hMenuSub)
			_PickUpCommandSub(hMenuSub, cmbCmd);

		UINT	nCmdID	 = menu.GetMenuItemID(ii);

		if ( _DontUseID(nCmdID) )
			break;

		CString strMenu;
		CToolTipManager::LoadToolTipText(nCmdID, strMenu);

		if ( strMenu.IsEmpty() )
			continue;

		int 	nIndex	 = cmbCmd.AddString(strMenu);
		cmbCmd.SetItemData(nIndex, nCmdID);
	}

	menu.Detach();

	//不要なセパレータの削除
	int   nCountSep = 0;
	int   nCountCmb = cmbCmd.GetCount();

	for (int i = 0; i < nCountCmb - 1; i++) {
		if (cmbCmd.GetItemData(i) == 0) {
			nCountSep++;

			if (cmbCmd.GetItemData(i + 1) == 0) {
				cmbCmd.DeleteString(i);
				nCountCmb--;
				i--;
			}
		}
	}

	if (nCountSep > 2) {
		if (cmbCmd.GetItemData(0) == 0)
			cmbCmd.DeleteString(0);

		int nIndexLast = cmbCmd.GetCount() - 1;

		if (cmbCmd.GetItemData(nIndexLast) == 0)
			cmbCmd.DeleteString(nIndexLast);
	}
}



//minit
BOOL _ReplaceImageList(CString strBmpFile, CImageList &imgs, DWORD dfltRes)
{
	CBitmap 	bmp;
	bmp.Attach( AtlLoadBitmapImage(strBmpFile.GetBuffer(0), LR_LOADFROMFILE) );
  #if 0 //+++
	return _ReplaceImageList(bmp.m_hBitmap, imgs);
  #else
	if (bmp.m_hBitmap == 0 && dfltRes) {
		bmp.LoadBitmap(dfltRes);	//+++	ファイルがなかったとき、デフォルトのリソースを読み込んでみる.
	}
	if (bmp.m_hBitmap) {
		int nCount = imgs.GetImageCount();
		for (int i = 0; i < nCount; i++) {
			if ( !imgs.Remove(0) )
				return FALSE;
		}
		imgs.Add( bmp, RGB(255, 0, 255) );
		return TRUE;
	}
	return FALSE;
  #endif
}



static int _Pack(int hi, int low)
{
	if ( !( ( ('0' <= low && low <= '9') || ('A' <= low && low <= 'F') || ('a' <= low && low <= 'f') )
		  && ( ('0' <= hi && hi  <= '9') || ('A' <= hi	&& hi  <= 'F') || ('a' <= hi  && hi  <= 'f') ) ) )
		return 0;	//数値ではない

	int nlow = ('0' <= low && low <= '9') ? low - '0'
			 : ('A' <= low && low <= 'F') ? low - 'A' + 0xA
			 :								low - 'a' + 0xA ;
	int nhi  = ('0' <= hi  && hi  <= '9') ? hi	- '0'
			 : ('A' <= hi  && hi  <= 'F') ? hi	- 'A' + 0xA
			 :								hi	- 'a' + 0xA ;

	return (nhi << 4) + nlow;
}



BOOL _QueryColorString(CIniFileI &pr, COLORREF &col, LPCTSTR lpstrKey)
{
	CString strCol = pr.GetString(lpstrKey, NULL, 20);
	//+++ 元々 pr.QueryValue(文字列)でのエラーは、長さが0の時のことなので、文字列が空かどうかのチェックだけで十分.
	if ( strCol.IsEmpty() )
		return FALSE;

	strCol.TrimLeft('#');

	col = RGB( _Pack(strCol[0], strCol[1]) ,
			   _Pack(strCol[2], strCol[3]) ,
			   _Pack(strCol[4], strCol[5])
			 );

	return TRUE;
}



// ===========================================================================
//+++ +mod追加



#if 1	//+++ 強制的に、その場でメッセージをさばく...
#include "dbg_wm.h"
int ForceMessageLoop(HWND hWnd)
{
#if 1
	MSG msg = {0};
	int ret;
	int n = 0;
wm_t wm;
	CMessageLoop *pLoop 	 = _Module.GetMessageLoop();
	while ((ret = ::PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE)) != 0) {
wm = (wm_t)msg.message;
		//if (ret < 0)
		//	break;
		if (!GetMessage (&msg,hWnd,0,0))	/* メッセージ処理. QUITだったら、速攻かえる*/
			return 0;	//x return msg.wParam ;
		if(pLoop->PreTranslateMessage(&msg) == 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		++n;
	}
	pLoop->OnIdle(1);
#endif
	return 1;
}
#endif



#if 0
#include "option/SkinOption.h"

///+++ キャプション無しの時、ツールバーの隙間をつかめるようにするために、キャプションを掴んだフリをする関数.
bool  Donut_FakeCaptionLButtonDown(HWND hWnd, HWND hWndTopLevelParent, LPARAM lParam)
{
	if (CSkinOption::s_nMainFrameCaption == 0) {
	  #if 1 //+++ メニューのお気に入りを出して、その項目を移動しようとしたら窓が移動してしまうので、その対策
		CRect	rc;
		::GetWindowRect(hWnd, rc);
		POINT	pt;
		::GetCursorPos(&pt);
		if (rc.PtInRect(pt))
	  #endif
		{
			//HWND	hWndTopLevelParent = GetTopLevelWindow();
			::PostMessage(hWndTopLevelParent, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			return true;
		}
	}
	return false;
}
#endif



#include "MtlProfile.h"
int GetFontHeight(HFONT hFont)
{
	MTL::CLogFont	lf;
	CFontHandle( hFont ).GetLogFont(&lf);
	int 	h	= lf.lfHeight;
	if (h < 0)
		h = -h;
	return h;
}


