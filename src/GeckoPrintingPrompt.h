#pragma once

#include "nsIPrintingPromptService.h"
#include "nsIFactory.h"
#include "mozilla/nsPrintingPromptService.h"

/*class CPrintingPromptService:
	public nsIPrintingPromptService
{
public:
	CPromptService(void);
	virtual ~CPromptService(void);
	NS_DECL_ISUPPORTS
	NS_DECL_NSIPRINTINGPROMPTSERVICE
};*/


class CPrintingPromptServiceFactory : public nsIFactory
{
public:
    CPrintingPromptServiceFactory(){}

    NS_DECL_ISUPPORTS
    
    NS_IMETHOD CreateInstance(nsISupports *aOuter, const nsIID & iid, void * *result)
    {
        nsresult rv;
        
        if (!result) return NS_ERROR_NULL_POINTER;
        if (aOuter)   return NS_ERROR_NO_AGGREGATION;
        
        nsPrintingPromptService* p = new nsPrintingPromptService;
        if (!p) return NS_ERROR_OUT_OF_MEMORY;
            
		NS_ADDREF(p);
        rv = p->QueryInterface(iid, result);
		NS_RELEASE(p);
        
        return rv;
    }
    
    NS_IMETHOD LockFactory(PRBool lock)
    {
        return NS_OK;
    }
};