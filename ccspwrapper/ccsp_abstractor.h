#ifndef CCSP_ABSTRACTOR_H_INCLUDED
#define CCSP_ABSTRACTOR_H_INCLUDED

#include "./cdm_client.h"

namespace ccspwrapper {

struct Tr181ConfigProvider;

class CcspAbstractorGuts;
class CcspAbstractor {
  CcspAbstractor(const CcspAbstractor&); //nocopy
  CcspAbstractorGuts * const _guts;

public:
  CcspAbstractor(const char * const configFilename, Tr181ConfigProvider& rootProvider);
  virtual ~CcspAbstractor();

  CdmClient& CDM;

};

} //namespace ccspwrapper


#endif //#ifndef CCSP_ABSTRACTOR_H_INCLUDED
