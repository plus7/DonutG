/**
 *	@file	ToolTipManager.h
 *	@brief	ツールチップ・マネージャ.
 */
#pragma once


#include "ExMenu.h"



class CToolTipManager {
public:

	static bool LoadToolTipText(UINT nCmdID, CString &strText)
	{
		enum { SIZE = 0x1000/*256*/ };
		TCHAR	szBuff[SIZE+2];
		TCHAR	szText[SIZE+2];

		szBuff[0] = 0;
		int 	nRet = ::LoadString(_Module.GetResourceInstance(), nCmdID, szBuff, SIZE);

		for (int i = 0; i < nRet; i++) {
			if ( szBuff[i] == _T('\n') ) {
				lstrcpyn(szText, &szBuff[i + 1], SIZE);
				strText = szText;
				return true;
			}
		}

		CString 	strTip = CExMenuManager::GetToolTip(nCmdID);

		if ( strTip.IsEmpty() )
			return false;
		else {
			strText = strTip;
			return true;
		}
	}

};

