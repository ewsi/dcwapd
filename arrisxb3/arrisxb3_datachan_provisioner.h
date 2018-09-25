#ifndef ARRISXB3_DATACHAN_PROVISIONER_H_INCLUDED
#define ARRISXB3_DATACHAN_PROVISIONER_H_INCLUDED

#include "./singlenetwork.h"
#include "ccspwrapper/cdm_client.h"

#include <string>


class ArrisXb3DatachanProvisioner {
  ::ccspwrapper::CdmClient&     _cdm;

  const unsigned                _dcIdx;
  const bool                    _noop;

  std::string _preservedSSID;
  std::string _preservedKeyPassphrase;
  std::string _preservedPreSharedKey;
  std::string _preservedModeEnabled;
  std::string _preservedEncMethod;

  bool        _preservedWpsEnable;
  bool        _preservedSSIDBcast;

  ArrisXb3DatachanProvisioner(const ArrisXb3DatachanProvisioner&); //nocopy
  void ActivateDataChannel(const bool activate);
  std::string ApIndexToIfName(const unsigned idx) const;

public:
  ArrisXb3DatachanProvisioner(const SingleNetwork& singleNetwork, ::ccspwrapper::CdmClient& cdm);
  virtual ~ArrisXb3DatachanProvisioner();

};




#endif //#ifndef ARRISXB3_DATACHAN_PROVISIONER_H_INCLUDED
