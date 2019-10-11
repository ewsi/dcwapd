#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <dcwproto.h>

namespace dcw {

struct Message : public ::dcwmsg {
  Message();
  Message(const enum dcwmsg_id mid);
  Message(const Message& rhv);

  void Marshall(const unsigned char * const buf, const unsigned size);
  unsigned Serialize(unsigned char * const buf, const unsigned size) const;
};

} // namespace dcw


#endif //#ifndef MESSAGE_H_INCLUDED
