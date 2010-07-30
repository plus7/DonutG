// MoveTab.cpp: CMoveTab クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TabMoveBar.h"
#include "MoveTab.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CMoveTab::CMoveTab()
{
}


CMoveTab::~CMoveTab()
{
}


BOOL CMoveTab::MoveTabLeft()
{
	if ( InitCom() ) {
		IAPI*	pDonutP = GetIAPI();
		int 	index	= pDonutP->TabIndex;
		pDonutP->MoveToTab(index - 1, index);
	}

	UnInitCom();
	return TRUE;
}


BOOL CMoveTab::MoveTabRight()
{
	if ( InitCom() ) {
		IAPI*	pDonutP = GetIAPI();
		int		index	= pDonutP->TabIndex;
		pDonutP->MoveToTab(index, index + 1);
	}

	UnInitCom();
	return TRUE;
}


BOOL CMoveTab::MoveTabLeftEnd()
{
	if ( InitCom() ) {
		IAPI*	pDonutP = GetIAPI();
		int 	index	= pDonutP->TabIndex;

		if (index != 0) {
			pDonutP->MoveToTab(index, 0);
			//これだけだと何故か左から二番目のところに移動するので
			//左端２つをさらに入れ替える
			pDonutP->MoveToTab(0, 1);
		}
	}

	UnInitCom();
	return TRUE;
}


BOOL CMoveTab::MoveTabRightEnd()
{
	if ( InitCom() ) {
		IAPI*	pDonutP = GetIAPI();
		int		index	= pDonutP->TabIndex;
		pDonutP->MoveToTab( index, (unsigned short) pDonutP->GetTabCount() );
	}

	UnInitCom();
	return TRUE;
}


BOOL CMoveTab::InitCom()
{
	return SUCCEEDED( ::CoInitialize(NULL) );
}


void CMoveTab::UnInitCom()
{
	::CoUninitialize();
}


IAPI *CMoveTab::GetIAPI()
{
	IAPI*	pDonutP;
	::CoCreateInstance(CLSID_API, NULL, CLSCTX_SERVER, IID_IAPI, (void **) &pDonutP);
	return pDonutP;
}
