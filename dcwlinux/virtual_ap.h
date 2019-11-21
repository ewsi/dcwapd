#ifndef VIRTUAL_AP_H_INCLUDED
#define VIRTUAL_AP_H_INCLUDED

#include "./brctlnetwork.h"
#include "dcw/controller.h"
#include "dcwposix/selectablemessagesocket.h"

namespace dcwlinux {

class VirtualAP :
  public BrctlNetwork,
  public dcwposix::SelectableMessageSocket,
  public dcw::Controller
  {

  VirtualAP(const VirtualAP&); //no copy

public:
  VirtualAP(
    const char * const            primarySsid,
    const char * const            primarySsidIfName,
    const ::dcw::DevicePolicy&    devicePolicy,
    ::dcw::TrafficSorter&         trafficSorter,
    ::dcw::EventReactor&          eventReactor
  );
  virtual ~VirtualAP();

  //XXX should ultimately implement these functons...
  //XXX this way things this object can signal the controller to correctly notify stations
  void AddDataChannel(const char * const ssid, const char * const ifname = NULL);
  void RemoveDataChannel(const char * const ssid, const char * const ifname = NULL);
  void RenameDataChannel(const char * const oldSsid, const char * const newSsid);
};

} // namespace dcwlinux

#endif
