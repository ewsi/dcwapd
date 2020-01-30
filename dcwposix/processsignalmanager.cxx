
#include "./processsignalmanager.h"


#include "dcw/dcwlog.h"

#include <exception>



namespace {
struct DuplicateManagerException : public std::exception {
  const char* what() const noexcept override {
    return "Only one process signal manager may be created!";
  }
};
} // namespace


using namespace dcwposix;


static const ProcessSignalManager *_sigman = NULL;

ProcessSignalManager::ProcessSignalManager() {
  if (_sigman != NULL) {
    throw DuplicateManagerException();
  }
  _sigman = this;
}

ProcessSignalManager::~ProcessSignalManager() {
  //restore all the preserved signals...
  //there should be none...
  for (SignalMapPreserve::const_iterator i = _sigmapPreserve.begin(); i != _sigmapPreserve.end(); i++) {
    dcwlogwarnf("Restoring preserved process signal #%d that was NOT unregistered before this objects deconstruction!\n", i->first);
    signal(i->first, i->second);
  }
  _sigman = NULL;
}

void ProcessSignalManager::RegisterEventHandler(const int signum, ::dcwposix::ProcessSignalManager::EventHandler& eventHandler) {
  auto i = _sigmap.find(signum);
  if (i == _sigmap.end()) {
    //be sure to preseve the old signal when inserting a new "unseen" signal
    _sigmap[signum].insert(&eventHandler);
    _sigmapPreserve[signum] = signal(signum, &dcwposix::ProcessSignalManager::OnSignal);
    dcwlogdbgf("Event handler %p registered for new process signal #%d \n", &eventHandler, signum);
    return;
  }
  i->second.insert(&eventHandler);
  dcwlogdbgf("Event handler %p registered for process signal #%d \n", &eventHandler, signum);
}

void ProcessSignalManager::UnRegisterEventHandler(const int signum, ::dcwposix::ProcessSignalManager::EventHandler& eventHandler) {
  auto i = _sigmap.find(signum);

  if (i == _sigmap.end()) {
    dcwlogwarnf("Attempting to unregister handler %p non-registered process signal #%d\n", &eventHandler, signum);
    return;
  }

  if (i->second.find(&eventHandler) == i->second.end()) {
    dcwlogwarnf("Attempting to unregister non-registered handler %p for process signal #%d\n", &eventHandler, signum);
    return;
  }
  if (i->second.size() == 1) {
    //removing last handler for this signal... restore the preserved old signal first...
    signal(signum, _sigmapPreserve[signum]);
    _sigmapPreserve.erase(signum);
    _sigmap.erase(signum);
    dcwlogdbgf("Event handler %p unregistered and signal restored for process signal #%d \n", &eventHandler, signum);
    return;
  }

  i->second.erase(&eventHandler);
  dcwlogdbgf("Event handler %p unregistered for process signal #%d \n", &eventHandler, signum);
}

void ProcessSignalManager::OnSignal(int signum) {

  if (_sigman == NULL) {
    //defensive... should never get here...
    dcwlogerrf("%s\n", "No signal manager instance!");
    return;
  }
  auto i = _sigman->_sigmap.find(signum);

  if (i == _sigman->_sigmap.end()) {
    //defensive... should never get here...
    dcwlogerrf("Got a non-registered signal #%d\n", signum);
    return;
  }

  //fire off this signal to each registered handler...
  for (auto handlerIter : i->second) {
    handlerIter->OnSignal(signum);
  }
}

