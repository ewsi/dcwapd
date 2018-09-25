
#include "./ccsp_abstractor.h"

#include "./ccsp_config.h"
#include "./ccsp_attacher.h"


namespace ccspwrapper {

  class CcspAbstractorGuts : public CcspConfig, public CcspAttacher {
  public:
    CcspAbstractorGuts(const char * const configFilename, Tr181ConfigProvider& rootProvider) :
      CcspConfig(configFilename),
      CcspAttacher(*this, rootProvider) {
      //
    }
    virtual ~CcspAbstractorGuts() {}
  };

} //namespace ccspwrapper

using namespace ccspwrapper;


CcspAbstractor::CcspAbstractor(const char * const configFilename, Tr181ConfigProvider& rootProvider) :
  _guts(new CcspAbstractorGuts(configFilename, rootProvider)),
  CDM(_guts->GetCdmClient()) {
}

CcspAbstractor::~CcspAbstractor() {
  delete _guts;
}


