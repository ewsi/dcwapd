#include "./tr181_sub_collection_provider.h"


#include "./tr181_config_provider.h"

using namespace ccspwrapper;


Tr181SubCollectionProvider::~Tr181SubCollectionProvider() {
  //
}

void Tr181SubCollectionProvider::DisposeConfigProvider(Tr181ConfigProvider * const provider) {
  //default is do nothing...
}

Tr181ConfigProvider *Tr181SubCollectionProvider::RequestAddNewProviderElement(Tr181ConfigProvider& parent) {
  throw Tr181Exception();
}

void Tr181SubCollectionProvider::RequestDeleteConfigProviderElement(Tr181ConfigProvider& provider) {
  throw Tr181Exception();
}

AutoDeleteTr181SubCollectionProvider::~AutoDeleteTr181SubCollectionProvider() {
  //
}

void AutoDeleteTr181SubCollectionProvider::DisposeConfigProvider(Tr181ConfigProvider * const provider) {
  delete provider;
}



ProxyTr181SubCollectionProvider::ProxyTr181SubCollectionProvider(Tr181SubCollectionProvider& target) :
  Target(target) {
  //
}

ProxyTr181SubCollectionProvider::~ProxyTr181SubCollectionProvider() {
  //
}

void ProxyTr181SubCollectionProvider::PopulateConfigProviderCollection(Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
  Target.PopulateConfigProviderCollection(parent, collection);
}

void ProxyTr181SubCollectionProvider::DisposeConfigProvider(Tr181ConfigProvider * const provider) {
  Target.DisposeConfigProvider(provider);
}

Tr181ConfigProvider * ProxyTr181SubCollectionProvider::RequestAddNewProviderElement(Tr181ConfigProvider& parent) {
  return Target.RequestAddNewProviderElement(parent);
}

void ProxyTr181SubCollectionProvider::RequestDeleteConfigProviderElement(Tr181ConfigProvider& provider) {
  Target.RequestDeleteConfigProviderElement(provider);
}

