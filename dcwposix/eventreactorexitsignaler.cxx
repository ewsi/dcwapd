
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
  for (unsigned i = 0; i < (sizeof(_exitSignals) / sizeof(_exitSignals[0])); i++) {
    _psm.RegisterEventHandler(_exitSignals[i], *this);
  }
}

EventReactorExitSignalHandler::~EventReactorExitSignalHandler() {
  for (unsigned i = 0; i < (sizeof(_exitSignals) / sizeof(_exitSignals[0])); i++) {
    _psm.UnRegisterEventHandler(_exitSignals[i], *this);
  }
}

void EventReactorExitSignalHandler::OnSignal(int signum) {
  _er.Stop();
}
