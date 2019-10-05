

#include "./brctlnetwork.h"

#include "dcw/dcwlog.h"

#include <exception>

#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


namespace {
struct BadWiFiChannelInterfaceException : public std::exception {
  virtual const char* what() const noexcept {
    return "Bad WiFi Channel Interface Exception";
  }
};
} // namespace

using namespace dcwlinux;

BrctlChannel::BrctlChannel(const char * const ssidName, const char * const brifName) :
  _ssidName(ssidName),
  _brifName((brifName == NULL) ? "" : brifName) {
  ValidateBrifName();
}

BrctlChannel::BrctlChannel(const BrctlChannel& rhv) :
  _ssidName(rhv._ssidName),
  _brifName(rhv._brifName) {
  //
}

BrctlChannel::BrctlChannel(const BasicChannel& bc) :
  _ssidName(bc.GetSsidName()) {
  //
}

BrctlChannel::~BrctlChannel() {
  //
}

const char *BrctlChannel::GetSsidName() const {
  return _ssidName.c_str();
}

const char *BrctlChannel::GetIfName() const {
  return _brifName.c_str();
}

void BrctlChannel::ValidateBrifName() const {
  char sysdev[512];
  struct stat st;

  if (_brifName.empty()) return;

  //ensure said interface is a linux bridge (brctl)...
  //(dependency on MRM driver... may change later)
  snprintf(sysdev, sizeof(sysdev), "/sys/class/net/%s/brif", GetIfName());
  if (stat(sysdev, &st) == -1) {
    dcwlogerrf("Network bridge interface '%s' for WiFi channel '%s' is not a bridge\n", GetIfName(), GetSsidName());
    throw BadWiFiChannelInterfaceException();
  }
  if (!S_ISDIR(st.st_mode)) {
    dcwlogerrf("Network bridge interface '%s' for WiFi channel '%s' is not a bridge (but oddly has a 'brif')\n", GetIfName(), GetSsidName());
    throw BadWiFiChannelInterfaceException();
  }
}




BrctlNetwork::BrctlNetwork(const char * const primarySsidName, const char * const brifName) :
  _primaryChannel(primarySsidName, brifName) {
  if (brifName == NULL) {
    dcwlogerrf("Network bridge interface for WiFi channel '%s' is not provided\n", primarySsidName);
    throw BadWiFiChannelInterfaceException();
  }
  if (brifName[0] == '\0') {
    dcwlogerrf("Network bridge interface for WiFi channel '%s' is not provided\n", primarySsidName);
    throw BadWiFiChannelInterfaceException();
  }
  dcwlogdbgf("Successfully created brctl-based network '%s' running on bridge '%s'\n", primarySsidName, brifName);
}

BrctlNetwork::~BrctlNetwork() {
  //
}

const ::dcw::BasicChannel& BrctlNetwork::GetPrimaryChannel() const {
  return _primaryChannel;
}

void BrctlNetwork::GetDataChannels(ChannelSet& output) const {
  for (std::list<BrctlChannel>::const_iterator i = _dataChannels.begin(); i != _dataChannels.end(); i++) {
    output.insert(&(*i));
  }
}

void BrctlNetwork::InsertDataChannel(const char * const ssidName, const char *brifName) {
  if (brifName != NULL) {
    if (strcmp(_primaryChannel.GetIfName(), brifName) == 0) {
      //the bridge interface of the incoming data channel match the primary channel...
      //just null out this field as we dont need the driver to perform a remap
      brifName = NULL;
    }
  }
  _dataChannels.push_back(BrctlChannel(ssidName, brifName));
  dcwlogdbgf("Added brctl-based data channel \"%s\" to network \"%s\"\n", ssidName, _primaryChannel.GetSsidName());
}


