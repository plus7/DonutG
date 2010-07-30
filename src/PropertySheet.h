/**
 *	@file	PropertySheet.h
 *	@brief	ツリービュー形式でオプションを管理するためのダイアログ
 *			リソーステンプレートに依存した処理があるため汎用性は非常に低くなっています。
 */

#ifndef __PROPERTY_SHEET_H_
#define __PROPERTY_SHEET_H_


class CTreeViewPropertySheet : public CDialogImpl<CTreeViewPropertySheet> {
public:
	enum {
		IDD 		= IDD_PROPSHEET,
	};

private:
	enum {
		TITLELENGTH = 200,
		SPACE		= 2,
	};

	struct PAGEINFO {
		PROPSHEETPAGE	psp;
	  #if 1	//+++ UNICODE対応
		TCHAR			title[TITLELENGTH];
	  #else
		char			title[TITLELENGTH];
	  #endif
		HWND			hWnd;
		int 			nIndent;
	};


	CSimpleArray<CToolTipCtrl>		m_aryToolTip;
	CSimpleArray<PAGEINFO *>		m_aryPage;
	CString 						m_Title;
	CTreeViewCtrl					m_TreeView;
	HWND							m_hActiveWnd;

	int 							m_nWidth;
	int 							m_nHeight;
	bool							m_bInitPos;

	static HHOOK					s_hHook;
	static CTreeViewPropertySheet*	s_pThis;
	static int						m_nCurSel;


public:
	CTreeViewPropertySheet(LPCTSTR title = NULL)
	{
		m_Title    = title;
		m_nCurSel  = 0;
		m_bInitPos = false;
	  #if 1 //+++
		m_nWidth	= 0;
		m_nHeight	= 0;
		m_hActiveWnd= 0;
		m_bInitPos	= 0;
	  #endif
	}


	BOOL AddPage(PROPSHEETPAGE *pPage, int nIndent = -1)
	{
		ATLASSERT(pPage != NULL);

		PAGEINFO *ppi = new PAGEINFO;
		ppi->psp	 = *pPage;
		ppi->hWnd	 = NULL;
		ppi->nIndent = nIndent;
		m_aryPage.Add(ppi);
		return TRUE;
	}


	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL)
	{
		if (m_aryPage.GetSize() == 0)
			return 0;

		//キーアクセラレータ用にフックを準備
		s_pThis = this;

	  #ifndef NDEBUG	//+++ 追加.
		SetLastError(0);
	  #endif

		s_hHook = ::SetWindowsHookEx( WH_MSGFILTER, MsgFilterProc, _Module.GetResourceInstance(), ::GetCurrentThreadId() );

	  #ifndef NDEBUG	//+++ メモ: unDonut+4 で追加されたチェック. なぜか正常?な環境でも場合によっては、
						//+++ "指定されたリソース名がイメージファイルに見つかりません。"
						//+++ "このシステムではこの関数はサポートされません。"
						//+++ というエラーが出るらしい...ので、ここでの表示は無しにする.
		// オプションを表示すると落ちるらしいのでエラーを表示させてみる
		DWORD dw = GetLastError();
		if (dw != ERROR_SUCCESS) {
			LPTSTR lpBuffer;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, dw, LANG_USER_DEFAULT,
				// MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
				(LPTSTR) &lpBuffer, 0, NULL );
			::MessageBox(NULL, lpBuffer, _T("error message"), MB_ICONHAND | MB_OK);
		}
	  #endif

		return CDialogImpl<CTreeViewPropertySheet>::DoModal(hWndParent, dwInitParam);
	}


private:
	static LRESULT CALLBACK MsgFilterProc(int code, WPARAM wParam, LPARAM lParam)
	{
		if (code >= 0) {
			MSG *pMsg = (MSG *) lParam;

			if ( (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
			   || (WM_MOUSEFIRST <= pMsg->message && pMsg->message <= WM_MOUSELAST) )
			{
				HWND	  hWnd = s_pThis->_GetActivePage();

				if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) {
					HWND hWndFocus = ::GetFocus();
					HWND hWndDlg   = s_pThis->m_hActiveWnd;
					BOOL bPrevious = (::GetAsyncKeyState(VK_SHIFT) & 0x80000000) != 0;

					HWND hWndNext  = ::GetNextDlgTabItem(hWndDlg, hWndFocus, bPrevious);
					if (!hWndNext)
						hWndNext = ::GetNextDlgTabItem(hWndDlg, NULL, bPrevious);

				  #if 1 //+++	しょっぱなにSHIFT+TABで移動しようとすると、メインウィンドウにフォーカスが移ってしまう件を強引対処.
						//+++	※	...というか、donutのオプションを呼んだときのカーソル位置が"全般"の位置になっていないことが問題かも..
					if (!hWndNext && bPrevious) {
						hWndNext  = ::GetNextDlgTabItem(hWndDlg, NULL, 0);				//+++ まず先頭へ
						if (hWndNext) {
							::SetFocus(hWndNext);										//+++ フォーカスを移してから
							if (hWndNext) {
								hWndNext  = ::GetNextDlgTabItem(hWndDlg, hWndNext, 1);	//+++ 一つ戻る
							}
						}
					}
					if (hWndNext) {
						::SetFocus(hWndNext);
						return 1;
					}
				  #else
					::SetFocus(hWndNext);
					return 1;
				  #endif
				}

				//*+++ hWndの取得に失敗する状態があるので、暫定対処でチェック
				if (hWnd) {
					PSHNOTIFY	pn	= {0};
					pn.hdr.code 	= PSN_TRANSLATEACCELERATOR;
					pn.hdr.hwndFrom = s_pThis->m_hWnd;
					pn.hdr.idFrom	= 0;
					pn.lParam		= lParam;
					LRESULT   nRet = ::SendMessage(hWnd, WM_NOTIFY, (WPARAM) 0, (LPARAM) &pn);
					if (nRet)
						return nRet;
				}
			}
		}

		return ::CallNextHookEx(s_hHook, code, wParam, lParam);
	}


public:
	BEGIN_MSG_MAP(CTreeViewPropertySheet)
		MESSAGE_HANDLER( WM_INITDIALOG , OnInitDialog)
		MESSAGE_HANDLER( WM_DESTROY    , OnDestroy	 )
		MESSAGE_HANDLER( WM_SIZE	   , OnSize 	 )
		COMMAND_ID_HANDLER( IDOK	   , OnOk			)
		COMMAND_ID_HANDLER( IDCANCEL   , OnCancel		)
		COMMAND_ID_HANDLER( IDC_APPLY  , OnApply		)
	  #ifndef USE_DIET
		COMMAND_ID_HANDLER( IDC_TOOLTIP, OnCheckToolTip )
	  #endif
		//NOTIFY_HANDLER(IDC_TREEVIEW,TVN_SELCHANGED,OnSelChanged)
		NOTIFY_HANDLER(IDC_TREEVIEW, TVN_SELCHANGING, OnSelChanging)
		//+++ お試し...失敗... CHAIN_MSG_MAP(CDialogImpl<CTreeViewPropertySheet>)
	END_MSG_MAP()


private:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		//ダイアログテンプレートを少しいじくる
		_ReadyDialogResources();

		//ツリービューを準備する
		_InitTreeView();

	  #ifndef USE_DIET
		//ツールチップ表示の可否を取得
		CIniFileI	pr( g_szIniFileName, _T("Option" ) );
		DWORD		dwVal = pr.GetValue(_T("ToolTip"), 1/*0*/ );	//+++ デフォルトをonに変更.
		pr.Close();
		CheckDlgButton(IDC_TOOLTIP, dwVal);
	  #endif

		//最初のページを表示
		m_hActiveWnd = _LoadDialog(0);
		_BeChild(m_hActiveWnd, m_hWnd);
		::ShowWindow(m_hActiveWnd, SW_SHOW);
		NotifyToDialog(m_hActiveWnd, PSN_SETACTIVE);
		m_TreeView.Select(m_TreeView.GetFirstVisibleItem(), TVGN_CARET);

		MoveWindow(0, 0, m_nWidth + 180, m_nHeight + 70, TRUE);
		_SetTitle(0);

		::SetFocus(m_hActiveWnd);				//+++	カーソル位置を無理やり設定する...

		return 0;
	}


	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
	  #ifndef USE_DIET
		//OKキャンセルに関わらずチェック状態を保存
		DWORD		dwVal = (IsDlgButtonChecked(IDC_TOOLTIP) == BST_CHECKED);
		CIniFileO	pr( g_szIniFileName, _T("Option") );
		pr.SetValue( dwVal, _T("ToolTip") );
		pr.Close();
	  #endif
		::UnhookWindowsHookEx(s_hHook); //フック解除
		_ReleasePages();
		return 0;
	}


	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (!m_bInitPos) {
			CenterWindow( GetParent() );
			m_bInitPos = true;
		}

		CRect rc, rcb;
		GetClientRect(&rc);

		rc.right  -= SPACE;
		rc.left    = rc.right - m_nWidth;
		rc.top	   = SPACE * 5;
		rc.bottom  = m_nHeight + rc.top;
		CWindow(m_hActiveWnd).MoveWindow(&rc , TRUE);

		rc.right   = rc.left - SPACE;
		rc.left    = SPACE * 4;
		rc.bottom += 20;
		m_TreeView.MoveWindow(&rc, TRUE);

		GetClientRect(&rc);
		CWindow( GetDlgItem(IDC_APPLY) ).GetClientRect(&rcb);
		rc.right  -= SPACE * 10;
		rc.left    = rc.right - rcb.Width();
		rc.bottom -= SPACE * 3;
		rc.top	   = rc.bottom - rcb.Height();
		CWindow( GetDlgItem(IDC_APPLY) ).MoveWindow(&rc);
		rc.right   = rc.left - SPACE * 3;
		rc.left   -= rcb.Width();
		CWindow( GetDlgItem(IDCANCEL) ).MoveWindow(&rc);
		rc.right   = rc.left - SPACE * 3;
		rc.left   -= rcb.Width();
		CWindow( GetDlgItem(IDOK) ).MoveWindow(&rc);

		CWindow tooltip = GetDlgItem(IDC_TOOLTIP);
		CRect	rcc;
		tooltip.GetClientRect(rcc);
		rc.right   = rc.left - SPACE * 3;
		rc.left   -= rcc.Width() + 24;
		tooltip.MoveWindow(&rc);

		return S_OK;
	}


	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		if ( !_ApplyToDialogs() )
			return 0;

		EndDialog(wID);
		return 0;
	}


	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}


	LRESULT OnApply(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_ApplyToDialogs();
		return 0;
	}


  #ifndef USE_DIET
	LRESULT OnCheckToolTip(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
	{
		_ShowToolTip();
		return 0;
	}
  #endif

	BOOL _ApplyToDialogs()
	{
		int 		count	= m_aryPage.GetSize();
		PAGEINFO*	ppi 	= m_aryPage[m_nCurSel];

		//まずカレントタブに変更を通知してDDXなどに引っかからないか確認
		//カレントタブ以外はDDX_INT_RANGEなどに引っかかることはないのでまとめてforで処理
		if ( ::IsWindow(ppi->hWnd) )
			if (NotifyToDialog(ppi->hWnd, PSN_APPLY) != FALSE)
				return FALSE;

		for (int i = 0; i < count; i++) {
			if (m_nCurSel == i) continue;

			PAGEINFO *ppi = m_aryPage[i];

			if ( ::IsWindow(ppi->hWnd) )
				if (NotifyToDialog(ppi->hWnd, PSN_APPLY) != FALSE)
					return FALSE;

		}

		return TRUE;
	}


	LRESULT OnSelChanging(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		NMTREEVIEW *ptv 	 = (NMTREEVIEW *) pnmh;
		int 		newindex = (int) ptv->itemNew.lParam;
		int 		oldindex = (int) ptv->itemOld.lParam;

		if (newindex == oldindex) return 0;

		HWND		hOldWnd  = m_aryPage[oldindex]->hWnd;
		HWND		hNewWnd  = m_aryPage[newindex]->hWnd;

		if ( ::IsWindow(hOldWnd) ) {
			//古いのは隠す
			BOOL	bRet = NotifyToDialog(hOldWnd, PSN_KILLACTIVE);
			if (bRet != FALSE) {
				return 1; //キャンセルされた
			}

			::ShowWindow(hOldWnd, SW_HIDE);
		}

		if (hNewWnd == NULL) {
			//新しいのは必要なら作成する
			m_aryPage[newindex]->hWnd = _LoadDialog(newindex);
			_BeChild(m_aryPage[newindex]->hWnd, m_hWnd);
		}

		m_hActiveWnd = m_aryPage[newindex]->hWnd;
		_MoveChild(m_hActiveWnd);
		::ShowWindow(m_hActiveWnd, SW_SHOW);

		NotifyToDialog(m_hActiveWnd, PSN_SETACTIVE);
		_SetTitle(newindex);

		::SetFocus(m_hActiveWnd);
		m_nCurSel	 = newindex;

		return 0;
	}


	void _ReadyDialogResources()
	{
		int   max_w, max_h;
		int   count = m_aryPage.GetSize();

		max_w = max_h = 0;

		for (int i = 0; i < count; i++) {
			PAGEINFO *ppi  = m_aryPage[i];

			HRSRC	  hRes =  ::FindResource(_Module.GetResourceInstance(), ppi->psp.pszTemplate, RT_DIALOG);
			if (hRes) {
				HGLOBAL hMem = ::LoadResource(_Module.GetResourceInstance(), hRes);
				if (hMem) {
					LPBYTE pMem = (LPBYTE) ::LockResource(hMem);
					if (pMem) {
						//ダイアログのサイズを取得
						WCHAR *wpstr = (WCHAR *) pMem;
						if ( (*wpstr == 1) && (*(wpstr + 1) == 0xFFFF) ) {
							//拡張テンプレート
							DLGTEMPLATEEX_HALF *dpe = (DLGTEMPLATEEX_HALF *) pMem;

							if (max_w < dpe->cx)  max_w = dpe->cx;
							if (max_h < dpe->cy)  max_h = dpe->cy;

							wpstr += 13;

						} else {
							//普通のテンプレート
							DLGTEMPLATE *dp   = (DLGTEMPLATE *) pMem;

							if (max_w < dp->cx)  max_w = dp->cx;
							if (max_h < dp->cy)  max_h = dp->cy;

							char *		 pstr = (char *) ( pMem + sizeof (DLGTEMPLATE) );
							wpstr = (WCHAR *) pstr;
						}

						for (int j = 0; j < 2; j++) { //メニューやクラスなどの情報を読み飛ばす
							if (*wpstr == 0)			wpstr++;
							else if (*wpstr == 0xFFFF)	wpstr += 2;
							else						_SeekUnicode(&wpstr);
						}

						//キャプションを取得
					  #ifdef UNICODE	//+++
						::lstrcpyn(ppi->title, wpstr, TITLELENGTH);
					  #else
						::WideCharToMultiByte(CP_ACP, 0, wpstr, -1, ppi->title, TITLELENGTH, NULL, NULL);
					  #endif
					}
				}
			}
		}

		//ダイアログ単位からピクセル単位に変換
		CRect rc(0, 0, max_w, max_h);
		MapDialogRect(&rc);
		m_nWidth   = rc.Width();
		m_nHeight  = rc.Height();
	}


	void _SeekUnicode(WCHAR **wppstr)
	{
		WCHAR *wpstr = *wppstr;

		while (*wpstr != 0)
			wpstr++;

		*wppstr = ++wpstr;
	}


	void _InitTreeView()
	{
		m_TreeView = GetDlgItem(IDC_TREEVIEW);

		//登録
		HTREEITEM hParent = TVI_ROOT;
		HTREEITEM hItem   = TVI_ROOT;
		int 	  count   = m_aryPage.GetSize();

		for (int i = 0; i < count; i++) {
			PAGEINFO *ppi = m_aryPage[i];
			m_TreeView.Expand(hParent, TVE_EXPAND);

			if (ppi->nIndent == -1) {
				hItem	= m_TreeView.InsertItem(ppi->title, TVI_ROOT, TVI_LAST);
				hParent = TVI_ROOT;
			} else if (ppi->nIndent == 0) {
				hItem = m_TreeView.InsertItem(ppi->title, hParent, TVI_LAST);
			} else {
				hParent = hItem;
				hItem	= m_TreeView.InsertItem(ppi->title, hItem, TVI_LAST);
			}

			m_TreeView.SetItemData(hItem, i);
		}

		//設定
		m_TreeView.SetItemHeight(16);
		int 	  style   = m_TreeView.GetWindowLong(GWL_STYLE);
		style &= ~(WS_HSCROLL | WS_TABSTOP);
		m_TreeView.SetWindowLong(GWL_STYLE, style);
	}


	void _ReleasePages()
	{
		ShowWindow(SW_HIDE);

		int count = m_aryPage.GetSize();

		for (int i = 0; i < count; i++) {
			PAGEINFO *ppi = m_aryPage[i];

			if ( ::IsWindow(ppi->hWnd) )
				::DestroyWindow(ppi->hWnd);

			delete ppi;
		}

		m_aryPage.RemoveAll();
	}


	void _BeChild(HWND hWnd, HWND hParent)
	{
		CWindow Wnd   = hWnd;
		int 	style = Wnd.GetWindowLong(GWL_STYLE);

		style |= WS_CHILD | WS_GROUP | WS_TABSTOP;
		style &= ~(WS_POPUP | WS_CAPTION | WS_DISABLED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		Wnd.SetWindowLong(GWL_STYLE, style);
		Wnd.SetParent(hParent);
	}


	HWND _LoadDialog(int nIndex)
	{
		PAGEINFO *ppi = m_aryPage[nIndex];

		ATLASSERT(ppi->hWnd == NULL);

		//コールバックを送る
		ppi->psp.pfnCallback( NULL, PSPCB_CREATE, &(ppi->psp) );

		//ダイアログを作成
		ppi->hWnd = ::CreateDialog(_Module.GetModuleInstance(), ppi->psp.pszTemplate, GetParent(), ppi->psp.pfnDlgProc);

	  #ifndef USE_DIET
		//ツールチップを設定
		_SetToolTip(nIndex);
	  #endif

		return ppi->hWnd;
	}


	void _SetTitle(int index)
	{
		PAGEINFO *ppi = m_aryPage[index];
		CString   strTitle;

		strTitle.Format(_T("%s - %s"), m_Title, ppi->title);
		SetWindowText(strTitle);
	}


	BOOL NotifyToDialog(HWND hWnd, int code)
	{
		//ダイアログに各種通知メッセージを送る
		NMHDR nmh;

		nmh.code	 = code;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom	 = 0;
		ATLASSERT(hWnd != NULL);
		return ::SendMessage(hWnd, WM_NOTIFY, 0, (LPARAM) &nmh) != 0;
	}


	void _MoveChild(HWND hWnd)
	{
		CWindow Wnd = hWnd;
		CRect	rc;

		GetClientRect(&rc);
		rc.right -= SPACE;
		rc.left   = rc.right - m_nWidth;
		rc.top	  = SPACE;
		rc.bottom = m_nHeight + rc.top;
		Wnd.MoveWindow(&rc, TRUE);
	}


	typedef struct	DLGTEMPLATEEX_HALF {
		WORD	dlgVer;
		WORD	signature;
		DWORD	helpID;
		DWORD	exStyle;
		DWORD	style;
		WORD	cDlgItems;
		short	x;
		short	y;
		short	cx;
		short	cy;
	};	//DLGTEMPLATEEXの前半分


  #ifndef USE_DIET
	void _SetToolTip(int nIndex)
	{
	 #if _ATL_VER >= 0x700
		//Option.xmlをロード
		CComPtr<MSXML2::IXMLDOMDocument2> Doc;
		Doc.CoCreateInstance(CLSID_DOMDocument30);				//+++ xp,vistaに入っている版
		if (Doc.p == NULL) {
			Doc.CoCreateInstance(CLSID_DOMDocument40);			//+++ unDonut+で使われていた版
			if (Doc.p == NULL) {
				//Doc.CoCreateInstance(CLSID_DOMDocument60);	//+++ 実はちゃんとは機能していないようなので、止め.
				//if (Doc.p == NULL)
				//	Doc.CoCreateInstance(CLSID_DOMDocument20);	//+++ xp(32)で入ってる?
			}
		}
		//+++	ヘルプはでないけど、ハングはしないようにする.
		if (Doc.p == NULL)
			return;

		Doc->put_validateOnParse(VARIANT_FALSE);
		Doc->put_async(VARIANT_FALSE);

		CString 	strPath    = _GetFilePath( _T("help\\Option.xml") );
		if (Doc->load( _variant_t(strPath) ) == VARIANT_FALSE)		//+++ 実はmsxml6だとここのチェックに引っかかって終わってた...
			return;

		//ルート<option>を取得、<option>の子ノードを取得
		CComPtr<MSXML2::IXMLDOMElement>   root		   = Doc->GetdocumentElement();
		CComPtr<MSXML2::IXMLDOMNodeList>  rootChildren = root->GetchildNodes();
		PAGEINFO *						  ppi		   = m_aryPage[nIndex];
		int 							  nRootChildLen= rootChildren->Getlength(); 	//+++ デバッガで見やすいように一旦変数に入れる
		for (int i = 0; i < nRootChildLen; ++i) {
			// xmlからタイトルを取得
			CComQIPtr<MSXML2::IXMLDOMElement> elPage   = rootChildren->Getitem(i);
			_variant_t						  v 	   = elPage->getAttribute( _bstr_t( _T("description") ) );
			CString 						  xmlTitle = (LPCSTR) _bstr_t(v);		//+++ //*メモ UNICODE対策いる?いらない?

			//xmlから取得したタイトルとページのタイトルを比較
			if ( !xmlTitle.Compare(ppi->title) ) {
m_aryToolTip.RemoveAll();	//+++ 消しておかないとまずい?	//*
				//一致したらxmlからIDとチップテキストを取得してコントロールにチップを追加する
				CComPtr<MSXML2::IXMLDOMNodeList> nlPageItem = elPage->GetchildNodes();
				int 	len = nlPageItem->Getlength();		//+++ デバッガで見やすいように一旦変数に入れる
				for (int k = 0; k < len; ++k) {
					//xmlからコントロールのIDとチップテキストを取得
					CComQIPtr<MSXML2::IXMLDOMElement> elItem	  = nlPageItem->Getitem(k);
					int 							  xmlID 	  = (int) (long) elItem->getAttribute( _bstr_t( _T("id") ) );
					CString 						  strItem	  = (LPCSTR) elItem->Gettext();		//+++ //*メモ UNICODE対策いる?いらない?

					//該当IDのコントロールがなかったら戻す
					HWND	hWndControl = ::GetDlgItem(ppi->hWnd, xmlID);
					if (!hWndControl)
						continue;

					//タブ文字を削除 - 削除しないとチップがちゃんと表示されない
					while (strItem.Find('\t') != -1)  strItem.Delete( strItem.Find('\t') );

					//チップテキストを付与
					CToolInfo		ti( TTF_SUBCLASS, hWndControl, 0, NULL, const_cast<LPTSTR>( LPCTSTR(strItem) ) /*+++ strItem.GetBuffer(0)*/ );
					CToolTipCtrl	tip;
					tip.Create(m_hWnd);
					tip.SetMaxTipWidth(500);
					tip.AddTool(ti);

					//配列に追加
					m_aryToolTip.Add(tip);
				}

				_ShowToolTip();
				return;
			}
		}
	 #endif
	}


	void _ShowToolTip()
	{
		//+++ デバッガで見やすいように一旦変数に入れる
		BOOL  bButtonChecked = IsDlgButtonChecked(IDC_TOOLTIP);
		UINT  size = m_aryToolTip.GetSize();
		for (UINT i = 0; i < size; ++i) {
			m_aryToolTip[i].Activate(bButtonChecked);
		}
	}
  #endif


	HWND _GetActivePage()
	{
		int 	  nIndex = 0;
		if (m_TreeView.m_hWnd) {
			HTREEITEM hItem  = m_TreeView.GetSelectedItem();
			if (hItem) {
				ATLASSERT(hItem != NULL);
				nIndex = (int) m_TreeView.GetItemData(hItem);
			}
		}
		PAGEINFO *ppi = m_aryPage[nIndex];
		if (ppi == 0)
			return m_TreeView.m_hWnd/*NULL*/;
		return ppi->hWnd;
	}

};



__declspec(selectany) HHOOK 					CTreeViewPropertySheet::s_hHook		= NULL;
__declspec(selectany) CTreeViewPropertySheet*	CTreeViewPropertySheet::s_pThis		= NULL;
__declspec(selectany) int						CTreeViewPropertySheet::m_nCurSel	= 0;


#endif //__PROPERTY_SHEET_H_
