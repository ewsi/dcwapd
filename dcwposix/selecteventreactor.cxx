
#include "./selecteventreactor.h"

#include "dcw/dcwlog.h"

#include <cerrno>
#include <cstring>
#include <sys/select.h>
#include <unistd.h>

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#include <ctime>

#include <exception>

namespace {
struct IncompatiblePublisherException : public std::exception {
  virtual const char* what() const noexcept {
    return "Publisher not compatible with this event reactor";
  }
};
} // namespace

using namespace dcwposix;


SelectEventReactor::SelectEventReactor() :
  _nfds(0),
  _stopRequested(false) {
  //
}

SelectEventReactor::~SelectEventReactor() {
  //
}

void SelectEventReactor::Stop() {
  dcwloginfof("%s\n", "Event reactor received a request to stop");
  _stopRequested = true;
}

void SelectEventReactor::Run() {
  int rv;
  fd_set readfds;

  IOSubMap::const_iterator subi;
  IOPubSet::const_iterator pubi;

  dcwloginfof("%s\n", "Event reactor is now running");

  while ( !_stopRequested ) {
    //determing which file descriptors we need to monitor
    FD_ZERO(&readfds);
    for (subi = _ioSubs.begin(); subi != _ioSubs.end(); subi++) {
      for (pubi = subi->second.begin(); pubi != subi->second.end(); pubi++) {
        FD_SET((*pubi)->GetSelectableFd(), &readfds);
      }
    }

    // wait here until we got something to do...
    rv = select(_nfds, &readfds, NULL, NULL, NULL);
    if (rv == -1) {
      //something failed...
      if (errno == EINTR) {
        continue; //process was interrupted...
      }
      dcwlogerrf("Event reactor select() failed: %s\n", strerror(errno));
      sleep(1);
      continue;
    }
    if (rv == 0) {
      //timeout (XXX not implemented yet!)
      continue;
    }

    //got something to do...
    for (subi = _ioSubs.begin(); subi != _ioSubs.end(); subi++) {
      for (pubi = subi->second.begin(); pubi != subi->second.end(); pubi++) {
        if (FD_ISSET((*pubi)->GetSelectableFd(), &readfds)) {
          subi->first->OnIOReady(*(*pubi));
        }
      }
    }
  }
}

void SelectEventReactor::RegisterIOSubscriber(IOSubscriber& sub, EventReactor::IOProvider& pub) {

  dcwlogdbgf("Event Reactor got subscriber %p requesting IO events published from (%p)\n", &sub, &pub);
  SelectableIOProvider * const lpub = dynamic_cast<SelectableIOProvider *>(&pub);
  if (lpub == NULL) {
    throw IncompatiblePublisherException();
  }

  _ioSubs[&sub].insert(lpub);
  updateNfds();
}

void SelectEventReactor::UnegisterIOSubscriber(IOSubscriber& sub) {
  dcwlogdbgf("Event Reactor for a request to unsubscribe %p from all\n", &sub);
  _ioSubs.erase(&sub);
  updateNfds();
}

void SelectEventReactor::ArmTimer(Timer& timer, unsigned seconds) {
  //
}

void SelectEventReactor::DisarmTimer(Timer& timer) {
  //
}

void SelectEventReactor::SleepSec(const unsigned seconds) const {
  sleep(seconds);
}

void SelectEventReactor::SleepMs(const unsigned milliseconds) const {
  struct timespec req = { 0, static_cast<long>(milliseconds) * 1000 * 1000};
  nanosleep(&req, NULL);
}

void SelectEventReactor::updateNfds() {
  _nfds = 0;
  for (IOSubMap::const_iterator subi = _ioSubs.begin(); subi != _ioSubs.end(); subi++) {
    for (IOPubSet::const_iterator pubi = subi->second.begin(); pubi != subi->second.end(); pubi++) {
      const int nfd = (*pubi)->GetSelectableFd() + 1;
      if (nfd > _nfds) _nfds = nfd;
    }
  }
}
