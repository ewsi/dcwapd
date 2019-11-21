#ifndef MACADDRESS_H_INCLUDED
#define MACADDRESS_H_INCLUDED

#include <string>

namespace dcw {

struct MacAddress {

  unsigned char Value[6];

  MacAddress();                                   //initialize as broadcast
  MacAddress(const MacAddress& rhv);              //copy
  explicit MacAddress(const char * const macStr); //parse string from macStr
  MacAddress(const unsigned char * const value);  //6-byte value
  virtual ~MacAddress();

  bool operator<(const MacAddress& rhv) const;
  bool operator==(const MacAddress& rhv) const;

  std::string ToString() const;
};

} // namespace dcw


#endif //#ifndef MACADDRESS_H_INCLUDED
