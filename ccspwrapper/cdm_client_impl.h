#ifndef CDM_CLIENT_IMPL_H_INCLUDED
#define CDM_CLIENT_IMPL_H_INCLUDED

#include "./cdm_client.h"
#include "./ccsp_include.h"

namespace ccspwrapper {

class CdmClientImpl : public CdmClient {
public:
  //note:
  //      . null bus handle = create
  //      . null subsystem  = "eRT"
  //      . unfortunately component id cant be null...
  CdmClientImpl(const char * const componentId = "TestCdmClient", ANSC_HANDLE busHandle = NULL, const char * const subsystem = NULL);
  virtual ~CdmClientImpl();

  virtual CopyScalar GetValue(const char * const path) const;
  virtual void SetValue(const char * const path, const Tr181Scalar& value);
};

}


#endif //#ifndef CDM_CLIENT_IMPL_H_INCLUDED
