/**
 *	@file	Misc.h
 *	@biref	雑多なルーチン群
 */
#pragma once

#include <stdarg.h>


namespace Misc {


inline bool IsExistFile(LPCTSTR fname)
{
	return ::GetFileAttributes(fname) != 0xFFFFFFFF;
}


/// .exeのあるディレクトリのフルパスを返す. 最後に'\\'を含む.
//   カレントディレクトリの取得.
inline const CString GetCurDirectory()
{
	TCHAR dir[MAX_PATH+2];
	dir[0] = 0;
	::GetCurrentDirectory(MAX_PATH, dir);
	return CString(dir);
}


//+++
inline int SetCurDirectory(const CString& strDir)
{
	return ::SetCurrentDirectory(LPCTSTR(strDir));
}



//+++ ファイルパス名より、ディレクトリ名を取得. 最後の'\\'は含まない.
inline const CString	GetDirName(const CString& strFileName)
{
	int 	n	= strFileName.ReverseFind( _T('\\') );
	if (n < 0)
		return _T(".");	//strFileName;
	return	strFileName.Left(n);
}


///+++ exe(dll)のフルパス名を返す.
CString 	GetExeFileName();


///+++ exe(dll)のあるフォルダを返す. 最後に'\\'が付かない
inline CString GetExeDirName()
{
	CString str = GetExeFileName();
	int 	n	= str.ReverseFind( _T('\\') );
	return	str.Left(n);
}


//+++ ファイルパス名より、ファイル名を取得
inline const CString	GetFileBaseName(const CString& strFileName)
{
	int 	n	= strFileName.ReverseFind( _T('\\') );
	if (n < 0)
		return strFileName;
	return	strFileName.Mid(n + 1);
}


///+++ ファイル名の拡張子の取得. ※ 結果の文字列には'.'は含まれない.
inline const CString	GetFileExt(const CString& strFileName)
{
	const CString strBaseName = GetFileBaseName(strFileName);
	int 	n	= strBaseName.ReverseFind( _T('.') );
	if (n < 0)
		return CString();
	return strBaseName.Mid(n + 1);
}


///+++ 拡張子無しのファイル名の取得. ※ 結果の文字列には'.'は含まれない.
inline const CString	GetFileNameNoExt(const CString& strFileName)
{
	const CString strBaseName = GetFileBaseName(strFileName);
	int 	n	= strBaseName.ReverseFind( _T('.') );
	if (n < 0)
		return strFileName;

	int 	nd	= strFileName.ReverseFind( _T('\\') );
	if (nd >= 0)
		return strFileName.Left(nd + 1) + strBaseName.Left(n);
	return strBaseName.Left(n);
}



inline const CString StrFmt(const char* fmt, ...)
{
	va_list app;
	va_start(app, fmt);
	CString 	str;
	str.FormatV(fmt, app);
	va_end(app);
	return str;
}



}	// namespace Misc
