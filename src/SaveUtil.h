#pragma once
#ifndef SAVE_UTIL_H
#define SAVE_UTIL_H

#define SAVEMODE_FILEONLY 0
#define SAVEMODE_COMPLETE_TEXT 1
#define SAVEMODE_COMPLETE_DOM 2

/*
CString ValidateFileName(const CString& aFileName)
{
	CString result(aFileName);
	result.Replace(_T('\"'),_T('\''));
	result.Replace(_T('*'),_T(' '));
	result.Replace(_T(':'),_T(' '));
	result.Replace(_T('?'),_T(' '));
	result.Replace(_T('<'),_T('('));
	result.Replace(_T('>'),_T(')'));
	result.Replace(_T('/'),_T('_'));
	result.Replace(_T('\\'),_T('_'));
	result.Replace(_T('|'),_T('_'));
	return result;
}
*/
/*
int GetSaveModeForContentType(const CString& aContentType,
							  nsIDOMDocument* aDocument)
{
	if(aDocument) return SAVEMODE_FILEONLY;
	int saveMode = SAVEMODE_FILEONLY;
	/*if( aContentType == "text/html" ||
		aContentType == "application/xhtml+xml" ||
		aContentType == "image/svg+xml"){
			saveMode |= SAVEMODE_COMPLETE_TEXT;
	}else if(aContentType == "text/xml" || aContentType == "application/xml"){
		saveMode |= SAVEMODE_COMPLETE_DOM;
	}*/
/*	return saveMode;
}*/

#endif