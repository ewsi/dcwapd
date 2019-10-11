

#include "./singlenetwork.h"

#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>



SingleNetwork::SingleNetwork() :
  _networkDataChannelSubCollectionProvider(*this),
  _networkDataChannelConfigProvider(*this),
  _networkInstanciated(true),     // instanciate by default
  _dataChannelInstanciated(true), // instanciate by default
  _enabled(false),
  _primaryChannelApIndex(1),
  _dataChannelApIndex(2) {
  //
}

SingleNetwork::~SingleNetwork() {
  //
}

::ccspwrapper::Tr181SubCollectionProvider& SingleNetwork::GetNetworkDataChannelSubCollectionProvider() {
  return _networkDataChannelSubCollectionProvider;
}

void SingleNetwork::PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
  if (((::ccspwrapper::Tr181ConfigProvider*)this) == (&parent)) {
    //network data channel
    if (_dataChannelInstanciated)
      collection.push_back(&_networkDataChannelConfigProvider);
  }
  else {
    //network collection
    if (_networkInstanciated)
      collection.push_back(this);
  }
}

bool SingleNetwork::GetEnabled() const {
  return _networkInstanciated && _dataChannelInstanciated && _enabled;
}

unsigned SingleNetwork::GetPrimaryChannelApIndex() const {
  return _primaryChannelApIndex;
}

unsigned SingleNetwork::GetDataChannelApIndex() const {
  return _dataChannelApIndex;
}

::ccspwrapper::Tr181ConfigProvider *SingleNetwork::RequestAddNewProviderElement(Tr181ConfigProvider& parent) {
  if (((::ccspwrapper::Tr181ConfigProvider*)this) == (&parent)) {
    //network data channel
    if (_dataChannelInstanciated)
      throw ::ccspwrapper::Tr181Exception();

    _dataChannelInstanciated = true;
    return &_networkDataChannelConfigProvider;
  }
  else {
    //network collection
    if (_networkInstanciated)
      throw ::ccspwrapper::Tr181Exception();

    _networkInstanciated = true;
    return this;
  }
}

void SingleNetwork::RequestDeleteConfigProviderElement(Tr181ConfigProvider& provider) {
  if (((::ccspwrapper::Tr181ConfigProvider*)&_networkDataChannelConfigProvider) == (&provider)) {
    //network data channel
    if (!_dataChannelInstanciated)
      throw ::ccspwrapper::Tr181Exception();

    _dataChannelInstanciated = false;
  }
  else {
    //network collection
    if (!_networkInstanciated)
      throw ::ccspwrapper::Tr181Exception();

    _networkInstanciated = false;
  }
}

void SingleNetwork::GetValue(const char * const name, ccspwrapper::Tr181Scalar& value) {
  if (strcmp(name, "Enable") == 0) {
    value = _enabled;
  }
  else if (strcmp(name, "PrimaryChannelAPIndex") == 0) {
    value = (uint64_t)_primaryChannelApIndex;
  }
  else if (strcmp(name, "APIndex") == 0) {
    value = (uint64_t)_dataChannelApIndex;
  }
  else {
    throw ::ccspwrapper::Tr181Exception();
  }
}

void SingleNetwork::SetValue(const char * const name, const ccspwrapper::Tr181Scalar& value) {
  if (strcmp(name, "Enable") == 0) {
    _enabled = value;
  }
  else if (strcmp(name, "PrimaryChannelAPIndex") == 0) {
    _primaryChannelApIndex = (uint64_t)value;
  }
  else if (strcmp(name, "APIndex") == 0) {
    _dataChannelApIndex = (uint64_t)value;
  }
  else {
    throw ::ccspwrapper::Tr181Exception();
  }
}

bool SingleNetwork::Validate() {
  if (_enabled) {
    if ((!_networkInstanciated) || (!_dataChannelInstanciated))
      return false;
    if (_primaryChannelApIndex == _dataChannelApIndex)
      return false;
  }
  return true;
}

void SingleNetwork::Commit() {
  kill(getpid(), SIGHUP);
}

void SingleNetwork::Rollback() {
  _enabled = false;
  kill(getpid(), SIGHUP);
}

