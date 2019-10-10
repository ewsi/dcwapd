

#include "./json_configuration_provider.h"

#include "./jsonparser.ixx"

#include "dcw/macaddress.h"
#include "dcwposix/filterdirscanner.h"

namespace dcwlinux {

class JsonConfigurationProviderGuts : public APConfigurationProvider {
  JsonConfigurationProviderGuts(const JsonConfigurationProviderGuts&) = delete; //no copy
  const std::string _confFilename;

  typedef std::map<std::string, std::string>  DataChannelBridgeMap;
  struct PrimaryChannel {
    std::string            bridgeName;
    DataChannelBridgeMap   dataChannels;
  };
  typedef std::map<std::string, PrimaryChannel>   PrimaryChannelMap;
  typedef std::map<dcw::MacAddress, std::string>  StationFilterMap;


  std::string        _filterDirectory;
  PrimaryChannelMap  _primaryChannels;
  StationFilterMap   _stationFilters;


public:
  explicit JsonConfigurationProviderGuts(const char * const confFilename) : _confFilename(confFilename) {
    JSONParser jp(_confFilename);

    if (!jp.IsObject()) {
      throw JSONParserException("JSON configuration != Object");
    }

    //Filter Profiles...
    if (!jp.HasMember("filterdirectory")) {
      throw JSONParserException("JSON configuration does not have \"filterdirectory\"");
    }
    if (!jp["filterdirectory"].IsString()) {
      throw JSONParserException("JSON configuration \"filterdirectory\" is not a string");
    }
    _filterDirectory = jp["filterdirectory"].GetString();

    //VAPs...
    if (!jp.HasMember("virtualaps")) {
      throw JSONParserException("JSON configuration does not have \"virtualaps\"");
    }
    const JSONValue& virtualaps = jp["virtualaps"];
    if (!virtualaps.IsObject()) {
      throw JSONParserException("JSON configuration \"virtualaps\" is not an object");
    }

    for (JSONValue::ConstMemberIterator i = virtualaps.MemberBegin(); i != virtualaps.MemberEnd(); i++) {
      if (!i->name.IsString()) {
        throw JSONParserException("JSON configuration \"virtualaps\" contains a key which is not a string");
      }
      if (!i->value.IsObject()) {
        throw JSONParserException("JSON configuration \"virtualaps\" contains a value which is not an object");
      }
      PrimaryChannel& pc = _primaryChannels[i->name.GetString()];

      if (i->value.HasMember("bridge")) {
        if (!i->value["bridge"].IsString()) {
          throw JSONParserException("JSON configuration \"bridge\" is not a string");
        }
        pc.bridgeName = i->value["bridge"].GetString();
      }

      if (i->value.HasMember("datachannels")) {
        const JSONValue& datachannels = i->value["datachannels"];
        if (!datachannels.IsObject()) {
          throw JSONParserException("JSON configuration \"datachannels\" is not an object");
        }
        for (JSONValue::ConstMemberIterator j = datachannels.MemberBegin(); j != datachannels.MemberEnd(); j++) {
          if (!j->name.IsString()) {
            throw JSONParserException("JSON configuration \"datachannels\" contains a key which is not a string");
          }
          if (!j->value.IsObject()) {
            throw JSONParserException("JSON configuration \"datachannels\" contains a value which is not an object");
          }
          pc.dataChannels[j->name.GetString()];
          if (j->value.HasMember("bridge")) {
            if (!j->value["bridge"].IsString()) {
              throw JSONParserException("JSON configuration \"bridge\" is not a string");
            }
             pc.dataChannels[j->name.GetString()] = j->value["bridge"].GetString();
          }
        }
      }
    }

    //station filter mappings...
    if (!jp.HasMember("stations")) {
      return; //nothing more to do...
    }
    const JSONValue& stations = jp["stations"];
    if (!stations.IsObject()) {
      throw JSONParserException("JSON configuration \"stations\" is not an object");
    }

    for (JSONValue::ConstMemberIterator i = stations.MemberBegin(); i != stations.MemberEnd(); i++) {
      if (!i->name.IsString()) {
        throw JSONParserException("JSON configuration \"stations\" contains a key which is not a string");
      }
      if (!i->value.IsObject()) {
        throw JSONParserException("JSON configuration \"stations\" contains a value which is not an object");
      }
      if (i->value.HasMember("filterprofile")) {
        if (!i->value["filterprofile"].IsString()) {
          throw JSONParserException("JSON configuration \"filterprofile\" is not a string");
        }
        _stationFilters[::dcw::MacAddress(i->name.GetString())] = i->value["filterprofile"].GetString();
      }
    }
  }

  ~JsonConfigurationProviderGuts() override {
    //
  }

  void InstanciateCFileTrafficFilterProfiles(CFTFPList& output) const override {
    ::dcwposix::FilterdirScanner::FileFilterProfileList ffpl;
    ::dcwposix::FilterdirScanner dirScanner(_filterDirectory.c_str());
    dirScanner.Scan(ffpl);

    for (::dcwposix::FilterdirScanner::FileFilterProfileList::const_iterator i = ffpl.begin(); i != ffpl.end(); i++) {
      output.push_back(new ::dcw::FileTrafficFilterProfile(*i));
    }
  }

  void GetPrimarySsids(SsidSet& output) const override {
    for (PrimaryChannelMap::const_iterator i = _primaryChannels.begin(); i != _primaryChannels.end(); i++) {
      output.insert(i->first);
    }
  }

  void GetDataSsids(SsidSet& output, const char * const primarySsid) const override {
    const PrimaryChannelMap::const_iterator pssid = _primaryChannels.find(primarySsid);
    if (pssid == _primaryChannels.end()) return;

    for (DataChannelBridgeMap::const_iterator i = pssid->second.dataChannels.begin(); i != pssid->second.dataChannels.end(); i++) {
      output.insert(i->first);
    }
  }

  const char *GetSsidIfname(const char * const ssid) const override {
    PrimaryChannelMap::const_iterator pssid = _primaryChannels.find(ssid);
    if (pssid != _primaryChannels.end()) {
      if (pssid->second.bridgeName.empty()) {
        return NULL;
      }
      return pssid->second.bridgeName.c_str();
    }

    for (pssid = _primaryChannels.begin(); pssid != _primaryChannels.end(); pssid++) {
      const DataChannelBridgeMap& dataChannels = pssid->second.dataChannels;
      const DataChannelBridgeMap::const_iterator dc = dataChannels.find(ssid);
      if (dc == dataChannels.end()) continue;
      if (dc->second.empty()) {
        return NULL;
      }
      return dc->second.c_str();
    }

    return NULL;
  }
  void GetStationTrafficFilterProfiles(StationTFPMap& output) const override {
    for (StationFilterMap::const_iterator i = _stationFilters.begin(); i != _stationFilters.end(); i++) {
      output[i->first] = i->second;
    }

  }
};

} // namespace dcwlinux


using namespace dcwlinux;

JsonConfigurationProvider::JsonConfigurationProvider(const char * const confFilename) :
  _guts(new JsonConfigurationProviderGuts(confFilename)) {
  //
}

JsonConfigurationProvider::~JsonConfigurationProvider() {
  delete _guts;
}

void JsonConfigurationProvider::InstanciateCFileTrafficFilterProfiles(CFTFPList& output) const {
  _guts->InstanciateCFileTrafficFilterProfiles(output);
}

void JsonConfigurationProvider::GetPrimarySsids(SsidSet& output) const {
  _guts->GetPrimarySsids(output);
}

void JsonConfigurationProvider::GetDataSsids(SsidSet& output, const char * const primarySsid) const {
  _guts->GetDataSsids(output, primarySsid);
}

const char *JsonConfigurationProvider::GetSsidIfname(const char * const ssid) const {
  return _guts->GetSsidIfname(ssid);
}

void JsonConfigurationProvider::GetStationTrafficFilterProfiles(StationTFPMap& output) const {
  return _guts->GetStationTrafficFilterProfiles(output);
}


