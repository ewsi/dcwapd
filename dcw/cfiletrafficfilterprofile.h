#ifndef CFILETRAFFICFILTERPROFILE_H_INCLUDED
#define CFILETRAFFICFILTERPROFILE_H_INCLUDED

#include "./trafficfilterprofile.h"

#include <cstdio>

namespace dcw {

class CFileTrafficFilterProfile : public TrafficFilterProfile {
public:
  explicit CFileTrafficFilterProfile(const char * const name);
  virtual ~CFileTrafficFilterProfile();
  CFileTrafficFilterProfile(const CFileTrafficFilterProfile& rhv); //no reason this can't be copied...

  virtual std::FILE *fopen() const = 0;
};

class AutoCFTFP {
  AutoCFTFP(const AutoCFTFP&); //no copy

  std::FILE * const _openFile;
public:
  explicit AutoCFTFP(const CFileTrafficFilterProfile& cftfp);
  virtual ~AutoCFTFP();

  operator std::FILE*() const;
};

} // namespace dcw


#endif // #ifndef CFILETRAFFICFILTERPROFILE_H_INCLUDED
