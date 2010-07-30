#pragma once

#include "nsIWindowCreator.h"
#include "nsIWebBrowserChrome.h"
class CWindowCreator:public nsIWindowCreator
{
public:
	CWindowCreator(void);
	virtual ~CWindowCreator(void);
	NS_DECL_ISUPPORTS
	NS_DECL_NSIWINDOWCREATOR
};

