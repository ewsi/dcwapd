#ifndef EVENTREACTOREXITSIGNALER_H_INCLUDED
#define EVENTREACTOREXITSIGNALER_H_INCLUDED

#include "./processsignalmanager.h"

namespace dcw {
struct EventReactor;
};


namespace dcwposix {

class EventReactorExitSignalHandler : public ::dcwposix::ProcessSignalManager::EventHandler {
  EventReactorExitSignalHandler(const EventReactorExitSignalHandler&); //no copy

  ProcessSignalManager&   _psm;
  ::dcw::EventReactor&    _er;

public:
  EventReactorExitSignalHandler(ProcessSignalManager& psm, ::dcw::EventReactor& er);
  virtual ~EventReactorExitSignalHandler();
  virtual void OnSignal(int signum);

};

}; //namespace dcwposix {

#endif //#ifndef EVENTREACTOREXITSIGNALER_H_INCLUDED
