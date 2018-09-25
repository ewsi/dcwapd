

#include "./message_bus.h"
#include "./ccsp_config.h"

#include <strings.h>

#include <exception>


#include "dcw/dcwlog.h"

namespace {
struct CCSPMessageBusException : public std::exception {
  virtual const char* what() const throw() {
    return "CSSP Message Bus Exception";
  }
};
};


using namespace ccspwrapper;


extern void *bus_handle;
MessageBus::MessageBus(const CcspConfig& cfg) {
  CCSP_Base_Func_CB cb;

  _busHandle = NULL;
  bzero(&cb, sizeof(cb));

  dcwlogdbgf("%s\n", "CCSP Initializing Message Bus...");


  if (CCSP_Message_Bus_Init(
    (char*)cfg.ApplySubsystemPrefix(cfg.ComponentId).c_str(), //XXX ComponentId vs ComponentName ?
    (char*)CCSP_MSG_BUS_CFG,
    &_busHandle,
    (CCSP_MESSAGE_BUS_MALLOC)&::Ansc_AllocateMemory_Callback,
    &::Ansc_FreeMemory_Callback
  ) != ANSC_STATUS_SUCCESS) {
    dcwlogerrf("%s\n", "CCSP_Message_Bus_Init() failed!");
    throw CCSPMessageBusException();
  }
  dcwlogdbgf("%s\n", "CCSP_Message_Bus_Init() OK");

  //this is some wicked evil right here...
  //when getting called upon with an addtable action, 
  //the "Notify_Table_Entry()" function in dslh_objro_access.c 
  //gets invoked after our "AddEntry()" callback returns a
  //non-NULL pointer...
  //the "Notify_Table_Entry()" function passes the global "bus_handle"
  //into the CcspBaseIf_setParameterValues() funcion causing a
  //spectactular crash... we work around this by setting the 
  //global "bus_handle" to the one obtains in this object...
  //shame on you rdk...
  bus_handle = _busHandle;

  //Note: these two ugly commented out lines were int the
  //      example from open rdk...
  //      i left them commented out as I currently do not
  //      see any reason why this uglyness is needed...
  //strcpy((char*)_busHandle, (char*)cfg.GetSubsystemPrefix().c_str());
  //CCSP_Msg_SleepInMilliSeconds(1000); // XXX why?

  //ccsp callbacks...
  cb.getParameterValues     = CcspCcMbi_GetParameterValues;
  cb.setParameterValues     = CcspCcMbi_SetParameterValues;
  cb.setCommit              = CcspCcMbi_SetCommit;
  cb.setParameterAttributes = CcspCcMbi_SetParameterAttributes;
  cb.getParameterAttributes = CcspCcMbi_GetParameterAttributes;
  cb.AddTblRow              = CcspCcMbi_AddTblRow;
  cb.DeleteTblRow           = CcspCcMbi_DeleteTblRow;
  cb.getParameterNames      = CcspCcMbi_GetParameterNames;
  cb.currentSessionIDSignal = CcspCcMbi_CurrentSessionIdSignal;

  //"us" callbacks...
  cb.initialize             = &MessageBus::_c_mbi_initialize;
  cb.finalize               = &MessageBus::_c_mbi_finalize;
  cb.busCheck               = &MessageBus::_c_mbi_buscheck;
  cb.freeResources          = &MessageBus::_c_mbi_freeResources;

  CcspBaseIf_SetCallback(_busHandle, &cb);

  if (CCSP_Message_Bus_Register_Path(
    _busHandle,
    cfg.DbusPath,
    &MessageBus::_c_ccspcomp_path_message_func,
    _busHandle
  ) != CCSP_Message_Bus_OK) {
    dcwlogerrf("%s\n", "CCSP_Message_Bus_Register_Path() failed!");
    throw CCSPMessageBusException();
  }
  dcwlogdbgf("%s\n", "CCSP_Message_Bus_Register_Path() OK");

  if (CcspBaseIf_Register_Event(
    _busHandle,
    0,
    "currentSessionIDSignal"
  ) != CCSP_Message_Bus_OK) {
    dcwlogerrf("%s\n", "CcspBaseIf_Register_Event() failed!");
    throw CCSPMessageBusException();
  }
  dcwlogdbgf("%s\n", "CcspBaseIf_Register_Event() OK");

  dcwloginfof("%s\n", "CCSP Message Bus Initialized Successfully");
}

MessageBus::~MessageBus() {
  //
}

ANSC_HANDLE MessageBus::GetBusHandle() const {
  return _busHandle;
}


int MessageBus::_c_mbi_initialize(void *userData) {
  return ANSC_STATUS_SUCCESS;
}

int MessageBus::_c_mbi_finalize(void *userData) {
  return ANSC_STATUS_SUCCESS;
}

int MessageBus::_c_mbi_buscheck(void *userData) {
  return ANSC_STATUS_SUCCESS;
}

int MessageBus::_c_mbi_freeResources(int priority, void *userData) {
  return ANSC_STATUS_SUCCESS;
}

DBusHandlerResult MessageBus::_c_ccspcomp_path_message_func(DBusConnection *conn, DBusMessage *msg, void *userData) {
  CCSP_MESSAGE_BUS_INFO *bus_info =(CCSP_MESSAGE_BUS_INFO *) userData;
  const char *interface = dbus_message_get_interface(msg);
  const char *method   = dbus_message_get_member(msg);
  DBusMessage *reply;

  reply = dbus_message_new_method_return (msg);
  if (reply == NULL) {
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  return CcspBaseIf_base_path_message_func (
    conn,
    msg,
    reply,
    interface,
    method,
    bus_info
  );
}


