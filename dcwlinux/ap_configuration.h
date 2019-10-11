#ifndef AP_CONFIGURATION_H_INCLUDED
#define AP_CONFIGURATION_H_INCLUDED

#include "dcw/devicepolicy.h"
#include "dcw/cfiletrafficfilterprofile.h"
#include "dcw/macaddress.h"

#include <set>
#include <list>
#include <map>
#include <string>

namespace dcw {
  struct EventReactor;
} // namespace dcw

namespace dcwlinux {

class VAPManager;
class MacRemapperDriver;

struct APConfigurationProvider {
  typedef std::list<dcw::CFileTrafficFilterProfile *>   CFTFPList;
  typedef std::set<std::string>                         SsidSet;
  typedef std::map<dcw::MacAddress, std::string>        StationTFPMap;

  virtual ~APConfigurationProvider(){}
  virtual void InstanciateCFileTrafficFilterProfiles(CFTFPList& output) const = 0; //WARNING: caller MUST delete!
  virtual void GetPrimarySsids(SsidSet& output) const = 0;
  virtual void GetDataSsids(SsidSet& output, const char * const primarySsid) const = 0;
  virtual const char *GetSsidIfname(const char * const ssid) const = 0;
  virtual void GetStationTrafficFilterProfiles(StationTFPMap& output) const = 0;
};

class APConfiguration : public dcw::DevicePolicy {
  APConfiguration(const APConfiguration&); //no copy

public:
  explicit APConfiguration(const APConfigurationProvider& initialConfiguration);
  virtual ~APConfiguration();

  void Apply(MacRemapperDriver& driver, VAPManager& vapman, ::dcw::EventReactor& eventReactor) const;

  void ReloadRunningConfiguration(const APConfigurationProvider& newConfiguration);

  void Dump() const;

  virtual const dcw::TrafficFilterProfile& GetTrafficFilterProfile(const dcw::MacAddress& device) const;
  virtual void FilterPermittedDataChannels(const dcw::MacAddress& device, const unsigned deviceTotalCapableDataChannels, dcw::BasicNetwork::ChannelSet& allowedDataChannels) const;


private:

  void LoadConfiguration(const APConfigurationProvider& conf);
  void Cleanup();
  void SelfValidate() const;

  typedef APConfigurationProvider::SsidSet                                  SsidSet;
  typedef std::map<std::string, dcw::CFileTrafficFilterProfile *>           CFTFPMap;
  typedef std::map<std::string, SsidSet>                                    PrimaryDataMap;
  typedef std::map<std::string, std::string>                                SsidIfnameMap;
  typedef std::map<dcw::MacAddress, const dcw::CFileTrafficFilterProfile *> StationCFTFPMap;

  CFTFPMap              _trafficFilterProfiles;
  PrimaryDataMap        _primaryDataMap;
  SsidIfnameMap         _ssidIfnameMap;
  StationCFTFPMap       _stationFilterProfiles;

};

} // namespace dcwlinux


#endif //#ifndef AP_CONFIGURATION_H_INCLUDED
