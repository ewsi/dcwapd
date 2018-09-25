#ifndef CCSP_INCLUDE_H_INCLUDED
#define CCSP_INCLUDE_H_INCLUDED

#include <time.h>
#include <stdint.h>

extern "C" {
#include <ccsp/ansc_platform.h>
#include <ccsp/slap_definitions.h>
#include <ccsp/ccsp_message_bus.h>
#include <ccsp/ccsp_base_api.h>
#include <ccsp/dslh_cpeco_interface.h>
#include <ccsp/dslh_cpeco_exported_api.h>
#include <ccsp/slap_vco_exported_api.h>
#include <ccsp/dslh_ifo_mpa.h>
#include <ccsp/dslh_dmagnt_interface.h>
#include <ccsp/dslh_dmagnt_exported_api.h>
#include <ccsp/ccsp_ifo_ccd.h>
#include <ccsp/ccc_ifo_mbi.h>
#include <ccsp/messagebus_interface_helper.h>
#include <ccsp/ccsp_custom.h>
#include <ccsp/ccsp_component_helper.h>
#include <ccsp/ccsp_dm_api.h>
#include <syscfg/syscfg.h>
}


#endif //#ifndef CCSP_INCLUDE_H_INCLUDED
