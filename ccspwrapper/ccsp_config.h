#ifndef CCSP_CONFIG_H_INCLUDED
#define CCSP_CONFIG_H_INCLUDED


#include "./ccsp_include.h"

#include <string>

namespace ccspwrapper {

class CcspConfig : public CCSP_COMPONENT_CFG {
public:
  CcspConfig(const char * const configFilename, const char * const subsystem = "eRT");
  virtual ~CcspConfig();

  const std::string& GetSubsystemPrefix() const;
  std::string ApplySubsystemPrefix(const char * const componentName) const;
  const std::string& GetTr181Filename() const;

private:
  CcspConfig(const CcspConfig&); //no copy

  std::string LoadTr181Filename() const;
  void dumpCcspConfig() const;

  const std::string _subsystemPrefix;
  std::string _tr181Filename;
};


} //namespace ccspwrapper


#endif //#ifndef CCSP_CONFIG_H_INCLUDED
