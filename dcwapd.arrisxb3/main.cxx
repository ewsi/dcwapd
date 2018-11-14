
#include <unistd.h>

#include "dcwposix/processsignalmanager.h"
#include "dcwposix/eventreactorexitsignaler.h"
#include "dcwposix/selecteventreactor.h"
#include "dcwposix/filterdirscanner.h"

#include "dcwlinux/macremapper_driver.h"
#include "dcwlinux/ap_configuration.h"
#include "dcwlinux/vap_manager.h"

#include "./tr181_configuration_provider.h"
#include "./arrisxb3_datachan_provisioner.h"

#include "dcw/dcwlog.h"

struct MainRunTask : public ::dcwposix::EventReactorExitSignalHandler {
  Tr181ConfigurationProvider&        _config;
  ::dcwposix::ProcessSignalManager&  _sigman;

  volatile bool                  _mustExit;
  volatile bool                  _reloadRequested;
  ::dcw::EventReactor * volatile _eventReactor;

  MainRunTask(Tr181ConfigurationProvider& config, ::dcwposix::ProcessSignalManager& sigman) :
    ::dcwposix::EventReactorExitSignalHandler(sigman, *(::dcw::EventReactor*)NULL), //this is totally cheating... just using the base to register the exit signals we want...
    _config(config),
    _sigman(sigman),
    _mustExit(false),
    _reloadRequested(false),
    _eventReactor(NULL) {

    _sigman.RegisterEventHandler(SIGHUP, *this);
  }
  ~MainRunTask() {
    _sigman.UnRegisterEventHandler(SIGHUP, *this);
  }
  virtual void OnSignal(int signum) {
    //WARNING: dont call the base function as it WILL CRASH!
    if (_eventReactor != NULL) _eventReactor->Stop();
    if (signum == SIGHUP) {
      _reloadRequested = true;
      dcwloginfof("%s\n", "Got signalled to refresh running configuration...");
    }
    if (signum != SIGHUP) {
      _mustExit = true;
      dcwloginfof("%s\n", "Got signalled to exit...");
    }
      
  }

  void RunLoop() {
    while (!_mustExit) {
      //create the event reactor...
      //XXX once Stop is called on this, it must be re-contrsuted to be used again...
      dcwposix::SelectEventReactor eventReactor;
      _eventReactor = &eventReactor;
      if (_reloadRequested || _mustExit) {
        //we just gont interruped sometime between the beginning of this
        //loop iteration and the creation of the event reactor...
        //the event reactor could of missed the Stop() event so lets
        //stop it just to make sure we dont miss anything...
        eventReactor.Stop();
      }

      //its finally time to start bringing up the DCW stuff...
      dcwlinux::MacRemapperDriver driver;
      try {
        dcwlinux::APConfiguration conf(_config);
        dcwlinux::VAPManager      vapman;

        //take the AP configuration and apply it into the VAP manager...
        //this instanciates the controller and friends...
        conf.Apply(driver, vapman, eventReactor);

        driver.Dump();

        //isolate the data-channel for the life of this object...
        ArrisXb3DatachanProvisioner provisioner(_config.Network, _config.Cdm);

        //route the telemetry events from the core dcw controller
        //to the tr181 model...
        vapman.SetAllTelemetryCollector(&_config.TelemetryCollector);

        dcwlogdbgf("%s\n", "Entering event reactor run()...");
        eventReactor.Run();
        dcwlogdbgf("%s\n", "Exited event reactor run()...");

        //cleanup all telemetry events... (defensive)
        _config.TelemetryCollector.Telemetry_Clear();

        //XXX should probably wipe the macremapper here before "provisioner"
        //XXX goes out of scope as there may be a few second "dead zone" !!
      }
      catch (std::exception& e) {
        dcwlogerrf("Something blew up while starting/running the DCW main loop: %s\n", e.what());
        sleep(2);
      }
      _reloadRequested = false;
      _eventReactor = NULL;
    }
  }
};

int
main( void ) {
  try {

    dcwloginfof("%s\n", "Registering our CCSP/TR-181 data-model providers...");
    Tr181ConfigurationProvider      config;

    dcwloginfof("%s\n", "CCSP/TR-181 is locked and loaded. Firing up the MRT...");
    dcwposix::ProcessSignalManager  sigman;
    MainRunTask                     mrt(config, sigman);
    mrt.RunLoop();
  }
  catch (std::exception& e) {
    dcwlogerrf("Caught main() exception: %s\n", e.what());
    return 1;
  }
  return 0;
}

