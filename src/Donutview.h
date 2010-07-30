/**
 *	@file	DonutView.h
 *	@brief	View.
 */

#pragma once

#include "DonutViewOption.h"
#include "DocHostUIHandlerDispatch.h"
#include "AmbientDispatch.h"
#include "MtlBrowser.h"
#include "HLinkDataObject.h"
#include "GeckoBrowserImpl.h"
#include "IGeckoBrowserEventListener.h"
#include "nsIDOMEventListener.h"

template <class T>
class CDonutViewFileDropTarget : public IDropTargetImpl<T> {
	bool	m_bDragAccept;

public:
  #if 1 //*+++ 追加: どうも m_bDragAcceptが0以外に未初期化されていることに依存していた模様...
		//*    というか、OnDragEnter()に来ていないのが問題かも?.(そのためm_bDrawgAcceptが初期化されず)
		//* 	とりあえず、m_bDragAccept=1にして暫定対処.
	CDonutViewFileDropTarget()
		: m_bDragAccept(1)
	{
	}
  #endif

	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
	{
		m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
		return _MtlStandardDropEffect(dwKeyState);
	}


	DROPEFFECT	OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect)
	{
		if (!m_bDragAccept)
			return DROPEFFECT_NONE;

		return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
	}


	DROPEFFECT	OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
	{
		T * 	pT = static_cast<T *>(this);

		CString strText;

		if ( MtlGetHGlobalText(pDataObject, strText)
		   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
		{
			DonutOpenFile( pT->m_hWnd, strText, DonutGetStdOpenFlag() );
			return DROPEFFECT_COPY;
		}

		return DROPEFFECT_NONE;
	}
};



class CDonutView
	: public CWindowImpl<CDonutView/*, CAxWindow*/>
	, public CGeckoBrowser
	, public nsIDOMEventListener
	//, public CDonutViewFileDropTarget<CDonutView>
{
public:
	// Declaration
	DECLARE_WND_CLASS( NULL )
	NS_DECL_ISUPPORTS
	NS_DECL_NSIDOMEVENTLISTENER

private:

	// Data members
  #if defined USE_ATL3_BASE_HOSTEX == 0 /*_ATL_VER >= 0x700*/	//+++
//	CComQIPtr<IAxWinAmbientDispatchEx>			m_spAxAmbient;
  #else 	//+++ 旧版のIAxWinAmbientDispatchExを使う.
	CComQIPtr<IAxWinAmbientDispatchEx_UNDONUT>	m_spAxAmbient;
  #endif

	DWORD								m_dwDefaultDLControlFlags;
	//DWORD								m_dwDefaultExtendedStyleFlags;	//+++

  #if defined USE_ATL3_BASE_HOSTEX == 0 /*_ATL_VER >= 0x700*/	//+++
	CAmbientDispatch					m_ExternalAmbientDispatch;
  #endif

	int 								m_nDDCommand;

    CGeckoBrowserImpl *m_impl;
	void CreateBrowser();

public:
	CDonutViewOption<CDonutView>		m_ViewOption;

public:
	// Constructor
	CDonutView(DWORD dwDefaultDLControlFlags, DWORD dwDefaultExtendedStyleFlags);

	// Methods
	int     AttachEventListener(IGeckoBrowserEventListener* intf);
    void    DetachEventListener();
	DWORD	GetDLControlFlags();
	void	PutDLControlFlags(DWORD dwDLControlFlags);

	//ドラッグドロップ時の操作を制御するかIEコンポに任せるか
	void	SetOperateDragDrop(BOOL bOn, int nCommand);

	// Overrides
	BOOL	PreTranslateMessage(MSG *pMsg);

	DWORD	_GetDLControlFlags();
	DWORD	_GetExtendedStypeFlags();

	bool		OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true);
	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	void		OnDragLeave();
	DROPEFFECT	OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);

	// Message map and handlers
	BEGIN_MSG_MAP( CDonutView )
		MESSAGE_HANDLER( WM_CREATE, OnCreate )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_MOUSEWHEEL( OnMouseWheel )
		MSG_WM_SIZE( OnSize )
		COMMAND_ID_HANDLER_EX( ID_DLCTL_IMAGES			, OnMultiImages			)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_FRAMES			, OnSecurFrames			)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_PLUGINS			, OnSecurPlugins		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_SCRIPTS			, OnSecurScripts		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_REDIR			, OnSecurMetaRedir		)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_DNS				, OnSecurDNSPrefetch	)
		CHAIN_MSG_MAP_MEMBER( m_ViewOption )
		COMMAND_ID_HANDLER_EX( ID_DLCTL_CHG_SECU		, OnSecuChg 			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_DLCTL_CHG_MULTI		, OnMultiChg 			)	// UDT DGSTR
		COMMAND_ID_HANDLER_EX( ID_DLCTL_ON_OFF_MULTI	, OnAllOnOff			)
		COMMAND_ID_HANDLER_EX( ID_DLCTL_ON_OFF_SECU 	, OnAllOnOff			)
	END_MSG_MAP()


	BEGIN_UPDATE_COMMAND_UI_MAP( CDonutView )
		CHAIN_UPDATE_COMMAND_UI_MEMBER( m_ViewOption )
		UPDATE_COMMAND_UI( ID_DLCTL_IMAGES	  , OnUpdateDLCTL_IMAGES		 )		// with popup
		UPDATE_COMMAND_UI( ID_DLCTL_FRAMES	  , OnUpdateDLCTL_FRAMES		 )
		UPDATE_COMMAND_UI( ID_DLCTL_PLUGINS	  , OnUpdateDLCTL_PLUGINS		 )
		UPDATE_COMMAND_UI( ID_DLCTL_SCRIPTS	  , OnUpdateDLCTL_SCRIPTS		 )
		UPDATE_COMMAND_UI( ID_DLCTL_REDIR	  , OnUpdateDLCTL_REDIR		 )
		UPDATE_COMMAND_UI( ID_DLCTL_DNS		  , OnUpdateDLCTL_DNS		 )

		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_IMAGES,  GECKO_DLCTL_IMAGES,  GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_FRAMES,  GECKO_DLCTL_FRAMES,  GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_PLUGINS, GECKO_DLCTL_PLUGINS, GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_SCRIPTS, GECKO_DLCTL_SCRIPTS, GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_REDIR,   GECKO_DLCTL_REDIR,   GetDLControlFlags() )
		UPDATE_COMMAND_UI_ENABLE_SETCHECK_FLAG( ID_DLCTL_DNS,     GECKO_DLCTL_DNS,     GetDLControlFlags() )

		// UH
		UPDATE_COMMAND_UI( ID_DLCTL_CHG_MULTI , OnUpdateDLCTL_ChgMulti  ) 				// with popup
		UPDATE_COMMAND_UI( ID_DLCTL_CHG_SECU , OnUpdateDLCTL_ChgSecu  ) 				// with popup

		UPDATE_COMMAND_UI_ENABLE_SETCHECK_IF( ID_DLCTL_ON_OFF_MULTI,
				( ( GetDLControlFlags() & (GECKO_DLCTL_IMAGES) )
				  == (GECKO_DLCTL_IMAGES)))

		UPDATE_COMMAND_UI_ENABLE_SETCHECK_IF( ID_DLCTL_ON_OFF_SECU,
				( ( GetDLControlFlags() & (GECKO_DLCTL_FRAMES  |
                                           GECKO_DLCTL_PLUGINS | 
										   GECKO_DLCTL_SCRIPTS | 
										   GECKO_DLCTL_REDIR   | 
										   GECKO_DLCTL_DNS     )) == 0 ) )

	END_UPDATE_COMMAND_UI_MAP()


private:
	LRESULT OnMouseWheel(UINT fwKeys, short zDelta, CPoint point);
	void OnSize(UINT nType, CSize size);

	// UDT DGSTR
	void	OnMultiChg(WORD, WORD, HWND);
	void	OnSecuChg(WORD, WORD, HWND);

	void	OnAllOnOff(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &);
	void	OnDestroy();

	void	OnMultiImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurFrames(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurPlugins(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurScripts(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurMetaRedir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void	OnSecurDNSPrefetch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	void	_InitDLControlFlags();

	// Implementation
	//protected:
	void	_LightRefresh();

	void		OnUpdateDLCTL_ChgMulti(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_ChgSecu(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_IMAGES(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_FRAMES(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_PLUGINS(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_SCRIPTS(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_REDIR(CCmdUI *pCmdUI);
	void		OnUpdateDLCTL_DNS(CCmdUI *pCmdUI);
	void		OnUpdateDocHostUIOpenNewWinUI(CCmdUI *pCmdUI);

	// Overrides
	STDMETHOD	(GetHostInfo) (DWORD  * pdwFlags, DWORD  * pdwDoubleClick);
	STDMETHOD	(GetDropTarget) (IUnknown	* pDropTarget, IUnknown  * *ppDropTarget);
	void		_DrawDragEffect(bool bRemove);

};



/////////////////////////////////////////////////////////////////////////////
