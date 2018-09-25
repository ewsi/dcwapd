
#include "./tr181_interface.h"

#include "./tr181_config_provider.h"
#include "./tr181_sub_collection_provider.h"


#include <string.h>
#include <strings.h>

#include <string>
#include <exception>

#include "dcw/dcwlog.h"


using namespace ccspwrapper;



namespace {
//
// this class essentially provides a "closure" for
// instanciating sub-object callbacks...
// 
// table callbacks such as "GetEntry()", "Synchronize()", etc
// down-cast the caller as an instance of
// SubCollectionCallback in order to invoke the 
// registered sub collection provider callback
//
class SubCollectionCallback : public FfCallback {
public:
  const std::string                                       ObjectName;
  Tr181SubCollectionProvider&                             Provider;

  SubCollectionCallback(Callable& callable, const char * const objectName, Tr181SubCollectionProvider& provider) : 
    FfCallback(callable),
    ObjectName(objectName),
    Provider(provider) {
    //
  }
  virtual ~SubCollectionCallback() {
    //
  }
};
};








Tr181Interface::Tr181Interface(Tr181ConfigProvider& rootProvider) 
  : _rootProvider(rootProvider),
    _treeTracker(rootProvider),
    _declareSubCollectionPluginInfo(NULL) {
  bzero(&_dslhLcbIf, sizeof(_dslhLcbIf));

  strncpy(_dslhLcbIf.Name, CCSP_LIBCBK_INTERFACE_NAME, sizeof(_dslhLcbIf.Name));

  _dslhLcbIf.InterfaceId = CCSP_LIBCBK_INTERFACE_ID;
  _dslhLcbIf.hOwnerContext = NULL;
  _dslhLcbIf.Size = sizeof(_dslhLcbIf);
  _dslhLcbIf.InitLibrary = (COSAInitProc)TrackCallbackTramp(new FfCallback(*(CcspCbCosaInit*)this));

  dcwlogdbgf("%s\n", "CCSP TR-181 Interface Initialized");
}

Tr181Interface::~Tr181Interface() {
  dcwlogdbgf("%s\n", "CCSP TR-181 Interface Destroyed");

  for(CallbackSet::const_iterator i = _callbacks.begin(); i != _callbacks.end(); i++) {
    delete *i;
  }

  //XXX release/dispose synchronized providers?
  //XXX skip for now as this probably gets destroyed 
  //XXX AFTER the provider is already gone
}

ANSC_HANDLE Tr181Interface::GetTr181InterfaceHandle() const {
  return (ANSC_HANDLE)&_dslhLcbIf;
}


#define REGISTER_SUB_FUNC(FUNC_TYPE) (*_declareSubCollectionPluginInfo->RegisterFunction)(_declareSubCollectionPluginInfo->hContext, (char*)(std::string(#FUNC_TYPE) + functionSuffix).c_str(), TrackCallbackTramp(new SubCollectionCallback(*(FUNC_TYPE*)this, objectName, provider)))
void Tr181Interface::DeclareSubCollectionProvider(const char * const objectName, Tr181SubCollectionProvider& provider) {
  _subCollections[objectName] = &provider;

  std::string functionSuffix = "_";
  functionSuffix += objectName;

  REGISTER_SUB_FUNC(CcspCbGetEntryCount);
  REGISTER_SUB_FUNC(CcspCbGetEntry);
  REGISTER_SUB_FUNC(CcspCbAddEntry);
  REGISTER_SUB_FUNC(CcspCbDelEntry);
  REGISTER_SUB_FUNC(CcspCbIsUpdated);
  REGISTER_SUB_FUNC(CcspCbSynchronize);
}

void *Tr181Interface::TrackCallbackTramp(FfCallback *newCb) {
  _callbacks.insert(newCb);
  return newCb->GetTramp();
}

#define THIS_PLUGIN_VERSION 1
#define REGISTER_THIS_FUNC(FUNC_TYPE) (*pPlugInfo->RegisterFunction)(pPlugInfo->hContext, (char*)#FUNC_TYPE, TrackCallbackTramp(new FfCallback(*(FUNC_TYPE*)this)))
int Tr181Interface::COSAInit(FfCallback& caller, ULONG uMaxVersionSupported, void *hCosaPlugInfo) {
  PCOSA_PLUGIN_INFO              pPlugInfo  = (PCOSA_PLUGIN_INFO)hCosaPlugInfo;

  if (uMaxVersionSupported < THIS_PLUGIN_VERSION) {
    return -1;
  }

  pPlugInfo->uPluginVersion = THIS_PLUGIN_VERSION;

  REGISTER_THIS_FUNC(CcspCbGetParamUlongValue);
  REGISTER_THIS_FUNC(CcspCbSetParamUlongValue);
  REGISTER_THIS_FUNC(CcspCbGetParamIntValue);
  REGISTER_THIS_FUNC(CcspCbSetParamIntValue);
  REGISTER_THIS_FUNC(CcspCbGetParamBoolValue);
  REGISTER_THIS_FUNC(CcspCbSetParamBoolValue);
  REGISTER_THIS_FUNC(CcspCbGetParamStringValue);
  REGISTER_THIS_FUNC(CcspCbSetParamStringValue);
  REGISTER_THIS_FUNC(CcspCbValidate);
  REGISTER_THIS_FUNC(CcspCbCommit);
  REGISTER_THIS_FUNC(CcspCbRollback);

  _declareSubCollectionPluginInfo = pPlugInfo;
  _rootProvider.GetSubCollections(*this);
  _declareSubCollectionPluginInfo = NULL;

  return 0; //success
}

BOOL Tr181Interface::GetParamUlongValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG *value) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    Tr181Scalar scalar((uint64_t)0);
    provider.GetValue(name, scalar);
    (*value) = scalar.ulong_value;
  }
  catch (std::exception& e) {
    return FALSE;
  }
  return TRUE;
}

BOOL Tr181Interface::SetParamUlongValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG value) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    Tr181Scalar scalar((uint64_t)value);
    provider.SetValue(name, scalar);
  }
  catch (std::exception& e) {
    return FALSE;
  }
  return TRUE;
}

BOOL Tr181Interface::GetParamIntValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, int *value) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    Tr181Scalar scalar((int)0);
    provider.GetValue(name, scalar);
    (*value) = scalar.int_value;
  }
  catch (std::exception& e) {
    return FALSE;
  }
  return TRUE;
}

BOOL Tr181Interface::SetParamIntValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, int value) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    Tr181Scalar scalar((int)value);
    provider.SetValue(name, scalar);
  }
  catch (std::exception& e) {
    return FALSE;
  }
  return TRUE;
}

BOOL Tr181Interface::GetParamBoolValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, BOOL *value) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    Tr181Scalar scalar((bool)false);
    provider.GetValue(name, scalar);
    (*value) = scalar.bool_value ? TRUE : FALSE;
  }
  catch (std::exception& e) {
    return FALSE;
  }
  return TRUE;
}

BOOL Tr181Interface::SetParamBoolValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, BOOL value) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    Tr181Scalar scalar((bool)((value == FALSE) ? false : true));
    provider.SetValue(name, scalar);
  }
  catch (std::exception& e) {
    return FALSE;
  }
  return TRUE;
}

ULONG Tr181Interface::GetParamStringValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, char *value, ULONG *valueSize) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    std::string val;
    Tr181Scalar scalar(val);
    provider.GetValue(name, scalar);

    if ((val.length() + 1) > (*valueSize)) {
      //provided buffer aint big enough...
      //tell the caller how much buffer we need...
      //then we should be called again...
      (*valueSize) = val.length() + 1;
      return 1; //need more buffer...
    }
    strncpy(value, val.c_str(), *valueSize);

  }
  catch (std::exception& e) {
    return ANSC_STATUS_FAILURE;
  }
  return ANSC_STATUS_SUCCESS;
}

BOOL Tr181Interface::SetParamStringValue(FfCallback& caller, ANSC_HANDLE ctx, char *name, char *value) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    std::string val(value);
    Tr181Scalar scalar(val);
    provider.SetValue(name, scalar);
  }
  catch (std::exception& e) {
    return FALSE;
  }
  return TRUE;
}

BOOL Tr181Interface::Validate(FfCallback& caller, ANSC_HANDLE ctx, char *name, ULONG *length) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    return provider.Validate() ? TRUE : FALSE;
  }
  catch (std::exception& e) {
    return FALSE;
  }
}

ULONG Tr181Interface::Commit(FfCallback& caller, ANSC_HANDLE ctx) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    provider.Commit();
  }
  catch (std::exception& e) {
    return ANSC_STATUS_FAILURE;
  }
  return ANSC_STATUS_SUCCESS;
}

ULONG Tr181Interface::Rollback(FfCallback& caller, ANSC_HANDLE ctx) {
  Tr181ConfigProvider& provider = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  try {
    provider.Rollback();
  }
  catch (std::exception& e) {
    return ANSC_STATUS_FAILURE;
  }
  return ANSC_STATUS_SUCCESS;
}

ULONG Tr181Interface::GetEntryCount(FfCallback& caller, ANSC_HANDLE ctx) {
  //
  // for an object type of "dynamicTable" this function
  // gets called by CCSP once on startup and after a
  // IsUpdated() call returns TRUE, and after a
  // "Synchronize()" call
  //
  // for an object type of "writableTable" this function
  // gets called by CCSP generally only once on startup
  //

  SubCollectionCallback& scc     = (SubCollectionCallback&)caller;
  Tr181ConfigProvider&   parent  = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;
  ULONG rv = _treeTracker.GetChildCount(parent, scc.Provider);

  if (rv == 0) {
    // WARNING: this is kinda hacky...
    // in the long run, should probably change the tree tracker
    // to have each node track which sub collection providers
    // have been sync'd...
    //
    // since "Synchronize()" never gets called for "writableTable"
    // object types, calling "GetChildCount()" will always return
    // 0 for a writable table since the "PopulateConfigProviderCollection()"
    // function only gets invoked from "Synchronize()"
    //
    // to work around this for now, we will synthesize a
    // "Synchronize()" call here... this will not de-sync
    // an already sync'd dynamic table as if the previous
    // count was 0, then there is nothing to go out of sync

    this->Synchronize(caller, ctx);

    // refresh the child count... this time if its zero then its zero for good...
    rv = _treeTracker.GetChildCount(parent, scc.Provider);
  }
  return rv;
}

ANSC_HANDLE Tr181Interface::GetEntry(FfCallback& caller, ANSC_HANDLE ctx, ULONG nIndex, ULONG *pInsNum) {
  //
  // for an object type of "dynamicTable" this function
  // gets called by CCSP once for each entry on startup
  // and after a IsUpdated() call returns TRUE, and
  // after a "Synchronize()" call
  // pesudo-code caller logic would look something like this:
  //   if (IsUpdated()) {
  //     Synchronize()
  //     count = GetEntryCount();
  //     for (nIndex = 0; nIndex < count; i++) {
  //       GetEntryCount(nIndex);
  //     }
  //   }
  // 
  //
  // for an object type of "writableTable" this function
  // gets called by CCSP generally only once for each
  // entry on startup
  // pesudo-code caller logic would look something like this:
  //   count = GetEntryCount();
  //   for (nIndex = 0; nIndex < count; i++) {
  //     GetEntryCount(nIndex);
  //   }
  //
  // since "nIndex" is simply a contigious value from 0 to 
  // (count - 1), the actual index in the data model need
  // to be specified by the "pInsNum" variable
  // pInsNum is an "output variable" to set which is the index
  // in the data model shown to the user for this entry element
  // 

  SubCollectionCallback& scc     = (SubCollectionCallback&)caller;
  Tr181ConfigProvider&   parent  = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  Tr181ConfigProvider *rv = NULL;

  try {
    //IMPORTANT: pay attention to the differences between
    //           "GetChildAtZeroIndex()" and "GetChildAtDataModelIndex()"
    rv = &_treeTracker.GetChildAtZeroIndex(parent, scc.Provider, nIndex);
    (*pInsNum) = _treeTracker.GetDataModelIndex(*rv);
  }
  catch (...) {} //rv = NULL on exception...

  return rv;
}

ANSC_HANDLE Tr181Interface::AddEntry(FfCallback& caller, ANSC_HANDLE ctx, PULONG pInsNumber) {
  //
  // for an object type of "dynamicTable" this function
  // gets called by CCSP when a user adds a table
  // 
  // example:
  //   $ dmcli eRT addtable Device.MyComponent.TableName.
  //
  SubCollectionCallback& scc     = (SubCollectionCallback&)caller;
  Tr181ConfigProvider&   parent  = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  Tr181ConfigProvider *rv = NULL;
  try {
    rv = scc.Provider.RequestAddNewProviderElement(parent);
    if (rv == NULL) throw 0;
    try {
      const unsigned newIndex = _treeTracker.GetFreeChildIndex(parent, scc.Provider);
      //XXX in the future should implement a mechanism to allow the
      //XXX provider to optionally specify the index if it so desires
      _treeTracker.Append(*rv, parent, scc.Provider, newIndex);
      (*pInsNumber) = newIndex;
    }
    catch (...) {
      scc.Provider.DisposeConfigProvider(rv);
      rv = NULL;
      throw;
    }
  }
  catch (...) {} //rv = NULL on exception

  return rv;
}

ULONG Tr181Interface::DelEntry(FfCallback& caller, ANSC_HANDLE ctx, ANSC_HANDLE hInstance) {
  //
  // for an object type of "dynamicTable" this function
  // gets called by CCSP when a user deletes a table
  // 
  // example:
  //   $ dmcli eRT deltable Device.MyComponent.TableName.
  //
  if (hInstance == NULL) return ANSC_STATUS_FAILURE;

  SubCollectionCallback& scc     = (SubCollectionCallback&)caller;
  Tr181ConfigProvider&   entry   = *(Tr181ConfigProvider*)hInstance;

  try {
    scc.Provider.RequestDeleteConfigProviderElement(entry);
    _treeTracker.DisposeNode(entry);
  }
  catch (...) {
    return ANSC_STATUS_FAILURE;
  }
    
  return ANSC_STATUS_SUCCESS;
}

BOOL Tr181Interface::IsUpdated(FfCallback& caller, ANSC_HANDLE ctx) {
  //AFAIK, this is only called for an object type of "dynamicTable"
  return TRUE; //returning tells the caller dont cache any previous returned values... ask every time...
}

ULONG Tr181Interface::Synchronize(FfCallback& caller, ANSC_HANDLE ctx) {
  //
  // for an object type of "dynamicTable" this function
  // gets called by CCSP once for each entry on startup
  // and after a IsUpdated() call returns TRUE
  //
  // the idea behind this is that the CCSP libraries
  // need the table's indexed items to remain consistent
  // until the caller is ready for a "refreshed" version
  // of the table. this function is responsible for
  // signalling to us that the caller is ready for us
  // to go ahead and modify the underlying providers
  //
  // calls to GetEntryCount() and GetEntry() will
  // occur after this
  //
  // note: this never gets called for an object type of "writableTable"
  //       therfore, "GetEntryCount()" may synthesize a call to this...
  //
  SubCollectionCallback&                                  scc       = (SubCollectionCallback&)caller;
  Tr181ConfigProvider&                                    parent    = (ctx != NULL) ? *(Tr181ConfigProvider*)ctx : _rootProvider;

  //first dispose of everything currently synchronized
  _treeTracker.DisposeProvidedChildren(parent, scc.Provider);

  //then request a refreshed copy of the synchronized provider list...
  try {
    //since we just disposed all children for this parent/provider combo,
    //there is no need to call GetFreeChildIndex()...
    //we can start with 1 and sequentially increment
    //
    //XXX in the future should implement a mechanism to allow the
    //XXX provider to optionally specify the index if it so desires
    //XXX right now, the provider has some control as the order
    //XXX is enforced
    unsigned index = 1;

    Tr181SubCollectionProvider::ConfigProviderCollection children;
    scc.Provider.PopulateConfigProviderCollection(parent, children);
    for (Tr181SubCollectionProvider::ConfigProviderCollection::const_iterator i = children.begin(); i != children.end(); i++) {
      _treeTracker.Append(*(*i), parent, scc.Provider, index++);
    }
  }
  catch (std::exception& e) {
    //XXX return what on failure... ?
  }

  return 0; //success?
}

