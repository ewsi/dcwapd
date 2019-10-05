
#include "./filetrafficfilterprofile.h"

#include <cerrno>
#include <cstring>
#include <exception>

namespace {
struct FOpenFailedException : public std::exception {
  const std::string _msg;
  static std::string GenMsg(const char * const filename) {
    std::string msg;

    msg  = "fopen(";
    msg += filename;
    msg  = ") failed: ";
    msg += std::strerror(errno);
    
    return msg;
  }
  explicit FOpenFailedException(const char * const filename) :
    _msg(GenMsg(filename)) {
    //
  }
  virtual ~FOpenFailedException() noexcept = default;
  virtual const char* what() const noexcept {
    return _msg.c_str();
  }
};
} // namespace

using namespace dcw;

FileTrafficFilterProfile::FileTrafficFilterProfile(const char * const name, const char * const filename) :
  CFileTrafficFilterProfile(name),
  _filename(filename) {
  //
}

FileTrafficFilterProfile::~FileTrafficFilterProfile() {
  //
}

FileTrafficFilterProfile::FileTrafficFilterProfile(const FileTrafficFilterProfile& rhv) :
  CFileTrafficFilterProfile(rhv),
  _filename(rhv._filename) {
  //
}

const char *FileTrafficFilterProfile::GetFilename() const {
  return _filename.c_str();
}

FILE *FileTrafficFilterProfile::fopen() const {
  FILE * const result = ::fopen(GetFilename(), "r");
  if (result == NULL) throw FOpenFailedException(GetFilename());
  return result;
}



