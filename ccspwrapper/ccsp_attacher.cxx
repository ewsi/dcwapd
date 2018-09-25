

#include "./ccsp_attacher.h"
#include "./ccsp_config.h"

#include <exception>

#include "dcw/dcwlog.h"

namespace {
struct CCSPAttachException : public std::exception {
  virtual const char* what() const throw() {
    return "CSSP Attach Failed";
  }
};
};

using namespace ccspwrapper;


CcspAttacher::CcspAttacher(const CcspConfig& cfg, Tr181ConfigProvider& configProvider) :
  MessageBus(cfg),
  CcdInterface(cfg),
  Tr181Interface(configProvider),
  _cfg(cfg),
  _cdmClient(_cfg.ComponentId, GetBusHandle(), _cfg.GetSubsystemPrefix().c_str()) {


  dcwlogdbgf("%s\n", "CCSP Attacher Base Initialization Successful");

  _pDslhCpeController = (PDSLH_CPE_CONTROLLER_OBJECT)DslhCreateCpeController(NULL, NULL, NULL);
  if (_pDslhCpeController == NULL) {
    dcwlogerrf("%s\n", "DslhCreateCpeController() failed!");
    throw CCSPAttachException();
  }

  dcwlogdbgf("%s\n", "DslhCreateCpeController() OK");

  _pSsdMbiIf = (PCCC_MBI_INTERFACE)MsgHelper_CreateCcdMbiIf(this->GetBusHandle(), (char*)_cfg.GetSubsystemPrefix().c_str());

  (*_pDslhCpeController->AddInterface)((ANSC_HANDLE)_pDslhCpeController, this->GetTr181InterfaceHandle());
  (*_pDslhCpeController->AddInterface)((ANSC_HANDLE)_pDslhCpeController, (ANSC_HANDLE)_pSsdMbiIf);
  (*_pDslhCpeController->AddInterface)((ANSC_HANDLE)_pDslhCpeController, this->GetCcdHandle());
  (*_pDslhCpeController->SetDbusHandle)((ANSC_HANDLE)_pDslhCpeController, this->GetBusHandle());
  (*_pDslhCpeController->Engage)((ANSC_HANDLE)_pDslhCpeController);

  if ((*_pDslhCpeController->RegisterCcspDataModel)(
    (ANSC_HANDLE)_pDslhCpeController,
    (char*)_cfg.ApplySubsystemPrefix(CCSP_DBUS_INTERFACE_CR).c_str(),
    (char*)_cfg.GetTr181Filename().c_str(),
    (char*)_cfg.ComponentName,
    _cfg.Version,
    (char*)_cfg.DbusPath,
    (char*)_cfg.GetSubsystemPrefix().c_str()
  ) != ANSC_STATUS_SUCCESS) {
    dcwlogerrf("%s\n", "(*_pDslhCpeController->RegisterCcspDataModel)() failed!");
    throw CCSPAttachException();
  }

  dcwloginfof("%s\n", "CCSP Attach Success");
}

CcspAttacher::~CcspAttacher() {
  //XXX theres probably a leak here... however, on shutdown, who really cares...

  dcwlogdbgf("%s\n", "CCSP Attacher Cleaning Up...");

  CcspBaseIf_unregisterComponent(
    this->GetBusHandle(),
    (char*)_cfg.ApplySubsystemPrefix(CCSP_DBUS_INTERFACE_CR).c_str(),
    (char*)_cfg.ApplySubsystemPrefix(_cfg.ComponentName).c_str()  //XXX ComponentName vs ComponentId ?
  );

  (*_pDslhCpeController->Cancel)((ANSC_HANDLE)_pDslhCpeController);
  AnscFreeMemory(_pDslhCpeController);

  dcwlogdbgf("%s\n", "CCSP Attacher Destroyed");
}

CdmClient& CcspAttacher::GetCdmClient() {
  return _cdmClient;
}
