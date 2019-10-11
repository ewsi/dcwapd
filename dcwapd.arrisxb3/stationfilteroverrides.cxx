
#include "./stationfilteroverrides.h"

#include "ccspwrapper/tr181_config_provider.h"

#include <string.h>
#include <arpa/inet.h>

namespace {
class SingleStationFilterOverride : public ::ccspwrapper::Tr181ConfigProvider {
  ::dcw::MacAddress                                    _stationMacAddress;
  ::dcwlinux::APConfigurationProvider::StationTFPMap&  _stationTfpMap;

public:
  SingleStationFilterOverride(
    ::dcwlinux::APConfigurationProvider::StationTFPMap& stationTfpMap) :
    _stationTfpMap(stationTfpMap) {
    //creating a filter override for a new override...
    //keep incrementing until we have a macaddress not in the map...
    memset(_stationMacAddress.Value, 0, sizeof(_stationMacAddress.Value));
    uint32_t suffix = 0;
    do {
      uint32_t nbo_suffix = htonl(suffix++);
      memcpy(&_stationMacAddress.Value[2], &nbo_suffix, sizeof(nbo_suffix));
    } while (stationTfpMap.find(_stationMacAddress) != stationTfpMap.end());

    stationTfpMap[_stationMacAddress] = "TFP_Default";
  }

  SingleStationFilterOverride(
    const ::dcw::MacAddress& stationMacAddress,
    ::dcwlinux::APConfigurationProvider::StationTFPMap& stationTfpMap) :
    _stationMacAddress(stationMacAddress),
    _stationTfpMap(stationTfpMap) {
    //creating a  filter override for an existing override...
  }

  virtual ~SingleStationFilterOverride() {}

  const ::dcw::MacAddress& GetMacAddress() const {
    return _stationMacAddress;
  }

  virtual void GetValue(const char * const name, ccspwrapper::Tr181Scalar& value) {
    if (strcmp(name, "PrimaryMACAddress") == 0) {
      value = _stationMacAddress.ToString();
    }
    else if (strcmp(name, "FilterName") == 0) {
      value = _stationTfpMap[_stationMacAddress];
    }
    else {
      throw ::ccspwrapper::Tr181Exception();
    }
  }
  virtual void SetValue(const char * const name, const ccspwrapper::Tr181Scalar& value) {
    if (strcmp(name, "PrimaryMACAddress") == 0) {
      const ::dcw::MacAddress macaddr(value);

      if (_stationMacAddress == macaddr) return;

      if (_stationTfpMap.find(macaddr) != _stationTfpMap.end())
        throw ::ccspwrapper::Tr181Exception(); //specified mac address already exists...

      _stationTfpMap[macaddr] = _stationTfpMap[_stationMacAddress];
      _stationTfpMap.erase(_stationMacAddress);
      _stationMacAddress = macaddr;
    }
    else if (strcmp(name, "FilterName") == 0) {
      //XXX should we validate valid filter name?
      _stationTfpMap[_stationMacAddress] = (const char*)value;
    }
    else {
      throw ::ccspwrapper::Tr181Exception();
    }
  }
  virtual bool Validate() {
    return true;
  }
  virtual void Commit() { }
  virtual void Rollback() { }
};
} // namespace

StationFilterOverrides::StationFilterOverrides() {
  //
}

StationFilterOverrides::~StationFilterOverrides() {
  //
}

void StationFilterOverrides::PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
  for (::dcwlinux::APConfigurationProvider::StationTFPMap::const_iterator i = _stationTfpMap.begin(); i != _stationTfpMap.end(); i++) {
    collection.push_back(new SingleStationFilterOverride(i->first, _stationTfpMap));
  }
}

::ccspwrapper::Tr181ConfigProvider *StationFilterOverrides::RequestAddNewProviderElement(::ccspwrapper::Tr181ConfigProvider& parent) {
  return new SingleStationFilterOverride(_stationTfpMap);
}

void StationFilterOverrides::RequestDeleteConfigProviderElement(::ccspwrapper::Tr181ConfigProvider& provider) {
  SingleStationFilterOverride& ssfo = (SingleStationFilterOverride&)provider;

  if (_stationTfpMap.find(ssfo.GetMacAddress()) == _stationTfpMap.end())
    throw ::ccspwrapper::Tr181Exception(); //shouldnt get here...

  _stationTfpMap.erase(ssfo.GetMacAddress());
}

void StationFilterOverrides::GetStationTrafficFilterProfiles(::dcwlinux::APConfigurationProvider::StationTFPMap& output) const {
  output = _stationTfpMap;
}

