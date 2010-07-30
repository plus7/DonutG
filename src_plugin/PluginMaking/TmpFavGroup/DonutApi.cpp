// DonutApi.cpp: CDonutApi クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TmpFavGroup.h"
#include "DonutApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



CDonutApi::CDonutApi()
{
	::CoCreateInstance(CLSID_API,NULL,CLSCTX_ALL,IID_IAPI3,(void**)&m_pDonut);
}

CDonutApi::~CDonutApi()
{
	if(m_pDonut != NULL)
		m_pDonut->Release();
}

BOOL CDonutApi::IsNull()
{
	return m_pDonut ? FALSE : TRUE;
}

