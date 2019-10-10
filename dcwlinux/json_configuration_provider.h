#ifndef JSON_CONFIGURATION_PROVIDER_H_INCLUDED
#define JSON_CONFIGURATION_PROVIDER_H_INCLUDED

#include "./ap_configuration.h"

namespace dcwlinux {


class JsonConfigurationProviderGuts;
class JsonConfigurationProvider : public APConfigurationProvider {
  JsonConfigurationProvider(const JsonConfigurationProvider&); //no copy

  JsonConfigurationProviderGuts * const _guts;

public:
  explicit JsonConfigurationProvider(const char * const confFilename);
  virtual ~JsonConfigurationProvider();

  virtual void InstanciateCFileTrafficFilterProfiles(CFTFPList& output) const;
  virtual void GetPrimarySsids(SsidSet& output) const;
  virtual void GetDataSsids(SsidSet& output, const char * const primarySsid) const;
  virtual const char *GetSsidIfname(const char * const ssid) const;
  virtual void GetStationTrafficFilterProfiles(StationTFPMap& output) const;
};

}; //namespace dcwlinux {


#endif //#ifndef JSON_CONFIGURATION_PROVIDER_H_INCLUDED
