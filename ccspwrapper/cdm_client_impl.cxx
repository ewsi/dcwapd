

#include "./cdm_client_impl.h"

namespace {
struct MultipleInstanceException : public std::exception {
  virtual const char* what() const throw() {
    return "CdmClient can't be instanciated more than once";
  }
};
struct CdmOperationFailedException : public std::exception {
  virtual const char* what() const throw() {
    return "CDM operation failed";
  }
};

struct CdmParamDisposeOnReturn {
  DmParamVal_t * const _ptr;
  CdmParamDisposeOnReturn(DmParamVal_t * const ptr) : _ptr(ptr) {}
  ~CdmParamDisposeOnReturn() { Cdm_FreeParam(_ptr); }
};
};

static bool _instanciated = false;

using namespace ccspwrapper;

CdmClientImpl::CdmClientImpl(const char * const componentId, ANSC_HANDLE busHandle, const char * const subsystem) {
  if (_instanciated)
    throw MultipleInstanceException(); //this is so stupid...

  Cdm_Init(busHandle, subsystem, NULL, NULL, componentId);
  _instanciated = true;
}

CdmClientImpl::~CdmClientImpl() {
  Cdm_Term();
  _instanciated = false;
}

CdmClient::CopyScalar CdmClientImpl::GetValue(const char * const path) const {
  DmParamVal_t *val;

  //yuck... dynamic memory allocation...
  if (Cdm_GetParam(path, &val) != CCSP_SUCCESS)
    throw CdmOperationFailedException();

  //ensure that the dynamically allocated memory is automatically cleaned up on return or throw
  CdmParamDisposeOnReturn cpdor(val);

  switch (val->type) {
  case DM_PT_INT:    return (int)val->v_int;
  case DM_PT_ULONG:  return (uint64_t)val->v_ulong;
  case DM_PT_UINT:   return (uint64_t)val->v_uint;
  case DM_PT_BOOL:   return val->v_bool != 0;
  case DM_PT_STR: {
    std::string strval;
    strval.resize(val->len);
    memcpy(&strval[0], val->v_str, val->len); //the things i do for a proper null terminated string...
    return strval;
  }
  default:
    throw CdmOperationFailedException();
  }
  throw CdmOperationFailedException();
}

void CdmClientImpl::SetValue(const char * const path, const Tr181Scalar& value) {
  //enforce the type of the current value in the model
  //(this lets Tr181Scalar class sort out type conversion issues)
  CopyScalar csValue(GetValue(path));
  Tr181Scalar& typedValue(csValue); //hacky cause base assignment operators (=) are masked

  DmParamVal_t val; //mmm... stack allocation...

  switch (typedValue.Type) {
  case Tr181Scalar::SCALAR_TYPE_INT:
    typedValue = (int)value;
    val.type = DM_PT_INT;
    val.len = sizeof(val.v_int);
    val.v_int = (int)typedValue;
    break;
  case Tr181Scalar::SCALAR_TYPE_ULONG:
    typedValue = (uint64_t)value;
    val.type = DM_PT_ULONG;
    val.len = sizeof(val.v_ulong);
    val.v_ulong = (unsigned long)(uint64_t)typedValue;
    break;
  case Tr181Scalar::SCALAR_TYPE_BOOL:
    typedValue = (bool)value;
    val.type = DM_PT_BOOL;
    val.len = sizeof(val.v_bool);
    val.v_bool = (bool)typedValue;
    break;
  case Tr181Scalar::SCALAR_TYPE_STRING: {
    typedValue = (std::string&)value;
    const std::string& strval = typedValue;
    val.type = DM_PT_STR;
    val.len = strval.length();
    val.v_str = (char*)strval.c_str();
    break;
  }
  default:
    throw CdmOperationFailedException();
  }

  if (Cdm_SetParam(path, &val, 1) != CCSP_SUCCESS) //1 = commit?
    throw CdmOperationFailedException();
}




