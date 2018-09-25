
#include "./virtual_ap.h"

#include "dcw/dcwlog.h"

using namespace dcwlinux;

// this is the core function (if youd call it that)
// that pretty much glues this contraption together...
VirtualAP::VirtualAP(
  const char * const            primarySsid,
  const char * const            primarySsidIfName,
  const ::dcw::DevicePolicy&    devicePolicy,
  dcw::TrafficSorter&           trafficSorter,
  dcw::EventReactor&            eventReactor
) :
  dcwlinux::BrctlNetwork(primarySsid, primarySsidIfName),
  dcwposix::SelectableMessageSocket(primarySsidIfName),
  dcw::Controller(
    devicePolicy,      //devicePolicy
    trafficSorter,     //trafficSorter
    *this,             //network
    eventReactor,      //eventReactor
    *this              //msgSocket
  )
{
  dcw::MacAddress myMacAddr;
  GetSourceMacAddress(myMacAddr); //dcwposix::SelectableMessageSocket
  dcwloginfof("VAP Successfully Instanciated: %s (%s)\n", GetPrimaryChannel().GetSsidName(), myMacAddr.ToString().c_str());
}

VirtualAP::~VirtualAP() {
  dcwloginfof("Destroying VAP: %s\n", GetPrimaryChannel().GetSsidName());
}


