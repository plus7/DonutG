

#if !defined(AFX_MYDIALOG_H__D05BCCEC_0364_40C9_8C38_E66140DC55DB__INCLUDED_)
#define AFX_MYDIALOG_H__D05BCCEC_0364_40C9_8C38_E66140DC55DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDialog.h : ヘッダー ファイル
//

#pragma warning (disable: 4786)
#undef new


#define USE_IRIA
#define USE_IRVINE
#ifndef WIN64
#define USE_DCSAKURA
#define USE_FLASHGET
#define USE_NETTRANS
//#define USE_REGETPRO
#endif

#include <vector>
#include <string>
#include <map>
#include <atlbase.h>
#include "Mshtml.h"

#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <cstdio>
#include <stdlib.h>

#include "objbase.h"

//unDonut.exeのパスを指定してください
//#import "C:\\Program Files\\unDonut\\unDonut.exe" no_namespace named_guids
#import "../../../ExecuteEnv/unDonut.exe" no_namespace named_guids


//各種ダウンローダのパスを指定してください
//IriaとIrvineはそのままインポートしたタイプライブラリを使うとエラーが出ることがあるので
//エラーを修正してから#importなしで直接#includeする
#ifdef USE_IRIA
//#import "C:\Program Files\iria\iria.exe" no_namespace named_guids
//#import "C:\\tool\\iria\\iria.exe" no_namespace named_guids
#include "iria.tlh"
#endif

#ifdef USE_IRVINE
//#import "C:\\Program Files\\irvine\\irvine.exe" no_namespace named_guids
//#import "C:\\tool\\irvine\\irvine.exe" no_namespace named_guids
#include "irvine.tlh"
#endif

#ifdef USE_DCSAKURA
//#import "C:\\Program Files\\DC-Sakura\\api.dll" no_namespace named_guids
//#import "C:\\Program Files (x86)\\DC-Sakura\\api.dll" no_namespace named_guids
#include "DcSakuraApi.tlh"
#endif

#ifdef USE_FLASHGET
//#import "C:\Program Files\FlashGet\Jccatch.dll" no_namespace named_guids
//#import "C:\Program Files (x86)\FlashGet\Jccatch.dll" no_namespace named_guids
#include "jccatch.tlh"
#endif

#ifdef USE_NETTRANS	//+++ 実は、この情報自体は使わずにスクリプト起動で実装されている... が、最近のNet Transportだと使えない模様...
//#import "C:\Program Files\Xi\Net Transport\NTIEHelper.dll" no_namespace named_guids
//#import "C:\Program Files (x86)\Xi\NetXfer\NXIEHelper.dll" no_namespace named_guids
#include "NXIEHelper.tlh"
#endif

#ifdef USE_REGETPRO
#import "C:\Program Files\ReGetPro\regetpro.exe" no_namespace named_guids
#endif



using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CMyDialog ダイアログ

class CMyDialog : public CDialog
{
// コンストラクション
public:
	enum { IDD = IDD_DIALOG1 };

	void DoFilter();
	CMyDialog(CWnd* pParent = NULL);   // 標準のコンストラクタ
	BOOL InitComboFrame();
	void AddFrameLinksToList(IHTMLFramesCollection2 *frameCol,int *totalframe);
	void AddMainLinksToList(IHTMLDocument2 *mainDoc);
	void AddLink_Anchor(IHTMLElementCollection *elemCol, int Num);
	void AddLink_Image(IHTMLElementCollection *elemCol, int Num);
	void AddItem(LPCTSTR str,int Num, int img = 0);
	void ShowItem(int FrameNum);
	void InsertItemToList(int count, string *str);
	void GetRect(LPRECT rec);

	void ParseString(vector<string> *ary,string *str);
	bool GetFilterTexts(DWORD CmbID,vector<string> *ary);
	bool CheckIncludeExt(string *str,vector<string> *ary);
	CWnd* GetEditWnd(CComboBox* Parent);

	bool SaveURLList(LPCTSTR strFN);	//+++ 引数修正
	void AdjustButtonState();
	void ShowItemCount();
	bool GetTempName(string *str,const char* ext);

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareFunc2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void RenumberItem();

	int StrMatch( const char *ptn, const char *str );
	BOOL CheckSelectedUse();
	BOOL IsSelected(int index);
	void OnOpen_Sub(BOOL bForceFlag, BOOL bSelected);

	void SendToDownloader();									//+++
	void autoSendToDownloader() { m_autoDownloader = true; }	//+++


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

protected:
	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CMyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCloseDlg();
	afx_msg void OnFilter();
	afx_msg void OnListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAllSelect();
	afx_msg void OnSelectRelease();
	afx_msg void OnSelectReverse();
	afx_msg void OnExcludeNotSelect();
	afx_msg void OnExcludeSelect();
	afx_msg void OnSave();
	afx_msg void OnCopy();
	afx_msg void OnIria();
	afx_msg void OnIrvine();
	afx_msg void OnDcSakura();
	afx_msg void OnFlashGet();
	afx_msg void OnNetTrans();
	afx_msg void OnReget();						//+++
	afx_msg void OnOpen();
	afx_msg void OnHdrClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnSelChangeFrameCmb();
	afx_msg void OnSelected();
	afx_msg void OnSetFocusList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnImgExtract();
	afx_msg void OnSelectedItemOpen();
	afx_msg void OnAllItemOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  #if 1	//+++
	void 	WriteProfile();
	void 	InitDownloader();
	void	DoDownloader( const CString& name );
	void 	DoIrvine();
	void 	DoIria();
	void 	DoDcSakura();
  #endif


// インプリメンテーション
protected:
	// ダイアログ データ
	//{{AFX_DATA(CMyDialog)
	enum { BUF_SIZE = 0x4000 };	//+++
	//}}AFX_DATA
	typedef map<string,vector<int> >	SviMap;

	SviMap		m_map;
	CButton		m_Selected;
	CComboBox 	m_cmbFrame;
	CComboBox 	m_cmbExt;
	CComboBox 	m_cmbNExt;
	CListCtrl 	m_List;

  #if 1	//+++ Sボタン追加関係
	CString		m_dir;
	CString		m_baseName;
	CString		m_iniFile;
	CButton		m_chkSbtn;
	CButton		m_ImageChk;
	CButton		m_chkRegex;
	CButton		m_chkSave;
	CEdit		m_editSearch;
	CComboBox	m_cmbSbtnSel;
	bool		m_autoDownloader;
	int			m_nSbtnDownloader;
	unsigned	m_downloaderSize;
	std::vector< CString > 	m_downloader;
  #endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MYDIALOG_H__D05BCCEC_0364_40C9_8C38_E66140DC55DB__INCLUDED_)
