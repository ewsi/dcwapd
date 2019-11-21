
#include "./ap_configuration.h"
#include "./macremapper_driver.h"
#include "./vap_manager.h"
#include "./virtual_ap.h"

#include "dcw/dcwlog.h"


#include <exception>

namespace {
struct ReloadRunningConfigurationNotImplementedException : public std::exception {
  virtual const char* what() const throw() {
    return "ReloadRunningConfiguration() Not Yet Implemented";
  }
};
struct ValidationFailureException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to validate configuration!";
  }
};
} // namespace



using namespace dcwlinux;

static const char _defaultProfileName[] = "TFP_Default";

APConfiguration::APConfiguration(const APConfigurationProvider& initialConfiguration) {
  try {
    LoadConfiguration(initialConfiguration);
  }
  catch (...) {
    Cleanup();
    throw;
  }
}

APConfiguration::~APConfiguration() {
  Cleanup();
}

void APConfiguration::Apply(MacRemapperDriver& driver, VAPManager& vapman, ::dcw::EventReactor& eventReactor) const {
  //load the traffic filter profiles into the driver...
  for (CFTFPMap::const_iterator i = _trafficFilterProfiles.begin(); i != _trafficFilterProfiles.end(); i++) {
    driver.ParseAndLoadFilter(*i->second); //XXX defensive check for NULL?
  }


  //instanciate all the virtual APs... (one per primary SSID)
  for (PrimaryDataMap::const_iterator i = _primaryDataMap.begin(); i != _primaryDataMap.end(); i++) {
    const char * const pssidName = i->first.c_str();
    const SsidIfnameMap::const_iterator ifnameIter = _ssidIfnameMap.find(pssidName);
    if (ifnameIter == _ssidIfnameMap.end()) {
      //defensive
      dcwlogerrf("For whatever reason primary SSID '%s' dont have an interface name...\n", pssidName);
      throw ValidationFailureException();
       
    }

    //instanciate the actual VirtualAP object...
    VirtualAP& vap = vapman.InstanciateVAP(
      pssidName,                    //primarySsidName
      ifnameIter->second.c_str(),   //primarySsidIfName
      *this,                        //devicePolicy
      driver,                       //trafficSorter
      eventReactor                  //eventReactor
    );

    //give it its data channels...
    for (SsidSet::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
      const char * const dssidName = j->c_str();

      //determine the data channel interface name (if any)
      const char * dssidIfname = NULL; //note: this may be null...
      const SsidIfnameMap::const_iterator difnameIter = _ssidIfnameMap.find(dssidName);
      if (difnameIter != _ssidIfnameMap.end()) dssidIfname = difnameIter->second.c_str();

      //XXX should eventually use the proper functions...
      //XXX InsertDataChannel() comes from "VirtualAP" base class "Brctlnetwork"
      vap.InsertDataChannel(dssidName, dssidIfname);
    }
  }
}

void APConfiguration::ReloadRunningConfiguration(const APConfigurationProvider& newConfiguration) {
  throw ReloadRunningConfigurationNotImplementedException();
}

void APConfiguration::Dump() const {
  dcwlogdbgf("%s\n", "AP Configuration Dump:");

  dcwlogdbgf("%s\n", "  Traffic Filter Profiles:");
  for (CFTFPMap::const_iterator i = _trafficFilterProfiles.begin(); i != _trafficFilterProfiles.end(); i++) {
    dcwlogdbgf("    %s\n", i->second->GetName());
  }

  dcwlogdbgf("%s\n", "  SSIDs:");
  for (PrimaryDataMap::const_iterator i = _primaryDataMap.begin(); i != _primaryDataMap.end(); i++) {
    dcwlogdbgf("    Primary '%s'\n", i->first.c_str());
    for(SsidSet::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
      dcwlogdbgf("      Data '%s'\n", j->c_str());
    }
  }

  dcwlogdbgf("%s\n", "  SSID Interfaces:");
  for (SsidIfnameMap::const_iterator i = _ssidIfnameMap.begin(); i != _ssidIfnameMap.end(); i++) {
    dcwlogdbgf("    '%s' -> '%s'\n", i->first.c_str(), i->second.c_str());
  }

  dcwlogdbgf("%s\n", "  Station Traffic Filter Profiles:");
  for (StationCFTFPMap::const_iterator i = _stationFilterProfiles.begin(); i != _stationFilterProfiles.end(); i++) {
    dcwlogdbgf("    '%s' -> '%s'\n", i->first.ToString().c_str(), i->second->GetName());
  }
}

const dcw::TrafficFilterProfile& APConfiguration::GetTrafficFilterProfile(const dcw::MacAddress& device) const {
  //first lookup the TFP name we intend to use for this device...
  StationCFTFPMap::const_iterator stationTfp = _stationFilterProfiles.find(device);
  if (stationTfp == _stationFilterProfiles.end()) {
    //lookup failed... use the default traffic filter profile for this device...
    dcwlogdbgf("Defaulting device %s to default profile\n", device.ToString().c_str());
    const CFTFPMap::const_iterator defaultCftfp = _trafficFilterProfiles.find(_defaultProfileName);
    if (defaultCftfp == _trafficFilterProfiles.end()) {
      //defensive...
      dcwlogerrf("%s\n", "Unable to lookup traffic default filter profile");
      throw ValidationFailureException();
    }
    if (defaultCftfp->second == NULL) {
      //defensive... (since we are dereferencing to return a reference)
      dcwlogerrf("%s\n", "Default traffic filter profile is NULL");
      throw ValidationFailureException();
    }
    return *defaultCftfp->second;
  }

  if (stationTfp->second == NULL) {
    //defensive... (since we are dereferencing to return a reference)
    dcwlogerrf("Traffic filter profile for device '%s' is NULL\n", stationTfp->first.ToString().c_str());
    throw ValidationFailureException();
  }

  return *stationTfp->second;
}

void APConfiguration::FilterPermittedDataChannels(const dcw::MacAddress& device, const unsigned deviceTotalCapableDataChannels, dcw::BasicNetwork::ChannelSet& allowedDataChannels) const {
  //for now, do nothing...
  //(let the station have all data channels)

  //note: it is only advised to do this for load-balancing purposes
  //      the optimal solution is to offer the station all available
  //      data-channels, then let the stations detemine which is the 
  //      best setup
  //
  //in the future though, this may be used for load-balancing...
  //here we could algorithmically spread-out the clients on
  //different data-channels if we have many

}

void APConfiguration::LoadConfiguration(const APConfigurationProvider& conf) {
  //read in the "C FILE" compatible traffic filter profiles...
  //remember, it is OUR (the called or InstanciateCFileTrafficFilterProfiles()) to delete these...
  APConfigurationProvider::CFTFPList tfps;
  conf.InstanciateCFileTrafficFilterProfiles(tfps);
  for (APConfigurationProvider::CFTFPList::const_iterator i = tfps.begin(); i != tfps.end(); i++) {
    if ((*i) == NULL) {
      dcwlogwarnf("%s\n", "A NULL traffic filter profile was detected!");
      continue;
    }
    if (_trafficFilterProfiles.find((*i)->GetName()) != _trafficFilterProfiles.end()) {
      dcwlogwarnf("Ignoring existing traffic filter profile: %s\n", (*i)->GetName());
      delete *i;
      continue;
    }
    _trafficFilterProfiles[(*i)->GetName()] = *i;
  }

  //read in the primary SSIDs...
  SsidSet primarySsids;
  conf.GetPrimarySsids(primarySsids); //XXX... is it a good idea to load directly here? (as opposed to a seperate set declared here on the stack)

  //load each primary SSID's configuration...
  for (SsidSet::const_iterator pssidIter = primarySsids.begin(); pssidIter != primarySsids.end(); pssidIter++) {
    const char * const pssidName = pssidIter->c_str();

    //create an entry in the primary data map...
    //note: not really necessary, but helps with
    //      detecting primary channels with no
    //      data SSIDs on validation...
    _primaryDataMap[pssidName];

    //get the interface name (ifconfig) used for the primary ssid...
    const char * const pssidIfName = conf.GetSsidIfname(pssidName);
    if (pssidIfName == NULL) {
      dcwlogerrf("No interface provided for primary SSID: %s\n", pssidName);
      throw ValidationFailureException();
    }
    if (pssidIfName[0] == '\0') {
      dcwlogerrf("Empty interface provided for primary SSID: %s\n", pssidName);
      throw ValidationFailureException();
    }
    _ssidIfnameMap[pssidName] = pssidIfName;

    //get the associated data channels for this primary ssid...
    SsidSet dataSsids;
    conf.GetDataSsids(dataSsids, pssidName);
    for (SsidSet::const_iterator dssidIter = dataSsids.begin(); dssidIter != dataSsids.end(); dssidIter++) {
      const char * const dssidName = dssidIter->c_str();

      //get the data channel's interface name (if any)
      const char * const dssidIfName = conf.GetSsidIfname(dssidName);
      if (dssidIfName != NULL) _ssidIfnameMap[dssidName] = dssidIfName;

      //add this as a data channel ssid to the primary ssid...
      _primaryDataMap[pssidName].insert(dssidName);
    }
  }

  //read in station TFPs...
  APConfigurationProvider::StationTFPMap stationTfpMap;
  conf.GetStationTrafficFilterProfiles(stationTfpMap);
  for (APConfigurationProvider::StationTFPMap::const_iterator i = stationTfpMap.begin(); i != stationTfpMap.end(); i++) {
    const CFTFPMap::const_iterator cftfpIter = _trafficFilterProfiles.find(i->second);
    if (cftfpIter == _trafficFilterProfiles.end()) {
      dcwlogerrf("Invalid traffic filter profile name '%s' for MAC address '%s'\n", i->second.c_str(), i->first.ToString().c_str());
      throw ValidationFailureException();
    }
    _stationFilterProfiles[i->first] = cftfpIter->second;
  }

  //one we have read everything in, validate it...
  Dump();
  SelfValidate();
}

void APConfiguration::Cleanup() {
  
  //delete any instanciated traffic filter profiles...
  for (CFTFPMap::const_iterator i = _trafficFilterProfiles.begin(); i != _trafficFilterProfiles.end(); i++) {
    delete i->second;
  }
  _trafficFilterProfiles.clear();

}


void APConfiguration::SelfValidate() const {

  for (PrimaryDataMap::const_iterator pdcIter = _primaryDataMap.begin(); pdcIter != _primaryDataMap.end(); pdcIter++) {
    const char * const pssidName = pdcIter->first.c_str();

    //ensure each primary SSID has at least one data channel
    if (pdcIter->second.empty()) {
      dcwlogerrf("Configured primary SSID \"%s\" has no associated data channels\n", pssidName);
      throw ValidationFailureException();
    }

    //validate the associated data channels:
    for (SsidSet::const_iterator dssidIter = pdcIter->second.begin(); dssidIter != pdcIter->second.end(); dssidIter++) {
      const char * const dssidName = dssidIter->c_str();

      //ensure the data ssid name is NOT used as a primary...
      if (_primaryDataMap.find(dssidName) != _primaryDataMap.end()) {
        dcwlogerrf("Configured primary SSID \"%s\" declares primary SSID \"%s\" as data SSID.\n", pssidName, dssidName);
        throw ValidationFailureException();
      }

      //warn for each data SSID that does not have an associated network interface
      SsidIfnameMap::const_iterator sifnIter = _ssidIfnameMap.find(dssidName);
      if (sifnIter == _ssidIfnameMap.end()) {
        dcwlogwarnf("Configured data SSID \"%s\" for primary SSID \"%s\" has no associated network interface. Will use the primary interface.\n", dssidName, pssidName);
      }
    }


    //ensure each primary SSID has an associated network interface
    SsidIfnameMap::const_iterator sifnIter = _ssidIfnameMap.find(pssidName);
    if (sifnIter == _ssidIfnameMap.end()) {
      dcwlogerrf("Configured primary SSID \"%s\" has no associated network interface\n", pssidName);
      throw ValidationFailureException();
    }
  } 

  //ensure we have at least one traffic filter progile
  if (_trafficFilterProfiles.empty()) {
    dcwlogerrf("%s\n", "We don't have a single traffic profile");
    throw ValidationFailureException();
  }

  //ensure we have a default traffic filter profile
  CFTFPMap::const_iterator tfpIter = _trafficFilterProfiles.find(_defaultProfileName);
  if (tfpIter == _trafficFilterProfiles.end()) {
    dcwlogerrf("We don't have a default traffic profile (\"%s\")!\n", _defaultProfileName);
    throw ValidationFailureException();
  }

  //ensure that the filter files are parsable...
  for (tfpIter = _trafficFilterProfiles.begin(); tfpIter != _trafficFilterProfiles.end(); tfpIter++) {
    if (tfpIter->second == NULL) {
      dcwlogerrf("NULL traffic filter profile: %s\n", tfpIter->first.c_str());
      throw ValidationFailureException();
    }
    if (!MacRemapperDriver::ValidateFilter(*tfpIter->second)) {
      dcwlogerrf("Failed to parse filter \"%s\"\n", tfpIter->second->GetName());
      throw ValidationFailureException();
    }
  }
}

