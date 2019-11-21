#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <set>

namespace dcw {

struct BasicChannel {
  virtual ~BasicChannel() {}
  virtual const char *GetSsidName() const = 0;
};

struct BasicNetwork {
  typedef std::set<const ::dcw::BasicChannel *> ChannelSet;

  virtual ~BasicNetwork() {}
  virtual const BasicChannel& GetPrimaryChannel() const = 0;
  virtual void GetDataChannels(ChannelSet& output) const = 0;
};

} // namespace dcw

#endif //#ifndef NETWORK_H_INCLUDED
