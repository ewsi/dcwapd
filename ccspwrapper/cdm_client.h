#ifndef CDM_CLIENT_H_INCLUDED
#define CDM_CLIENT_H_INCLUDED

#include "./tr181_scalar.h"

#include <string>

//note: the actual implementation of this class can be
//      found in "cdm_client_impl.h"
//      its split like this to prevent pollution from
//      "ccsp_include.h" to contaminate the rest
//      of the program...

namespace ccspwrapper {

struct CdmClient {

  //a "copyable" version of Tr181Scalar...
  //not great but works for now...
  //shouldnt prohibit from fixing later...
  //this should only be used for reading
  //from so we dont care about the masked
  //assignment operators...
  struct CopyScalar : public Tr181Scalar {
    std::string _strStorage;
    inline CopyScalar(const CopyScalar& rhv) : Tr181Scalar(rhv.Type, _strStorage) {
      switch (Type) {
      case SCALAR_TYPE_ULONG:   ulong_value  = rhv.ulong_value;  break;
      case SCALAR_TYPE_INT:     int_value    = rhv.int_value;    break;
      case SCALAR_TYPE_BOOL:    bool_value   = rhv.bool_value;   break;
      case SCALAR_TYPE_STRING:  str_value    = rhv.str_value;    break;
      }
    }
    inline CopyScalar(const uint64_t val) : Tr181Scalar(val) {}
    inline CopyScalar(const int val) : Tr181Scalar(val) {}
    inline CopyScalar(const bool val) : Tr181Scalar(val) {}
    inline CopyScalar(const std::string& val) : Tr181Scalar(_strStorage) { _strStorage = val; }
    inline CopyScalar(const char * const val) : Tr181Scalar(_strStorage) { _strStorage = val; }
  };


  virtual ~CdmClient();

  virtual CopyScalar GetValue(const char * const path) const = 0;
  virtual void SetValue(const char * const path, const Tr181Scalar& value) = 0;

  struct CdmParam {
    const std::string _path;
    CdmClient&        _client;

    inline CdmParam(const char * path, CdmClient& client) : _path(path), _client(client) {}
    inline CdmParam(const CdmParam& rhv) : _path(rhv._path), _client(rhv._client) {}

    //setter...
    inline CdmParam& operator=(const CopyScalar& value) { _client.SetValue(_path.c_str(), value); return *this; }
    inline CdmParam& operator=(const CdmParam& cp) { _client.SetValue(_path.c_str(), (CopyScalar)cp.GetValue()); return *this; }

    //getters...
    inline CopyScalar GetValue() const { return _client.GetValue(_path.c_str()); }
    inline operator uint64_t() const { return (uint64_t)GetValue(); }
    inline operator int() const { return (int)GetValue(); }
    inline operator bool() const { return (bool)GetValue(); }
    inline operator std::string() const { return (std::string&)GetValue(); }
  };

  inline CdmParam Param(const char * const path) { return CdmParam(path, *this); }
  inline CdmParam operator[](const char * const path) { return Param(path); }

  CdmParam Paramf(const char * const pathf, ...);

};

} //namespace ccspwrapper



#endif //#ifndef CDM_CLIENT_H_INCLUDED
