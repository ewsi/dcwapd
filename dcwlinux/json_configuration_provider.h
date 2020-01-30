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
  ~JsonConfigurationProvider() override;

  void InstanciateCFileTrafficFilterProfiles(CFTFPList& output) const override;
  void GetPrimarySsids(SsidSet& output) const override;
  void GetDataSsids(SsidSet& output, const char * const primarySsid) const override;
  const char *GetSsidIfname(const char * const ssid) const override;
  void GetStationTrafficFilterProfiles(StationTFPMap& output) const override;
};

} // namespace dcwlinux


#endif //#ifndef JSON_CONFIGURATION_PROVIDER_H_INCLUDED
