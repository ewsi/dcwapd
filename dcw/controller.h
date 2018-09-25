
#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "./network.h"
#include "./eventreactor.h"
#include "./messagesocket.h"
#include "./trafficsorter.h"
#include "./devicepolicy.h"

#include <string>
#include <map>

namespace dcw {

class Controller : private EventReactor::IOSubscriber {
  const DevicePolicy&   _devicePolicy;
  TrafficSorter&        _trafficSorter;
  const BasicNetwork&   _network;
  EventReactor&         _eventReactor;
  MessageSocket&        _msgSocket;

  Controller(const Controller&); //no copy

public:
  Controller(const DevicePolicy& devicePolicy, TrafficSorter& trafficSorter, const BasicNetwork& network, EventReactor& eventReactor, MessageSocket& msgSocket);
  virtual ~Controller();

private:
  struct ClientState {
    typedef std::map<std::string, const ::dcw::BasicChannel*> PermittedChannelMap;
    //               ^ AP ssid name     ^ AP ssid...
    PermittedChannelMap  permittedChannels;
    TrafficPolicy        policy;
  };
  typedef std::map<dcw::MacAddress, ClientState> ClientStateMap;
  ClientStateMap _clients;

  virtual void OnIOReady(EventReactor::IOProvider& iop);
  void OnMessage(const MacAddress& source, const Message& msg);
  void OnStationJoin(const MacAddress& primaryMacAddr, const Message& msg);
  void OnStationUnjoin(const MacAddress& primaryMacAddr, const Message& msg);
  void OnStationAck(const MacAddress& primaryMacAddr, const Message& msg);
  void OnStationNack(const MacAddress& primaryMacAddr, const Message& msg);

  void ReplyToStation(const MacAddress& primaryMacAddr, const Message& reply);

  void NotifyAllQuit();

};

}; //namespace dcw



#endif /* #ifndef CONTROLLER_H_INCLUDED */
