#ifndef TR181_CONFIGURATION_PROVIDER_H_INCLUDED
#define TR181_CONFIGURATION_PROVIDER_H_INCLUDED



#include "dcwlinux/ap_configuration.h"
#include "ccspwrapper/tr181_config_provider.h"
#include "ccspwrapper/ccsp_abstractor.h"

#include "./filters.h"
#include "./singlenetwork.h"
#include "./stationfilteroverrides.h"
#include "./stationtelemetry.h"

#include "./arrisxb3_dcw.h"

class Tr181ConfigurationProvider :
  public ::dcwlinux::APConfigurationProvider,
  public ::ccspwrapper::Tr181ConfigProvider {
public:
  Tr181ConfigurationProvider();
  virtual ~Tr181ConfigurationProvider();

  //APConfigurationProvider Functions...
  virtual void InstanciateCFileTrafficFilterProfiles(CFTFPList& output) const;
  virtual void GetPrimarySsids(SsidSet& output) const;
  virtual void GetDataSsids(SsidSet& output, const char * const primarySsid) const;
  virtual const char *GetSsidIfname(const char * const ssid) const;
  virtual void GetStationTrafficFilterProfiles(StationTFPMap& output) const;


  //Tr181ConfigProvider Functions...
  virtual void GetSubCollections(ccspwrapper::Tr181CollectionRegistry& registry);
  virtual void GetValue(const char * const name, ccspwrapper::Tr181Scalar& value);
  virtual void SetValue(const char * const name, const ccspwrapper::Tr181Scalar& value);
  virtual bool Validate();
  virtual void Commit();
  virtual void Rollback();

private:
  Tr181ConfigurationProvider(const Tr181ConfigurationProvider&); //no copy

  //things that provide TR181 "sub-data"...
  Filters                                           _filters;
  SingleNetwork                                     _singleNetwork;
  StationFilterOverrides                            _stationFilterOverrides;
  StationTelemetry                                  _stationTelemetry;

  //declare this thing last so that we initialize all our stuff 
  //before this will fire up tr181 on construction...
  ccspwrapper::CcspAbstractor _ccspAbstractor;

public:
  ArrisXb3Dcw                 Xb3;
  SingleNetwork&              Network;
  ::ccspwrapper::CdmClient&   Cdm;
  ::dcw::TelemetryCollector&  TelemetryCollector;
};


#endif //#ifndef TR181_CONFIGURATION_PROVIDER_H_INCLUDED
