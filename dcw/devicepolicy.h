#ifndef DEVICEPOLICY_H_INCLUDED
#define DEVICEPOLICY_H_INCLUDED

#include "./network.h"

namespace dcw {

class TrafficFilterProfile;
struct MacAddress;

struct DevicePolicy {
  virtual ~DevicePolicy() {}
  virtual const TrafficFilterProfile& GetTrafficFilterProfile(const MacAddress& device) const = 0;
  virtual void FilterPermittedDataChannels(const MacAddress& device, const unsigned deviceTotalCapableDataChannels, BasicNetwork::ChannelSet& allowedDataChannels) const = 0;
};

}; //namespace dcw

#endif //#ifndef DEVICEPOLICY_H_INCLUDED
