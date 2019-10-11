#ifndef VAP_MANAGER_H_INCLUDED
#define VAP_MANAGER_H_INCLUDED

#include <set>

namespace dcw {
  struct DevicePolicy;
  struct TrafficSorter;
  struct EventReactor;
  struct TelemetryCollector;
} // namespace dcw

namespace dcwlinux {

class VirtualAP;
class VAPManager {
  typedef std::set<VirtualAP*> VAPSet;
  VAPSet _vaps;
  VAPManager(const VAPManager&); //no copy

public:
  VAPManager();
  virtual ~VAPManager();
  VirtualAP& InstanciateVAP(const char * const primarySsidName, const char * const primarySsidIfName, const ::dcw::DevicePolicy& devicePolicy, ::dcw::TrafficSorter& trafficSorter, ::dcw::EventReactor& eventReactor);

  VirtualAP& operator[](const char * const primarySsidName) const;

  void DestroyVAP(const char * const primarySsidName);

  void SetAllTelemetryCollector(::dcw::TelemetryCollector * const tc);
};

} // namespace dcwlinux

#endif //#ifndef VAP_MANAGER_H_INCLUDED
