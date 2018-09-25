

#include "./tr181_scalar.h"

#include <exception>


using namespace ccspwrapper;

namespace {
struct ScalarTypeMismatchException : public std::exception {
  virtual const char* what() const throw() {
    return "Scalar Type Mismatch Exception";
  }
};
};


Tr181Scalar::Tr181Scalar(const uint64_t val) : 
  Type(SCALAR_TYPE_ULONG), ulong_value(val),
  str_value(*(std::string*)NULL) {
  //
}

Tr181Scalar::Tr181Scalar(const int val) : 
  Type(SCALAR_TYPE_INT), int_value(val),
  str_value(*(std::string*)NULL) {
  //
}

Tr181Scalar::Tr181Scalar(const bool val) :
  Type(SCALAR_TYPE_BOOL), bool_value(val),
  str_value(*(std::string*)NULL) {
  //
}

Tr181Scalar::Tr181Scalar(std::string& val) :
  Type(SCALAR_TYPE_STRING), str_value(val) {
  //
}


Tr181Scalar& Tr181Scalar::operator=(const uint64_t val) {
  switch (Type) {
  case SCALAR_TYPE_ULONG:
    ulong_value = val;
    break;
  case SCALAR_TYPE_INT:
    int_value = (int)val;
    break;
  case SCALAR_TYPE_BOOL:
    bool_value = (val != 0);
    break;
  default:
    throw ScalarTypeMismatchException();
  }
  return *this;
}

Tr181Scalar& Tr181Scalar::operator=(const int val) {
  switch (Type) {
  case SCALAR_TYPE_ULONG:
    ulong_value = (uint64_t)val;
    break;
  case SCALAR_TYPE_INT:
    int_value = val;
    break;
  case SCALAR_TYPE_BOOL:
    bool_value = (val != 0);
    break;
  default:
    throw ScalarTypeMismatchException();
  }
  return *this;
}

Tr181Scalar& Tr181Scalar::operator=(const bool val) {
  switch (Type) {
  case SCALAR_TYPE_BOOL:
    bool_value = val;
    break;
  case SCALAR_TYPE_ULONG:
    ulong_value = val ? 1 : 0;
    break;
  case SCALAR_TYPE_INT:
    int_value = val ? 1 : 0;
    break;
  default:
    throw ScalarTypeMismatchException();
  }
  return *this;
}

Tr181Scalar& Tr181Scalar::operator=(const std::string& val) {
  switch (Type) {
  case SCALAR_TYPE_STRING:
    str_value = val;
    break;
  default:
    throw ScalarTypeMismatchException();
  }
  return *this;
}

Tr181Scalar& Tr181Scalar::operator=(const char * const val) {
  switch (Type) {
  case SCALAR_TYPE_STRING:
    str_value = val;
    break;
  default:
    throw ScalarTypeMismatchException();
  }
  return *this;
}



Tr181Scalar::operator uint64_t() const {
  switch (Type) {
  case SCALAR_TYPE_ULONG:
    return ulong_value;
  case SCALAR_TYPE_INT:
    return (uint64_t)int_value;
  case SCALAR_TYPE_BOOL:
    return bool_value ? 1 : 0;
  default: break;
  }
  throw ScalarTypeMismatchException();
}

Tr181Scalar::operator int() const {
  switch (Type) {
  case SCALAR_TYPE_ULONG:
    return (uint64_t)int_value;
  case SCALAR_TYPE_INT:
    return int_value;
  case SCALAR_TYPE_BOOL:
    return bool_value ? 1 : 0;
  default: break;
  }
  throw ScalarTypeMismatchException();
}

Tr181Scalar::operator bool() const {
  switch (Type) {
  case SCALAR_TYPE_ULONG:
    return ulong_value != 0;
  case SCALAR_TYPE_INT:
    return int_value != 0;
  case SCALAR_TYPE_BOOL:
    return bool_value;
  default: break;
  }
  throw ScalarTypeMismatchException();
}

Tr181Scalar::operator const char *() const {
  if (Type != SCALAR_TYPE_STRING)
    throw ScalarTypeMismatchException();
  return str_value.c_str();
}

Tr181Scalar::operator std::string&() const {
  if (Type != SCALAR_TYPE_STRING)
    throw ScalarTypeMismatchException();
  return str_value;
}


