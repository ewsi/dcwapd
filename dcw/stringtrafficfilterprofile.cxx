
#include "./stringtrafficfilterprofile.h"

#include <cerrno>
#include <cstring>
#include <exception>

namespace {
struct FMemOpenFailedException : public std::exception {
  const std::string _msg;
  static std::string GenMsg() {
    std::string msg;

    msg  = "fmemopen() failed: ";
    msg += std::strerror(errno);
    
    return msg;
  }
  FMemOpenFailedException() :
    _msg(GenMsg()) {
    //
  }
  virtual ~FMemOpenFailedException() noexcept = default;
  virtual const char* what() const noexcept {
    return _msg.c_str();
  }
};
} // namespace


using namespace dcw;



StringTrafficFilterProfile::StringTrafficFilterProfile(const char * const name, const char * const contents) :
  CFileTrafficFilterProfile(name),
  _contents(contents) {
  //
}

StringTrafficFilterProfile::~StringTrafficFilterProfile() {
  //
}

StringTrafficFilterProfile::StringTrafficFilterProfile(const StringTrafficFilterProfile& rhv) :
  CFileTrafficFilterProfile(rhv),
  _contents(rhv._contents) {
  //
}

const char *StringTrafficFilterProfile::GetContents() const {
  return _contents.c_str();
}

FILE *StringTrafficFilterProfile::fopen() const {
  FILE * const result = ::fmemopen(const_cast<char*>(_contents.c_str()), _contents.length(), "r");
  if (result == NULL) throw FMemOpenFailedException();
  return result;
}
