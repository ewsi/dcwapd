#ifndef SELECTABLEMESSAGESOCKET_H_INCLUDED
#define SELECTABLEMESSAGESOCKET_H_INCLUDED

#include "dcw/messagesocket.h"
#include "./selecteventreactor.h"

extern "C" {
  typedef struct dcw_socket * dcw_socket_t;
};

namespace dcwposix {

class SelectableMessageSocket : public dcw::MessageSocket, public SelectEventReactor::SelectableIOProvider {
public:
  explicit SelectableMessageSocket(const char * const interfaceName);
  virtual ~SelectableMessageSocket();

  virtual void ReceiveMessage(dcw::MacAddress& source, dcw::Message& msg);
  virtual void TransmitMessage(const dcw::MacAddress& dest, const dcw::Message& msg);
  virtual void GetSourceMacAddress(dcw::MacAddress& source);

  virtual int GetSelectableFd() const;

private:
  SelectableMessageSocket(SelectableMessageSocket& rhv); //no copy

  dcw_socket_t _sock;
};

}; //namespace dcwposix {

#endif //#ifndef SELECTABLEMESSAGESOCKET_H_INCLUDED
