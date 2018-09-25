#ifndef STATIONFILTEROVERRIDES_H_INCLUDED
#define STATIONFILTEROVERRIDES_H_INCLUDED


#include "ccspwrapper/tr181_sub_collection_provider.h"

#include "dcwlinux/ap_configuration.h"

#include <map>

class StationFilterOverrides :
  public ::ccspwrapper::AutoDeleteTr181SubCollectionProvider {

  ::dcwlinux::APConfigurationProvider::StationTFPMap _stationTfpMap;

public:
  StationFilterOverrides();
  virtual ~StationFilterOverrides();

  //Tr181SubCollectionProvider functions...
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection);
  virtual ::ccspwrapper::Tr181ConfigProvider *RequestAddNewProviderElement(::ccspwrapper::Tr181ConfigProvider& parent);
  virtual void RequestDeleteConfigProviderElement(::ccspwrapper::Tr181ConfigProvider& provider);

  virtual void GetStationTrafficFilterProfiles(::dcwlinux::APConfigurationProvider::StationTFPMap& output) const;

};

#endif //#ifndef STATIONFILTEROVERRIDES_H_INCLUDED
