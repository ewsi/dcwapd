#ifndef TR181_CONFIG_PROVIDER_H_INCLUDED
#define TR181_CONFIG_PROVIDER_H_INCLUDED


#include <exception>

#include "./tr181_scalar.h"


namespace ccspwrapper {

struct Tr181Exception : public std::exception {
  Tr181Exception();
  virtual ~Tr181Exception() throw();
  virtual const char* what() const throw();
};


struct Tr181CollectionRegistry;
struct Tr181ConfigProvider {
  virtual ~Tr181ConfigProvider();

  virtual void GetSubCollections(Tr181CollectionRegistry& registry);
  
  virtual void GetValue(const char * const name, Tr181Scalar& value) = 0;
  virtual void SetValue(const char * const name, const Tr181Scalar& value);

  virtual bool Validate();
  virtual void Commit();
  virtual void Rollback();
};

struct ProxyTr181ConfigProvider : public Tr181ConfigProvider {
  Tr181ConfigProvider& Target;

  ProxyTr181ConfigProvider(Tr181ConfigProvider& target);
  virtual ~ProxyTr181ConfigProvider();

  virtual void GetSubCollections(Tr181CollectionRegistry& registry);
  
  virtual void GetValue(const char * const name, Tr181Scalar& value);
  virtual void SetValue(const char * const name, const Tr181Scalar& value);

  virtual bool Validate();
  virtual void Commit();
  virtual void Rollback();
};

}; //namespace ccspwrapper


#endif //#ifndef TR181_CONFIG_PROVIDER_H_INCLUDED
