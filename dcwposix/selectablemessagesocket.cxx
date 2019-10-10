

#include "./selectablemessagesocket.h"
#include "dcw/message.h"
#include "dcw/macaddress.h"

#include "dcw/dcwlog.h"

#include <dcwsocket.h>


#include <exception>

namespace {
struct SelectableMessageSocketOpenException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to open selectable DCW message socket";
  }
};
struct MessageSocketRecvException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to recv on socket messag socket";
  }
};
struct MessageSocketSendException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to send on socket messag socket";
  }
};
};

using namespace dcwposix;


SelectableMessageSocket::SelectableMessageSocket(const char * const interfaceName) : 
  _sock(dcwsock_open(interfaceName)) {
  if (_sock == NULL) {
    dcwlogerrf("Failed while opening a selectable DCW socket on interface '%s'...\n", interfaceName);
    throw SelectableMessageSocketOpenException();
  }
  dcwlogdbgf("Successfully opened a selectable DCW socket on interface '%s' as FD# %d\n", interfaceName, this->GetSelectableFd());
}

SelectableMessageSocket::~SelectableMessageSocket() {
  dcwlogdbgf("Closing selectable DCW socket FD# %d\n", this->GetSelectableFd());
  dcwsock_close(_sock);
}

void SelectableMessageSocket::ReceiveMessage(dcw::MacAddress& source, dcw::Message& msg) {
  int  rv;
  unsigned char buf[2048];

  rv = dcwsock_recv(_sock, buf, sizeof(buf), source.Value);
  if (rv == -1) {
    throw MessageSocketRecvException();
  }
  if (rv == 0) {
    throw MessageSocketRecvException(); // discarded frame...
  }

  msg.Marshall(buf, static_cast<unsigned int>(rv));
}

void SelectableMessageSocket::TransmitMessage(const dcw::MacAddress& dest, const dcw::Message& msg) {
  unsigned char buf[2048];
  unsigned len;

  len = msg.Serialize(buf, sizeof(buf));
  if (dcwsock_send(_sock, buf, len, dest.Value) != static_cast<int>(len)) {
    throw MessageSocketSendException();
  }
}

void SelectableMessageSocket::GetSourceMacAddress(dcw::MacAddress& source) {
  dcwsock_get_macaddr(_sock, source.Value);
}

int SelectableMessageSocket::GetSelectableFd() const {
  return dcwsock_get_fd(_sock);
}

