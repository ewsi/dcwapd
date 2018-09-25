
#ifndef TRAFFICSORTER_H_INCLUDED
#define TRAFFICSORTER_H_INCLUDED

#include "./macaddress.h"
#include "./trafficfilterprofile.h"
#include "./network.h"

#include <map>

namespace dcw {

struct TrafficPolicy {
  typedef std::map< ::dcw::MacAddress, const ::dcw::BasicChannel*> DataChannelMap;
  //               ^ client DC macaddr      ^ AP ssid

  DataChannelMap                dataChannels;
  const TrafficFilterProfile   *trafficFilterProfile;
  TrafficPolicy() : trafficFilterProfile(NULL) {}
};

struct TrafficSorter {
  virtual ~TrafficSorter() {}

  virtual void ApplyClientTrafficPolicy(const MacAddress& primaryAddr, const TrafficPolicy& policy) = 0;
  virtual void RemoveClientTrafficPolicy(const MacAddress& primaryAddr) = 0;
};

}; //namespace dcw


#endif // #ifndef TRAFFICSORTER_H_INCLUDED
