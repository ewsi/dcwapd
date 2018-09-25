

#include "./ff_callback.h"

#include "dcw/dcwlog.h"

#include <callback.h>

#include <exception>


namespace {
struct CCSPFfCallbackAllocException : public std::exception {
  virtual const char* what() const throw() {
    return "CSSP FF Callback Allocation Exception";
  }
};
};


using namespace ccspwrapper;


FfCallback::FfCallback(Callable& callable) :
  _callable(callable),
  _ffTramp((void*)alloc_callback(&_c_ffCallback, this)) {

  if (_ffTramp == NULL) {
    throw CCSPFfCallbackAllocException();
  }
}

FfCallback::~FfCallback() {
  free_callback((callback_t)_ffTramp);
}

void *FfCallback::GetTramp() const {
  return _ffTramp;
}

void FfCallback::_c_ffCallback(void *data, va_alist alist) {
  // called from libccsp calling the libffcall trampoline...

  //recover the "this" pointer...
  FfCallback& inst = *((FfCallback*)data);

  //fire the callback up the inheritance chain...
  try {
    inst._callable.OnFfCallback(inst, alist);
  }
  catch (std::exception& e) {
    dcwlogerrf("Blocking exception from crossing trampoline: %s\n", e.what());
  }
  catch (...) {
    dcwlogerrf("%s\n", "Blocking unknown exception from crossing trampoline");
  }
}


