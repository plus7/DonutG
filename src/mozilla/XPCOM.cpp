/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: Mozilla-sample-code 1.0
 *
 * Copyright (c) 2002 Netscape Communications Corporation and
 * other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this Mozilla sample software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Contributor(s):
 *   Doug Turner <dougt@netscape.com>
 *   Adam Lock <adamlock@netscape.com>
 *   NOSE, Takafumi <ahya365@gmail.com>
 *
 * ***** END LICENSE BLOCK ***** */

#include "stdafx.h"
/* XPCOM */
#include "nsXULAppAPI.h"

XRE_InitEmbeddingType XRE_InitEmbedding;
XRE_TermEmbeddingType XRE_TermEmbedding;

#include "nsXPCOMGlue.h"
#include "nsStringAPI.h"
#include "nsAppDirectoryServiceDefs.h"
#include "nsDirectoryServiceDefs.h"
#include "nsProfileDirServiceProvider.h"
#include "nsIWindowWatcher.h"
#include "WindowCreator.h"
#include "nsIGenericFactory.h" 
#include "nsIComponentRegistrar.h"
#include "nsIComponentManager.h"
#include "PromptService.h"
#include "GeckoPrintingPrompt.h"
#include "XPCOMAPI.h"

#include "XPCOM.h"

char*
ns_strrpbrk(char *string, const char *strCharSet)
{
    char *found = NULL;
    for (; *string; ++string) {
        for (const char *search = strCharSet; *search; ++search) {
            if (*search == *string) {
                found = string;
                // Since we're looking for the last char, we save "found"
                // until we're at the end of the string.
            }
        }
    }

    return found;
}


int InitWindowCreator(){
	nsCOMPtr<nsIWindowCreator> creator(new CWindowCreator());
	if (!creator)
		return -1;
	nsCOMPtr<nsIWindowWatcher> wwatch(do_GetService(NS_WINDOWWATCHER_CONTRACTID));
	if (!wwatch)
		return -1;
	return wwatch->SetWindowCreator(creator);
}

#define NS_PROMPTSERVICE_CID {0x95611356, 0xf583, 0x46f5, {0x81, 0xff, 0x4b, 0x3e, 0x01, 0x62, 0xc6, 0x19}}

#define NS_PRINTINGPROMPTSERVICE_CID {0xe042570c, 0x62de, 0x4bb6, { 0xa6, 0xe0, 0x79, 0x8e, 0x3c, 0x7, 0xb4, 0xdf}}

#define DONUTAPI_CID { 0xbde1eb6, 0xe1b4, 0x4ab0, { 0xa5, 0xc6, 0x94, 0xf7, 0x3e, 0xcc, 0xb3, 0xfe } } // {0BDE1EB6-E1B4-4ab0-A5C6-94F73ECCB3FE}

NS_GENERIC_FACTORY_CONSTRUCTOR(CPromptService)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsPrintingPromptService)
NS_GENERIC_FACTORY_CONSTRUCTOR(DonutAPI)

const nsModuleComponentInfo defaultAppComps[] = 
{
	{
		"Prompt Service",
	   	NS_PROMPTSERVICE_CID,
		"@mozilla.org/embedcomp/prompt-service;1",
		CPromptServiceConstructor
	},
	{
		"Printing Prompt Service",
	   	NS_PRINTINGPROMPTSERVICE_CID,
		"@mozilla.org/embedcomp/printingprompt-service;1",
		nsPrintingPromptServiceConstructor
	},
	{
		"Donut XPCOM API Service",
	   	NS_PRINTINGPROMPTSERVICE_CID,
		"@tnose.net/donut/api-service;1",
		DonutAPIConstructor
	}
};


void RegisterAdditionalComponents(){
	//Gecko 1.9
	nsCOMPtr<nsIComponentRegistrar> cr;
	nsresult rv = NS_GetComponentRegistrar(getter_AddRefs(cr));
	if(NS_FAILED(rv)) return;

    nsCOMPtr<nsIFactory> psfactory;
    CPromptServiceFactory* p = new CPromptServiceFactory();
    NS_ADDREF(p);
	psfactory = p;

    nsCID cid = NS_PROMPTSERVICE_CID;
    rv = cr->RegisterFactory(cid,
		                     "Prompt Service",
                             "@mozilla.org/embedcomp/prompt-service;1",
                             psfactory);

    nsCOMPtr<nsIFactory> ppsfactory;
    CPrintingPromptServiceFactory* p2 = new CPrintingPromptServiceFactory();
    NS_ADDREF(p2);
	ppsfactory = p2;

    nsCID cid2 = NS_PRINTINGPROMPTSERVICE_CID;
    rv = cr->RegisterFactory(cid2,
		                     "Printing Prompt Service",
							 "@mozilla.org/embedcomp/printingprompt-service;1",
                             ppsfactory);

    nsCOMPtr<nsIFactory> dntfactory;
    DonutAPIFactory* p3 = new DonutAPIFactory();
    NS_ADDREF(p3);
	dntfactory = p3;

    nsCID cid3 = DONUTAPI_CID;
    rv = cr->RegisterFactory(cid3,
		                     "Donut XPCOM API Service",
							 "@tnose.net/donut/api-service;1",
                             dntfactory);

   //2.0 XRE_AddStaticComponent(&kDefaultPromptModule);
}

 int StartXPCOM(){
	nsresult rv;
    const GREVersionRange vr = {
        "1.9.2",
        PR_TRUE,
        "2.0",
        PR_FALSE
    };

    char xpcomPath[_MAX_PATH];
    rv = GRE_GetGREPathWithProperties(&vr, 1, nsnull, 0,
                                      xpcomPath, sizeof(xpcomPath));
    if (NS_FAILED(rv))
        return 1;
    char *lastslash = ns_strrpbrk(xpcomPath, "/\\");
    if (!lastslash)
        return 2;

    rv = XPCOMGlueStartup(xpcomPath);
    if (NS_FAILED(rv))
        return 3;

    *lastslash = '\0';

    char xulPath[_MAX_PATH];
    _snprintf(xulPath, sizeof(xulPath), "%s\\xul.dll", xpcomPath);
    xulPath[sizeof(xulPath) - 1] = '\0';


    HINSTANCE xulModule = LoadLibraryEx(CString(xulPath), NULL, 0);
    if (!xulModule)
        return 4;

    TCHAR temp[_MAX_PATH];
    GetModuleFileName(xulModule, temp, sizeof(temp));

    XRE_InitEmbedding =
        (XRE_InitEmbeddingType) GetProcAddress(xulModule, "XRE_InitEmbedding");
    if (!XRE_InitEmbedding) {
       //fprintf(stderr, "Error: %i\n", GetLastError());
       return 5;
    }

    XRE_TermEmbedding =
        (XRE_TermEmbeddingType) GetProcAddress(xulModule,  "XRE_TermEmbedding");
    if (!XRE_TermEmbedding) {
        fprintf(stderr, "Error: %i\n", GetLastError());
        return 5;
    }

    // Scope all the XPCOM stuff
    {
        nsCOMPtr<nsILocalFile> xuldir;
        rv = NS_NewNativeLocalFile(nsCString(xpcomPath), PR_FALSE,
                                   getter_AddRefs(xuldir));
        if (NS_FAILED(rv))
            return 6;

        TCHAR self[_MAX_PATH];
		HINSTANCE hInstanceApp = NULL;
		hInstanceApp = GetModuleHandle(NULL);
        GetModuleFileName(hInstanceApp, self, sizeof(self));
        lastslash = ns_strrpbrk(xpcomPath, "/\\");
        if (!lastslash)
            return 7;

        *lastslash = '\0';

        nsCOMPtr<nsILocalFile> appdir;
		nsEmbedCString cself;
		NS_UTF16ToCString(nsEmbedString(self), NS_CSTRING_ENCODING_UTF8, cself);
        rv = NS_NewNativeLocalFile(cself, PR_FALSE,
                                   getter_AddRefs(appdir));
        if (NS_FAILED(rv))
            return 8;

        rv = XRE_InitEmbedding(xuldir, appdir, nsnull, nsnull, 0);
        if (NS_FAILED(rv))
            return 9;

		RegisterAdditionalComponents();

	}
	return rv;
}

int TermXPCOM(){
	XRE_TermEmbedding();
	XPCOMGlueShutdown();
	return 0;
}

