/**
 *	@file	appconst.cpp
 *	@brief	アプリケーション独自のデータを記述するファイル
 */
#include "stdafx.h"
#include "appconst.h"

namespace app {


#define VERSTR			_T("0.2.7")

#ifdef UNICODE
#define MBVER
#else
#define MBVER			_T("-mb")
#endif

#ifdef USE_DIET
#define DIETVER			_T("-DIET")
#else
#define DIETVER
#endif

/// アプリケーション名
const TCHAR *cnt_AppName = DONUT_NAME;

#if defined _WIN64
/// アプリケーションバージョン
const TCHAR *cnt_AppVersion = VERSTR DIETVER MBVER ;
#else
/// アプリケーションバージョン
const TCHAR *cnt_AppVersion = VERSTR DIETVER MBVER;
#endif



} //namespace app
