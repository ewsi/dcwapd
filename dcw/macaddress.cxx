

#include "./macaddress.h"

#include <cstdio>
#include <cstring>

#include <exception>


namespace {
struct MacAddressParseException : public std::exception {
  const char* what() const noexcept override {
    return "MAC Address Parse Exception";
  }
};
} // namespace

using namespace dcw;


MacAddress::MacAddress() {
  std::memset(this->Value, 0xFF, sizeof(this->Value));
}

MacAddress::MacAddress(const MacAddress& rhv) {
  std::memcpy(this->Value, rhv.Value, sizeof(this->Value));
}

MacAddress::MacAddress(const char * const macStr) {
  unsigned octets[6];

  if (std::sscanf(macStr, "%X-%X-%X-%X-%X-%X",
             &octets[0], &octets[1],&octets[2],
             &octets[3],&octets[4],&octets[5]) != 6) {
    // failed to parse with dashes... try colons...
    if (std::sscanf(macStr, "%X:%X:%X:%X:%X:%X",
               &octets[0], &octets[1],&octets[2],
               &octets[3],&octets[4],&octets[5]) != 6) {
      // failed to parse... throw exception...
      throw MacAddressParseException();
    }
  }

  for (int i = 0; i < 6; i++)
    this->Value[i] = static_cast<unsigned char>(octets[i]);
}

MacAddress::MacAddress(const unsigned char * const value) {
  std::memcpy(this->Value, value, sizeof(this->Value));
}

MacAddress::~MacAddress() {
  //
}

bool MacAddress::operator<(const MacAddress& rhv) const {
  return std::memcmp(Value, rhv.Value, sizeof(Value)) < 0;
}

bool MacAddress::operator==(const MacAddress& rhv) const {
  if (this == &rhv) return true;
  return std::memcmp(Value, rhv.Value, sizeof(Value)) == 0;
}

std::string MacAddress::ToString() const {
  char macStr[24];

  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
    static_cast<unsigned>(this->Value[0]),
    static_cast<unsigned>(this->Value[1]),
    static_cast<unsigned>(this->Value[2]),
    static_cast<unsigned>(this->Value[3]),
    static_cast<unsigned>(this->Value[4]),
    static_cast<unsigned>(this->Value[5])
  );
  return std::string(macStr);
}


