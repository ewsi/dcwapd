#ifndef CCSP_CCD_INTERFACE_H_INCLUDED
#define CCSP_CCD_INTERFACE_H_INCLUDED


#include "./ccsp_include.h"


namespace ccspwrapper {

class CcspConfig;
class CcdInterface {
public:
  CcdInterface(const CcspConfig& cfg);
  virtual ~CcdInterface();

  ANSC_HANDLE GetCcdHandle() const;

protected:
  //virtual/overriable version of the CCD functions: 
  virtual const char *   GetComponentName();
  virtual ULONG          GetComponentVersion();
  virtual const char *   GetComponentAuthor();
  virtual ULONG          GetComponentHealth();
  virtual ULONG          GetComponentState();
  virtual BOOL           GetLoggingEnabled();
  virtual ANSC_STATUS    SetLoggingEnabled(BOOL bEnabled);
  virtual ULONG          GetLoggingLevel();
  virtual ANSC_STATUS    SetLoggingLevel(ULONG logLevel);
  virtual ULONG          GetMemMaxUsage();
  virtual ULONG          GetMemMinUsage();
  virtual ULONG          GetMemConsumed();
  virtual ANSC_STATUS    ApplyChanges();

private:
  CcdInterface(const CcdInterface &rhv); //no copy

  const CcspConfig& _cfg;
  CCSP_CCD_INTERFACE _ccdInterface;

  static char *         _c_GetComponentName(ANSC_HANDLE hThisObject);
  static ULONG          _c_GetComponentVersion(ANSC_HANDLE hThisObject);
  static char *         _c_GetComponentAuthor(ANSC_HANDLE hThisObject);
  static ULONG          _c_GetComponentHealth(ANSC_HANDLE hThisObject);
  static ULONG          _c_GetComponentState(ANSC_HANDLE hThisObject);
  static BOOL           _c_GetLoggingEnabled(ANSC_HANDLE hThisObject);
  static ANSC_STATUS    _c_SetLoggingEnabled(ANSC_HANDLE hThisObject, BOOL bEnabled);
  static ULONG          _c_GetLoggingLevel(ANSC_HANDLE hThisObject);
  static ANSC_STATUS    _c_SetLoggingLevel(ANSC_HANDLE hThisObject, ULONG logLevel);
  static ULONG          _c_GetMemMaxUsage(ANSC_HANDLE hThisObject);
  static ULONG          _c_GetMemMinUsage(ANSC_HANDLE hThisObject);
  static ULONG          _c_GetMemConsumed(ANSC_HANDLE hThisObject);
  static ANSC_STATUS    _c_ApplyChanges(ANSC_HANDLE hThisObject);

};

}; //namespace ccspwrapper


#endif //#ifndef CCSP_CCD_INTERFACE_H_INCLUDED
