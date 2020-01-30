

#include "./arrisxb3_datachan_provisioner.h"

#include "dcw/dcwlog.h"

#include <exception>


namespace {
struct ActivateDataChannelFailedException : public std::exception {
  virtual const char* what() const noexcept {
    return "Failed to activate data channel";
  }
};
} // namespace


ArrisXb3DatachanProvisioner::ArrisXb3DatachanProvisioner(const SingleNetwork& singleNetwork, ::ccspwrapper::CdmClient& cdm) :
  _cdm(cdm),
  _dcIdx(singleNetwork.GetDataChannelApIndex()),
  _noop(!singleNetwork.GetEnabled()) {

  if (_noop) return;

  dcwloginfof("%s\n", "Provisioning data channel...");
  const unsigned pcIdx = singleNetwork.GetPrimaryChannelApIndex();

  //first back up all relavent data-channel parameters
  _preservedSSID           = std::string(_cdm.Paramf("%sSSID", std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%u.SSIDReference", _dcIdx)).c_str()));
  _preservedKeyPassphrase  = std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.KeyPassphrase", _dcIdx));
//  _preservedPreSharedKey   = std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.PreSharedKey", _dcIdx));
  _preservedModeEnabled    = std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.ModeEnabled", _dcIdx));
  _preservedEncMethod      = std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.X_CISCO_COM_EncryptionMethod", _dcIdx));
  _preservedWpsEnable      = _cdm.Paramf("Device.WiFi.AccessPoint.%u.WPS.Enable", _dcIdx);
  _preservedSSIDBcast      = _cdm.Paramf("Device.WiFi.AccessPoint.%u.SSIDAdvertisementEnabled", _dcIdx);

  //provision the data channel...
  _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.KeyPassphrase", _dcIdx) = 
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.KeyPassphrase", pcIdx);
//  _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.PreSharedKey", _dcIdx) = 
//    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.PreSharedKey", pcIdx);
  _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.ModeEnabled", _dcIdx) = 
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.ModeEnabled", pcIdx);
  _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.X_CISCO_COM_EncryptionMethod", _dcIdx) = 
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.X_CISCO_COM_EncryptionMethod", pcIdx);

  _cdm.Paramf("Device.WiFi.AccessPoint.%u.WPS.Enable", _dcIdx) = false;
  _cdm.Paramf("Device.WiFi.AccessPoint.%u.SSIDAdvertisementEnabled", _dcIdx) = false;

  ActivateDataChannel(true);
}

ArrisXb3DatachanProvisioner::~ArrisXb3DatachanProvisioner() {
  if (_noop) return;

  //restore everything back to the way it was...
  try {
    ActivateDataChannel(false);

    dcwloginfof("%s\n", "Restoring data channel...");
    _cdm.Paramf("%sSSID", std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%u.SSIDReference", _dcIdx)).c_str())   = _preservedSSID;
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.KeyPassphrase", _dcIdx)                                      = _preservedKeyPassphrase;
//    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.PreSharedKey", _dcIdx)                                       = _preservedPreSharedKey;
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.ModeEnabled", _dcIdx)                                        = _preservedModeEnabled;
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.Security.X_CISCO_COM_EncryptionMethod", _dcIdx)                       = _preservedEncMethod;
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.WPS.Enable", _dcIdx)                                                  = _preservedWpsEnable;
    _cdm.Paramf("Device.WiFi.AccessPoint.%u.SSIDAdvertisementEnabled", _dcIdx)                                    = _preservedSSIDBcast;
  }
  catch (std::exception& e) {
    dcwlogerrf("Exception thrown on data-channel restoration: %s\n", e.what());
  }
}

void ArrisXb3DatachanProvisioner::ActivateDataChannel(const bool activate) {
  dcwloginfof("%s data channel...\n", activate ? "Activating" : "Deactivating");
  try {
    const std::string dataChannelIfName = ApIndexToIfName(_dcIdx);
    for (int ifIdx = 1; /* go until we throw */; ifIdx++) {
      if (dataChannelIfName == std::string(_cdm.Paramf("Device.DCWXVS.Interface.%d.Name", ifIdx))) {
        _cdm.Paramf("Device.DCWXVS.Interface.%d.DataEnable", ifIdx) = activate;
        return;
      }
    }
  }
  catch (...) { }
  throw ActivateDataChannelFailedException();
}

std::string ArrisXb3DatachanProvisioner::ApIndexToIfName(const unsigned idx) const {
  return _cdm.Paramf("%sName",
    std::string(_cdm.Paramf("Device.WiFi.AccessPoint.%d.SSIDReference", idx)).c_str()
  );
}
