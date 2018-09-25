

#include "./ccd_interface.h"
#include "./ccsp_config.h"

#include <string.h>
#include <strings.h>


#include "dcw/dcwlog.h"



using namespace ccspwrapper;

CcdInterface::CcdInterface(const CcspConfig& cfg) : _cfg(cfg) {
  bzero(&_ccdInterface, sizeof(_ccdInterface));

  strncpy(_ccdInterface.Name, CCSP_CCD_INTERFACE_NAME, sizeof(_ccdInterface.Name));
  _ccdInterface.InterfaceId        = CCSP_CCD_INTERFACE_ID;
  _ccdInterface.hOwnerContext      = this;
  _ccdInterface.Size               = sizeof(_ccdInterface);

  _ccdInterface.GetComponentName     = &CcdInterface::_c_GetComponentName;
  _ccdInterface.GetComponentVersion  = &CcdInterface::_c_GetComponentVersion;
  _ccdInterface.GetComponentAuthor   = &CcdInterface::_c_GetComponentAuthor;
  _ccdInterface.GetComponentHealth   = &CcdInterface::_c_GetComponentHealth;
  _ccdInterface.GetComponentState    = &CcdInterface::_c_GetComponentState;
  _ccdInterface.GetLoggingEnabled    = &CcdInterface::_c_GetLoggingEnabled;
  _ccdInterface.SetLoggingEnabled    = &CcdInterface::_c_SetLoggingEnabled;
  _ccdInterface.GetLoggingLevel      = &CcdInterface::_c_GetLoggingLevel;
  _ccdInterface.SetLoggingLevel      = &CcdInterface::_c_SetLoggingLevel;
  _ccdInterface.GetMemMaxUsage       = &CcdInterface::_c_GetMemMaxUsage;
  _ccdInterface.GetMemMinUsage       = &CcdInterface::_c_GetMemMinUsage;
  _ccdInterface.GetMemConsumed       = &CcdInterface::_c_GetMemConsumed;
  _ccdInterface.ApplyChanges         = &CcdInterface::_c_ApplyChanges;

  dcwlogdbgf("%s\n", "CCSP CCD Interface Initialized");
}

CcdInterface::~CcdInterface() {
  dcwlogdbgf("%s\n", "CCSP CCD Interface Destroyed");
}

ANSC_HANDLE CcdInterface::GetCcdHandle() const {
  return (ANSC_HANDLE)&_ccdInterface;
}

//virtual/overriable version of the CCD functions: 
const char *CcdInterface::GetComponentName() {
  return _cfg.ComponentName;
}

ULONG CcdInterface::GetComponentVersion() {
 return _cfg.Version;
}

const char *CcdInterface::GetComponentAuthor() {
  return "CableLabs";
}

ULONG CcdInterface::GetComponentHealth() {
  return 1;
}

ULONG CcdInterface::GetComponentState() {
  return 1;
}

BOOL CcdInterface::GetLoggingEnabled() {
  return TRUE;
}

ANSC_STATUS CcdInterface::SetLoggingEnabled(BOOL bEnabled) {
  return ANSC_STATUS_SUCCESS;
}

ULONG CcdInterface::GetLoggingLevel() {
  return CCSP_TRACE_LEVEL_DEBUG;
}

ANSC_STATUS CcdInterface::SetLoggingLevel(ULONG logLevel) {
  return ANSC_STATUS_SUCCESS;
}

ULONG CcdInterface::GetMemMaxUsage() {
  return 0;
}

ULONG CcdInterface::GetMemMinUsage() {
  return 0;
}

ULONG CcdInterface::GetMemConsumed() {
  return 0;
}

ANSC_STATUS CcdInterface::ApplyChanges() {
  return ANSC_STATUS_SUCCESS;
}






// all the CCD -> C++ "vtable glue" functions begin here...
#define CIOBJ ((CcdInterface*)hThisObject)
char *CcdInterface::_c_GetComponentName(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return (char*)CIOBJ->GetComponentName();
}

ULONG CcdInterface::_c_GetComponentVersion(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetComponentVersion();
}

char *CcdInterface::_c_GetComponentAuthor(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return (char*)CIOBJ->GetComponentAuthor();
}

ULONG CcdInterface::_c_GetComponentHealth(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetComponentHealth();
}

ULONG CcdInterface::_c_GetComponentState(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetComponentState();
}

BOOL CcdInterface::_c_GetLoggingEnabled(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetLoggingEnabled();
}

ANSC_STATUS CcdInterface::_c_SetLoggingEnabled(ANSC_HANDLE hThisObject, BOOL bEnabled) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->SetLoggingEnabled(bEnabled);
}

ULONG CcdInterface::_c_GetLoggingLevel(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetLoggingLevel();
}

ANSC_STATUS CcdInterface::_c_SetLoggingLevel(ANSC_HANDLE hThisObject, ULONG logLevel) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->SetLoggingLevel(logLevel);
}

ULONG CcdInterface::_c_GetMemMaxUsage(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetMemMaxUsage();
}

ULONG CcdInterface::_c_GetMemMinUsage(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetMemMinUsage();
}

ULONG CcdInterface::_c_GetMemConsumed(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->GetMemConsumed();
}

ANSC_STATUS CcdInterface::_c_ApplyChanges(ANSC_HANDLE hThisObject) {
dcwlogdbgf("here %s:%d\n", __FILE__, __LINE__);
  return CIOBJ->ApplyChanges();
}





