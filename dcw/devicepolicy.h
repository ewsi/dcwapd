#ifndef DEVICEPOLICY_H_INCLUDED
#define DEVICEPOLICY_H_INCLUDED

namespace dcw {

class TrafficFilterProfile;
struct MacAddress;

struct DevicePolicy {
  virtual ~DevicePolicy() {}
  virtual const TrafficFilterProfile& GetTrafficFilterProfile(const MacAddress& device) const = 0;
  virtual const unsigned GetMaximumAllowedDataChannels(const MacAddress& device) const = 0;
};

}; //namespace dcw

#endif //#ifndef DEVICEPOLICY_H_INCLUDED
