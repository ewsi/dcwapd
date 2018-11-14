#ifndef STATIONTELEMETRY_H_INCLUDED
#define STATIONTELEMETRY_H_INCLUDED


#include "ccspwrapper/tr181_sub_collection_provider.h"
#include "dcw/telemetrycollector.h"

class SingleNetwork;

class StationTelemetry : public ::ccspwrapper::AutoDeleteTr181SubCollectionProvider, public ::dcw::TelemetryCollector {

public:
  StationTelemetry(const SingleNetwork& singleNetwork);
  virtual ~StationTelemetry();

  //tr181 accessors...
  ::ccspwrapper::Tr181SubCollectionProvider& GetStationDataChannelSubCollectionProvider();

  //Tr181SubCollectionProvider functions...
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection);

  //DCW telemetry event callbacks...
  virtual void Telemetry_OnStationUpdate(const ::dcw::BasicNetwork& network, const ::dcw::MacAddress& primaryMacAddr, const ::dcw::TrafficPolicy::DataChannelMap& dataChannels, const ::dcw::TrafficFilterProfile *trafficFilterProfile);
  virtual void Telemetry_OnForgetStation(const ::dcw::BasicNetwork& network, const ::dcw::MacAddress& primaryMacAddr);
  virtual void Telemetry_Clear();

private:
  StationTelemetry(const StationTelemetry&); //no copy

  ::ccspwrapper::ProxyTr181SubCollectionProvider  _stationDataChannelSubCollectionProvider;
  const SingleNetwork&                            _singleNetwork;
  
};


#endif //#ifndef STATIONTELEMETRY_H_INCLUDED
