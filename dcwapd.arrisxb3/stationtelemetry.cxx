

#include "./stationtelemetry.h"
#include "./singlenetwork.h"

#include "ccspwrapper/tr181_config_provider.h"

#include "dcw/macaddress.h"
#include "dcw/dcwlog.h"

#include <string.h>

namespace {

struct SingleStationDataChannel : public ::ccspwrapper::Tr181ConfigProvider {

  const ::dcw::MacAddress _macAddr;
  const SingleNetwork&    _singleNetwork;

  SingleStationDataChannel(const ::dcw::MacAddress& macAddr, const SingleNetwork& singleNetwork) :
    _macAddr(macAddr),
    _singleNetwork(singleNetwork)
    {}
  virtual ~SingleStationDataChannel() {}

  virtual void GetValue(const char * const name, ::ccspwrapper::Tr181Scalar& value) {

    //Station.X.BondedDataChannel.X. values...
    if (strcmp(name, "APIndex") == 0) {
      value = (uint64_t)_singleNetwork.GetDataChannelApIndex();
    }
    else if (strcmp(name, "MACAddress") == 0) {
      value = _macAddr.ToString();
    }
    else {
      dcwlogerrf("Unhandled station/datachannel param name: %s\n", name);
      throw ccspwrapper::Tr181Exception();
    }

  }
};

struct SingleStation :
  public ::ccspwrapper::Tr181ConfigProvider,
  public ::ccspwrapper::AutoDeleteTr181SubCollectionProvider {

  const ::dcw::MacAddress                      _macAddr;
  const SingleNetwork&                         _singleNetwork;
  const ::dcw::TrafficPolicy::DataChannelMap   _dataChannels;
  const ::dcw::TrafficFilterProfile           *_trafficFilterProfile;

  SingleStation( const ::dcw::MacAddress&                     macAddr,
                 const SingleNetwork&                         singleNetwork,
                 const ::dcw::TrafficPolicy::DataChannelMap&  dataChannels,
                 const ::dcw::TrafficFilterProfile           *trafficFilterProfile

  ) :
    _macAddr(macAddr),
    _singleNetwork(singleNetwork),
    _dataChannels(dataChannels),
    _trafficFilterProfile(trafficFilterProfile)
    {}
  virtual ~SingleStation() {}
  virtual void GetValue(const char * const name, ::ccspwrapper::Tr181Scalar& value) {
    //Station.X. values...
    if (strcmp(name, "PrimaryMACAddress") == 0) {
      value = _macAddr.ToString();
    }
    else if (strcmp(name, "NetworkIndex") == 0) {
      value = (uint64_t)1; //hard-coding to 1 as this is a single-network implementation
    }
    else if (strcmp(name, "FilterName") == 0) {
      if (_trafficFilterProfile == NULL) {
        value = std::string("");
      }
      else {
        value = std::string(_trafficFilterProfile->GetName());
      }
    }
    else {
      dcwlogerrf("Unhandled station/datachannel param name: %s\n", name);
      throw ccspwrapper::Tr181Exception();
    }
  }
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
    if (_trafficFilterProfile == NULL) {
      return; //the data channels are NOT (yet) bonded
    }
    for (::dcw::TrafficPolicy::DataChannelMap::const_iterator i = _dataChannels.begin(); i != _dataChannels.end(); ++i) { 
      collection.push_back(new SingleStationDataChannel(i->first, _singleNetwork));
      //Note: if you are looking for the delete it can be found in 
      //      the base class "AutoDeleteTr181SubCollectionProvider"
    }
  }
};
} //namespace




StationTelemetry::StationTelemetry(const SingleNetwork& singleNetwork) :
  _stationDataChannelSubCollectionProvider(*this),
  _singleNetwork(singleNetwork) {
  pthread_mutex_init(&_stationStatesLock, NULL);
}

StationTelemetry::~StationTelemetry() {
  pthread_mutex_destroy(&_stationStatesLock);
}

::ccspwrapper::Tr181SubCollectionProvider& StationTelemetry::GetStationDataChannelSubCollectionProvider() {
  return _stationDataChannelSubCollectionProvider;
}

void StationTelemetry::PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
  //asking to provide data channel elements for a single station?
  SingleStation * const ss = dynamic_cast<SingleStation*>(&parent);
  if (ss != NULL) {
    //yup... forward the poulate request on...
    ss->PopulateConfigProviderCollection(parent, collection);
    return;
  }

  //nope... asking for the station list itself...
  if (!_singleNetwork.GetEnabled()) {
    return; //network aint enabled... meaning no connected DCW stations...
  }

  //generate the list of stations and their states...
  pthread_mutex_lock(&_stationStatesLock);
  //note: none of the code in the critical section should throw anything...
  for (StationStates::const_iterator i = _stationStates.begin(); i != _stationStates.end(); i++) {
    collection.push_back(new SingleStation(i->first, _singleNetwork, i->second.dataChannels, i->second.trafficFilterProfile));
    //Note: if you are looking for the delete it can be found in 
    //      the base class "AutoDeleteTr181SubCollectionProvider"
  }
  pthread_mutex_unlock(&_stationStatesLock);
}


void StationTelemetry::Telemetry_OnStationUpdate(const ::dcw::BasicNetwork& network, const ::dcw::MacAddress& primaryMacAddr, const ::dcw::TrafficPolicy::DataChannelMap& dataChannels, const ::dcw::TrafficFilterProfile *trafficFilterProfile) {

  //note: if this is called with a NULL traffic filter profile then
  //      the data channels are just known, but not active/bonded

  SingleStationState sss;
  sss.dataChannels = dataChannels;
  sss.trafficFilterProfile = trafficFilterProfile;

  pthread_mutex_lock(&_stationStatesLock);
  _stationStates[primaryMacAddr] = sss;
  pthread_mutex_unlock(&_stationStatesLock);
}

void StationTelemetry::Telemetry_OnForgetStation(const ::dcw::BasicNetwork& network, const ::dcw::MacAddress& primaryMacAddr) {
  pthread_mutex_lock(&_stationStatesLock);
  _stationStates.erase(primaryMacAddr);
  pthread_mutex_unlock(&_stationStatesLock);
}

void StationTelemetry::Telemetry_Clear() {
  pthread_mutex_lock(&_stationStatesLock);
  _stationStates.clear();
  pthread_mutex_unlock(&_stationStatesLock);
}

