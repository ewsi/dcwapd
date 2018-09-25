

#include "./ccsp_callbacks.h"

#include <callback.h>


//note: since RDK-B/CCSP defines its own types, here are some ffcall macros
//      to abstract this a bit...

#define va_start_BOOL                  va_start_uchar
#define va_return_BOOL                 va_return_uchar
#define va_start_ULONG                 va_start_ulong
#define va_return_ULONG                va_return_ulong
#define va_start_ANSC_HANDLE(L)        va_start_ptr(L, ANSC_HANDLE*)
#define va_return_ANSC_HANDLE(L, VAL)  va_return_ptr(L, ANSC_HANDLE*, VAL)

#define va_arg_ULONG                   va_arg_ulong
#define va_arg_BOOL                    va_arg_uchar



using namespace ccspwrapper;


void CcspCbCosaInit::OnFfCallback(FfCallback& caller, va_alist alist) {
  //declare the variables needed for the inner call...
  ULONG uMaxVersionSupported;
  void *hCosaPlugInfo;

  //start parsing the parameter list...
  va_start_int(alist); //return type is int, therfore use "va_start_int"

  //read in each parameter from the passed in parameter list
  uMaxVersionSupported  = va_arg_ulong(alist);
  hCosaPlugInfo         = va_arg_ptr(alist, void*);

  //finalize things...
  va_return_int(alist, //return type is int, therfore use "va_return_int"

    //invoke the actual target function...
    this->COSAInit(caller, uMaxVersionSupported, hCosaPlugInfo)
  ); 
}

void CcspCbGetParamUlongValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  ULONG *value;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  value   = va_arg_ptr(alist, ULONG*);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->GetParamUlongValue(caller, ctx, name, value)
  );
}

void CcspCbSetParamUlongValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  ULONG value;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  value   = va_arg_ULONG(alist);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->SetParamUlongValue(caller, ctx, name, value)
  );
}

void CcspCbGetParamIntValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  int *value;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  value   = va_arg_ptr(alist, int*);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->GetParamIntValue(caller, ctx, name, value)
  );
}

void CcspCbSetParamIntValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  int value;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  value   = va_arg_int(alist);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->SetParamIntValue(caller, ctx, name, value)
  );
}

void CcspCbGetParamBoolValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  BOOL *value;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  value   = va_arg_ptr(alist, BOOL*);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->GetParamBoolValue(caller, ctx, name, value)
  );
}

void CcspCbSetParamBoolValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  BOOL value;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  value   = va_arg_BOOL(alist);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->SetParamBoolValue(caller, ctx, name, value)
  );
}

void CcspCbGetParamStringValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  char *value;
  ULONG *valueSize;

  //start parsing the parameter list...
  va_start_ULONG(alist); //return type is ULONG, therfore use "va_start_ULONG"

  //read in each parameter from the passed in parameter list
  ctx        = va_arg_ptr(alist, ANSC_HANDLE);
  name       = va_arg_ptr(alist, char*);
  value      = va_arg_ptr(alist, char*);
  valueSize  = va_arg_ptr(alist, ULONG*);

  //finalize things...
  va_return_ULONG(alist,

    //invoke the actual target function...
    this->GetParamStringValue(caller, ctx, name, value, valueSize)
  );
}

void CcspCbSetParamStringValue::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  char *value;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  value   = va_arg_ptr(alist, char*);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->SetParamStringValue(caller, ctx, name, value)
  );
}

void CcspCbValidate::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  char *name;
  ULONG *length;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  name    = va_arg_ptr(alist, char*);
  length  = va_arg_ptr(alist, ULONG*);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->Validate(caller, ctx, name, length)
  );
}

void CcspCbCommit::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;

  //start parsing the parameter list...
  va_start_ULONG(alist); //return type is ULONG, therfore use "va_start_ULONG"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);

  //finalize things...
  va_return_ULONG(alist,

    //invoke the actual target function...
    this->Commit(caller, ctx)
  );
}

void CcspCbRollback::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;

  //start parsing the parameter list...
  va_start_ULONG(alist); //return type is ULONG, therfore use "va_start_ULONG"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);

  //finalize things...
  va_return_ULONG(alist,

    //invoke the actual target function...
    this->Rollback(caller, ctx)
  );
}

void CcspCbGetEntryCount::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;

  //start parsing the parameter list...
  va_start_ULONG(alist); //return type is ULONG, therfore use "va_start_ULONG"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);

  //finalize things...
  va_return_ULONG(alist,

    //invoke the actual target function...
    this->GetEntryCount(caller, ctx)
  );
}

void CcspCbGetEntry::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  ULONG nIndex;
  ULONG *pInsNum;

  //start parsing the parameter list...
  va_start_ANSC_HANDLE(alist); //return type is ANSC_HANDLE, therfore use "va_start_ANSC_HANDLE"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);
  nIndex  = va_arg_ULONG(alist);
  pInsNum = va_arg_ptr(alist, ULONG *);

  //finalize things...
  va_return_ANSC_HANDLE(alist,

    //invoke the actual target function...
    this->GetEntry(caller, ctx, nIndex, pInsNum)
  );
}

void CcspCbAddEntry::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  PULONG pInsNumber;

  //start parsing the parameter list...
  va_start_ANSC_HANDLE(alist); //return type is ANSC_HANDLE, therfore use "va_start_ANSC_HANDLE"

  //read in each parameter from the passed in parameter list
  ctx         = va_arg_ptr(alist, ANSC_HANDLE);
  pInsNumber  = va_arg_ptr(alist, PULONG);

  //finalize things...
  va_return_ANSC_HANDLE(alist,

    //invoke the actual target function...
    this->AddEntry(caller, ctx, pInsNumber)
  );
}

void CcspCbDelEntry::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;
  ANSC_HANDLE hInstance;

  //start parsing the parameter list...
  va_start_ULONG(alist); //return type is ULONG, therfore use "va_start_ULONG"

  //read in each parameter from the passed in parameter list
  ctx       = va_arg_ptr(alist, ANSC_HANDLE);
  hInstance = va_arg_ptr(alist, ANSC_HANDLE);

  //finalize things...
  va_return_ULONG(alist,

    //invoke the actual target function...
    this->DelEntry(caller, ctx, hInstance)
  );
}

void CcspCbIsUpdated::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;

  //start parsing the parameter list...
  va_start_BOOL(alist); //return type is BOOL, therfore use "va_start_BOOL"

  //read in each parameter from the passed in parameter list
  ctx     = va_arg_ptr(alist, ANSC_HANDLE);

  //finalize things...
  va_return_BOOL(alist,

    //invoke the actual target function...
    this->IsUpdated(caller, ctx)
  );
}

void CcspCbSynchronize::OnFfCallback(FfCallback& caller, va_alist alist) {
  ANSC_HANDLE ctx;

  //start parsing the parameter list...
  va_start_ULONG(alist); //return type is ULONG, therfore use "va_start_ULONG"

  //read in each parameter from the passed in parameter list
  ctx       = va_arg_ptr(alist, ANSC_HANDLE);

  //finalize things...
  va_return_ULONG(alist,

    //invoke the actual target function...
    this->Synchronize(caller, ctx)
  );
}

