#ifndef CCSP_MESSAGE_BUS_H_INCLUDED
#define CCSP_MESSAGE_BUS_H_INCLUDED


#include "./ccsp_include.h"

namespace ccspwrapper {

class CcspConfig;
class MessageBus {
public:
  MessageBus(const CcspConfig& cfg);
  virtual ~MessageBus();

  ANSC_HANDLE GetBusHandle() const;

private:
  MessageBus(const MessageBus&); //no copy

  ANSC_HANDLE _busHandle;

  static int _c_mbi_initialize(void *userData);
  static int _c_mbi_finalize(void *userData);
  static int _c_mbi_buscheck(void *userData);
  static int _c_mbi_freeResources(int priority, void *userData);
  static DBusHandlerResult _c_ccspcomp_path_message_func(DBusConnection *conn, DBusMessage *msg, void *userData);

};

} //namespace ccspwrapper

#endif //#ifndef CCSP_MESSAGE_BUS_H_INCLUDED
