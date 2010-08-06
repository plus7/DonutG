/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM DonutScriptLoader.idl
 */

#ifndef __gen_DonutScriptLoader_h__
#define __gen_DonutScriptLoader_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIDOMWindow; /* forward declaration */

class nsILocalFile; /* forward declaration */


/* starting interface:    donutIScriptLoader */
#define DONUTISCRIPTLOADER_IID_STR "c0db1326-c4e7-44ca-a930-c2019a8ae4aa"

#define DONUTISCRIPTLOADER_IID \
  {0xc0db1326, 0xc4e7, 0x44ca, \
    { 0xa9, 0x30, 0xc2, 0x01, 0x9a, 0x8a, 0xe4, 0xaa }}

class NS_NO_VTABLE NS_SCRIPTABLE donutIScriptLoader : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(DONUTISCRIPTLOADER_IID)

  /* void loadFile (in nsILocalFile file); */
  NS_SCRIPTABLE NS_IMETHOD LoadFile(nsILocalFile *file) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(donutIScriptLoader, DONUTISCRIPTLOADER_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_DONUTISCRIPTLOADER \
  NS_SCRIPTABLE NS_IMETHOD LoadFile(nsILocalFile *file); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_DONUTISCRIPTLOADER(_to) \
  NS_SCRIPTABLE NS_IMETHOD LoadFile(nsILocalFile *file) { return _to LoadFile(file); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_DONUTISCRIPTLOADER(_to) \
  NS_SCRIPTABLE NS_IMETHOD LoadFile(nsILocalFile *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->LoadFile(file); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public donutIScriptLoader
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_DONUTISCRIPTLOADER

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, donutIScriptLoader)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* void loadFile (in nsILocalFile file); */
NS_IMETHODIMP _MYCLASS_::LoadFile(nsILocalFile *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_DonutScriptLoader_h__ */
