#ifndef CCSP_CALLBACKS_H_INCLUDED
#define CCSP_CALLBACKS_H_INCLUDED


//
//
//
// These classes are responsible for re-marshaling the 
// function parameters and return values from the
// CCSP C callbacks invoked by the FF trampolines 
//
//
// Each one of these classes implements the
// "FfCallback::Callable" interface/abstract class
// so that it may be passed into the "FfCallback"
// constructor; this enforces a design of one
// callback funtion per class
//
//
// Multiple inheritance is then used to combine all
// callback functions into one single class instance.
// This enables a single class instance to have several
// instances of the "FfCallback::Callable" interface.
//
//
// In order to access a specific "Callable" base, one
// must simply cast the "this" pointer. Example:
//   new FfCallback(*(CcspCbCosaInit*)this);
//
//
// When getting called back from the CCSP library,
// the call chain will look something like this:
//              << CCSP Library >>
//                      |
//                      V
//      << dynamic libffcall trampoline >>
//                      |
//                      V
//          FfCallback::_c_ffCallback()
//                      |
//                      V
//         CcspCbCosaInit::OnFfCallback()
//                      |
//                      V
//           Tr181Interface::COSAInit()
//                      |
//                      V
//  << ...and whatever else in the C++ world >>
//
//
// Note: CcspCbCosaInit/COSAInit() was used as
//       an example, but could be replaced 
//       with any of these classes/functions...
//
// The "FfCallback::_c_ffCallback()" function is
// what actually recovers the C++ "this" pointer
// and performs the glue/transition from the C
// world back to the C++ world.
//
//



#include "./ff_callback.h"
#include "./ccsp_include.h"

namespace ccspwrapper {


// CCSP init function:
class CcspCbCosaInit : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual int COSAInit(FfCallback& caller, ULONG uMaxVersionSupported, void *hCosaPlugInfo) = 0;
};


// CCSP Ulong Getter/Setter:
class CcspCbGetParamUlongValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL GetParamUlongValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG *value) = 0;
};
class CcspCbSetParamUlongValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL SetParamUlongValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG value) = 0;
};

// CCSP Int Getter/Setter:
class CcspCbGetParamIntValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL GetParamIntValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, int *value) = 0;
};
class CcspCbSetParamIntValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL SetParamIntValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, int value) = 0;
};

// CCSP Bool Getter/Setter:
class CcspCbGetParamBoolValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL GetParamBoolValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, BOOL *value) = 0;
};
class CcspCbSetParamBoolValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL SetParamBoolValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, BOOL value) = 0;
};

// CCSP String Getter/Setter:
class CcspCbGetParamStringValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ULONG GetParamStringValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, char *value, ULONG *valueSize) = 0;
};
class CcspCbSetParamStringValue : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL SetParamStringValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, char *value) = 0;
};

// CCSP concurrency functions...
class CcspCbValidate : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL Validate(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG *length) = 0;
};
class CcspCbCommit : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ULONG Commit(FfCallback& caller, ANSC_HANDLE ctx) = 0;
};
class CcspCbRollback : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ULONG Rollback(FfCallback& caller, ANSC_HANDLE ctx) = 0;
};

// CCSP table functions...
class CcspCbGetEntryCount : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ULONG GetEntryCount(FfCallback& caller, ANSC_HANDLE ctx) = 0;
};
class CcspCbGetEntry : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ANSC_HANDLE GetEntry(FfCallback& caller, ANSC_HANDLE ctx, ULONG nIndex, ULONG *pInsNum) = 0;
};
class CcspCbAddEntry : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ANSC_HANDLE AddEntry(FfCallback& caller, ANSC_HANDLE ctx, PULONG pInsNumber) = 0;
};
class CcspCbDelEntry : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ULONG DelEntry(FfCallback& caller, ANSC_HANDLE ctx, ANSC_HANDLE hInstance) = 0;
};
class CcspCbIsUpdated : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual BOOL IsUpdated(FfCallback& caller, ANSC_HANDLE ctx) = 0;
};
class CcspCbSynchronize : public FfCallback::Callable {
public:
  virtual void OnFfCallback(FfCallback& caller, va_alist alist);
  virtual ULONG Synchronize(FfCallback& caller, ANSC_HANDLE ctx) = 0;
};




//inherit from this to get all callbacks...
struct CcspCbAll :
  // CCSP init function:
  public CcspCbCosaInit,

  // CCSP Ulong Getter/Setter:
  public CcspCbGetParamUlongValue,
  public CcspCbSetParamUlongValue,

  // CCSP Int Getter/Setter:
  public CcspCbGetParamIntValue,
  public CcspCbSetParamIntValue,

  // CCSP Bool Getter/Setter:
  public CcspCbGetParamBoolValue,
  public CcspCbSetParamBoolValue,

  // CCSP String Getter/Setter:
  public CcspCbGetParamStringValue,
  public CcspCbSetParamStringValue,

  // CCSP concurrency functions...
  public CcspCbValidate,
  public CcspCbCommit,
  public CcspCbRollback,

  // CCSP table functions...
  public CcspCbGetEntryCount,
  public CcspCbGetEntry,
  public CcspCbAddEntry,
  public CcspCbDelEntry,
  public CcspCbIsUpdated,
  public CcspCbSynchronize
  {
  virtual ~CcspCbAll() {}
};


}; //namespace ccspwrapper

#endif //#ifndef CCSP_CALLBACKS_H_INCLUDED
