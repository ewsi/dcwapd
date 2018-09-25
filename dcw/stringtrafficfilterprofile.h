#ifndef STRINGTRAFFICFILTERPROFILE_H_INCLUDED
#define STRINGTRAFFICFILTERPROFILE_H_INCLUDED


#include "./cfiletrafficfilterprofile.h"

namespace dcw {

class StringTrafficFilterProfile : public CFileTrafficFilterProfile {
public:
  StringTrafficFilterProfile(const char * const name, const char * const contents);
  virtual ~StringTrafficFilterProfile();
  StringTrafficFilterProfile(const StringTrafficFilterProfile& rhv); //no reason this cant be copied

  const char *GetContents() const;
  virtual FILE *fopen() const;

private:
  const std::string _contents;
};

}; //namespace dcw


#endif // #ifndef STRINGTRAFFICFILTERPROFILE_H_INCLUDED
