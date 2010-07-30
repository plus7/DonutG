/**
 *	@file	MenuEncode.h
 *	@brief	エンコード選択メニュー.
 */
#ifndef __MENUENCODE_H_
#define __MENUENCODE_H_

#include <mshtmcid.h>
#include "nsIDOMNSDocument.h"
#include "nsIDocumentCharsetInfo.h"
#include "nsIRDFService.h"
#include "nsIRDFDataSource.h"
#include "nsIRDFContainer.h"
#include "nsIRDFResource.h"
#include "nsIRDFLiteral.h"
#include "nsIRDFNode.h"
#include "nsICharsetConverterManager.h"
#include "nsISimpleEnumerator.h"
#include "nsIStringEnumerator.h"
#include "nsIAtom.h"
#include "nsIAtomService.h"
#include "nsIDocCharset.h"
#include "nsIObserverService.h"

class ATL_NO_VTABLE 	CMenuEncode {
	CMenuHandle 				m_Menu;
	CMenuHandle 				m_MenuSub;

	CMenuHandle 				m_CtxMenu;
	CMenuHandle 				m_CtxMenuSub;
	CMenuHandle 				m_EncodeMenu;

	enum { ID_START = 0x6000, ID_END   = 0x6d00 };

	int 						m_posStart;
	int 						m_insertPoint;
	HWND						m_hWnd;
	HWND						m_hWndMDIClient;
	HWND						m_hWndTarget;

	std::map<int, CString>      m_idCharsetMap;

	BOOL						m_bFlag;


public:
	CMenuEncode()
	{
		m_posStart		= 0;
		m_insertPoint	= 0;
		m_hWnd			= NULL;
		m_hWndMDIClient = NULL;
		//m_hInstDLL		= NULL;
		m_hWndTarget	= NULL;
		m_bFlag 		= FALSE;
	}


	// Message map and handlers
	BEGIN_MSG_MAP( CMenuEncode )
		MSG_WM_INITMENUPOPUP( OnInitMenuPopup )
		USER_MSG_WM_RELEASE_CONTEXTMENU( OnReleaseContextMenu )
		MESSAGE_HANDLER( WM_EXITMENULOOP, OnExitMenuLoop )
		COMMAND_RANGE_HANDLER_EX( ID_START, ID_END, OnRangeCommand )
	END_MSG_MAP()


	void Init(HMENU hMenu, HWND hWnd, int insertPoint)
	{
		m_Menu		  = hMenu;
		m_hWnd		  = hWnd;
		m_insertPoint = insertPoint;
	}


private:

	void OnInitMenuPopup(HMENU hmenuPopup, UINT uPos, BOOL fSystemMenu)
	{
		SetMsgHandled(FALSE);

		HWND hWndActive = DonutGetActiveWindow(m_hWnd);

		if (hmenuPopup == m_Menu.m_hMenu) {

			ReleaseMenu();
			if (hWndActive) CreateMenu();

			MENUITEMINFO mii = { sizeof (MENUITEMINFO) };
			mii.fMask = MIIM_TYPE;
			m_Menu.GetMenuItemInfo(m_insertPoint, TRUE, &mii);
			if (mii.fType & MFT_SEPARATOR) {
				mii.fMask	   = MIIM_SUBMENU | MIIM_TYPE;
				mii.dwTypeData = _T("エンコード(&M)");
				mii.cch 	   = 15;
				mii.fType	   = MFT_STRING;
				mii.hSubMenu   = m_EncodeMenu.m_hMenu;
				m_Menu.InsertMenuItem(m_insertPoint, TRUE, &mii);
			}

		}

		if (!hWndActive) {
			EnableMenuItem(m_Menu.m_hMenu, m_insertPoint, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		} else
			EnableMenuItem(m_Menu.m_hMenu, m_insertPoint, MF_BYPOSITION | MF_ENABLED);
	}


	LRESULT OnReleaseContextMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		ReleaseMenu();
		return 0;
	}


	void ReleaseMenu()
	{
		if ( m_EncodeMenu.IsMenu() ) { m_EncodeMenu.DestroyMenu(); }
		m_posStart	 = 0;
		m_hWndTarget = NULL;
		m_idCharsetMap.clear();
		/*if (spCT)		{ spCT.Release() ;			 spCT		= NULL; }
		if (spWnd)		{ spWnd.Release();			 spWnd		= NULL; }
		if (m_hInstDLL) { ::FreeLibrary(m_hInstDLL); m_hInstDLL = NULL; }*/
	}


	void ClearMenu()
	{
		MENUITEMINFO mii = { sizeof (MENUITEMINFO) };
		mii.fMask = MIIM_TYPE;
		m_Menu.GetMenuItemInfo(m_insertPoint, TRUE, &mii);
		if ( !(mii.fType & MFT_SEPARATOR) )
			RemoveMenu(m_Menu.m_hMenu, m_insertPoint, MF_BYPOSITION);

		if ( m_CtxMenu.IsMenu() ) {
			m_CtxMenu.DestroyMenu();
		}
	}


	LRESULT OnExitMenuLoop(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		ClearMenu();
		return 0;
	}


	void OnRangeCommand(UINT, int nID, HWND hWndCtrl)
	{
		HWND	hWndActive = DonutGetActiveWindow(m_hWnd);
		if (!hWndActive) return;

		nsCOMPtr<nsIWebBrowser> wb = DonutGetNsIWebBrowser(hWndActive);
		if (!wb)		return;

		nsCOMPtr<nsIDocShell> docShell(do_GetInterface(wb));
		/*nsCOMPtr<nsIDocCharset> dc = do_QueryInterface(docShell);
		dc->SetCharset(NS_LossyConvertUTF16toASCII( m_idCharsetMap[nID]).get());*/
		nsCOMPtr<nsIDocumentCharsetInfo> dci;
		docShell->GetDocumentCharsetInfo(getter_AddRefs(dci));
		nsCOMPtr<nsIAtomService> as = do_CreateInstance("@mozilla.org/atom-service;1");
		nsCOMPtr<nsIAtom> atom;
		as->GetAtom(nsEmbedString(m_idCharsetMap[nID]), getter_AddRefs(atom));
		dci->SetForcedCharset(atom);
		nsCOMPtr<nsIWebNavigation> nav = do_QueryInterface(wb);
		nav->Reload(nsIWebNavigation::LOAD_FLAGS_CHARSET_CHANGE);
	}

	BOOL CreateMenu()
	{
		HWND	hWndActive = DonutGetActiveWindow(m_hWnd);
		if (!hWndActive) return FALSE;

		nsCOMPtr<nsIWebBrowser> wb = DonutGetNsIWebBrowser(hWndActive);
		if (!wb)		return FALSE;

		m_EncodeMenu.CreatePopupMenu();

		nsresult rv;
		nsCOMPtr<nsIRDFDataSource> ds = do_GetService("@mozilla.org/rdf/datasource;1?name=charset-menu", &rv);
		nsCOMPtr<nsIObserverService> os = do_GetService("@mozilla.org/observer-service;1");
		os->NotifyObservers(NULL, "charsetmenu-selected", _T("browser"));
		os->NotifyObservers(NULL, "charsetmenu-selected", _T("more-menu"));
		os->NotifyObservers(NULL, "charsetmenu-selected", _T("other"));

		int nID = ID_START;

		nsCOMPtr<nsIDOMWindow> win;
		wb->GetContentDOMWindow(getter_AddRefs(win));
		nsCOMPtr<nsIDOMDocument> doc;
		win->GetDocument(getter_AddRefs(doc));	
		nsCOMPtr<nsIDOMNSDocument> nsdoc = do_QueryInterface(doc);
		nsEmbedString current;
		nsdoc->GetCharacterSet(current);
		CString tmp(current.get());

		nID = CreateSubMenu(nID, 0, ds, tmp, "NC:BrowserMore1CharsetMenuRoot", _T("西欧"));
		nID = CreateSubMenu(nID, 1, ds, tmp, "NC:BrowserMore2CharsetMenuRoot", _T("東欧"));
		nID = CreateSubMenu(nID, 2, ds, tmp, "NC:BrowserMore3CharsetMenuRoot", _T("東アジア"));
		nID = CreateSubMenu(nID, 3, ds, tmp, "NC:BrowserMore4CharsetMenuRoot", _T("西南＆東南アジア"));
		nID = CreateSubMenu(nID, 4, ds, tmp, "NC:BrowserMore5CharsetMenuRoot", _T("中近東"));
		nID = CreateSubMenu(nID, 5, ds, tmp, "NC:BrowserUnicodeCharsetMenuRoot", _T("Unicode"));

		return TRUE;
	}

	int CreateSubMenu(int startid, int parentpos, nsIRDFDataSource *ds, const CString& currentCharset, char *rootURI, TCHAR *rootLabel){
		CMenuHandle Parent;
		Parent.CreatePopupMenu();
		nsresult rv;
		nsCOMPtr<nsIRDFService> rdfsvc = do_GetService("@mozilla.org/rdf/rdf-service;1", &rv);

		nsCOMPtr<nsIRDFResource> root, name;
		rv = rdfsvc->GetResource(NS_LITERAL_CSTRING(rootURI), getter_AddRefs(root));
		rv = rdfsvc->GetResource(NS_LITERAL_CSTRING("http://home.netscape.com/NC-rdf#Name"), getter_AddRefs(name));

		nsCOMPtr<nsIRDFContainer> ctr = do_CreateInstance("@mozilla.org/rdf/container;1", &rv);
		rv = ctr->Init(ds, root);

		nsCOMPtr<nsISimpleEnumerator> se;
		rv = ctr->GetElements(getter_AddRefs(se));

		PRBool hasmore;
		nsCOMPtr<nsISupports> elm;

		int nID = startid;
		rv = se->HasMoreElements(&hasmore);
		int pos = 0;
		while(hasmore){
			se->GetNext(getter_AddRefs(elm));
			nsCOMPtr<nsIRDFResource> charset = do_QueryInterface(elm);
			const char *val;
			charset->GetValueConst(&val);
			CString charsetCode(val);

			m_idCharsetMap.insert(std::map<int, CString>::value_type( nID, charsetCode ));

			nsCOMPtr<nsIRDFNode> node;
			ds->GetTarget(charset, name, PR_TRUE, getter_AddRefs(node));
			nsCOMPtr<nsIRDFLiteral> lit = do_QueryInterface(node);
			const PRUnichar *label;
			lit->GetValueConst(&label);

			MENUITEMINFO mii;
			TCHAR szName[512];
			wsprintf (szName, _T("%s"), label);
			mii.cbSize = sizeof (MENUITEMINFO);
			mii.fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE | MIIM_CHECKMARKS;
			mii.fType = MFT_STRING;
			mii.fState = (currentCharset == charsetCode) ? MFS_CHECKED : 0;
			mii.wID = nID++;
			mii.hSubMenu = NULL;
			mii.hbmpChecked = NULL;
			mii.hbmpUnchecked = NULL;
			mii.dwItemData = 0;
			mii.dwTypeData = szName;
			mii.cch = lstrlen (szName);

			Parent.InsertMenuItem(pos++, TRUE, &mii);
			se->HasMoreElements(&hasmore);
		}
		MENUITEMINFO mii2;
		TCHAR szName[512];
		wsprintf (szName, _T("%s"), rootLabel); //バッファオーバーフローするかも
		mii2.cbSize = sizeof (MENUITEMINFO);
		mii2.fMask = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE ;
		mii2.fType = MFT_STRING;
		mii2.fState = 0;
		mii2.wID = NULL;
		mii2.hSubMenu = Parent.m_hMenu;
		mii2.hbmpChecked = NULL;
		mii2.hbmpUnchecked = NULL;
		mii2.dwItemData = 0;
		mii2.dwTypeData = szName;
		mii2.cch = lstrlen (szName);

		m_EncodeMenu.InsertMenuItem(parentpos, TRUE, &mii2);

		return nID;
	}

};


#endif		//__MENUENCODE_H_
