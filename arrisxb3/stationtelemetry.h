#ifndef STATIONTELEMETRY_H_INCLUDED
#define STATIONTELEMETRY_H_INCLUDED


#include "ccspwrapper/tr181_sub_collection_provider.h"

class SingleNetwork;

class StationTelemetry : public ::ccspwrapper::AutoDeleteTr181SubCollectionProvider {

public:
  StationTelemetry(const SingleNetwork& singleNetwork);
  virtual ~StationTelemetry();

  //tr181 accessors...
  ::ccspwrapper::Tr181SubCollectionProvider& GetStationDataChannelSubCollectionProvider();

  //Tr181SubCollectionProvider functions...
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection);


private:
  StationTelemetry(const StationTelemetry&); //no copy

  ::ccspwrapper::ProxyTr181SubCollectionProvider  _stationDataChannelSubCollectionProvider;
  const SingleNetwork&                            _singleNetwork;
  
};


#endif //#ifndef STATIONTELEMETRY_H_INCLUDED
