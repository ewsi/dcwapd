#ifndef CCSP_TR181_TREE_TRACKER_H_INCLUDED
#define CCSP_TR181_TREE_TRACKER_H_INCLUDED


#include <list>
#include <map>



//
// the purpose of this class is to track the tree-relationship 
// of all the configuration node providers... essentially this
// is a cache of this component's CCSP/TR181 configuration tree
// (values are not cached, just the provider objects)
//
// this simplifies:
// . Dynamic table synchronization
// . Automatic disposal of providers
// . Cleanly deleting tables with sub-tables
// . Properly numbering/indexing nodes
//
//

namespace ccspwrapper {

struct Tr181ConfigProvider;
struct Tr181SubCollectionProvider;

class Tr181TreeTracker {

private:
  struct TrackedNode {
    typedef std::list<TrackedNode*> ChildList;

    Tr181ConfigProvider&                  ConfigProvider;  //THIS node... (redundant for map key)
    TrackedNode                   * const Parent;          //NULL for root
    Tr181SubCollectionProvider    * const Instanciator;    //NULL for root
    unsigned                        const Index;           //ignored for root
    ChildList                             Children;

    TrackedNode(
      Tr181ConfigProvider&               configProvider,
      TrackedNode                * const parent,
      Tr181SubCollectionProvider * const instanciator,
      const unsigned                     index
    ) :
      ConfigProvider(configProvider),
      Parent(parent),
      Instanciator(instanciator),
      Index(index)
    {}

    TrackedNode(const TrackedNode& rhv) :
      ConfigProvider(rhv.ConfigProvider),
      Parent(rhv.Parent),
      Instanciator(rhv.Instanciator),
      Index(rhv.Index)
      //note: dont care about copying the children as the copy constructor
      //      is only called when inserting a new node into the map
    {}
  };

  typedef std::map<const Tr181ConfigProvider *, TrackedNode> TrackedNodeMap;

  TrackedNodeMap _trackedNodes;
  TrackedNode&   _rootNode;

  void DisposeNode(TrackedNode& node, TrackedNode& parent);
  void DisposeChildren(TrackedNode& trackedNode, Tr181SubCollectionProvider * const collectionProviderFilter);
  bool IsChildIndexFree(const TrackedNode& parent, const Tr181SubCollectionProvider& collectionProvider, const unsigned index) const;

  static TrackedNode& InsertNewNode(TrackedNodeMap& trackedNodes, const TrackedNode& newNode);

public:
  Tr181TreeTracker(Tr181ConfigProvider& rootProvider);
  virtual ~Tr181TreeTracker();

  void Append(Tr181ConfigProvider& configProvider, Tr181ConfigProvider& parentConfigProvider, Tr181SubCollectionProvider& collectionProvider, const unsigned index);
  void DisposeNode(Tr181ConfigProvider& configProvider);
  void DisposeAllChildren(Tr181ConfigProvider& parent);
  void DisposeProvidedChildren(Tr181ConfigProvider& parent, Tr181SubCollectionProvider& collectionProvider);

  unsigned GetChildCount(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider) const;
  //see the comments in "tr181_tree_tracker.cxx" located in the 
  //"GetChildAtZeroIndex()" and "GetChildAtDataModelIndex()"
  //functions and UNDERSTAND the difference before using them!!!
  Tr181ConfigProvider& GetChildAtZeroIndex(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider, unsigned index) const;
  Tr181ConfigProvider& GetChildAtDataModelIndex(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider, const unsigned index) const;
  unsigned GetDataModelIndex(const Tr181ConfigProvider& configProvider) const;
  unsigned GetFreeChildIndex(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider) const;
  bool IsChildIndexFree(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider, const unsigned index) const;
};

};

#endif //#ifndef CCSP_TR181_TREE_TRACKER_H_INCLUDED
