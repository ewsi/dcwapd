#ifndef EVENTREACTOREXITSIGNALER_H_INCLUDED
#define EVENTREACTOREXITSIGNALER_H_INCLUDED

#include "./processsignalmanager.h"

namespace dcw {
struct EventReactor;
} // namespace dcw


namespace dcwposix {

class EventReactorExitSignalHandler : public ::dcwposix::ProcessSignalManager::EventHandler {
  EventReactorExitSignalHandler(const EventReactorExitSignalHandler&); //no copy

  ProcessSignalManager&   _psm;
  ::dcw::EventReactor&    _er;

public:
  EventReactorExitSignalHandler(ProcessSignalManager& psm, ::dcw::EventReactor& er);
  ~EventReactorExitSignalHandler() override;
  void OnSignal(int signum) override;

};

} // namespace dcwposix

#endif //#ifndef EVENTREACTOREXITSIGNALER_H_INCLUDED
