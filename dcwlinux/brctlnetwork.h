#ifndef BRCTLNETWORK_H_INCLUDED
#define BRCTLNETWORK_H_INCLUDED


//these classes implement a single wifi network (layer 2 broadcast domain)
//which is implemented using linux bridge interfaces (brctl)



#include "dcw/network.h"


#include <string>
#include <list>

namespace dcwlinux {

class BrctlChannel : public ::dcw::BasicChannel {
  const std::string _ssidName;
  const std::string _brifName;

public:
  BrctlChannel(const char * const ssidName, const char * const brifName);
  BrctlChannel(const BrctlChannel& rhv);
  BrctlChannel(const BasicChannel& bc);
  virtual ~BrctlChannel();

  virtual const char *GetSsidName() const;
  virtual const char *GetIfName() const;

private:
  void ValidateBrifName() const;
};

class BrctlNetwork : public ::dcw::BasicNetwork {
  const BrctlChannel       _primaryChannel;
  std::list<BrctlChannel>  _dataChannels;

public:
  BrctlNetwork(const char * const primarySsidName, const char * const primaryBrifName);
  virtual ~BrctlNetwork();
  virtual const ::dcw::BasicChannel& GetPrimaryChannel() const;
  virtual void GetDataChannels(ChannelSet& output) const;
  void InsertDataChannel(const char * const ssidName, const char *brifName = NULL); //NULL = no interface remap...

};

}; //namespace dcwlinux {


#endif // #ifndef BRCTLNETWORK_H_INCLUDED
