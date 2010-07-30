#pragma once

//Donut ExtendedStyle
enum EDvs_Ex {
	DVS_EX_OPENNEWWIN		= 0x00000001L,
  #ifdef USE_ORG_UNDONUT_INI				//+++ unDonutオリジナルの値
	DVS_EX_MESSAGE_FILTER	= 0x00000002L,
	DVS_EX_MOUSE_GESTURE	= 0x00000004L,
	DVS_EX_BLOCK_MAILTO 	= 0x00000008L,
	DVS_EX_FLATVIEW 		= 0x00000010L,	//+++ オリジナルにはないので適当に設定.
  #else										//+++ unDonut+ での値 (+modも)
	DVS_EX_FLATVIEW 		= 0x00000002L,
	DVS_EX_MESSAGE_FILTER	= 0x00000004L,
	DVS_EX_MOUSE_GESTURE	= 0x00000008L,
	DVS_EX_BLOCK_MAILTO 	= 0x00000010L,
  #endif
};

//+++ メモ:donut固有の定義でなく、ライブラリ側の定義.
//DL_Control_Flags
#define DLCTL_DLIMAGES			0x00000010
#define DLCTL_VIDEOS			0x00000020
#define DLCTL_BGSOUNDS			0x00000040
#define DLCTL_NO_SCRIPTS		0x00000080
#define DLCTL_NO_JAVA			0x00000100
#define DLCTL_NO_RUNACTIVEXCTLS 0x00000200
#define DLCTL_NO_DLACTIVEXCTLS	0x00000400


//AuroRefresh_Flags
enum EDvs_AutoRefresh {
	DVS_AUTOREFRESH_15SEC	= 0x00000001L,
	DVS_AUTOREFRESH_30SEC	= 0x00000002L,
	DVS_AUTOREFRESH_1MIN	= 0x00000004L,
	DVS_AUTOREFRESH_2MIN	= 0x00000008L,
	DVS_AUTOREFRESH_5MIN	= 0x00000010L,
	DVS_AUTOREFRESH_USER	= 0x00000020L,	// UDT DGSTR ( dai
};


//Get/SetExtendedStyle Api Flags
enum EFlag_Se {
	FLAG_SE_DLIMAGES		=	0x00000001,
	FLAG_SE_VIDEOS			=	0x00000002,
	FLAG_SE_BGSOUNDS		=	0x00000004,
	FLAG_SE_RUNACTIVEXCTLS	=	0x00000008,
	FLAG_SE_DLACTIVEXCTLS	=	0x00000010,
	FLAG_SE_SCRIPTS 		=	0x00000020,
	FLAG_SE_JAVA			=	0x00000040,

	FLAG_SE_NAVIGATELOCK	=	0x00000080,
#ifdef USE_ORG_UNDONUT_INI					//+++ unDonut オリジナル版の値
	FLAG_SE_MSGFILTER		=	0x00000100,
	FLAG_SE_MOUSEGESTURE	=	0x00000200,
	FLAG_SE_BLOCKMAILTO 	=	0x00000400,
	FLAG_SE_FLATVIEW		=	0x00000800, //+++ オリジナルにはないが設定.
#else										//+++ unDonut+ の値.
	FLAG_SE_FLATVIEW		=	0x00000100,
	FLAG_SE_MSGFILTER		=	0x00000200,
	FLAG_SE_MOUSEGESTURE	=	0x00000400,
	FLAG_SE_BLOCKMAILTO 	=	0x00000800,
#endif

	FLAG_SE_VIEWED			=	0x00001000,

	FLAG_SE_REFRESH_NONE	=	0x00010000,
	FLAG_SE_REFRESH_15		=	0x00020000,
	FLAG_SE_REFRESH_30		=	0x00040000,
	FLAG_SE_REFRESH_1M		=	0x00080000,
	FLAG_SE_REFRESH_2M		=	0x00100000,
	FLAG_SE_REFRESH_5M		=	0x00200000,
	FLAG_SE_REFRESH_USER	=	0x00400000,

	FLAG_SE_NOREFRESH		=	0x01000000,

	FLAGS_SE_RELOAD 		= (FLAG_SE_REFRESH_NONE | FLAG_SE_REFRESH_15 | FLAG_SE_REFRESH_30 | FLAG_SE_REFRESH_1M | FLAG_SE_REFRESH_2M | FLAG_SE_REFRESH_5M | FLAG_SE_REFRESH_USER),
};


//IAPI2のGet/SetExtendedTabStateのフラグ変数と
//お気に入りグループのフラグ変数の相互変換を行うクラス
class CApiExStyle {
public:
	DWORD m_dwApiExStyle;

	CApiExStyle() : m_dwApiExStyle(0)
	{
	}


	CApiExStyle(DWORD dwApiExStyle) : m_dwApiExStyle(dwApiExStyle)
	{
	}


	//セキュリティ関連のフラグ変数を取得する
	int GetDLControlFlags()
	{
		DWORD dwDLFlags = 0;

		if (m_dwApiExStyle & FLAG_SE_DLIMAGES)			dwDLFlags |= DLCTL_DLIMAGES;
		if (m_dwApiExStyle & FLAG_SE_VIDEOS)			dwDLFlags |= DLCTL_VIDEOS;
		if (m_dwApiExStyle & FLAG_SE_BGSOUNDS)			dwDLFlags |= DLCTL_BGSOUNDS;

		if (!(m_dwApiExStyle & FLAG_SE_RUNACTIVEXCTLS))	dwDLFlags |= DLCTL_NO_RUNACTIVEXCTLS;
		if (!(m_dwApiExStyle & FLAG_SE_DLACTIVEXCTLS ))	dwDLFlags |= DLCTL_NO_DLACTIVEXCTLS;
		if (!(m_dwApiExStyle & FLAG_SE_SCRIPTS) )		dwDLFlags |= DLCTL_NO_SCRIPTS;
		if (!(m_dwApiExStyle & FLAG_SE_JAVA) )			dwDLFlags |= DLCTL_NO_JAVA ;

		return dwDLFlags;
	}


	//タブの拡張スタイルを取得する
	int GetExtendedStyleFlags()
	{
		DWORD dwExFlag = 0;
		if (m_dwApiExStyle & FLAG_SE_NAVIGATELOCK)		dwExFlag |= DVS_EX_OPENNEWWIN;
		if (m_dwApiExStyle & FLAG_SE_MSGFILTER)			dwExFlag |= DVS_EX_MESSAGE_FILTER;
		if (m_dwApiExStyle & FLAG_SE_MOUSEGESTURE)		dwExFlag |= DVS_EX_MOUSE_GESTURE;
		if (m_dwApiExStyle & FLAG_SE_BLOCKMAILTO)		dwExFlag |= DVS_EX_BLOCK_MAILTO;
		return dwExFlag;
	}


	//自動更新のフラグ変数を取得する
	int GetAutoRefreshFlag()
	{
		DWORD dwRefreshFlag = m_dwApiExStyle / (FLAG_SE_REFRESH_15 / DVS_AUTOREFRESH_15SEC);
		DWORD dwCurFlag 	= DVS_AUTOREFRESH_USER;

		while (dwCurFlag > 0) {
			if (dwCurFlag & dwRefreshFlag) {
				return dwCurFlag;
			}
			dwCurFlag >>= 1;
		}

		return 0;
	}


	//セキュリティ・拡張スタイル・自動更新のフラグを与えてAPI形式のフラグ変数に変換する
	void SetApiExtendedStyle(DWORD dwDLFlags, DWORD dwExFlags, DWORD dwRefreshFlag)
	{
		DWORD dwApiStyle = 0;

		if (dwDLFlags & DLCTL_DLIMAGES)				dwApiStyle |= FLAG_SE_DLIMAGES;
		if (dwDLFlags & DLCTL_VIDEOS)				dwApiStyle |= FLAG_SE_VIDEOS;
		if (dwDLFlags & DLCTL_BGSOUNDS)				dwApiStyle |= FLAG_SE_BGSOUNDS;

		if (!(dwDLFlags & DLCTL_NO_RUNACTIVEXCTLS))	dwApiStyle |= FLAG_SE_RUNACTIVEXCTLS;
		if (!(dwDLFlags & DLCTL_NO_DLACTIVEXCTLS )) dwApiStyle |= FLAG_SE_DLACTIVEXCTLS;
		if (!(dwDLFlags & DLCTL_NO_SCRIPTS) )		dwApiStyle |= FLAG_SE_SCRIPTS;
		if (!(dwDLFlags & DLCTL_NO_JAVA) )			dwApiStyle |= FLAG_SE_JAVA;

		if (dwExFlags & DVS_EX_OPENNEWWIN)			dwApiStyle |= FLAG_SE_NAVIGATELOCK;
		if (dwExFlags & DVS_EX_MESSAGE_FILTER)		dwApiStyle |= FLAG_SE_MSGFILTER;
		if (dwExFlags & DVS_EX_MOUSE_GESTURE)		dwApiStyle |= FLAG_SE_MOUSEGESTURE;
		if (dwExFlags & DVS_EX_BLOCK_MAILTO)		dwApiStyle |= FLAG_SE_BLOCKMAILTO;
		if (dwRefreshFlag)							dwApiStyle |= ( dwRefreshFlag * ( FLAG_SE_REFRESH_15 / DVS_AUTOREFRESH_15SEC) );
		else										dwApiStyle |= FLAG_SE_REFRESH_NONE;

		m_dwApiExStyle = dwApiStyle;
	}
};
