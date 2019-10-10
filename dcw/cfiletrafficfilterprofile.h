#ifndef CFILETRAFFICFILTERPROFILE_H_INCLUDED
#define CFILETRAFFICFILTERPROFILE_H_INCLUDED

#include "./trafficfilterprofile.h"

#include <stdio.h>

namespace dcw {

class CFileTrafficFilterProfile : public TrafficFilterProfile {
public:
  explicit CFileTrafficFilterProfile(const char * const name);
  virtual ~CFileTrafficFilterProfile();
  CFileTrafficFilterProfile(const CFileTrafficFilterProfile& rhv); //no reason this can't be copied...

  virtual FILE *fopen() const = 0;
};

class AutoCFTFP {
  AutoCFTFP(const AutoCFTFP&); //no copy

  FILE * const _openFile;
public:
  explicit AutoCFTFP(const CFileTrafficFilterProfile& cftfp);
  virtual ~AutoCFTFP();

  operator ::FILE*() const;
};

}; //namespace dcw


#endif // #ifndef CFILETRAFFICFILTERPROFILE_H_INCLUDED
