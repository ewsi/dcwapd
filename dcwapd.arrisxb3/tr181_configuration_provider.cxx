#include "./tr181_configuration_provider.h"

#include "ccspwrapper/tr181_collection_registry.h"

#include "dcw/dcwlog.h"

#include <cstring>


Tr181ConfigurationProvider::Tr181ConfigurationProvider() :
  _stationTelemetry(_singleNetwork),
  _ccspAbstractor("dcwapd.ccsp.cfg.xml", *this),
  Xb3(_singleNetwork, _ccspAbstractor.CDM),
  Network(_singleNetwork),
  Cdm(_ccspAbstractor.CDM),
  TelemetryCollector(_stationTelemetry) {
  //
}

Tr181ConfigurationProvider::~Tr181ConfigurationProvider() {
  //
}


//APConfigurationProvider Functions...
void Tr181ConfigurationProvider::InstanciateCFileTrafficFilterProfiles(CFTFPList& output) const {
  _filters.InstanciateCFileTrafficFilterProfiles(output);
}

void Tr181ConfigurationProvider::GetPrimarySsids(SsidSet& output) const {
  //only provide this if the network is enabled...
  //if the network is not enabled, we dont want any
  //configuration validation issues to throw an exception
  //preventing the event reactor from running...
  //the event reactor will "sleep forever" if the
  //configuration is empty which is what we want
  if (!_singleNetwork.GetEnabled()) return;

  output.insert(Xb3.GetPrimaryChannelSsidName());
}

void Tr181ConfigurationProvider::GetDataSsids(SsidSet& output, const char * const primarySsid) const {
  if (Xb3.GetPrimaryChannelSsidName() != primarySsid) {
    //defensive...
    dcwlogwarnf("Got a request to provide data SSIDs for non-declared primary SSID %s\n", primarySsid);
    throw ::ccspwrapper::Tr181Exception();
  }

  output.insert(Xb3.GetDataChannelSsidName());
}

const char *Tr181ConfigurationProvider::GetSsidIfname(const char * const ssid) const {
  const char * const rv = Xb3.GetSsidIfName(ssid);
  if (rv == NULL)
    dcwlogwarnf("Got a request to provide an interface for non-declared SSID %s\n", ssid);

  return rv;
}

void Tr181ConfigurationProvider::GetStationTrafficFilterProfiles(StationTFPMap& output) const {
  //only provide this if the network is enabled...
  //if the network is not enabled, we dont want any
  //configuration validation issues to throw an exception
  //preventing the event reactor from running...
  //the event reactor will "sleep forever" if the
  //configuration is empty which is what we want
  if (!_singleNetwork.GetEnabled()) return;

  _stationFilterOverrides.GetStationTrafficFilterProfiles(output);
}



//Tr181ConfigProvider Functions...
void Tr181ConfigurationProvider::GetSubCollections(ccspwrapper::Tr181CollectionRegistry& registry) {
  registry.DeclareSubCollectionProvider("Network",                _singleNetwork);
  registry.DeclareSubCollectionProvider("NetworkDataChannel",     _singleNetwork.GetNetworkDataChannelSubCollectionProvider());

  registry.DeclareSubCollectionProvider("Station",                _stationTelemetry);
  registry.DeclareSubCollectionProvider("StationDataChannel",     _stationTelemetry.GetStationDataChannelSubCollectionProvider());

  registry.DeclareSubCollectionProvider("Filter",                 _filters);
  registry.DeclareSubCollectionProvider("StationFilterOverride",  _stationFilterOverrides);
}

void Tr181ConfigurationProvider::GetValue(const char * const name, ccspwrapper::Tr181Scalar& value) {
  if (strcmp(name, "FilterPath") == 0) {
    value = _filters.GetScanPath();
  }
  else {
    throw ccspwrapper::Tr181Exception();
  }
}

void Tr181ConfigurationProvider::SetValue(const char * const name, const ccspwrapper::Tr181Scalar& value) {
  if (strcmp(name, "FilterPath") == 0) {
    _filters.SetScanPath(value);
  }
  else {
    throw ccspwrapper::Tr181Exception();
  }
}

bool Tr181ConfigurationProvider::Validate() {
  dcwlogdbgf("TR181: %s()\n", __func__);
  return true;
}

void Tr181ConfigurationProvider::Commit() {
  dcwlogdbgf("TR181: %s()\n", __func__);
  //
}

void Tr181ConfigurationProvider::Rollback() {
  dcwlogdbgf("TR181: %s()\n", __func__);
  //
}


