// MoveTab.h: CMoveTab クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVETAB_H__EE715D23_BE8D_4F82_97D5_ADA875E1F162__INCLUDED_)
#define AFX_MOVETAB_H__EE715D23_BE8D_4F82_97D5_ADA875E1F162__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMoveTab  {
public:
	CMoveTab();
	virtual ~CMoveTab();

	BOOL MoveTabLeft();
	BOOL MoveTabRight();
	BOOL MoveTabLeftEnd();
	BOOL MoveTabRightEnd();

protected:
	BOOL  InitCom();
	void  UnInitCom();
	IAPI* GetIAPI();
};

#endif // !defined(AFX_MOVETAB_H__EE715D23_BE8D_4F82_97D5_ADA875E1F162__INCLUDED_)
