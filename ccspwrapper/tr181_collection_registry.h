#ifndef TR181_COLLECTION_REGISTRY_H_INCLUDED
#define TR181_COLLECTION_REGISTRY_H_INCLUDED


namespace ccspwrapper {

struct Tr181SubCollectionProvider;

struct Tr181CollectionRegistry {
  virtual ~Tr181CollectionRegistry() {}

  virtual void DeclareSubCollectionProvider(const char * const objectName, Tr181SubCollectionProvider& provider) = 0;
};

} // namespace ccspwrapper


#endif //#ifndef TR181_COLLECTION_REGISTRY_H_INCLUDED

