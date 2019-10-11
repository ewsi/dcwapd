#ifndef FILETRAFFICFILTERPROFILE_H_INCLUDED
#define FILETRAFFICFILTERPROFILE_H_INCLUDED


#include "./cfiletrafficfilterprofile.h"

namespace dcw {

class FileTrafficFilterProfile : public CFileTrafficFilterProfile {
public:
  FileTrafficFilterProfile(const char * const name, const char * const filename);
  virtual ~FileTrafficFilterProfile();
  FileTrafficFilterProfile(const FileTrafficFilterProfile& rhv); //no reason this cant be copied

  const char *GetFilename() const;
  virtual FILE *fopen() const;

private:
  const std::string _filename;
};

} // namespace dcw


#endif // #ifndef FILETRAFFICFILTERPROFILE_H_INCLUDED
