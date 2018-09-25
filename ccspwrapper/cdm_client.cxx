

#include "./cdm_client.h"

#include <stdio.h>
#include <stdarg.h>

using namespace ccspwrapper;


CdmClient::~CdmClient() {
  //
}

CdmClient::CdmParam CdmClient::Paramf(const char * const pathf, ...) {
  std::string path;
  va_list ap;

  //first get the size of the operation...
  va_start(ap, pathf);
  path.resize(vsnprintf(&path[0], path.length(), pathf, ap));
  va_end(ap);

  //then snprintf() into our string...
  va_start(ap, pathf);
  path.resize(vsnprintf(&path[0], path.length() + 1, pathf, ap)); //the resize() here should be a noop
                                                                  //+1 is because std::string does not
                                                                  //include the '\0' in length()
  va_end(ap);

  return Param(path.c_str());
}

