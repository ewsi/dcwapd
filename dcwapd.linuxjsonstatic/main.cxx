
#include <cstring>

#include "dcwposix/processsignalmanager.h"
#include "dcwposix/eventreactorexitsignaler.h"
#include "dcwposix/selecteventreactor.h"
#include "dcwposix/filterdirscanner.h"

#include "dcwlinux/macremapper_driver.h"
#include "dcwlinux/ap_configuration.h"
#include "dcwlinux/vap_manager.h"
#include "dcwlinux/json_configuration_provider.h"

#include "dcw/dcwlog.h"

#include <exception>

int
main( void ) {

  try {
    dcwlinux::JsonConfigurationProvider      configProvider("./dcwapdconf.json");

    dcwposix::ProcessSignalManager           sigman;
    dcwposix::SelectEventReactor             eventReactor;
    dcwposix::EventReactorExitSignalHandler  eventReactorCleanExit(sigman, eventReactor);

    dcwlinux::MacRemapperDriver              driver;
    dcwlinux::APConfiguration                conf(configProvider);
    dcwlinux::VAPManager                     vapman;

    conf.Apply(driver, vapman, eventReactor);

    driver.Dump();

    eventReactor.Run();
  }
  catch (std::exception& e) {
    dcwlogerrf("Caught main() exception: %s\n", e.what());
    return 1;
  }
  return 0;
}

