

#include "./stationtelemetry.h"
#include "./singlenetwork.h"

#include "ccspwrapper/tr181_config_provider.h"

#include "dcw/macaddress.h"
#include "dcw/dcwlog.h"

#include <string.h>

namespace {
struct SingleStation :
  public ::ccspwrapper::Tr181ConfigProvider,
  public ::ccspwrapper::Tr181SubCollectionProvider {

  const ::dcw::MacAddress _macAddr;
  const SingleNetwork&    _singleNetwork;

  SingleStation(const ::dcw::MacAddress& macAddr, const SingleNetwork& singleNetwork) :
    _macAddr(macAddr), _singleNetwork(singleNetwork) {}
  virtual ~SingleStation() {}
  virtual void GetValue(const char * const name, ::ccspwrapper::Tr181Scalar& value) {
    //Station.X. values...
    if (strcmp(name, "PrimaryMACAddress") == 0) {
      value = _macAddr.ToString();
    }
    else if (strcmp(name, "NetworkIndex") == 0) {
      value = (uint64_t)1; //hard-coding to 1 as this is a single-network implementation
    }
    else if (strcmp(name, "FilterName") == 0) {
      value = std::string("TFP_Default"); //XXX this need to be dynamic...
    }

    //Station.X.BondedDataChannel.1. values...
    //(single network implementation so theres only one data channel)
    else if (strcmp(name, "APIndex") == 0) {
      value = (uint64_t)_singleNetwork.GetDataChannelApIndex();
    }
    else if (strcmp(name, "MACAddress") == 0) {
      //XXX for the love of god... this is only a place holder...
      //    PLEASE DO NOT PERFORM ARBITRY ARITHMETIC ON MAC ADDRESSES IN PRODUCTION CODE!!!
      ::dcw::MacAddress dcMacAddr(_macAddr); dcMacAddr.Value[5]++;
      
      value = dcMacAddr.ToString();
    }
    else {
      dcwlogerrf("Unhandled station/datachannel param name: %s\n", name);
      throw ccspwrapper::Tr181Exception();
    }
  }
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
    //theres one and only one data channel for this implementation
    collection.push_back(new ::ccspwrapper::ProxyTr181ConfigProvider(*this));
  }
};
} //namespace




StationTelemetry::StationTelemetry(const SingleNetwork& singleNetwork) :
  _stationDataChannelSubCollectionProvider(*this),
  _singleNetwork(singleNetwork) {
  //
}

StationTelemetry::~StationTelemetry() {
  //
}

::ccspwrapper::Tr181SubCollectionProvider& StationTelemetry::GetStationDataChannelSubCollectionProvider() {
  return _stationDataChannelSubCollectionProvider;
}

void StationTelemetry::PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
  //asking to provide data channel elements for a single station?
  SingleStation * const ss = dynamic_cast<SingleStation*>(&parent);
  if (ss != NULL) {
    //yup... forward the poulate request on...
    ss->PopulateConfigProviderCollection(parent, collection);
    return;
  }

  //nope... asking for the station list itself...
  //XXX synthesizing this for now...
  if (_singleNetwork.GetEnabled()) {
    collection.push_back(new SingleStation("11-22-33-44-55-66", _singleNetwork));
    collection.push_back(new SingleStation("AA:BB:CC:dd:eE:Ff", _singleNetwork));
  }
}


void StationTelemetry::Telemetry_OnStationUpdate(const ::dcw::BasicNetwork& network, const ::dcw::MacAddress& primaryMacAddr, const ::dcw::TrafficPolicy::DataChannelMap& dataChannels, const ::dcw::TrafficFilterProfile *trafficFilterProfile) {
  //
}

void StationTelemetry::Telemetry_OnForgetStation(const ::dcw::BasicNetwork& network, const ::dcw::MacAddress& primaryMacAddr) {
  //
}

void StationTelemetry::Telemetry_Clear() {
  //
}

