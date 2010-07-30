/**
 *	@file	DebugWindow.h
 *	@brief	とりあえず的な適当デバッグ用ダイアログを生成するクラス
 */
#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atlcrack.h>

const bool	g_cnt_b_use_debug_window	=	false;


/**
	とりあえず的な適当デバッグ用ダイアログを生成するクラス

	リリースビルド時にはコンパイルすらされないようにすべきではあるが、ﾏﾝﾄﾞｸｾ('A`)
	使い方はCreate()とDestroy()の呼び出しの間にOutPutStringを呼び出すだけ
 */

class CDebugWindow : public CWindowImpl<CDebugWindow> {
	typedef CWindowImpl<CDebugWindow>  baseclass;
  #ifndef NDEBUG
	CEdit					m_wndEdit;
	static CDebugWindow*	s_pThis;
  #endif

private:
	LRESULT 	OnCreate(LPCREATESTRUCT lpcs);
	LRESULT 	OnSize(UINT, CSize size);

public:
	void		Create();
	void		Destroy();

	static void OutPutString(CString strText, bool bReturn = true);


	BEGIN_MSG_MAP( CDebugWindow )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_SIZE  ( OnSize	)
	END_MSG_MAP()
};


#ifdef NDEBUG	//+++ リリース時用を用意.
inline LRESULT	CDebugWindow::OnCreate(LPCREATESTRUCT) { return 0; }
inline LRESULT	CDebugWindow::OnSize(UINT, CSize) { return 0; }
__inline void	CDebugWindow::Create()	{}
__inline void	CDebugWindow::Destroy() {}
__inline void	CDebugWindow::OutPutString(CString, bool) {}
#endif

