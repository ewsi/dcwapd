#ifndef SIMPLENETWORK_H_INCLUDED
#define SIMPLENETWORK_H_INCLUDED


#include "./network.h"

#include <list>
#include <string>

namespace dcw {


//SimpleChannel -- Simple text-only (ssid) implementation of a channel
class SimpleChannel : public BasicChannel {
  const std::string _ssidName;

public:
  SimpleChannel(const char * const ssidName);
  SimpleChannel(const SimpleChannel& rhv);
  explicit SimpleChannel(const BasicChannel& bc);
  ~SimpleChannel() override;

  const char *GetSsidName() const override;
};

//SimpleNetwork -- Simple text-only (ssid) implementation of a network
class SimpleNetwork : public BasicNetwork {
  const SimpleChannel       _primaryChannel;
  std::list<SimpleChannel>  _dataChannels;

public:
  explicit SimpleNetwork(const char * const primarySsidName);
  ~SimpleNetwork() override;
  const BasicChannel& GetPrimaryChannel() const override;
  void GetDataChannels(ChannelSet& output) const override;
  void InsertDataChannel(const char * const ssidName);
};

} // namespace dcw


#endif //#ifndef SIMPLENETWORK_H_INCLUDED
