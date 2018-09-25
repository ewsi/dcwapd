#ifndef SINGLENETWORK_H_INCLUDED
#define SINGLENETWORK_H_INCLUDED

#include "ccspwrapper/tr181_sub_collection_provider.h"
#include "ccspwrapper/tr181_config_provider.h"

class SingleNetwork :
  public ::ccspwrapper::Tr181SubCollectionProvider,
  public ::ccspwrapper::Tr181ConfigProvider {

public:
  SingleNetwork();
  virtual ~SingleNetwork();

  //config accessors...
  bool GetEnabled() const;
  unsigned GetPrimaryChannelApIndex() const;
  unsigned GetDataChannelApIndex() const;

  //tr181 accessors...
  ::ccspwrapper::Tr181SubCollectionProvider& GetNetworkDataChannelSubCollectionProvider();

  //Tr181SubCollectionProvider functions...
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection);
  virtual ::ccspwrapper::Tr181ConfigProvider *RequestAddNewProviderElement(Tr181ConfigProvider& parent);
  virtual void RequestDeleteConfigProviderElement(Tr181ConfigProvider& provider);

  //Tr181ConfigProvider Functions...
  virtual void GetValue(const char * const name, ccspwrapper::Tr181Scalar& value);
  virtual void SetValue(const char * const name, const ccspwrapper::Tr181Scalar& value);
  virtual bool Validate();
  virtual void Commit();
  virtual void Rollback();

private:

  //for now, simply proxy the data channel sub config collection to us...
  ::ccspwrapper::ProxyTr181SubCollectionProvider  _networkDataChannelSubCollectionProvider;
  ::ccspwrapper::ProxyTr181ConfigProvider         _networkDataChannelConfigProvider;

  bool     _networkInstanciated;
  bool     _dataChannelInstanciated;
  bool     _enabled;
  unsigned _primaryChannelApIndex;
  unsigned _dataChannelApIndex;

};


#endif //#ifndef SINGLENETWORK_H_INCLUDED
