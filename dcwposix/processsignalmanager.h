#ifndef PROCESSSIGNALMANAGER_H_INCLUDED
#define PROCESSSIGNALMANAGER_H_INCLUDED

#include <map>
#include <set>

#include <signal.h>


namespace dcwposix {

class ProcessSignalManager {
  ProcessSignalManager(const ProcessSignalManager&); //nocopy

public:
  struct EventHandler {
    virtual ~EventHandler() {}
    virtual void OnSignal(int signum) = 0;
  };

  ProcessSignalManager();
  virtual ~ProcessSignalManager();

  void RegisterEventHandler(const int signum, ::dcwposix::ProcessSignalManager::EventHandler& eventHandler);
  void UnRegisterEventHandler(const int signum, ::dcwposix::ProcessSignalManager::EventHandler& eventHandler);

private:
  typedef ::std::set<dcwposix::ProcessSignalManager::EventHandler *> SignalHandlerSet;
  typedef ::std::map<int, SignalHandlerSet> SignalMap;
  typedef ::std::map<int, sighandler_t> SignalMapPreserve;

  static void OnSignal(int signum);

  SignalMap          _sigmap;
  SignalMapPreserve  _sigmapPreserve;
};

} // namespace dcwposix

#endif //#ifndef PROCESSSIGNALMANAGER_H_INCLUDED
