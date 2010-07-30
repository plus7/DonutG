#ifndef __BOSUKE_H_NEMUCHECKER__
#define __BOSUKE_H_NEMUCHECKER__

#import "CheckSrv.tlb" no_namespace named_guids
#include <string>
#include <vector>

//+++ undonut+modの開発環境に変更.
//x #import "..\\Donut\\Debug\\DonutP.exe" no_namespace named_guids
#ifdef _WIN64
#import "..\\..\\und\\executeenv\\64unDonut.exe" no_namespace named_guids
#else
#import "..\\..\\und\\executeenv\\unDonut.exe" no_namespace named_guids
#endif

//HPがアップデートされた時に送るウインドウメッセージ
#define WM_NEMU_HPUPDATE	(WM_USER + 1000)

//Nemuイベントが起こった時に送るウインドウメッセージ
#define WM_NEMU_EVENT		(WM_USER + 1001)

//Nemuイベント種類
#define NEMUEVENTID_ALLCHECKDONE  (1)	//全アイテム強制チェック終了
#define NEMUEVENTID_NOUPDATEITEM  (2)	//更新されたアイテムが無くなった

//ボタンのID
#define ID_CMD_NEMU		23 

#include "AtlMisc.h"
#include "ExDisp.h"
////////////////////////////////////////////////////////////////////////////
// CNemuChecker  ChercSrvからのイベントを受け取る
#define CHECKSINKID_COUNTEREVENTS 0

class ATL_NO_VTABLE CDonutPEvents :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispEventImpl<CHECKSINKID_COUNTEREVENTS, CDonutPEvents,&DIID_IDonutPEvents,
		&LIBID_DONUTPLib,1,0>
{
public:
	CDonutPEvents(){};

	BEGIN_COM_MAP(CDonutPEvents)
		COM_INTERFACE_ENTRY_IID(DIID_IDonutPEvents,CDonutPEvents)
	END_COM_MAP()

	BEGIN_SINK_MAP(CDonutPEvents)
		SINK_ENTRY_EX(CHECKSINKID_COUNTEREVENTS, DIID_IDonutPEvents, 1, DocumentComplete)
	END_SINK_MAP()

    STDMETHODIMP DocumentComplete (long nIndex, IDispatch* pDisp, BSTR bstrURL);
};


class ATL_NO_VTABLE CUpdateEvent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispEventImpl<CHECKSINKID_COUNTEREVENTS,CUpdateEvent,&DIID__IUpdateCheckerEvents,
							&LIBID_CHECKSRVLib,1,0>
{
public:
	CUpdateEvent():m_hWnd(NULL) {};
	virtual ~CUpdateEvent(){
		DispEventUnadvise(m_pChecker);
	}

	BEGIN_COM_MAP(CUpdateEvent)
		COM_INTERFACE_ENTRY_IID(DIID__IUpdateCheckerEvents,CUpdateEvent)
		COM_INTERFACE_ENTRY_IID(DIID__IUpdateCheckerEvents,CUpdateEvent)
	END_COM_MAP()

	BEGIN_SINK_MAP(CUpdateEvent)
		SINK_ENTRY_EX(CHECKSINKID_COUNTEREVENTS,DIID__IUpdateCheckerEvents,1,OnHpChange)
		SINK_ENTRY_EX(CHECKSINKID_COUNTEREVENTS,DIID__IUpdateCheckerEvents,2,OnNemuEvent)
	END_SINK_MAP()

	HRESULT SetAdvise(IUpdateChecker* checker,HWND hwnd)
	{
		//セット
		m_pChecker = checker;
		m_hWnd = hwnd;
		return DispEventAdvise(m_pChecker);
	}

	HRESULT _stdcall OnHpChange(IUpdateChecker* checker)
	{
		//更新されたことをメインウインドウに通達する
		if(m_hWnd != NULL)
			::PostMessage(m_hWnd,WM_NEMU_HPUPDATE,0,0);
		return S_OK;
	}

	HRESULT _stdcall OnNemuEvent(long id,long param)
	{
		if(m_hWnd != NULL)
			::PostMessage(m_hWnd,WM_NEMU_EVENT,id,param);
		return S_OK;
	}

private:

	CComPtr<IUpdateChecker> m_pChecker;
	HWND m_hWnd;
};

/////////////////////////////////////////////////////////////////////////////
// CNemuCombo　WTLさっぱり分からないのでATL/SDKで実装・・・・
class CNemuCombo : public CWindowImpl<CNemuCombo>
{
public:

	DECLARE_WND_SUPERCLASS(NULL,WC_COMBOBOXEX)

	BEGIN_MSG_MAP(CNemuCombo)
	END_MSG_MAP()

	CNemuCombo():m_hImageList(NULL){};
	virtual ~CNemuCombo()
	{
		if(m_hImageList != NULL){
			ImageList_Destroy(m_hImageList);
		}
	}
	
	bool Create(HWND hParent)
	{
		//コンボボックスとイメージリストを作成する

		RECT rect = {0,0,0,200};
		if(CWindowImpl<CNemuCombo>::Create(hParent,rect,NULL,WS_CHILD | WS_VISIBLE | 
			CBS_DROPDOWNLIST |CBS_AUTOHSCROLL) == NULL){
			return false;
		}

		HINSTANCE hInst = _Module.GetResourceInstance();
		HICON hNormal = LoadIcon(hInst,MAKEINTRESOURCE(IDI_IEICON_NORMAL));
		HICON hNew = LoadIcon(hInst,MAKEINTRESOURCE(IDI_IEICON_NEW));
		HICON hErr = LoadIcon(hInst,MAKEINTRESOURCE(IDI_IEICON_ERR));
		m_hImageList = ImageList_Create(16,15,ILC_COLOR16,3,0);
		ImageList_SetBkColor(m_hImageList,RGB(255,255,255));
		ImageList_AddIcon(m_hImageList,hNormal);
		ImageList_AddIcon(m_hImageList,hNew);
		ImageList_AddIcon(m_hImageList,hErr);
		DestroyIcon(hNormal);
		DestroyIcon(hNew);
		DestroyIcon(hErr);

		SendMessage(CBEM_SETIMAGELIST,0,(LPARAM)m_hImageList);
		SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));

		return true;

	}

	int GetCurSel()
	{ 
		return (int)SendMessage(CB_GETCURSEL, 0, 0);
	}
	int SetCurSel(int index) 
	{ 
		return (int)SendMessage(CB_SETCURSEL,index,0);
	}

	bool AppendItem(LPCTSTR lpszName,int status)
	{
		//アイテムを追加する
		COMBOBOXEXITEM citem;
		citem.mask = CBEIF_TEXT;
		if(status != -1){
			citem.mask |= CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		}
		citem.pszText = (LPTSTR)lpszName;
		citem.iItem = -1;
		if(status == 0){
			//アップデートアイテム
			citem.iImage = 1;
		}else if(status == 4 || status == 5){
			//エラーアイテム
			citem.iImage = 2;
		}else{
			//通常
			citem.iImage = 0;
		}
		citem.iSelectedImage = citem.iImage;
		return (SendMessage(CBEM_INSERTITEM,0,(LPARAM)&citem) != -1);
	}
	void Clear()
	{ 
		SendMessage(CB_RESETCONTENT,0,0);
	}
	bool GetItemText(int index,LPTSTR lpszText)
	{
		return (SendMessage(CB_GETLBTEXT,index,(LPARAM)lpszText) != CB_ERR);
	}

private:
	HIMAGELIST m_hImageList;
};

////////////////////////////////////////////////////////////////////////////
// NemuBar 　(同じくWTLさっぱり・・)
class CNemuToolBar : public CWindowImpl<CNemuToolBar>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, TOOLBARCLASSNAME)

	CComObject<CDonutPEvents>* m_pDonutPEven;
		CNemuToolBar():m_hImageList(NULL),m_pDonutP(NULL),m_pDonutPEven(NULL){}
	virtual ~CNemuToolBar()
	{
		if(IsWindow()){
			DestroyWindow();
		}
		if(m_hImageList != NULL){
			ImageList_Destroy(m_hImageList);
		}
	}

	BEGIN_MSG_MAP(CNemuToolBar)
	CHAIN_MSG_MAP_MEMBER(m_ComboWnd)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_NEMU_HPUPDATE,OnHpUpdate)
	MESSAGE_HANDLER(WM_NEMU_EVENT,OnNemuEvent)
	COMMAND_CODE_HANDLER(CBN_DROPDOWN,OnComboDropDown)
	COMMAND_CODE_HANDLER(CBN_SELENDOK,OnComboSelect)
	COMMAND_CODE_HANDLER(CBN_SELENDCANCEL,OnComboSelCancel)
	COMMAND_CODE_HANDLER(0,OnMenuCommand)
	END_MSG_MAP()

	
	//NemuBarの初期化と解放
	static bool InitNemuBar()
	{
		if(CoCreateInstance(CLSID_UpdateChecker,NULL,CLSCTX_ALL,
			IID_IUpdateChecker,(void**)&m_pChecker) == S_OK){
			m_pChecker->StartCheck();
			return true;
		}
		return false;
	}
	static void UninitNemuBar()
	{
		if(m_pChecker != NULL){
			m_pChecker->Release();
			m_pChecker = NULL;
		}
	}

	
	HWND Create(HWND hwndParent,UINT id)
	{
		//ツールバーの作成
		RECT rc = {0,0,200,20};
		DWORD style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_TABSTOP |
						TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_LIST | TBSTYLE_CUSTOMERASE |
                         TBSTYLE_WRAPABLE | CCS_TOP | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE;
		if(CWindowImpl<CNemuToolBar>::Create(hwndParent,rc,NULL,style,0,id)){
			return m_hWnd;
		}
		
		return NULL;
	}

	static void NotifyViewPage(LPCTSTR lpszUrl)
	{
		//NemuBarにページを見たことを通知する
		m_pChecker->NavigateComplete(lpszUrl);

		//ボタン変化中であれば更新アイテムがまだあるかチェックする
		if(m_bUpdate){
			if(m_pChecker->UpdateItemCount == 0){
				//更新アイテムが無くなった
				m_bUpdate = false;
				m_pChecker->SendNemuEvent(NEMUEVENTID_NOUPDATEITEM,0);
			}
		}

	}

	void ShowMenu(WPARAM iItem)
	{
		//NemuBarのボタンが押されたんでメニューを表示する
			
		//座標を設定
		RECT rc;
		SendMessage(TB_GETRECT,iItem,(LPARAM)&rc);
		::MapWindowPoints(m_hWnd,HWND_DESKTOP,(LPPOINT)&rc,2);
		TPMPARAMS tpm;
		tpm.cbSize = sizeof(TPMPARAMS);
		tpm.rcExclude.top = rc.top;
		tpm.rcExclude.left = rc.left;
		tpm.rcExclude.bottom = rc.bottom;
		tpm.rcExclude.right = rc.right;

		//メニューを表示
		HMENU hMenu, hPopup;
		hMenu = LoadMenu(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_MENU_NEMUBAR));
		hPopup = GetSubMenu(hMenu,0);
		TrackPopupMenuEx(hPopup,TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
			rc.left,rc.bottom,m_hWnd,&tpm);
		DestroyMenu(hMenu);

	}

	void ShowSettingDlg()
	{
		//設定画面表示
		m_pChecker->ShowSettingDlg();
	}

protected:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//NemuBarのイベントを受ける準備
		CComObject<CUpdateEvent>::CreateInstance(&m_pSink);
		m_pSink->SetAdvise(m_pChecker,m_hWnd);

		// DonutPのイベントを受ける準備
		if ( m_pDonutP==NULL )
		{
			CoCreateInstance( CLSID_API, NULL, CLSCTX_SERVER, IID_IAPI, (void**)&m_pDonutP );
			CComObject<CDonutPEvents>::CreateInstance( &m_pDonutPEven );
			m_pDonutPEven->DispEventAdvise( m_pDonutP );
		}
		
		//ツールバーの設定とイメージリストの作成
		SendMessage(TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
		SendMessage(TB_SETMAXTEXTROWS,1,0);
		SendMessage(TB_SETEXTENDEDSTYLE,0,TBSTYLE_EX_DRAWDDARROWS);

		HICON hIcon = LoadIcon(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDI_NEMUICON_NORMAL));
		HICON hIconNew = LoadIcon(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDI_NEMUICON_NEW));
		m_hImageList = ImageList_Create(48,16,ILC_COLOR16 | ILC_MASK ,2,0);
		ImageList_SetBkColor(m_hImageList,CLR_NONE);
		
		ImageList_AddIcon(m_hImageList,hIcon);
		ImageList_AddIcon(m_hImageList,hIconNew);
		DestroyIcon(hIcon);
		DestroyIcon(hIconNew);
		SendMessage(TB_SETIMAGELIST,0,(LPARAM)m_hImageList);

		//ボタンの作成
		TBBUTTON button;
		ZeroMemory(&button,sizeof(TBBUTTON));
		button.idCommand = ID_CMD_NEMU;
		button.fsState = TBSTATE_ENABLED;
		button.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_WHOLEDROPDOWN;
		//更新されているアイテムがあるか否かでICONを変化させる
		m_bUpdate = (m_pChecker->UpdateItemCount != 0);
		button.iBitmap = m_bUpdate ? 1 : 0;
		SendMessage(TB_INSERTBUTTON,0,(LPARAM)&button);

		//コンボボックスの作成
		if(!m_ComboWnd.Create(m_hWnd))
			return -1;

		//NemuBarの文字を追加しておく
		m_ComboWnd.AppendItem(_T("-- NemuBar --"),-1);
		m_ComboWnd.SetCurSel(0);

		return 0;
	}


	LRESULT OnComboDropDown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//コンボボックスの展開　登録アイテムをガシガシついか
		if(hWndCtl == m_ComboWnd.m_hWnd){
			//アイテムを取得
			IUpdateItemCollectionPtr pCol;
			pCol = m_pChecker->Items;
			pCol->Sort(0);

			//ガシガシ追加
			long count = pCol->Count;
			if(count){
				//今のクリア
				m_ComboWnd.Clear();
				m_vJumpUrl.clear();
				
				m_vJumpUrl.reserve(count);
				for(long i = 0; i < count; i++){
					IUpdateItemPtr pItem = pCol->Item[i];
					if(m_ComboWnd.AppendItem(pItem->Name,pItem->Status)){
						m_vJumpUrl.push_back(std::string(pItem->OpenUrl));
					}
				}
				m_ComboWnd.SetCurSel(-1);
			}
		}
		return 0;
	}


	LRESULT OnComboSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//コンボボックスのアイテムを選択した
		if(hWndCtl == m_ComboWnd.m_hWnd){
			int index = m_ComboWnd.GetCurSel();
			if(index != CB_ERR && index < m_vJumpUrl.size()){
				std::string& tmp = m_vJumpUrl[index];

				CComBSTR bstrURL(tmp.c_str());
				if ( m_pDonutP )
					m_pDonutP->NewWindow(tmp.c_str(), TRUE);
			}
		}
		m_vJumpUrl.clear();
		
		return 0;
	}

	LRESULT OnComboSelCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//コンボボックスの選択をキャンセル
		m_vJumpUrl.clear();
		m_ComboWnd.Clear();

		m_ComboWnd.AppendItem(_T("-- NemuBar --"),-1);
		m_ComboWnd.SetCurSel(0);

		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//コンボボックスをクライアント領域一杯に
		
		RECT wndRect,btnRect;
		GetClientRect(&wndRect);
		SendMessage(TB_GETITEMRECT,0,(LPARAM)&btnRect);
		
		RECT comboRect;
		comboRect.top = 0;
		comboRect.bottom = 22;
		comboRect.left = wndRect.left + (btnRect.right - btnRect.left);
		comboRect.right = wndRect.right;
		m_ComboWnd.MoveWindow(&comboRect,TRUE);

		return 0;
	}

	LRESULT OnHpUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		//HP更新された
		TBBUTTON button;
		SendMessage(TB_GETBUTTON,0,(LPARAM)&button);
		if(button.iBitmap != 1){
			button.iBitmap = 1;
			SendMessage(TB_DELETEBUTTON,0,0);
			SendMessage(TB_INSERTBUTTON,0,(LPARAM)&button);
		}
		m_bUpdate = true;
		return 0;
	}

	LRESULT OnNemuEvent(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		//Nemuイベント
		if(wParam == NEMUEVENTID_NOUPDATEITEM){
			//更新アイテムをすべてみた アイコンの色を元に戻す
			TBBUTTON button;
			SendMessage(TB_GETBUTTON,0,(LPARAM)&button);
			if(button.iBitmap == 1){
				button.iBitmap = 0;
				SendMessage(TB_DELETEBUTTON,0,0);
				SendMessage(TB_INSERTBUTTON,0,(LPARAM)&button);
			}
			m_bUpdate = false;
		}else if(wParam == NEMUEVENTID_ALLCHECKDONE){
			//チェック終了表示	Egamin
			m_vJumpUrl.clear();
			m_ComboWnd.Clear();
			m_ComboWnd.AppendItem(_T("-- AllCheckDone --"),-1);
			m_ComboWnd.SetCurSel(0);
		}
		return 0;
	}

	LRESULT OnMenuCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//メニューのアイテムが押された
		if(wID == ID_NEMU_OPENSETTING){
			//設定画面表示
			m_pChecker->ShowSettingDlg();
		}else if(wID == ID_NEMU_ADDITEM){
			//新しいアイテムの追加
			m_pChecker->AddNewItem("","","");
		}else if(wID == ID_NEMU_ALLOPEN){
			//全更新アイテムを一気に開く
			
			//アイテムを取得
			IUpdateItemCollectionPtr pCol;
			pCol = m_pChecker->UpdateItems;

			//ばしばし開く
			long count = pCol->Count;
			for(long i = 0; i < count; i++){
				IUpdateItemPtr pItem = pCol->Item[i];
				if ( m_pDonutP )
					m_pDonutP->NewWindow((LPCTSTR)pItem->OpenUrl, FALSE);
//				DonutOpenFile(GetParent(), (LPCTSTR)pItem->OpenUrl, DonutGetStdOpenFlag());
			}
		}else if(wID == ID_NEMU_STARTALLCHECK){
			//全アイテム強制チェック	Egamin
			m_vJumpUrl.clear();
			m_ComboWnd.Clear();
			m_ComboWnd.AppendItem(_T("-- NowAllChecking --"),-1);
			m_ComboWnd.SetCurSel(0);
			m_pChecker->StartAllCheck();
		}else if(wID == ID_NEMU_ADDITEMURL){
			//このURLをアイテムに追加	Egamin
			CComPtr<IDispatch> pDisp;
			if( m_pDonutP )
				pDisp = m_pDonutP->GetWebBrowserObject( m_pDonutP->GetTabIndex() );
			if( pDisp ){
				CComPtr<IWebBrowser2> spBrowser;
				CComBSTR bstr;
				pDisp->QueryInterface(IID_IWebBrowser2, (void**)&spBrowser);
				spBrowser->get_LocationURL(&bstr);
				CString strURL(bstr);
				spBrowser->get_LocationName(&bstr);
				CString strTITLE(bstr);
				m_pChecker->AddNewItem((LPCTSTR)strTITLE,(LPCTSTR)strURL,(LPCTSTR)strURL);
			}else{
				m_pChecker->AddNewItem("","","");
			}
		}else if(wID == ID_NEMU_ITEMVIEWBROWSER){
			//各アイテムの状況をブラウザで見る	Egamin
			if ( m_pDonutP )
					m_pDonutP->NewWindow((LPCTSTR)m_pChecker->GetStatusFilePath(), TRUE);
		}
		return 0;
	}

	//コンボボックス
	CNemuCombo m_ComboWnd;

	//ジャンプ先URL
	std::vector<std::string> m_vJumpUrl;

	//NemuBar
	static IUpdateChecker* m_pChecker;
	static bool m_bUpdate;
	CComObject<CUpdateEvent>* m_pSink;

	// DonutP
	CComPtr<IAPI> m_pDonutP;

	//イメージリスト
	HIMAGELIST m_hImageList;
};

#endif
