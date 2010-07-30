#pragma once

#include "resource.h"
#include <atlctrls.h>
#include <atldlgs.h>
#include <stdio.h>



class CMemo : public CDialogImpl<CMemo>, public CMessageFilter {
public:
	enum { IDD = IDD_DIALOG_MAIN }; //ダイアログリソースを元にウィンドウを作成
	enum { s_cxyBorder = 2 };

	CEdit 	m_Edit;


	BOOL PreTranslateMessage(MSG * /*pMsg*/)
	{
		return FALSE;
	}


	//メッセージマップ
	BEGIN_MSG_MAP(CMemo)
		COMMAND_ID_HANDLER(ID_MEMO_FILE_NEW , OnFileNew )
		COMMAND_ID_HANDLER(ID_MEMO_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_MEMO_FILE_SAVE, OnFileSave)
		MESSAGE_HANDLER(WM_SIZE		 , OnSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()


	//ダイアログが作成される
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		m_Edit = GetDlgItem(IDC_EDIT);
		return 0;
	}


	//ウィンドウの大きさを変更するよう要求された
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL & /*bHandled*/)
	{
		int   Width  = LOWORD(lParam);
		int   Height = HIWORD(lParam);

		CEdit edit	 = GetDlgItem(IDC_EDIT);

		edit.MoveWindow( s_cxyBorder, s_cxyBorder, Width - s_cxyBorder, Height - s_cxyBorder, TRUE);
		return 0;
	}


	//新規作成
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		ResetProcedure();
		return 0;
	}


	//開く
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( !ResetProcedure() )
			return 0;

		CFileDialog fdlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
						 _T("テキストファイル(*.txt)\0*.txt\0全てのファイル(*.*)\0*.*\0"), m_hWnd);

		if (fdlg.DoModal() == IDOK) {
			ReadFromFile(fdlg.m_szFileName);
		}

		return 0;
	}


	//名前をつけて保存
	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		CFileDialog fdlg(FALSE, _T("txt"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
						 _T("テキストファイル(*.txt)\0*.txt\0全てのファイル(*.*)\0*.*\0"), m_hWnd);

		if (fdlg.DoModal() == IDOK) {
			SaveToFile(fdlg.m_szFileName);
		}

		return 0;
	}


	//戻り値 IDYES - 保存を必要とする
	//		 IDNO  - 保存しない
	//		 IDCANCEL - 取り消し
	int _QueryResetData()
	{
		if ( m_Edit.GetModify() ) {
			return MessageBox(_T("内容が変更されています。変更を保存しますか？"), _T("Memoプラグイン"), MB_YESNOCANCEL);
		}

		return IDNO;
	}


	//エディットコントロールの初期化
	void _ResetData()
	{
		m_Edit.SetWindowText( _T("") );
		m_Edit.EmptyUndoBuffer();
		m_Edit.SetModify(FALSE);
	}


	//新規作成・開く時の手続き
	BOOL ResetProcedure()
	{
		int nRet = _QueryResetData();

		if (nRet == IDCANCEL) {
			return FALSE;
		} else if (nRet == IDYES) {
			CFileDialog fdlg(FALSE, _T("txt"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
							 _T("テキストファイル(*.txt)\0*.txt\0全てのファイル(*.*)\0*.*\0"), m_hWnd);

			if (fdlg.DoModal() == IDOK) {
				SaveToFile(fdlg.m_szFileName);
			} else {
				return FALSE;
			}
		}

		_ResetData();
		return TRUE;
	}


	//ファイルの読み込み　32KBまで
	void ReadFromFile(LPCTSTR lpstrFile)
	{
		int   	bufsize = 2 << 15;
		char*	buf	   	= new char[bufsize + 1];		//+++ +1を追加.(readcout==bufsizeの時、終端\0のためのメモリが範囲外になるので、\0用のメモリを確保.)
		int    	readcount;

		FILE*	fp	   	= _tfopen( lpstrFile, _T("rb") );

		if (fp == NULL) {
			MessageBox( _T("ファイルの読み込みに失敗しました。") );
			goto EXIT;
		}

		//適当に読み込みます
		readcount	   = fread(buf, 1, bufsize, fp);
		buf[readcount] = '\0';
	  #ifdef UNICODE	//+++ 読み込まれたのはsjisテキストなので、変換が必要. でsjis->utf16変換をCStringに任せる
		{
			CString	strBuf(buf);
			if ( !m_Edit.SetWindowText(strBuf) ) {
				MessageBox( _T("ファイルの読み込みに失敗しました。") );
				goto EXIT;
			}
		}
	  #else
		//面倒なのでUNICODEとか考えない
		if ( !m_Edit.SetWindowText(buf) ) {
			MessageBox( _T("ファイルの読み込みに失敗しました。") );
			goto EXIT;
		}
	  #endif

	  EXIT:
		if (fp)
			fclose(fp);

		if (buf)
			delete[] buf;
	}


	//ファイルに保存
	BOOL SaveToFile(LPCTSTR lpstrFile)
	{
		BOOL		 bRet	= TRUE;
		unsigned int length = m_Edit.GetWindowTextLength();

		if (length == 0)
			return FALSE;

		TCHAR*		 buf	= new TCHAR[length + 1 + 16];
		FILE *		 fp 	= _tfopen( lpstrFile, _T("wb") );
		if (fp == NULL) {
			MessageBox( _T("ファイルの保存に失敗しました。") );
			bRet = FALSE;
			goto EXIT;
		}

		m_Edit.GetWindowText(buf, length + 1);			//+++ UNICODE対応していない状態...

	  #ifdef UNICODE
		{
			const int	 BUF2SZ = length * 2 + 1 + 15;
			char*		 buf2	= new char[ BUF2SZ ];
			::WideCharToMultiByte(932, 0, buf, length+1, buf2, BUF2SZ, 0, 0);
			length  = strlen(buf2);
			bool rc = fwrite(buf2, 1, length, fp) == length;
			delete[] buf2;
			if (rc == 0) {
				MessageBox( _T("ファイルの保存に失敗しました。") );
				bRet = FALSE;
				goto EXIT;
			}
		}
	  #else
		if (fwrite(buf, 1, length, fp) != length) {
			MessageBox( _T("ファイルの保存に失敗しました。") );
			bRet = FALSE;
			goto EXIT;
		}
	  #endif

	  EXIT:
		if (fp)
			fclose(fp);

		if (buf)
			delete[] buf;

		return bRet;
	}
};
