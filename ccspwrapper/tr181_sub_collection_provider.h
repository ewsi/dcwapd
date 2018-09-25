#ifndef TR181_SUB_COLLECTION_PROVIDER_H_INCLUDED
#define TR181_SUB_COLLECTION_PROVIDER_H_INCLUDED


#include <list>

namespace ccspwrapper {

struct Tr181ConfigProvider;
struct Tr181SubCollectionProvider {
public:
  typedef std::list<Tr181ConfigProvider *> ConfigProviderCollection;

  virtual ~Tr181SubCollectionProvider();

  //
  // PopulateConfigProviderCollection() 
  //
  // Called when the configuration model is
  // asking us for an ordered list of all
  // the current elements
  //
  // (Note: The returned objects must be valid until they are given to DisposeConfigProvider())
  //
  virtual void PopulateConfigProviderCollection(Tr181ConfigProvider& parent, ConfigProviderCollection& collection) = 0;

  //
  // PopulateConfigProviderCollection() 
  //
  // Called when the configuration model is
  // done with a previous instance we gave it
  //
  // This does not necessarily mean we need to
  // free() this object, it just means that the
  // configuration model is done using it which
  // guarantees no calls to its members will 
  // occur after this point
  //
  virtual void DisposeConfigProvider(Tr181ConfigProvider * const provider);

  //
  // RequestAddNewProviderElement()
  //
  // Called when the configuration model is
  // asking us for a new element
  //
  // (Note: The returned object must be valid until it is given to DisposeConfigProvider())
  //
  virtual Tr181ConfigProvider *RequestAddNewProviderElement(Tr181ConfigProvider& parent);

  // RequestInstanciateNewProviderElement()
  //
  // Called when the configuration model is
  // asking us to delete an element returned
  // by PopulateConfigProviderCollection()
  //
  // (Note: The given object instance must remain valid until it is later given to DisposeConfigProvider())
  //
  virtual void RequestDeleteConfigProviderElement(Tr181ConfigProvider& provider);
};


//
//
// This class implements a default delete on dispose
//
//
struct AutoDeleteTr181SubCollectionProvider : public Tr181SubCollectionProvider {
  virtual ~AutoDeleteTr181SubCollectionProvider();
  virtual void DisposeConfigProvider(Tr181ConfigProvider * const provider);
};


struct ProxyTr181SubCollectionProvider : public Tr181SubCollectionProvider {
  Tr181SubCollectionProvider& Target;

  ProxyTr181SubCollectionProvider(Tr181SubCollectionProvider& target);
  virtual ~ProxyTr181SubCollectionProvider();

  virtual void PopulateConfigProviderCollection(Tr181ConfigProvider& parent, ConfigProviderCollection& collection);
  virtual void DisposeConfigProvider(Tr181ConfigProvider * const provider);
  virtual Tr181ConfigProvider *RequestAddNewProviderElement(Tr181ConfigProvider& parent);
  virtual void RequestDeleteConfigProviderElement(Tr181ConfigProvider& provider);
};

}; //namespace ccspwrapper


#endif //#ifndef TR181_SUB_COLLECTION_PROVIDER_H_INCLUDED
