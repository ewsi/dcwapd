#ifndef FILTERS_H_INCLUDED
#define FILTERS_H_INCLUDED


#include "ccspwrapper/tr181_sub_collection_provider.h"

#include "dcwlinux/ap_configuration.h" //CFTFPList

class Filters :
  public ::ccspwrapper::AutoDeleteTr181SubCollectionProvider {
public:
  Filters();
  virtual ~Filters();

  const char *GetScanPath() const;
  void SetScanPath(const char * const scanPath);

  //Tr181SubCollectionProvider functions...
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection);

  void InstanciateCFileTrafficFilterProfiles(::dcwlinux::APConfigurationProvider::CFTFPList& output) const;

private:
  Filters(const Filters&); //no copy

  std::string _scanPath;

};

#endif //#ifndef TR181_FILTER_H_INCLUDED
