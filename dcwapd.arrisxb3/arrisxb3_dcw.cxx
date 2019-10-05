

#include "./arrisxb3_dcw.h"

#include "dcw/dcwlog.h"

#include <exception>

namespace {
struct BridgeResoveFailedException : public std::exception {
  const char* what() const noexcept override {
    return "Failed to resolv bridge interface name";
  }
};
} // namespace





ArrisXb3Dcw::ArrisXb3Dcw(const SingleNetwork& singleNetwork, ::ccspwrapper::CdmClient& cdm) :
  _singleNetwork(singleNetwork), _cdm(cdm) {
  //
}

ArrisXb3Dcw::~ArrisXb3Dcw() {
  //
}

const char *ArrisXb3Dcw::GetSsidIfName(const char * const ssid) const {
  if (ApIndexToSsid(_singleNetwork.GetPrimaryChannelApIndex()) == ssid) {
    _primaryIfNameCache = ResolvePrimaryChannelIfname(); //yuck
    return _primaryIfNameCache.c_str();
  }
  else if (ApIndexToSsid(_singleNetwork.GetDataChannelApIndex()) == ssid) {
    return "brdcw0"; //defined in "utopia_init_dcw.sh"
  }
  return NULL;
}

std::string ArrisXb3Dcw::GetPrimaryChannelSsidName() const {
  return ApIndexToSsid(_singleNetwork.GetPrimaryChannelApIndex());
}

std::string ArrisXb3Dcw::GetDataChannelSsidName() const {
  return ApIndexToSsid(_singleNetwork.GetDataChannelApIndex());
}

std::string ArrisXb3Dcw::ApIndexToSsid(const unsigned idx) const {
  return _cdm.Paramf("%sSSID",
    std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%d.SSIDReference", idx)).c_str()
  );
}

std::string ArrisXb3Dcw::ResolvePrimaryChannelIfname() const {
  std::string lowerLayer = _cdm.Paramf("Device.WiFi.AccessPoint.%d.SSIDReference",
                                       _singleNetwork.GetPrimaryChannelApIndex());

  //strip off any trailing '.'s...
  while ((!lowerLayer.empty()) && (lowerLayer[lowerLayer.length() - 1] == '.'))
    lowerLayer.resize(lowerLayer.length() - 1);

  //this is ugly...
  try {
    for (int bridgeIdx = 1; /* go until we throw */ ; bridgeIdx++) {
      if (!((bool)_cdm.Paramf("Device.Bridging.Bridge.%d.Enable", bridgeIdx))) //note: this is what will throw when we are out of bridges...
        continue;
      try {
        for (int portIdx = 1; /* go until we throw */; portIdx++) {
          const std::string portLowerLayer = 
            _cdm.Paramf("Device.Bridging.Bridge.%d.Port.%d.LowerLayers", bridgeIdx, portIdx);
          if (portLowerLayer == lowerLayer) break; //found it...
        }
      }
      catch (...) {
        //out of ports to compare... try the next bridge...
        continue;
      }

      //a port's lower layer on this bridge matches...
      //get the VLAN bridge ifname...
      return _cdm.Paramf("Device.Bridging.Bridge.%d.VLAN.1.Name", bridgeIdx);
    }
  }
  catch (...) {}

  //never found the bridge ifname... 
  throw BridgeResoveFailedException();
}
