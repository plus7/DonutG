// TmpFavTreeView.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "TmpFavGroup.h"
#include "TmpFavTreeView.h"
//#include "MtlProfile.h"
#include "IniFile.h"
//#include "Misc.h"			//+++


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CTmpFavTreeView

CTmpFavTreeView::CTmpFavTreeView()
	: m_dwExStyle(0)
	, m_nNewCount(0)		//+++
	, m_renameEndFunc(0)	//+++
{
}


CTmpFavTreeView::~CTmpFavTreeView()
{
	ASSERT(!m_nNewCount);
}


BEGIN_MESSAGE_MAP(CTmpFavTreeView, CTreeCtrl)
	//{{AFX_MSG_MAP(CTmpFavTreeView)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_RCLICK			, OnRclick)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN		, OnKeydown)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT	, OnEndlabeledit)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




// ***************************************************************************
// オーバーライドした関数
// ***************************************************************************


//項目のアプリケーション定義データ領域にnewで文字列用のメモリを確保したので
//標準の関数で項目を削除するとメモリリークが生じる
//よって項目を削除する場合はオーバーライドした以下の関数を用いる
BOOL CTmpFavTreeView::DeleteItem(HTREEITEM hItem)
{
	BOOL		  bRet	= DeleteChildItems(hItem);
	if (!bRet)
		return FALSE;

	FavGroupInfo *pInfo = (FavGroupInfo *) GetItemData(hItem);
	if (pInfo)
		FreeDataStruct(pInfo);

	return CTreeCtrl::DeleteItem(hItem);
}


BOOL CTmpFavTreeView::DeleteAllItems()
{
	return DeleteChildItems(TVI_ROOT);
}



// ***************************************************************************
// Privateメンバ
// ***************************************************************************

//指定アイテム下の項目を全て削除する
BOOL CTmpFavTreeView::DeleteChildItems(HTREEITEM hParent)
{
	//再帰的に子アイテムを削除する
	HTREEITEM	hChild 	= GetChildItem(hParent);
	if (hChild) {
		BOOL	bRet = DeleteChildItems(hChild);
		if (!bRet)
			return FALSE;

		HTREEITEM	hChildNext;
		while ( ( hChildNext = GetNextSiblingItem(hChild) ) != NULL ) {
			bRet   = DeleteItem(hChild);
			if (!bRet)
				return FALSE;

			bRet   = DeleteChildItems(hChildNext);
			if (!bRet)
				return FALSE;

			hChild = hChildNext;
		}

		bRet = DeleteItem(hChild);
		if (!bRet)
			return FALSE;
	}

	return TRUE;
}


//お気に入りグループをツリーに追加する
BOOL CTmpFavTreeView::InsertFavGroup(const CString &strGroupTitle, const CString &strFilePath)
{
	HTREEITEM hItem = InsertItem(strGroupTitle, 0, 1, TVI_ROOT);
	if (!hItem)
		return FALSE;

	SetItemData(hItem, (DWORD_PTR) NULL);

	return InsertFavGroupItem(strFilePath, hItem);
}


//お気に入りグループの項目のみを指定したグループアイテムの下に追加する
BOOL CTmpFavTreeView::InsertFavGroupItem(const CString &strFilePath, HTREEITEM hParent)
{
	int 		   nCount = 0;
	FavGroupInfo **infos  = ReadFavGroupFile(strFilePath, &nCount); 	//これは必ずdelete []する

	if (infos == NULL)
		return FALSE;

	for (int i = 0; i < nCount; i++) {
		FavGroupInfo *pInfo = infos[i];

		HTREEITEM	   hItemChild;
		if ( pInfo->strTitle.IsEmpty() ) {
			hItemChild = InsertItem(pInfo->strLocation, 2, 2, hParent);
		} else {
			hItemChild = InsertItem(pInfo->strTitle, 2, 2, hParent);	//タイトルが無いときはURLを表示
		}

		SetItemData(hItemChild, (DWORD_PTR) pInfo);
	}

	delete[] infos; 													//消す
	return TRUE;
}


//お気に入りグループファイルを開いてそのデータを格納したメモリ配列を返す
//この配列は使い終わったらdelete []すること
FavGroupInfo **CTmpFavTreeView::ReadFavGroupFile(const CString &strFilePath, int *pnCount)
{
	CIniFileI   pr( strFilePath, _T("Header") );
	DWORD		dwCount = 0;
	pr.QueryValue( dwCount, _T("Count") );
	pr.Close();

	if (dwCount == 0)
		return NULL;

	FavGroupInfo**	infos   = new FavGroupInfo *[dwCount];				//配列のメモリ確保

	CString 	   	strLocation;
	CString			strTitle;
	int 		   	nCount  = dwCount;

	for (int i = 0; i < nCount; i++) {
		GetGroupItemData(strFilePath, i, strLocation, strTitle);
		infos[i]			   = AllocDataStruct(); 					//アイテムのメモリ確保
		infos[i]->strIniFile   = strFilePath;
		infos[i]->strLocation  = strLocation;
		infos[i]->strTitle	   = strTitle;
		infos[i]->strSection.Format(_T("%s%d"), PREFIXSECTION, i);
		infos[i]->bUseIniStyle = TRUE;
	}

	*pnCount = nCount;
	return infos;
}


//iniファイルからアイテムに関する情報を取得する
void CTmpFavTreeView::GetGroupItemData(const CString &strDfgFile, int nSection, CString &strLocation, CString &strTitle)
{
	//const int	bufsize   = 1024;
	//DWORD		dwGetSize = bufsize;

	CString 	strKey;
	strKey.Format(_T("%s%d"), PREFIXSECTION, nSection);

	CIniFileI	 pr(strDfgFile, strKey);
	strTitle    = pr.GetStringUW(_T("Title"));
	strLocation = pr.GetString(_T("Location_URL"));
	//pr.Close();
}


//エクスプローラ・スタイルなファイル操作を行う
BOOL CTmpFavTreeView::FileOperation(const CString &strFromFilePath, const CString &strToFilePath, int nOperate, int nFlag)
{
	if ( strFromFilePath.IsEmpty() )
		return FALSE;

	TCHAR		bufFrom[MAX_PATH + 2];
	ZeroMemory(bufFrom, sizeof(TCHAR) * MAX_PATH + 2); 				//末尾は2つのヌル文字
	//+++ CopyMemory( bufFrom, strFromFilePath.GetBuffer(0), strFromFilePath.GetLength() );
	CopyMemory( bufFrom, LPCTSTR(strFromFilePath), sizeof(TCHAR) * strFromFilePath.GetLength() );			//+++ UNICODE 対応
	LPTSTR	lpstrFrom = bufFrom;

	LPTSTR		lpstrTo;
	TCHAR		bufTo[MAX_PATH + 2];
	if ( !strToFilePath.IsEmpty() ) {
		ZeroMemory(bufTo, sizeof(TCHAR) * MAX_PATH + 2);				//末尾は2つのヌル文字
		//+++ CopyMemory( bufTo, strToFilePath.GetBuffer(0), strToFilePath.GetLength() );
		CopyMemory( bufTo, LPCTSTR(strToFilePath), sizeof(TCHAR) * strToFilePath.GetLength() );				//+++ UNICODE 対応
		lpstrTo = bufTo;
	} else {
		lpstrTo = NULL;
	}

	SHFILEOPSTRUCT fop = {
		m_hWnd, nOperate, lpstrFrom, lpstrTo,
		nFlag,	FALSE,	  NULL, 	 NULL,
	};
	return (::SHFileOperation(&fop) == 0 && !fop.fAnyOperationsAborted );
}


//アイテムの情報を保存する構造体のメモリを確保する
FavGroupInfo *CTmpFavTreeView::AllocDataStruct()
{
	m_nNewCount++;
	return new FavGroupInfo;
}


//上で確保したメモリを開放する
void CTmpFavTreeView::FreeDataStruct(FavGroupInfo *ptr)
{
	m_nNewCount--;
	delete ptr;
}



// ***************************************************************************
// Publicメンバ
// ***************************************************************************



/////////////////////////////////////////////////////////////////////////////
//全般処理
/////////////////////////////////////////////////////////////////////////////

//指定フォルダのお気に入りグループをビューに登録する
BOOL CTmpFavTreeView::LoadFavGroups(const CString &strFolder)
{
	if (::GetFileAttributes(strFolder) == 0xFFFFFFFF)
		return FALSE;

	if ( !DeleteAllItems() )
		return FALSE;

	CString   strMatch = strFolder + "*.dfg";

	//.dfgで終わるファイルを探して登録
	CFileFind find;

	if ( find.FindFile(strMatch) ) {
		BOOL bContinue = TRUE;

		while (bContinue) {
			bContinue = find.FindNextFile();

			if ( !find.IsDirectory() )
				InsertFavGroup( find.GetFileTitle(), find.GetFilePath() );
		}
	}

	//ついでにソートする
	SortChildren(NULL);

	m_strDataFolder = strFolder;

	return TRUE;
}


//拡張スタイル取得
DWORD CTmpFavTreeView::GetExtendedStyle()
{
	return m_dwExStyle;
}


//拡張スタイル設定
void CTmpFavTreeView::SetExtendedStyle(DWORD dwStyle)
{
	m_dwExStyle = dwStyle;
}



/////////////////////////////////////////////////////////////////////////////
//お気に入りグループ処理
/////////////////////////////////////////////////////////////////////////////

//選択したグループを開く
BOOL CTmpFavTreeView::OpenFavGroup()
{
	HTREEITEM 	hItem = GetSelectedItem();
	if (hItem == NULL) {
		return FALSE;
	} else if ( GetItemData(hItem) ) {
		hItem = GetParentItem(hItem);
		if (hItem == NULL)
			return FALSE;
	}

	return OpenFavGroup(hItem);
}


//指定したグループを開く
BOOL CTmpFavTreeView::OpenFavGroup(HTREEITEM hItem)
{
	CString strFileName = m_strDataFolder + GetItemText(hItem) + _T(".dfg");	//+++ .dfg抜けを追加
	if (::GetFileAttributes(strFileName) == 0xFFFFFFFF)
		return FALSE;
	return OpenFavGroup(strFileName);
}


//指定したお気に入りグループファイルを開く
BOOL CTmpFavTreeView::OpenFavGroup(const CString &strFileName)
{
	CDonutApi 	Donut;
	if ( Donut.IsNull() )
		return FALSE;
	_bstr_t   bstrFileName = strFileName;
	BOOL	  bFlag 	   = (m_dwExStyle & TMPFAV_CLOSE) ? TRUE : FALSE;
	Donut.m_pDonut->LoadGroup(bstrFileName, bFlag);

	return TRUE;
}


//開かれているタブをスナップショットとして保存する
void CTmpFavTreeView::SaveGroup()
{
	CDonutApi Donut;
	if ( Donut.IsNull() )
		return;
	CTime	  time		    = CTime::GetCurrentTime();
	CString   strTitle		= time.Format( _T("%Y%m%d_%H%M%S") );	//ファイル名は時間に基づく
	CString	  strFileName	= strTitle + ".dfg";
	//+++ strFileName		= m_strDataFolder +	strFileName;
  #if 1	//+++
	if (m_dwExStyle & TMPFAV_SAVENAME) {
		CFileDialog		fileDialog(FALSE, _T("dfg"), strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL);
		fileDialog.m_ofn.lpstrInitialDir = m_strDataFolder;
		if (fileDialog.DoModal() != IDOK)
			return;
		strFileName			= fileDialog.GetPathName();
		strTitle			= Misc::GetFileNameNoExt(Misc::GetFileBaseName(strFileName));
	} else {
		strFileName			= m_strDataFolder +	strFileName;
	}
  #endif
	_bstr_t   bstrFileName	= strFileName;
	Donut.m_pDonut->SaveGroup(bstrFileName);
	InsertFavGroup(strTitle, strFileName);
}


//選択されているグループ項目を削除
void CTmpFavTreeView::DeleteGroup()
{
	HTREEITEM hItem = GetSelectedItem();
	if (!hItem)
		return;
	if ( GetItemData(hItem) ) {
		hItem = GetParentItem(hItem);
		if (!hItem)
			return;
	}

	DeleteGroup(hItem);
}


//指定したグループを削除（ファイルも削除）
void CTmpFavTreeView::DeleteGroup(HTREEITEM hItem)
{
	CString strDfgFile = m_strDataFolder + GetItemText(hItem) + _T(".dfg");
	int 	nFlag	   = 0;
	if ( !(::GetAsyncKeyState(VK_SHIFT) & 0x80000000) ) 			//シフト押しているときは直で削除
		nFlag |= FOF_ALLOWUNDO;										//そうでないときはゴミ箱行き
	BOOL	bOk 	   = FileOperation(strDfgFile, CString(), FO_DELETE, nFlag);
	if (bOk)
		DeleteItem(hItem);
}


//選択したグループの名前を変更（ファイルにも適用）
void CTmpFavTreeView::RenameGroup()
{
	HTREEITEM hItem = GetSelectedItem();
	if (!hItem)
		return;
	if ( GetItemData(hItem) )
		return;
	RenameGroup(hItem);
}


//指定したグループの名前を変更（ファイルにも適用）
void CTmpFavTreeView::RenameGroup(HTREEITEM hItem)
{
	//ラベル編集できるようにする
	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);

	lStyle |= TVS_EDITLABELS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	EditLabel(hItem);
	//OnEndLabelEditでまた編集できないようにする
}



/////////////////////////////////////////////////////////////////////////////
//お気に入りグループの子アイテム処理
/////////////////////////////////////////////////////////////////////////////

//グループ内の指定した項目を開く
BOOL CTmpFavTreeView::OpenGroupItem(HTREEITEM hItem)
{
	CDonutApi Donut;

	if ( !Donut.IsNull() ) {
		FavGroupInfo *pInfo = (FavGroupInfo *) GetItemData(hItem);

		if (pInfo) {
			_bstr_t bstrLocation = pInfo->strLocation;
			BOOL	bFlagActive  = (m_dwExStyle & TMPFAV_ACTIVE) != 0;	//? TRUE : FALSE;

			//仕様未公開の引数の使い方は解説しませんがこんな感じで↓
			_ExInfo ExInfo;
		  #if 1	//+++
			ExInfo.lpstrIniFile = LPTSTR( LPCTSTR(pInfo->strIniFile) );
			ExInfo.lpstrSection = LPTSTR( LPCTSTR(pInfo->strSection) );
		  #else
			ExInfo.lpstrIniFile = pInfo->strIniFile.GetBuffer(0);
			ExInfo.lpstrSection = pInfo->strSection.GetBuffer(0);
		  #endif
			ExInfo.bUseIniStyle = TRUE;
			Donut.m_pDonut->NewWindow3(bstrLocation, bFlagActive, 0, (int *) &ExInfo);

			return TRUE;
		}
	}

	return FALSE;
}


/** グループへのアクティブタブの追加.
 * +++ 引数が1なら全タブ追加モードする.
 */
void CTmpFavTreeView::AddItemToGroup(bool bAll)
{
	//グループの取得
	HTREEITEM	  hItem 	  = GetSelectedItem();

	if (!hItem)
		return;

	if (GetItemData(hItem) != 0) {
		hItem = GetParentItem(hItem);

		if (!hItem)
			return;
	}

	//グループへの追加
	CDonutApi	  Donut;

	if ( Donut.IsNull() )
		return;

	if (Donut.m_pDonut->TabIndex == -1)
		return;

	CString 	  strDfgFile  = m_strDataFolder + GetItemText(hItem) + _T(".dfg");
	_bstr_t 	  bstrDfgFile = strDfgFile;
	int			  tabIdx_bgn  = Donut.m_pDonut->TabIndex;			//+++
	int			  tabIdx_end  = tabIdx_bgn+1;						//+++
	if (bAll) {
		tabIdx_bgn = 0;
		tabIdx_end = Donut.m_pDonut->GetTabCount();
		if (tabIdx_end <= 0)
			return;
	}

	for (int tabIdx = tabIdx_bgn; tabIdx < tabIdx_end; ++tabIdx) {	//+++ カレントだけでなく、全タブセーブも行えるように修正.
	 #if 1	//+++ どうも AddGroupItemの帰値は 実際の実装は、インデックスでなく、個数情報の模様... なので、そのように修正.
		int 		nNum	= Donut.m_pDonut->AddGroupItem(bstrDfgFile, tabIdx/*Donut.m_pDonut->TabIndex*/);
		int			nIndex	= nNum - 1;
	 #else
		int 		nIndex	= Donut.m_pDonut->AddGroupItem(bstrDfgFile, tabIdx/*Donut.m_pDonut->TabIndex*/);
	 #endif
		if (nIndex < 0)
			return;

		//リストへの反映
		CString 	  strLocation, strTitle;
		//+++ GetGroupItemData(strDfgFile, nIndex - 1, strLocation, strTitle);	//+++ nIndexを個数でなく添え字番号に修正したので-1は不要.
		GetGroupItemData(strDfgFile, nIndex, strLocation, strTitle);

		HTREEITEM	  hItemChild;

		if ( strTitle.IsEmpty() )
			hItemChild = InsertItem(strLocation, 2, 2, hItem);
		else
			hItemChild = InsertItem(strTitle, 2, 2, hItem);

		FavGroupInfo*	pInfo 	= AllocDataStruct();
		pInfo->strIniFile		= strDfgFile;
		pInfo->strSection.Format(_T("%s%d"), PREFIXSECTION, nIndex);
		pInfo->strLocation 		= strLocation;
		pInfo->strTitle    		= strTitle;
		SetItemData(hItemChild, (DWORD_PTR) pInfo);
	}	//+++
}


//選択したアイテムを削除
void CTmpFavTreeView::DeleteGroupItem()
{
	//グループの取得
	HTREEITEM hItem = GetSelectedItem();

	if (!hItem)
		return;

	if (GetItemData(hItem) == 0)
		return;

	DeleteGroupItem(hItem);
}


//指定したアイテムを削除
void CTmpFavTreeView::DeleteGroupItem(HTREEITEM hItem)
{
	CDonutApi	  Donut;
	if ( Donut.IsNull() )
		return;

	FavGroupInfo *pInfo 		= (FavGroupInfo *) GetItemData(hItem);

	if (!pInfo)
		return;

	int 		  nIndex		= _ttoi( pInfo->strSection.Mid(PREFIXSIZE) );
	HTREEITEM	  hParent		= GetParentItem(hItem);
	if (!hParent)
		return;

	CString 	  strGroupFile	= m_strDataFolder + GetItemText(hParent) + _T(".dfg");
	_bstr_t 	  bstrGroupFile = strGroupFile;
	int 		  lRet			= Donut.m_pDonut->DeleteGroupItem(bstrGroupFile, nIndex);

	if (lRet == 0)
		return;

	DeleteChildItems(hParent);
	InsertFavGroupItem(strGroupFile, hParent);
}


//選択したアイテムの設定を行う
void CTmpFavTreeView::SetItemOption()
{
	HTREEITEM hItem = GetSelectedItem();

	if (hItem == NULL || GetItemData(hItem) == 0)
		return;

	SetItemOption(hItem);
}


//指定したアイテムの設定を行う
void CTmpFavTreeView::SetItemOption(HTREEITEM hItem)
{
	FavGroupInfo * info = (FavGroupInfo *) GetItemData(hItem);
	if (info == NULL)
		return;
  #if 1	//+++
	CApiExStyle    apiStyle;
	CIniFileIO		pr(info->strIniFile	, info->strSection);
	DWORD dwDLStyle		= pr.GetValue( _T("DL_Control_Flags") );
	DWORD dwReloadStyle	= pr.GetValue( _T("Auto_Refresh_Style") );
	DWORD dwExStyle		= pr.GetValue( _T("Extended_Style") );
  #else
	DWORD		   dwDLStyle, dwReloadStyle, dwExStyle;
	CApiExStyle    apiStyle;
	CIniFileIO		pr(info->strIniFile	, info->strSection);
	pr.QueryValue( dwDLStyle	, _T("DL_Control_Flags") );
	pr.QueryValue( dwReloadStyle, _T("Auto_Refresh_Style") );
	pr.QueryValue( dwExStyle	, _T("Extended_Style") );
  #endif

	apiStyle.SetApiExtendedStyle(dwDLStyle, dwExStyle, dwReloadStyle);
	CExStyleDialog dlg(apiStyle);

	if (dlg.DoModal() != IDOK)
		return;						//ダイアログ表示

	//更新
	pr.SetValue( apiStyle.GetDLControlFlags()	 , _T("DL_Control_Flags") );
	pr.SetValue( apiStyle.GetAutoRefreshFlag()	 , _T("Auto_Refresh_Style") );
	pr.SetValue( apiStyle.GetExtendedStyleFlags(), _T("Extended_Style") );

	pr.Close();
}



// ***************************************************************************
// メッセージハンドラ
// ***************************************************************************

//メッセージをハンドリングする
BOOL CTmpFavTreeView::PreTranslateMessage(MSG *pMsg)
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
		//キーメッセージは自分で処理する
		TranslateMessage(pMsg);
		DispatchMessage(pMsg);
		return TRUE;
	}

	return CTreeCtrl::PreTranslateMessage(pMsg);
}


//ウィンドウが生成される
int CTmpFavTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style &= ~TVS_EDITLABELS;	//ユーザーのファイル名編集を禁止
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


//ウィンドウが削除される
void CTmpFavTreeView::OnDestroy()
{
	DeleteAllItems();							//削除されてしまうまえにアイテムを破棄してメモリを開放
	CTreeCtrl::OnDestroy(); 					//削除される
}


//左クリックした
void CTmpFavTreeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT	  nRetFlag = 0;
	HTREEITEM hItem    = HitTest(point, &nRetFlag);
	if (hItem) {
		OpenGroupItem(hItem);					//カーソル下に項目があれば開く
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}


//左ダブルクリックした
void CTmpFavTreeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	HTREEITEM hItem = HitTest(point);

	if (hItem && m_dwExStyle & TMPFAV_DBLCLICK) {
		//オプション設定によってはお気に入りグループを開く
		CString strText    = GetItemText(hItem);
		CString strDfgFile = m_strDataFolder + strText + _T(".dfg");
		OpenFavGroup(strDfgFile);
	}

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}


//右クリックした
void CTmpFavTreeView::OnRclick(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	CPoint	  point, pointClient;

	GetCursorPos(&point);
	pointClient = point;
	ScreenToClient(&pointClient);

	//グループかアイテムかを判別してそれぞれのメニューを表示する
	HTREEITEM hItem = HitTest(pointClient);

	if (hItem) {
		SelectItem(hItem);

		if ( GetItemData(hItem) ) {
			//子（アイテム）
			CMenu  menu;
			menu.LoadMenu(IDR_ITEM);
			CMenu *pmenuSub = menu.GetSubMenu(0);
			pmenuSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, GetParent(), NULL);
			*pResult = 1;
			return;

		} else {
			//親（グループ）
			CMenu	  menu;
			menu.LoadMenu(IDR_GROUP);
			CDonutApi Donut;
			BOOL	  bTabExist = ( !Donut.IsNull() && (Donut.m_pDonut->GetTabIndex() != -1) );
			if (!bTabExist) {
				menu.EnableMenuItem(ID_ADDITEMTOGROUP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				menu.EnableMenuItem(ID_ADDITEMTOGROUP_ALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
			CMenu *   pmenuSub	= menu.GetSubMenu(0);
			pmenuSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, GetParent(), NULL);
		}
	}

	//メニュー選択時に発生するコマンドメッセージは親ウィンドウで処理する

	*pResult = 0;
}


//キーが押された
void CTmpFavTreeView::OnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	TV_KEYDOWN *pTVKeyDown = (TV_KEYDOWN *) pNMHDR;

	//リターンキーで開く
	if (pTVKeyDown->wVKey == VK_RETURN) {
		HTREEITEM hItem = GetSelectedItem();
		if (hItem) {
			if ( GetItemData(hItem) ) {
				OpenGroupItem(hItem);
			} else {
				OpenFavGroup(hItem);
			}
		}
	}

	*pResult = 0;
}


//ラベル編集が終了した
void CTmpFavTreeView::OnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	TV_DISPINFO *pTVDispInfo   = (TV_DISPINFO *) pNMHDR;

	*pResult = 0;

	CString 	 strBadChar    = _T("*\\|<>:/?\"");
	CString 	 strText;
	if (pTVDispInfo->item.pszText == NULL)
		return;

	//名前に使えない文字が入ってないか判別する
	strText  = pTVDispInfo->item.pszText;

	if (strText.FindOneOf(strBadChar) != -1) {
		CString strErr;
		strErr.Format(_T("%s は名前に使用することはできせん。"), strBadChar);
		MessageBox( strErr, _T("エラー") );
		return;
	} else if ( strText.IsEmpty() ) {
		return;
	}

	CString 	 strFilePath   = m_strDataFolder + strText;

	if (strFilePath.Find( _T(".dfg") ) == -1)
		strFilePath += _T(".dfg");

	CString 	 strBeforeName = m_strDataFolder + GetItemText(pTVDispInfo->item.hItem) + _T(".dfg");
	FileOperation(strBeforeName, strFilePath, FO_RENAME, FOF_ALLOWUNDO);

	//メニューからしかラベル編集できないようにする
	LONG		 lStyle 	   = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle	&= ~TVS_EDITLABELS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	*pResult = 1;

	//+++ .dfg名を弄ったら、子供の情報を更新する必要がある...とりあえずリロードで対処.
	if (m_renameEndFunc)
		m_renameEndFunc();

}
