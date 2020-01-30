
#include "./eventreactorexitsignaler.h"
#include "dcw/eventreactor.h"

#include <csignal>

static const int _exitSignals[] = {
  SIGINT,
  SIGQUIT,
  SIGTERM,
};

using namespace dcwposix;

EventReactorExitSignalHandler::EventReactorExitSignalHandler(ProcessSignalManager& psm, ::dcw::EventReactor& er) :
  _psm(psm),
  _er(er) {
  for (int _exitSignal : _exitSignals) {
    _psm.RegisterEventHandler(_exitSignal, *this);
  }
}

EventReactorExitSignalHandler::~EventReactorExitSignalHandler() {
  for (int _exitSignal : _exitSignals) {
    _psm.UnRegisterEventHandler(_exitSignal, *this);
  }
}

void EventReactorExitSignalHandler::OnSignal(int signum) {
  _er.Stop();
}
