/**
 *	@file	DocHostUIHandlerDispatch.h
 *	@brief	主に右クリックメニューのカスタム処理を行えるようにするためのクラス
 */
#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlcom.h>
#include <mshtmdid.h>

#include "MtlDragDrop.h"
#include "CustomContextMenu.h"



/*
	CDocHostUIHandlerDispatch
	主に右クリックメニューのカスタム処理を行えるようにするためのクラス
	IEコンポーネントのUI処理を置き換えるIDocHostUIHandlerDispatchを実装する
	インスタンスを作成します。ホストウィンドウからクエリーしたIAxWinHostWindowを
	SetDefaultUIHandlerで設定します。続いて今度は逆にインスタンスのポインタを
	IAxWinHostWindow::SetExternalUIHandlerで設定します。

	もともとはatlhost.hを直接書き換えたatlhostEx.hを使うようになっていたのですが、
	ATL7になってからヘッダの依存関係が変わったのか、この方法が使いにくくなったため、
	書き換えていた部分を外部に移しIDocHostUIHandlerDispatchの実装としました。

	このクラスは本当に既存の処理を置き換えてしまうように定義されています(atlhost.h参照)
	しかし実際必要な関数はShowContextMenuだけで、それ以外はデフォルト動作するように
	なっていて欲しいのですが、なっていません。
	大体のメソッドは適当な戻り値を返せばデフォルトと同じ動作をしますが、
	そうでない幾つかのメソッドに対応するためにデフォルトのUIハンドラを所持し、
	一時的に当クラスを解除してデフォルト動作を行わせます(GetHostInfo,GetOptionKeyPath等参照)
 */
class CDocHostUIHandlerDispatch : public IDocHostUIHandlerDispatch {
private:
	bool					   m_bNoIECustom;
	CComPtr<IDocHostUIHandler> m_pDefaultHandler;
	_IDropTarget *			   m_pExternalDropTarget;

public: //CDocHostUIHandlerDispatch
	CDocHostUIHandlerDispatch()
		: m_bNoIECustom(false)
		, m_pExternalDropTarget(NULL)
	{
	}


	void SetIEContextMenuCustom(bool bCustom)
	{
		m_bNoIECustom = !bCustom;
	}


	bool GetIEContextMenuCustom()
	{
		return !m_bNoIECustom;
	}


	void SetDefaultUIHandler(const CComPtr<IDocHostUIHandler> &pDefaultHandler)
	{
		m_pDefaultHandler = pDefaultHandler;
	}


	bool InstallExternalUIHandler(IAxWinHostWindow *pAxWinHostWindow)
	{
		HRESULT hr = pAxWinHostWindow->SetExternalUIHandler(this);

		if ( FAILED(hr) )
			return false;

		return true;
	}


	bool UninstallExternalUIHandler(IAxWinHostWindow *pAxWinHostWindow)
	{
		HRESULT hr = pAxWinHostWindow->SetExternalUIHandler(NULL);

		if ( FAILED(hr) )
			return false;

		return true;
	}


	void SetExternalDropTarget(_IDropTarget *pExternalDropTarget)
	{
		m_pExternalDropTarget = pExternalDropTarget;
	}


public: 	//IUnknown atlhost.hを見る限り必要ではない

	STDMETHOD					(QueryInterface) (REFIID riid, void **ppvObject) { return E_NOINTERFACE; }
	ULONG STDMETHODCALLTYPE 	AddRef()	{ return 1; }
	ULONG STDMETHODCALLTYPE 	Release()	{ return 1; }


public: 	// IDispatch atlhost.hを見る限り必要ではない

	STDMETHOD	(GetTypeInfoCount) (UINT * pctinfo) { return E_NOTIMPL; }
	STDMETHOD	(GetTypeInfo) (UINT itinfo, LCID lcid, ITypeInfo * *pptinfo) { return E_NOTIMPL; }
	STDMETHOD	(GetIDsOfNames) (REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid)	{ return E_NOTIMPL; }

	STDMETHOD	(Invoke) (
			DISPID			dispidMember,
			REFIID			riid,
			LCID			lcid,
			WORD			/*wFlags*/,
			DISPPARAMS *	pdispparams,
			VARIANT *		pvarResult,
			EXCEPINFO * 	/*pexcepinfo*/,
			UINT *			/*puArgErr*/)
	{
		return E_NOTIMPL;
	}


public: 	// IDocHostUIHandlerDispatch
			//このインターフェイスは既存の動作を置き換える

	//カスタムコンテキストメニュー 本当に必要なのはこの関数だけ
	STDMETHOD(ShowContextMenu) (
		/* [in] */ DWORD				dwID,
		/* [in] */ DWORD				x,
		/* [in] */ DWORD				y,
		/* [in] */ IUnknown *			pcmdtReserved,
		/* [in] */ IDispatch *			pdispReserved,
		/* [retval][out] */ HRESULT *	dwRetVal)
	{
		if ( m_bNoIECustom && (GetKeyState(VK_LBUTTON) >= 0) )								//カスタム&左クリックされているかどうか
			return S_FALSE;
		*dwRetVal = CCustomContextMenu().Show(dwID, x, y, pcmdtReserved);					//丸投げ
		return S_OK;
	}


	STDMETHOD(GetHostInfo) (
		/* [out][in] */ DWORD  * pdwFlags,
		/* [out][in] */ DWORD  * pdwDoubleClick)
	{
		//一時的に外部IDispatchを無効にしデフォルト動作させる
		CComQIPtr<IAxWinHostWindow> pHostWindow = m_pDefaultHandler;
		pHostWindow->SetExternalUIHandler(NULL);

		DOCHOSTUIINFO	info= { sizeof (DOCHOSTUIINFO) };
		info.dwFlags		= *pdwFlags;
		info.dwDoubleClick	= *pdwDoubleClick;
		HRESULT 		hr	= m_pDefaultHandler->GetHostInfo(&info);	//デフォルト動作

		//外部IDispatchを有効にする
		pHostWindow->SetExternalUIHandler(this);

		*pdwFlags		   = info.dwFlags;
		*pdwDoubleClick    = info.dwDoubleClick;
		return hr;
	}


	STDMETHOD(ShowUI) (
		/* [in] */ DWORD			 dwID,
		/* [in] */ IUnknown  *		 pActiveObject,
		/* [in] */ IUnknown  *		 pCommandTarget,
		/* [in] */ IUnknown  *		 pFrame,
		/* [in] */ IUnknown  *		 pDoc,
		/* [retval][out] */ HRESULT* dwRetVal)
	{
		return S_OK;
	}

	STDMETHOD(HideUI) (void) { return S_OK; }
	STDMETHOD(UpdateUI) (void) { return S_OK; }
	STDMETHOD(EnableModeless) (/* [in] */ VARIANT_BOOL fEnable) { return S_OK; }
	STDMETHOD(OnDocWindowActivate) ( /* [in] */ VARIANT_BOOL fActivate) { return S_OK; }
	STDMETHOD(OnFrameWindowActivate) ( /* [in] */ VARIANT_BOOL fActivate) { return S_OK; }


	STDMETHOD(ResizeBorder) (
		/* [in] */ long 			left,
		/* [in] */ long 			top,
		/* [in] */ long 			right,
		/* [in] */ long 			bottom,
		/* [in] */ IUnknown  *		pUIWindow,
		/* [in] */ VARIANT_BOOL 	fFrameWindow)
	{
		return S_OK;
	}


	STDMETHOD(TranslateAccelerator) (
		/* [in] */ DWORD_PTR		hWnd,
		/* [in] */ DWORD			nMessage,
		/* [in] */ DWORD_PTR		wParam,
		/* [in] */ DWORD_PTR		lParam,
		/* [in] */ BSTR 			bstrGuidCmdGroup,
		/* [in] */ DWORD			nCmdID,
		/* [retval][out] */HRESULT* dwRetVal)
	{
		return S_FALSE;
	}


	STDMETHOD(GetOptionKeyPath) (/* [out] */ BSTR  * pbstrKey, /* [in] */ DWORD dw)
	{
		CComQIPtr<IAxWinHostWindow> 	pHostWindow = m_pDefaultHandler;
		pHostWindow->SetExternalUIHandler(NULL);

		HRESULT hr	= m_pDefaultHandler->GetOptionKeyPath(pbstrKey, dw);

		pHostWindow->SetExternalUIHandler(this);

		return hr;
	}


	STDMETHOD(GetDropTarget) (/* [in] */ IUnknown  * pDropTarget, /* [out] */ IUnknown	* *ppDropTarget)
	{
		CComQIPtr<IAxWinHostWindow> pHostWindow = m_pDefaultHandler;
		pHostWindow->SetExternalUIHandler(NULL);

		if (m_pExternalDropTarget) {
			*ppDropTarget = (IUnknown *) m_pExternalDropTarget;
			pHostWindow->SetExternalUIHandler(this);
			return S_OK;
		}

		CComQIPtr<IDropTarget>		pTarget 	= pDropTarget;
		HRESULT 					hr			= m_pDefaultHandler->GetDropTarget( pTarget, reinterpret_cast<IDropTarget **>(ppDropTarget) );
		pHostWindow->SetExternalUIHandler(this);
		return hr;
	}


	STDMETHOD(GetExternal) ( /* [out] */ IDispatch	* *ppDispatch)
	{
		CComQIPtr<IAxWinHostWindow> pHostWindow = m_pDefaultHandler;
		pHostWindow->SetExternalUIHandler(NULL);

		HRESULT 	hr			= m_pDefaultHandler->GetExternal(ppDispatch);
		pHostWindow->SetExternalUIHandler(this);
		return hr;
	}


	STDMETHOD(TranslateUrl) (
		/* [in] */ DWORD		dwTranslate,
		/* [in] */ BSTR 		bstrURLIn,
		/* [out] */ BSTR  * 	pbstrURLOut)
	{
		return E_NOTIMPL;
	}


	STDMETHOD(FilterDataObject)(
		/* [in] */ IUnknown  *		pDO,
		/* [out] */ IUnknown  * *	ppDORet)
	{
		return E_NOTIMPL;
	}

};
