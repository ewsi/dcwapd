
#include "./filetrafficfilterprofile.h"

#include <string.h>
#include <errno.h>

#include <exception>

namespace {
struct FOpenFailedException : public std::exception {
  const std::string _msg;
  std::string GenMsg(const char * const filename) {
    std::string msg;

    msg  = "fopen(";
    msg += filename;
    msg  = ") failed: ";
    msg += strerror(errno);
    
    return msg;
  }
  FOpenFailedException(const char * const filename) :
    _msg(GenMsg(filename)) {
    //
  }
  virtual ~FOpenFailedException() throw() {}
  virtual const char* what() const throw() {
    return _msg.c_str();
  }
};
};

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



