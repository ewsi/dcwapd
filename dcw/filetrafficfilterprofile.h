#ifndef FILETRAFFICFILTERPROFILE_H_INCLUDED
#define FILETRAFFICFILTERPROFILE_H_INCLUDED


#include "./cfiletrafficfilterprofile.h"

namespace dcw {

class FileTrafficFilterProfile : public CFileTrafficFilterProfile {
public:
  FileTrafficFilterProfile(const char * const name, const char * const filename);
  ~FileTrafficFilterProfile() override;
  FileTrafficFilterProfile(const FileTrafficFilterProfile& rhv); //no reason this cant be copied

  const char *GetFilename() const;
  FILE *fopen() const override;

private:
  const std::string _filename;
};

} // namespace dcw


#endif // #ifndef FILETRAFFICFILTERPROFILE_H_INCLUDED
