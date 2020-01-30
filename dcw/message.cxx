
#include "./message.h"

#include <cstring>

#include <exception>


namespace {
struct MessageMarshallException : public std::exception {
  const char* what() const noexcept override {
    return "Failed to marshall DCW message from buffer!";
  }
};
struct MessageSerializeException : public std::exception {
  const char* what() const noexcept override {
    return "Failed to serialize DCW message from buffer!";
  }
};
} // namespace


using namespace dcw;

Message::Message() {
  //
}

Message::Message(const enum dcwmsg_id mid) {
  this->id = mid;
}

Message::Message(const Message& rhv) {
  const struct dcwmsg *rhv_m = &rhv;
  struct dcwmsg *lhv_m = this;

  std::memcpy(lhv_m, rhv_m, sizeof(struct dcwmsg));
}


void Message::Marshall(const unsigned char * const buf, const unsigned size) {
  if (!dcwmsg_marshal(this, buf, size)) {
    throw MessageMarshallException();
  }
}

unsigned Message::Serialize(unsigned char * const buf, const unsigned size) const {
  unsigned rv = 0;

  rv = dcwmsg_serialize(buf, this, size);
  if (rv == 0) {
    throw MessageSerializeException();
  }

  return rv;
}




