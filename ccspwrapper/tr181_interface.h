#ifndef CCSP_TR181_INTERFACE_H_INCLUDED
#define CCSP_TR181_INTERFACE_H_INCLUDED


#include "./ccsp_include.h"
#include "./ccsp_callbacks.h"
#include "./tr181_collection_registry.h"
#include "./tr181_tree_tracker.h"

#include <map>
#include <set>
#include <string>

namespace ccspwrapper {

struct Tr181ConfigProvider;
struct Tr181SubCollectionProvider;

class Tr181Interface : 
  private Tr181CollectionRegistry,
  private CcspCbAll {
public:
  Tr181Interface(Tr181ConfigProvider& rootProvider);
  virtual ~Tr181Interface();

  ANSC_HANDLE GetTr181InterfaceHandle() const;

private:
  Tr181Interface(const Tr181Interface&); //no copy


  typedef std::map<std::string, Tr181SubCollectionProvider *> SubCollectionsMap;
  typedef std::set<FfCallback*> CallbackSet;

  Tr181ConfigProvider&   _rootProvider;
  SubCollectionsMap      _subCollections;
  CallbackSet            _callbacks;
  Tr181TreeTracker       _treeTracker; //note: this is essentially where the "synchronized" cache of the tables are kept...

  DSLH_LCB_INTERFACE _dslhLcbIf;

  PCOSA_PLUGIN_INFO _declareSubCollectionPluginInfo;

  virtual void DeclareSubCollectionProvider(const char * const objectName, Tr181SubCollectionProvider& provider);
  void *TrackCallbackTramp(FfCallback *newCb);


  //C++ implementation of the CCSP callbacks...
  virtual int COSAInit(FfCallback& caller, ULONG uMaxVersionSupported, void *hCosaPlugInfo);

  virtual BOOL GetParamUlongValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG *value);
  virtual BOOL SetParamUlongValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG value);
  virtual BOOL GetParamIntValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, int *value);
  virtual BOOL SetParamIntValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, int value);
  virtual BOOL GetParamBoolValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, BOOL *value);
  virtual BOOL SetParamBoolValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, BOOL value);
  virtual ULONG GetParamStringValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, char *value, ULONG *valueSize);
  virtual BOOL SetParamStringValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, char *value);

  virtual BOOL Validate(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG *length);
  virtual ULONG Commit(FfCallback& caller, ANSC_HANDLE ctx);
  virtual ULONG Rollback(FfCallback& caller, ANSC_HANDLE ctx);

  virtual ULONG GetEntryCount(FfCallback& caller, ANSC_HANDLE ctx);
  virtual ANSC_HANDLE GetEntry(FfCallback& caller, ANSC_HANDLE ctx, ULONG nIndex, ULONG *pInsNum);
  virtual ANSC_HANDLE AddEntry(FfCallback& caller, ANSC_HANDLE ctx, PULONG pInsNumber);
  virtual ULONG DelEntry(FfCallback& caller, ANSC_HANDLE ctx, ANSC_HANDLE hInstance);
  virtual BOOL IsUpdated(FfCallback& caller, ANSC_HANDLE ctx);
  virtual ULONG Synchronize(FfCallback& caller, ANSC_HANDLE ctx);
};

}; //namespace ccspwrapper


#endif //#ifndef CCSP_TR181_INTERFACE_H_INCLUDED
