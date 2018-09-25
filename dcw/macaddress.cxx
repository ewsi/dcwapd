

#include "./macaddress.h"

#include <stdio.h>
#include <string.h>

#include <exception>


namespace {
struct MacAddressParseException : public std::exception {
  virtual const char* what() const throw() {
    return "MAC Address Parse Exception";
  }
};
};

using namespace dcw;


MacAddress::MacAddress() {
  memset(this->Value, 0xFF, sizeof(this->Value));
}

MacAddress::MacAddress(const MacAddress& rhv) {
  memcpy(this->Value, rhv.Value, sizeof(this->Value));
}

MacAddress::MacAddress(const char * const macStr) {
  unsigned octets[6];

  if (sscanf(macStr, "%X-%X-%X-%X-%X-%X",
             &octets[0], &octets[1],&octets[2],
             &octets[3],&octets[4],&octets[5]) != 6) {
    // failed to parse with dashes... try colons...
    if (sscanf(macStr, "%X:%X:%X:%X:%X:%X",
               &octets[0], &octets[1],&octets[2],
               &octets[3],&octets[4],&octets[5]) != 6) {
      // failed to parse... throw exception...
      throw MacAddressParseException();
    }
  }

  for (int i = 0; i < 6; i++)
    this->Value[i] = (unsigned char)octets[i];
}

MacAddress::MacAddress(const unsigned char * const value) {
  memcpy(this->Value, value, sizeof(this->Value));
}

MacAddress::~MacAddress() {
  //
}

bool MacAddress::operator<(const MacAddress& rhv) const {
  return memcmp(Value, rhv.Value, sizeof(Value)) < 0;
}

bool MacAddress::operator==(const MacAddress& rhv) const {
  if (this == &rhv) return true;
  return memcmp(Value, rhv.Value, sizeof(Value)) == 0;
}

std::string MacAddress::ToString() const {
  char macStr[24];

  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
    (unsigned)this->Value[0],
    (unsigned)this->Value[1],
    (unsigned)this->Value[2],
    (unsigned)this->Value[3],
    (unsigned)this->Value[4],
    (unsigned)this->Value[5]
  );
  return std::string(macStr);
}


