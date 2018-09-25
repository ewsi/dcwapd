

#include "./tr181_tree_tracker.h"

#include "./tr181_sub_collection_provider.h"

#include <stddef.h>
#include <exception>

namespace {
struct NodeAlreadyExistsException : public std::exception {
  virtual const char* what() const throw() {
    return "Node Already Exists Exception";
  }
};
struct ParentNodeNotFoundException : public std::exception {
  virtual const char* what() const throw() {
    return "Parent Node Not Found Exception";
  }
};
struct NodeNotFoundException : public std::exception {
  virtual const char* what() const throw() {
    return "Node Not Found Exception";
  }
};
struct CantDisposeRootNodeException : public std::exception {
  virtual const char* what() const throw() {
    return "Can't Dispose Root Node Exception";
  }
};
struct ChildIndexBoundaryException : public std::exception {
  virtual const char* what() const throw() {
    return "Child Index Boundary Exception";
  }
};
struct IndexUnavailableException : public std::exception {
  virtual const char* what() const throw() {
    return "Index Unavailable Exception";
  }
};
};

using namespace ccspwrapper;


Tr181TreeTracker::TrackedNode& Tr181TreeTracker::InsertNewNode(TrackedNodeMap& trackedNodes, const TrackedNode& newNode) {

  //a "single-shot" function to insert a new node
  //into the map and return a reference to the
  //newly inserted node

  const std::pair<TrackedNodeMap::iterator, bool> result =
    trackedNodes.insert(
      std::pair<Tr181ConfigProvider*, TrackedNode>(
        &newNode.ConfigProvider,
        newNode
      )
    )
  ;

  if (!result.second) {
    //element &rootProvider already existed...
    throw NodeAlreadyExistsException();
  }

  return result.first->second;
}


Tr181TreeTracker::Tr181TreeTracker(Tr181ConfigProvider& rootProvider) :
  _trackedNodes(),
  _rootNode(InsertNewNode(_trackedNodes, TrackedNode(rootProvider, NULL, NULL, 0))) {
  //
}

Tr181TreeTracker::~Tr181TreeTracker() {
  //
}


void Tr181TreeTracker::Append(Tr181ConfigProvider& configProvider, Tr181ConfigProvider& parentConfigProvider, Tr181SubCollectionProvider& collectionProvider, const unsigned index) {
  if (_trackedNodes.find(&configProvider) != _trackedNodes.end())
    throw NodeAlreadyExistsException();

  const TrackedNodeMap::iterator parentIter = _trackedNodes.find(&parentConfigProvider);
  if (parentIter == _trackedNodes.end())
    throw ParentNodeNotFoundException();

  if (!IsChildIndexFree(parentIter->second, collectionProvider, index))
    throw IndexUnavailableException();
 
  TrackedNode& node =
    InsertNewNode(_trackedNodes, TrackedNode(configProvider, &parentIter->second, &collectionProvider, index));

  parentIter->second.Children.push_back(&node);
}

void Tr181TreeTracker::DisposeNode(TrackedNode& node, TrackedNode& parent) {
  //dispose of any children...
  DisposeChildren(node, NULL);

  //inform the instanciator of the disposal...
  if (node.Instanciator != NULL)
    node.Instanciator->DisposeConfigProvider(&node.ConfigProvider);

  // IMPORTANT: At this point, it is assumed that the reference 
  //            "node->ConfigProvider" is no longer refering to
  //            a valid instance...


  //remove it from the parent...
  for (TrackedNode::ChildList::iterator i = parent.Children.begin(); i != parent.Children.end(); i++) {
    if ((*i) == (&node)) {
      parent.Children.erase(i);
      // IMPORTANT: "i" is invalid at this point so its imerative to bail out of the loop here!
      break;
    }
  }

  //remove it from the tracked nodes map...
  _trackedNodes.erase(&node.ConfigProvider);

  // IMPORTANT: At this point, the reference "node" is
  //            no longer refering to a valid instance
}

void Tr181TreeTracker::DisposeNode(Tr181ConfigProvider& configProvider) {

  if ((&configProvider) == (&_rootNode.ConfigProvider))
    throw CantDisposeRootNodeException();

  const TrackedNodeMap::iterator i = _trackedNodes.find(&configProvider);
  if (i == _trackedNodes.end())
    throw NodeNotFoundException();

  if (i->second.Parent == NULL)
    throw ParentNodeNotFoundException(); //something is REALLY wrong here...
  const TrackedNodeMap::iterator parentIter = _trackedNodes.find(&i->second.Parent->ConfigProvider);
  if (parentIter == _trackedNodes.end())
    throw ParentNodeNotFoundException(); //something is REALLY wrong here...

  DisposeNode(i->second, parentIter->second);
}

void Tr181TreeTracker::DisposeChildren(TrackedNode& trackedNode, Tr181SubCollectionProvider * const collectionProviderFilter) {


  //note: the iteration is done in a way which
  //      allows the deletion of elements as
  //      we traverse the list...
  TrackedNode::ChildList::const_iterator i, nexti;
  for (i = trackedNode.Children.begin(); i != trackedNode.Children.end();) {
    //get the next value of "i" in advance in case its removed from the list
    nexti = i;
    ++nexti;

    //dispose if matches...
    if (collectionProviderFilter == NULL) {
      DisposeNode(*(*i), trackedNode);
    }
    else if ((*i)->Instanciator == collectionProviderFilter) {
      DisposeNode(*(*i), trackedNode);
    }

    i = nexti;
  }
}

void Tr181TreeTracker::DisposeAllChildren(Tr181ConfigProvider& parent) {
  const TrackedNodeMap::iterator i = _trackedNodes.find(&parent);
  if (i == _trackedNodes.end())
    throw NodeNotFoundException();

  DisposeChildren(i->second, NULL);
}

void Tr181TreeTracker::DisposeProvidedChildren(Tr181ConfigProvider& parent, Tr181SubCollectionProvider& collectionProvider) {
  const TrackedNodeMap::iterator i = _trackedNodes.find(&parent);
  if (i == _trackedNodes.end())
    throw NodeNotFoundException();

  DisposeChildren(i->second, &collectionProvider);
}



unsigned Tr181TreeTracker::GetChildCount(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider) const {
  const TrackedNodeMap::const_iterator parentIter = _trackedNodes.find(&parentProvider);
  if (parentIter == _trackedNodes.end())
    throw NodeNotFoundException();

  unsigned rv = 0;

  for (TrackedNode::ChildList::const_iterator i = parentIter->second.Children.begin(); i != parentIter->second.Children.end(); i++) {
    if ((*i)->Instanciator == &collectionProvider) rv++;
  }

  return rv;
}

Tr181ConfigProvider& Tr181TreeTracker::GetChildAtZeroIndex(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider, unsigned index) const {

  //
  // IMPORTANT: the functions "GetChildAtZeroIndex()" and "GetChildAtDataModelIndex()"
  //            are similiar but have critically different purposes!
  //
  //            This function ("GetChildAtZeroIndex()") returns the reference
  //            to the child config provider instance from the given zero-based
  //            index provided to us. The elements are indexed based on the order
  //            in which "Append()" was called to insert these elements.
  //
  //            This function is useful for the case when something wants to
  //            iterate through all matching child elements.
  //
  //            Example "for-each child" pseudo-code:
  //              count = GetChildCount();
  //              for (i = 0; i < count; i++) {
  //                child = GetChildAtZeroIndex(i);
  //                //do something with child here...
  //              }
  //

  const TrackedNodeMap::const_iterator parentIter = _trackedNodes.find(&parentProvider);
  if (parentIter == _trackedNodes.end())
    throw NodeNotFoundException();

  for (TrackedNode::ChildList::const_iterator i = parentIter->second.Children.begin(); i != parentIter->second.Children.end(); i++) {
    //XXX yikes... this is truely a terrible way to index an element...
    //             should probably be using a std::map or something

    if ((*i)->Instanciator == &collectionProvider) {
      if ((index--) < 1) return (*i)->ConfigProvider;
    }
  }

  throw ChildIndexBoundaryException();
}

Tr181ConfigProvider& Tr181TreeTracker::GetChildAtDataModelIndex(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider, const unsigned index) const {
  //
  // IMPORTANT: the functions "GetChildAtZeroIndex()" and "GetChildAtDataModelIndex()"
  //            are similiar but have critically different purposes!
  //
  //            This function ("GetChildAtDataModelIndex()") returns the reference
  //            to the child config provider instance from the given data-model-based
  //            index provided to us. The elements are indexed based on the caller-
  //            provided index pased into the "Append()" call
  //
  //            This function is useful for element lookup/retrieval based on
  //            the data-model index where it sits
  //
  const TrackedNodeMap::const_iterator parentIter = _trackedNodes.find(&parentProvider);
  if (parentIter == _trackedNodes.end())
    throw NodeNotFoundException();

  for (TrackedNode::ChildList::const_iterator i = parentIter->second.Children.begin(); i != parentIter->second.Children.end(); i++) {
    //XXX yikes... this is truely a terrible way to index an element...
    //             should probably be using a std::map or something

    if ((*i)->Instanciator == &collectionProvider) {
      if ((*i)->Index == index) {
        return (*i)->ConfigProvider;
      }
    }
  }

  throw ChildIndexBoundaryException();
}

unsigned Tr181TreeTracker::GetDataModelIndex(const Tr181ConfigProvider& configProvider) const {
  const TrackedNodeMap::const_iterator i = _trackedNodes.find(&configProvider);
  if (i == _trackedNodes.end())
    throw NodeNotFoundException();

  return i->second.Index;
}

unsigned Tr181TreeTracker::GetFreeChildIndex(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider) const {
  const TrackedNodeMap::const_iterator parentIter = _trackedNodes.find(&parentProvider);
  if (parentIter == _trackedNodes.end())
    throw NodeNotFoundException();

  unsigned rv;
  for (rv = 1; !IsChildIndexFree(parentIter->second, collectionProvider, rv); ++rv);
  return rv;
}


bool Tr181TreeTracker::IsChildIndexFree(const TrackedNode& parent, const Tr181SubCollectionProvider& collectionProvider, const unsigned index) const {
  for (TrackedNode::ChildList::const_iterator i = parent.Children.begin(); i != parent.Children.end(); i++) {
    if ((*i)->Instanciator != &collectionProvider) continue;

    if ((*i)->Index == index) return false;
  }

  return true;
}

bool Tr181TreeTracker::IsChildIndexFree(const Tr181ConfigProvider& parentProvider, const Tr181SubCollectionProvider& collectionProvider, const unsigned index) const {
  const TrackedNodeMap::const_iterator parentIter = _trackedNodes.find(&parentProvider);
  if (parentIter == _trackedNodes.end())
    throw NodeNotFoundException();

  return IsChildIndexFree(parentIter->second, collectionProvider, index);
}

