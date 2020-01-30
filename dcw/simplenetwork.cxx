
#include "./simplenetwork.h"

#include "./dcwlog.h"


using namespace dcw;

SimpleChannel::SimpleChannel(const char * const ssidName) :
  _ssidName(ssidName) {
  //
}

SimpleChannel::SimpleChannel(const SimpleChannel& rhv) :
  _ssidName(rhv._ssidName) {
  //
}

SimpleChannel::SimpleChannel(const BasicChannel& bc) :
  _ssidName(bc.GetSsidName()) {
  //
}

SimpleChannel::~SimpleChannel() {
  //
}

const char *SimpleChannel::GetSsidName() const {
  return _ssidName.c_str();
}






SimpleNetwork::SimpleNetwork(const char * const primarySsidName) :
  _primaryChannel(primarySsidName) {
  //
}

SimpleNetwork::~SimpleNetwork() {
  //
}

const BasicChannel& SimpleNetwork::GetPrimaryChannel() const {
  return _primaryChannel;
}

void SimpleNetwork::GetDataChannels(ChannelSet& output) const {
  for (auto i = _dataChannels.begin(); i != _dataChannels.end(); i++) {
    output.insert(&(*i));
  }
}

void SimpleNetwork::InsertDataChannel(const char * const ssidName) {
  _dataChannels.push_back(ssidName);
  dcwlogdbgf("Added data channel \"%s\" to network \"%s\"\n", ssidName, _primaryChannel.GetSsidName());
}

