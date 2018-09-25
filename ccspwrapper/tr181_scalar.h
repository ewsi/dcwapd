#ifndef CCSP_TR181_SCALAR_H_INCLUDED
#define CCSP_TR181_SCALAR_H_INCLUDED

#include <stdint.h>
#include <string>


namespace ccspwrapper {

struct Tr181Scalar {
  enum Tr181ScalarType {
    SCALAR_TYPE_ULONG,
    SCALAR_TYPE_INT,
    SCALAR_TYPE_BOOL,
    SCALAR_TYPE_STRING,
  } const Type;

  union {
    uint64_t      ulong_value;
    int           int_value;
    bool          bool_value;
  };
  std::string&    str_value;

  Tr181Scalar(const uint64_t val);
  Tr181Scalar(const int val);
  Tr181Scalar(const bool val);
  Tr181Scalar(std::string& val);

  Tr181Scalar& operator=(const uint64_t val);
  Tr181Scalar& operator=(const int val);
  Tr181Scalar& operator=(const bool val);
  Tr181Scalar& operator=(const std::string& val);
  Tr181Scalar& operator=(const char * const val);

  operator uint64_t() const;
  operator int() const;
  operator bool() const;
  operator const char *() const;
  operator std::string&() const;

private:
  Tr181Scalar(const Tr181Scalar&); //no copy (str_value)
protected:
  Tr181Scalar(const Tr181ScalarType type, std::string& str) : Type(type), str_value(str) {} //allow a derrived class to construct the constants...
};

}; //namespace ccspwrapper


#endif //#ifndef CCSP_TR181_SCALAR_H_INCLUDED
