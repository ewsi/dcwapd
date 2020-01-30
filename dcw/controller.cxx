
#include "./controller.h"
#include "./macaddress.h"
#include "./message.h"

#include "./dcwlog.h"

#include <cstring>
#include <exception>

namespace {
struct UnhandledMessageTypeException : public std::exception {
  const char* what() const noexcept override {
    return "Unhandled message type";
  }
};
} // namespace

using namespace dcw;


Controller::Controller(
  const DevicePolicy& devicePolicy,
  TrafficSorter& trafficSorter,
  const BasicNetwork& network,
  EventReactor& eventReactor,
  MessageSocket& msgSocket) :

  _devicePolicy(devicePolicy),
  _trafficSorter(trafficSorter),
  _network(network),
  _eventReactor(eventReactor),
  _msgSocket(msgSocket),
  _telemetryCollector(NULL) {

  // tell the event reactor to call us when "_msgSocket"
  // can be received from (non-blocking)
  _eventReactor.RegisterIOSubscriber(*this, _msgSocket);

  dcwloginfof("Controller for '%s' is now registered to receive DCW messages\n", _network.GetPrimaryChannel().GetSsidName());

  this->NotifyAllQuit(); // should we be doing this at startup? can't hurt...
}

Controller::~Controller() {
  try {
    this->NotifyAllQuit();
  } catch (...) {}
  _eventReactor.UnegisterIOSubscriber(*this);
  dcwloginfof("Controller for '%s' is now unregistered from receiving DCW messages\n", _network.GetPrimaryChannel().GetSsidName());
}

void Controller::SetTelemetryCollector(TelemetryCollector * const telemetryCollector) {
  _telemetryCollector = telemetryCollector;
}

void Controller::OnIOReady(EventReactor::IOProvider& iop) {
  if ((&_msgSocket) != (&iop)) return; //shouldnt get here...

  // if we get here it means that we should be able to call
  // _msgSocket.ReceiveMessage() once and not block
  // if we did not collect all pending messages to
  // be received we will get called again
  Message     m;
  MacAddress  source;
  try {
    _msgSocket.ReceiveMessage(source, m); //will throw on receive or parse failure
    OnMessage(source, m);
  }
  catch (std::exception& e) {
    dcwlogerrf("Failed to receive message: %s\n", e.what());
    return;
  }
}

void Controller::OnMessage(const MacAddress& source, const Message& msg) {
  //this function is called once per each succsesfully parsed
  //incoming message...

  dcwlogdbgf("Got a message from %s\n", source.ToString().c_str());

  switch(msg.id) {
  case DCWMSG_STA_JOIN:
    OnStationJoin(source, msg);
    break;
  case DCWMSG_STA_UNJOIN:
    OnStationUnjoin(source, msg);
    break;
  case DCWMSG_STA_ACK:
    OnStationAck(source, msg);
    break;
  case DCWMSG_STA_NACK:
    OnStationNack(source, msg);
    break;
  default:
    throw UnhandledMessageTypeException();
    break;
  }
}

void Controller::OnStationJoin(const MacAddress& primaryMacAddr, const Message& msg) {
  const struct dcwmsg_sta_join& m = msg.sta_join;

  BasicNetwork::ChannelSet       apDataChannels;

  dcwlogdbgf("Got a station join request from %s\n", primaryMacAddr.ToString().c_str());

  //does the station request have at least one data mac address?
  if (m.data_macaddr_count < 1) {
    dcwlogwarnf("Got a station join request from %s with no data MAC addresses\n", primaryMacAddr.ToString().c_str());
    Message reply(DCWMSG_AP_REJECT_STA);
    reply.ap_reject_sta.data_macaddr_count = 0;
    ReplyToStation(primaryMacAddr, reply);
    return;
  }

  //retrieve our network configuration and validate that 
  //we have at least one data ssid
  _network.GetDataChannels(apDataChannels);
  if (!apDataChannels.empty()) {
    //call upon the device policy to filter out if needed...
    _devicePolicy.FilterPermittedDataChannels(primaryMacAddr, m.data_macaddr_count, apDataChannels);
  }
  if (apDataChannels.empty()) {
    dcwlogwarnf("Got a station join request from %s, but no data SSIDs are available in the network\n", primaryMacAddr.ToString().c_str());
    Message reply(DCWMSG_AP_REJECT_STA);
    reply.ap_reject_sta.data_macaddr_count = m.data_macaddr_count;
    std::memcpy(reply.ap_reject_sta.data_macaddrs, m.data_macaddrs, m.data_macaddr_count * 6);
    ReplyToStation(primaryMacAddr, reply);
    return;
  }

  //add declared data mac addresses to client state
  ClientState& state = _clients[primaryMacAddr];
  for (unsigned i = 0; i < m.data_macaddr_count; i++) {
    state.policy.dataChannels[m.data_macaddrs[i]]; //will create and assign channel ptr to NULL if it does not exist
  }

  //start forming reply
  Message reply(DCWMSG_AP_ACCEPT_STA);
  BasicNetwork::ChannelSet::const_iterator apdc_iter;
  reply.ap_accept_sta.data_ssid_count = static_cast<unsigned int>(apDataChannels.size());
  unsigned i = 0;
  for (apdc_iter = apDataChannels.begin(); apdc_iter != apDataChannels.end(); ++apdc_iter, i++) {
    state.permittedChannels[(*apdc_iter)->GetSsidName()] = *apdc_iter;
    std::strncpy(reply.ap_accept_sta.data_ssids[i], (*apdc_iter)->GetSsidName(), sizeof(reply.ap_accept_sta.data_ssids[i]));
  }
  
  //reply back to the station letting it know which
  //MAC addresses and SSIDs it should use
  dcwlogdbgf("Telling station %s that it has %zu data channel(s) to use\n", primaryMacAddr.ToString().c_str(), apDataChannels.size());
  ReplyToStation(primaryMacAddr, reply);

  //notify telemetry that we have an update for this station...
  if (_telemetryCollector != NULL)
    _telemetryCollector->Telemetry_OnStationUpdate(_network, primaryMacAddr, state.policy.dataChannels, NULL);

}

void Controller::OnStationUnjoin(const MacAddress& primaryMacAddr, const Message& msg) {
  const struct dcwmsg_sta_unjoin& m = msg.sta_unjoin;
  dcwlogdbgf("Got a station unjoin request from %s\n", primaryMacAddr.ToString().c_str());

  //tell telemetry to forget this station no matter what...
  if (_telemetryCollector != NULL)
    _telemetryCollector->Telemetry_OnForgetStation(_network, primaryMacAddr);

  //does the station request have at least one data mac address?
  if (m.data_macaddr_count < 1) {
    dcwlogdbgf("Station unjoin request from %s has no MAC addresses. Assuming this is an unjoin all.\n", primaryMacAddr.ToString().c_str());
    _clients.erase(primaryMacAddr);
    try {
      _trafficSorter.RemoveClientTrafficPolicy(primaryMacAddr);
    }
    catch (...) {}
    return;
  }

  ClientState& state = _clients[primaryMacAddr]; //WARNING: this will insert if state dont currently exist

  //sanity check: the known client state permitted any data channels?
  if (state.permittedChannels.empty()) {
    //no... trash the known client state and do a sanity traffic polcy removal...
    dcwloginfof("Station unjoin request from %s has no permitted data channels. Trashing any (unlikely) known state for this client\n", primaryMacAddr.ToString().c_str());
    _clients.erase(primaryMacAddr);
    try {
      _trafficSorter.RemoveClientTrafficPolicy(primaryMacAddr);
    }
    catch(...) {}
    return;
  }

  //remove any channel bondings matching the provided data channel mac addresses
  for (unsigned i = 0; i < m.data_macaddr_count; i++) {
    const ::dcw::MacAddress dcaddr(m.data_macaddrs[i]);
    auto dcmEntry = state.policy.dataChannels.find(dcaddr);
    if (dcmEntry == state.policy.dataChannels.end()) continue;
    if (dcmEntry->second == NULL) {
      dcwlogwarnf("Data channel MAC address %s on client %s is not currently bonded\n", dcaddr.ToString().c_str(), primaryMacAddr.ToString().c_str());
      continue;
    }
    dcwlogdbgf("Removing data channel bond %s -> '%s' from station %s\n", dcaddr.ToString().c_str(), dcmEntry->second->GetSsidName(), primaryMacAddr.ToString().c_str());
    dcmEntry->second = NULL;
  }

  //does this client have any more bonded channels?
  for (auto dcmIter = state.policy.dataChannels.begin();
       dcmIter != state.policy.dataChannels.end(); dcmIter++) {
    if (dcmIter->second != NULL) {
      //yup... the client is still bonded to something...
      //simply update the traffic sorter policy...
      dcwloginfof("Updating traffic policy for station: %s.\n", primaryMacAddr.ToString().c_str());
      try {
        // XXX should we refresh the filter profile the client is assigned to here?
        _trafficSorter.ApplyClientTrafficPolicy(primaryMacAddr, state.policy);
      }
      catch(::std::exception& e) {
        dcwlogerrf("Traffic policy application for %s failed.\n", primaryMacAddr.ToString().c_str());
      }
      ReplyToStation(primaryMacAddr, DCWMSG_AP_ACK_DISCONNECT);
      return;
    }
  }

  //nope... client aint bonded... drop it off completely...
  dcwloginfof("Station %s has no bonded data channels. Dropping it.\n", primaryMacAddr.ToString().c_str());
  try {
    _trafficSorter.RemoveClientTrafficPolicy(primaryMacAddr);
  }
  catch (...) {}
  ReplyToStation(primaryMacAddr, DCWMSG_AP_ACK_DISCONNECT);
}

void Controller::OnStationAck(const MacAddress& primaryMacAddr, const Message& msg) {
  const struct dcwmsg_sta_ack& m = msg.sta_ack;
  dcwlogdbgf("Got a station ACK from %s\n", primaryMacAddr.ToString().c_str());

  // first make sure this client has actually sent a join first...
  auto client = _clients.find(primaryMacAddr);
  if (client == _clients.end()) {
    dcwlogerrf("Got a client ACK without a station join from %s\n", primaryMacAddr.ToString().c_str());
    Message reply(DCWMSG_AP_REJECT_STA);
    reply.ap_reject_sta.data_macaddr_count = m.bonded_data_channel_count;
    for (unsigned i = 0; i < reply.ap_reject_sta.data_macaddr_count; i++) {
      std::memcpy(reply.ap_reject_sta.data_macaddrs[i],  m.bonded_data_channels[i].macaddr, sizeof(reply.ap_reject_sta.data_macaddrs[i]));
    }
    ReplyToStation(primaryMacAddr, reply);
    return;
  }
  ClientState& state = client->second;

  //validate that the client's ACK'd bonded SSIDs match whats expected...
  for (unsigned bcIdx = 0; bcIdx < m.bonded_data_channel_count; bcIdx++) {

    //ensure that the SSID is permitted...
    const std::string bondedSsid(m.bonded_data_channels[bcIdx].ssid, strnlen(m.bonded_data_channels[bcIdx].ssid, sizeof(m.bonded_data_channels[bcIdx].ssid)));
    const ClientState::PermittedChannelMap::const_iterator permittedChannel = state.permittedChannels.find(bondedSsid);
    if (permittedChannel == state.permittedChannels.end()) {
      dcwlogerrf("Got a client ACK with an invalid SSID from %s\n", primaryMacAddr.ToString().c_str());
      Message reply(DCWMSG_AP_REJECT_STA);
      reply.ap_reject_sta.data_macaddr_count = m.bonded_data_channel_count;
      for (unsigned i = 0; i < reply.ap_reject_sta.data_macaddr_count; i++) {
        std::memcpy(reply.ap_reject_sta.data_macaddrs[i],  m.bonded_data_channels[i].macaddr, sizeof(reply.ap_reject_sta.data_macaddrs[i]));
      }
      ReplyToStation(primaryMacAddr, reply);
      return;
    }

    // ensure the MAC address is known...
    if (state.policy.dataChannels.find(m.bonded_data_channels[bcIdx].macaddr) == state.policy.dataChannels.end()) {
      dcwlogerrf("Got a client ACK with an invalid data channel MAC address from %s\n", primaryMacAddr.ToString().c_str());
      Message reply(DCWMSG_AP_REJECT_STA);
      reply.ap_reject_sta.data_macaddr_count = m.bonded_data_channel_count;
      for (unsigned i = 0; i < reply.ap_reject_sta.data_macaddr_count; i++) {
        std::memcpy(reply.ap_reject_sta.data_macaddrs[i],  m.bonded_data_channels[i].macaddr, sizeof(reply.ap_reject_sta.data_macaddrs[i]));
      }
      ReplyToStation(primaryMacAddr, reply);
      return;
    }

    // all is good... add this channel bond to the pending traffic policy application
    state.policy.dataChannels[m.bonded_data_channels[bcIdx].macaddr] = permittedChannel->second;
  }

  try {
    //set the traffic filter profile...
    state.policy.trafficFilterProfile = &_devicePolicy.GetTrafficFilterProfile(primaryMacAddr);

    //apply the policy...
    _trafficSorter.ApplyClientTrafficPolicy(primaryMacAddr, state.policy);

    //notify telemetry of the bond....
    if (_telemetryCollector != NULL)
      _telemetryCollector->Telemetry_OnStationUpdate(_network, primaryMacAddr, state.policy.dataChannels, state.policy.trafficFilterProfile);
  }
  catch (std::exception& e) {
    dcwlogerrf("Traffic policy application for %s failed.\n", primaryMacAddr.ToString().c_str());
  }

}

void Controller::OnStationNack(const MacAddress& primaryMacAddr, const Message& msg) {
  const struct dcwmsg_sta_nack& m_src = msg.sta_nack;

  Message unjoinMsg(DCWMSG_STA_UNJOIN);
  struct dcwmsg_sta_unjoin& m_dst = unjoinMsg.sta_unjoin;

  dcwlogdbgf("Got a station NACK from %s Processing as unjoin\n", primaryMacAddr.ToString().c_str());
  m_dst.data_macaddr_count = m_src.data_macaddr_count;
  std::memcpy(m_dst.data_macaddrs, m_src.data_macaddrs, sizeof(m_dst.data_macaddrs));
  this->OnStationUnjoin(primaryMacAddr, unjoinMsg);
  // XXX WARNING: THIS PROBABLY SHOULD NOT SEND THE DCWMSG_AP_ACK_DISCONNECT AS THE UNJOIN DOES!!
}


void Controller::ReplyToStation(const MacAddress& primaryMacAddr, const Message& reply) {
  try {
    if (reply.id == DCWMSG_AP_REJECT_STA) {
      try {
        //XXX is this really the place to put this?
        _trafficSorter.RemoveClientTrafficPolicy(primaryMacAddr);
      }
      catch(...) {}
      _clients.erase(primaryMacAddr);
    }
    _msgSocket.TransmitMessage(primaryMacAddr, reply);
  }
  catch (std::exception& e) {
    dcwlogerrf("Failed to send message reply to %s: %s\n", primaryMacAddr.ToString().c_str(), e.what());
    return;
  }
}



void Controller::NotifyAllQuit() {
  Message     quitMsg(DCWMSG_AP_QUIT);
  MacAddress  broadcast;

  dcwlogdbgf("Informing all clients network '%s' has reset\n", _network.GetPrimaryChannel().GetSsidName());

  for (unsigned i = 0; i < 3; i++) {
    _msgSocket.TransmitMessage(broadcast, quitMsg);
    _eventReactor.SleepMs(200);
  }
}



