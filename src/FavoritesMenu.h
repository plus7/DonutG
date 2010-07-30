/**
 *	@file	FavoritesMenu.h
 *	@brief	エクスプローラバーでのお気に入りメニュー.
 */
#pragma once

#include "ExplorerMenu.h"

#include <map>

// for debug
#ifdef _DEBUG
	const bool _Mtl_FavoritesMenu_traceOn = false;
	#define fvmTRACE	if (_Mtl_FavoritesMenu_traceOn) ATLTRACE
#else
	#define fvmTRACE
#endif


enum {
	EMS_ORDER_FAVORITES 	=	0x00010000L,
	EMS_USER_DEFINED_FOLDER =	0x00040000L,
};



template <class T>
class CFavoritesMenu : public CExplorerMenuImpl<T> {
	typedef CExplorerMenuImpl<T>  baseClass;
	typedef CFavoritesMenu<T>	  thisClass;
public:

	// Ctor/Dtor
	CFavoritesMenu(
			int 			nInsertPointMenuItemID,
			const CString&	strNoEntries			= _T("(empty)"),
			int 			nMinID					= 0x7000,
			int 			nMaxID					= 0x7FFF,
			const CString&	strAdditional			= _T("Open This Directory"),
			int 			nMaxMenuItemTextLength	= 55)
		: CExplorerMenuImpl<T>(nInsertPointMenuItemID, strNoEntries, nMinID, nMaxID, strAdditional, nMaxMenuItemTextLength)
	{
	}


	// Overrides
	void OnMenuItemInitialUpdate(const CString &strDirPath, CSimpleArray<CMenuItemInfo> &infos)
	{
		fvmTRACE( _T("CFavoritesMenu::OnMenuItemInitialUpdate\n") );

		if ( _check_flag( EMS_ORDER_FAVORITES, GetStyle() ) && !_check_flag( EMS_USER_DEFINED_FOLDER, GetStyle() ) ) {
			fvmTRACE( _T(" try to order favorites\n") );
			CFavoritesOrder order;

			if ( MtlGetFavoritesOrder(order, strDirPath) ) {
				//CDebugWindow::OutPutString("Menu Sorting - ie method");
				std::sort( _begin(infos), _end(infos), _FavoritesMenuItemCompare(order, this) );
				//CDebugWindow::OutPutString("Menu Sorting - ie method End");
			} else {
				fvmTRACE( _T(" default order\n") );
				baseClass::OnMenuItemInitialUpdate(strDirPath, infos);
			}
		} else {
			fvmTRACE( _T(" default order\n") );
			baseClass::OnMenuItemInitialUpdate(strDirPath, infos);
		}
	}


private:
	struct _FavoritesMenuItemCompare : public std::binary_function<const CMenuItemInfo &, const CMenuItemInfo &, bool> {
		CSimpleMapInt<CString> &	_order;
		baseClass * 				_pBase;

		_FavoritesMenuItemCompare(CSimpleMapInt<CString> &order, baseClass *pBase)
			: _order(order), _pBase(pBase)
		{
		}


		bool operator ()(const CMenuItemInfo &x, const CMenuItemInfo &y)
		{
			CString   strPathA = _pBase->_GetFilePath(x);
			CString   strPathB = _pBase->_GetFilePath(y);

			CString   strA	   = MtlGetFileName(strPathA);
			CString   strB	   = MtlGetFileName(strPathB);
			//			ATLTRACE(_T("strA(%s), strB(%s)\n"), strA, strB);

			const int enough   = 10000;
			bool	  bDirA    = MtlIsDirectoryPath(strPathA);
			bool	  bDirB    = MtlIsDirectoryPath(strPathB);

			int 	  itA	   = _order.Lookup(strA);
			int 	  itB	   = _order.Lookup(strB);

			if (itA == -1 || itA == FAVORITESORDER_NOTFOUND)	// fixed by fub, thanks.
				itA = enough;

			if (itB == -1 || itB == FAVORITESORDER_NOTFOUND)
				itB = enough;

			if (itA == enough && itB == enough) {
				if (bDirA == bDirB) {
					int nRet = ::lstrcmp(strA, strB);
					return (nRet < 0) /*? true : false*/;
					//return strA < strB;
				} else {
				  #if 1 //+++
					return bDirA;
				  #else
					if (bDirA)
						return true;
					else
						return false;
				  #endif
				}
			} else {
				return itA < itB;
			}
		}


		CString _GetFileName(const CMenuItemInfo &mii)
		{
			CString 	str = _pBase->m_mapID.Lookup(mii.wID);

			if ( str.IsEmpty() ) {			// popup menu = folder
				str = mii.dwTypeData;
			} else {
				str = MtlGetDisplayTextFromPath(str);
			}

			return str;
		}


		FILETIME _GetFileTime(const CString &strPath_)
		{
			CString 			strPath(strPath_);

			MtlRemoveTrailingBackSlash(strPath);
			WIN32_FIND_DATA 	data;
			HANDLE				h = ::FindFirstFile(strPath, &data);
			ATLASSERT(h != INVALID_HANDLE_VALUE);
			::FindClose(h);

			return data.ftCreationTime;
		}
	};


	void OnMenuPrepareContext(CItemIDList &idlFolder)
	{
		CString 	strFolder;
		MtlGetFavoritesFolder(strFolder);
		idlFolder = strFolder;
	}

};
