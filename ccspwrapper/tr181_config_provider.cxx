
#include "./tr181_config_provider.h"



using namespace ccspwrapper;


Tr181Exception::Tr181Exception() {
  //
}

Tr181Exception::~Tr181Exception() throw() {
  //
}

const char* Tr181Exception::what() const throw() {
  return "TR-181 Exception";
}

Tr181ConfigProvider::~Tr181ConfigProvider() {
  //
}

void Tr181ConfigProvider::GetSubCollections(Tr181CollectionRegistry& registry) {
  //default behavior is add no subcollection... (do nothing here)
}

void Tr181ConfigProvider::SetValue(const char * const name, const Tr181Scalar& value) {
  throw Tr181Exception(); //default behavior is not writable...
}

bool Tr181ConfigProvider::Validate() {
  return true; //default behavior is no validation...
}

void Tr181ConfigProvider::Commit() {
  //
}

void Tr181ConfigProvider::Rollback() {
  //
}







ProxyTr181ConfigProvider::ProxyTr181ConfigProvider(Tr181ConfigProvider& target) : 
  Target(target) {
  //
}

ProxyTr181ConfigProvider::~ProxyTr181ConfigProvider() {
  //
}

void ProxyTr181ConfigProvider::GetSubCollections(Tr181CollectionRegistry& registry) {
  Target.GetSubCollections(registry);
}

void ProxyTr181ConfigProvider::GetValue(const char * const name, Tr181Scalar& value) {
  Target.GetValue(name, value);
}

void ProxyTr181ConfigProvider::SetValue(const char * const name, const Tr181Scalar& value) {
  Target.SetValue(name, value);
}

bool ProxyTr181ConfigProvider::Validate() {
  return Target.Validate();
}

void ProxyTr181ConfigProvider::Commit() {
  Target.Commit();
}

void ProxyTr181ConfigProvider::Rollback() {
  Target.Rollback();
}
