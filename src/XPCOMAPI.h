#pragma once

#include "DonutG.h"
#include "nsIFactory.h"

//#include "MainFrame.h"
class CMainFrame;
extern CMainFrame *g_pMainWnd;

/* Header file */
class DonutAPI : public donutIAPI
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_DONUTIAPI

  DonutAPI();

private:
  ~DonutAPI();

protected:
  /* additional members */
};

class DonutAPIFactory : public nsIFactory
{
public:
    DonutAPIFactory(){}

    NS_DECL_ISUPPORTS
    
    NS_IMETHOD CreateInstance(nsISupports *aOuter, const nsIID & iid, void * *result)
    {
        nsresult rv;
        
        if (!result) return NS_ERROR_NULL_POINTER;
        if (aOuter)   return NS_ERROR_NO_AGGREGATION;
        
        DonutAPI* p = new DonutAPI;
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