#ifndef CCSP_ATTACHER_H_INCLUDED
#define CCSP_ATTACHER_H_INCLUDED


#include "./ccsp_include.h"
#include "./message_bus.h"
#include "./ccd_interface.h"
#include "./tr181_interface.h"
#include "./cdm_client_impl.h"

namespace ccspwrapper {

class CcspConfig;
struct Tr181ConfigProvider;

class CcspAttacher : public MessageBus, public CcdInterface, public Tr181Interface {
public:
  CcspAttacher(const CcspConfig& config, Tr181ConfigProvider& configProvider);
  virtual ~CcspAttacher();

  CdmClient& GetCdmClient();

private:
  CcspAttacher(const CcspAttacher&); //no copy

  const CcspConfig&             _cfg;
  PCCC_MBI_INTERFACE            _pSsdMbiIf;
  PDSLH_CPE_CONTROLLER_OBJECT   _pDslhCpeController;
  CdmClientImpl                 _cdmClient;
};

} //namespace ccspwrapper

#endif //#ifndef CCSP_ATTACHER_H_INCLUDED

