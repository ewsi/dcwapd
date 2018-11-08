#ifndef ARRISXB3_DCW_H_INCLUDED
#define ARRISXB3_DCW_H_INCLUDED

#include "./singlenetwork.h"
#include "ccspwrapper/cdm_client.h"

#include <string>


class ArrisXb3Dcw {
  const SingleNetwork&          _singleNetwork;
  ::ccspwrapper::CdmClient&     _cdm;

  mutable std::string           _primaryIfNameCache;

  std::string ApIndexToSsid(const unsigned idx) const;
  std::string ResolvePrimaryChannelIfname() const;

public:
  ArrisXb3Dcw(const SingleNetwork& singleNetwork, ::ccspwrapper::CdmClient& cdm);
  virtual ~ArrisXb3Dcw();

  const char *GetSsidIfName(const char * const ssid) const;

  std::string GetPrimaryChannelSsidName() const;
  std::string GetDataChannelSsidName() const;

  void ActivateDataChannel(const bool activate);
};


#endif //#ifndef ARRISXB3_DCW_H_INCLUDED
