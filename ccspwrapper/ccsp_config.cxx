

#include "./ccsp_config.h"


#include <exception>
#include <strings.h>

#include "dcw/dcwlog.h"


namespace {
struct CCSPConfigLoadException : public std::exception {
  virtual const char* what() const throw() {
    return "CSSP Configuration Load Failed";
  }
};
};

using namespace ccspwrapper;


CcspConfig::CcspConfig(const char * const configFilename, const char * const subsystem) :
  _subsystemPrefix(std::string(subsystem) + ".") {
  CCSP_COMPONENT_CFG& ccspCfg = *this;

  bzero(&ccspCfg, sizeof(ccspCfg));

  dcwloginfof("Reading CCSP top-level configuration: %s\n", configFilename);

  if (CcspComponentLoadCfg((char*)configFilename, &ccspCfg) != 0) {
    throw CCSPConfigLoadException();
  }

  dcwlogdbgf("%s\n", "Top-level CCSP configuration successfully read");

  _tr181Filename = LoadTr181Filename();

  dumpCcspConfig();
}

CcspConfig::~CcspConfig() {
  dcwlogdbgf("%s\n", "CCSP Configuration Destroyed");
}

const std::string& CcspConfig::GetSubsystemPrefix() const {
  return _subsystemPrefix;
}

std::string CcspConfig::ApplySubsystemPrefix(const char * const componentName) const {
  std::string result = _subsystemPrefix;
  result += componentName;
  return result;
}

const std::string& CcspConfig::GetTr181Filename() const {
  return _tr181Filename;
}

std::string CcspConfig::LoadTr181Filename() const {
  PCCSP_DM_XML_CFG_LIST pXmlCfgList = NULL;

  dcwloginfof("Reading CCSP data-model list configuration: %s\n", this->DmXmlCfgFileName);
  if (CcspComponentLoadDmXmlList((char*)this->DmXmlCfgFileName, &pXmlCfgList) != ANSC_STATUS_SUCCESS) {
    throw CCSPConfigLoadException();
  }

  dcwlogdbgf("%s\n", "CCSP data-model list configuration successfully read");

  const std::string result = pXmlCfgList->FileList[0];

  AnscFreeMemory(pXmlCfgList);

  return result;
}

void CcspConfig::dumpCcspConfig() const {
  dcwlogdbgf("CCSPCFG Component Id: %s\n",                   this->ComponentId);
  dcwlogdbgf("CCSPCFG Component Name: %s\n",                 this->ComponentName);
  dcwlogdbgf("CCSPCFG Version: %lu\n",       (unsigned long) this->Version);
  dcwlogdbgf("CCSPCFG DBUS Path: %s\n",                      this->DbusPath);
  dcwlogdbgf("CCSPCFG Data Model XML Config Filename: %s\n", this->DmXmlCfgFileName);
  dcwlogdbgf("CCSPCFG Subsystem Prefix: %s\n",               this->GetSubsystemPrefix().c_str());
  dcwlogdbgf("CCSPCFG TR-181 Configuration Filename: %s\n",  this->GetTr181Filename().c_str());
}

