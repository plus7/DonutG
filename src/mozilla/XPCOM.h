#ifndef XPCOM_H
#define XPCOM_H

#include <nsIDirectoryService.h>

class DonutDirectoryServiceProvider : public nsIDirectoryServiceProvider
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDIRECTORYSERVICEPROVIDER

  DonutDirectoryServiceProvider();
  ~DonutDirectoryServiceProvider();
};


int StartXPCOM();
int InitWindowCreator();
int TermXPCOM();

#endif