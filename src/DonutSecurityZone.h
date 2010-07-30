/**
 *	@file	DonutSecurityZone.h
 *	@brief	クッキーの扱いに関するコマンドメッセージをハンドリングするクラス
 */
#pragma once

#include "resource.h"
#include "MtlUpdateCmdUI.h"
#include "MtlWeb.h"


/*
	CDonutSecurityZone
	クッキーの扱いに関するコマンドメッセージをハンドリングするクラス
	Gecko用に全面書き換え中
 */

class CDonutSecurityZone {
public:
	// Message map and handlers
	BEGIN_MSG_MAP(CDonutSecurityZone)
		COMMAND_ID_HANDLER_EX(ID_COOKIES_ALL,        OnCookieAllowAll)
		COMMAND_ID_HANDLER_EX(ID_COOKIES_SAMEORIGIN, OnCookieAllowSameOrigin)
		COMMAND_ID_HANDLER_EX(ID_COOKIES_BLOCK,      OnCookieDenyAll)
		COMMAND_ID_HANDLER_EX(ID_COOKIES_CUSTOM,     OnCookieCustom)
	END_MSG_MAP()

private:
	void OnCookieAllowAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnCookieAllowSameOrigin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnCookieDenyAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	void OnCookieCustom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

public:
	// Update command UI and handlers
	BEGIN_UPDATE_COMMAND_UI_MAP(CDonutSecurityZone)
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_COOKIES_ALL,        (GetGlobalCookiePref() == 0) )
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_COOKIES_SAMEORIGIN, (GetGlobalCookiePref() == 1) )
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_COOKIES_BLOCK,      (GetGlobalCookiePref() == 2) )
		UPDATE_COMMAND_UI_SETCHECK_IF( ID_COOKIES_CUSTOM,     (GetGlobalCookiePref() == 3) )
	END_UPDATE_COMMAND_UI_MAP()
};
