#pragma once

#include "nsIPromptService.h"
#include "nsIPromptService2.h"
#include "nsIFactory.h"

class CPromptService: 
	public nsIPromptService2
{
public:
	CPromptService(void);
	virtual ~CPromptService(void);
	HWND GetHwndForWindow(nsIDOMWindow *win);
	NS_DECL_ISUPPORTS
	NS_DECL_NSIPROMPTSERVICE
	NS_DECL_NSIPROMPTSERVICE2
};

/*Gecko 2.0
		NS_DEFINE_NAMED_CID(NS_PROMPTSERVICE_CID);

		static const mozilla::Module::CIDEntry kDefaultPromptCIDs[] = {
		  { &kNS_PROMPTSERVICE_CID, false, NULL, CPromptServiceConstructor },
		  { NULL }
		};

		static const mozilla::Module::ContractIDEntry kDefaultPromptContracts[] = {
		  { "@mozilla.org/embedcomp/prompt-service;1", &kNS_PROMPTSERVICE_CID },
		  { NULL }
		};

		static const mozilla::Module kDefaultPromptModule = {
		  mozilla::Module::kVersion,
		  kDefaultPromptCIDs,
		  kDefaultPromptContracts
		};
*/

class CPromptServiceFactory : public nsIFactory
{
public:
    CPromptServiceFactory(){}

    NS_DECL_ISUPPORTS
    
    NS_IMETHOD CreateInstance(nsISupports *aOuter, const nsIID & iid, void * *result)
    {
        nsresult rv;
        
        if (!result) return NS_ERROR_NULL_POINTER;
        if (aOuter)   return NS_ERROR_NO_AGGREGATION;
        
        CPromptService* p = new CPromptService;
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