
#include "./filterdirscanner.h"

#include "dcw/dcwlog.h"

#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <cerrno>

#include <exception>

namespace {
struct FilterDirScanFailed : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to scan for filter files!";
  }
};
} // namespace

using namespace dcwposix;

FilterdirScanner::FilterdirScanner(const char * const path) : _path(path) {
  //
}

FilterdirScanner::~FilterdirScanner() {
  //
}



void FilterdirScanner::Scan(FileFilterProfileList& output) {
  DIR *d;
  struct dirent *de;
  std::string filterName;
  std::string filterFilePath;
  static const char matchExtension[] = ".tfp";

  dcwlogdbgf("Scanning directory \"%s\" for filters...\n", _path.c_str());

  d = opendir(_path.c_str());
  if (d == NULL) {
    dcwlogerrf("opendir('%s') failed: %s\n", _path.c_str(), strerror(errno));
    throw FilterDirScanFailed();
  }

  while ((de = readdir(d))) {
    if (de->d_name[0] == '.') continue; // ignore file starting with a "."
    if (strlen(de->d_name) < strlen(matchExtension)) continue; //filename too short
    if (strcmp(&de->d_name[strlen(de->d_name) - strlen(matchExtension)], matchExtension) != 0) continue; //not a matching extension...

    //found a file with a matching extension...
    //dissect the filter name... (filename without the extension)
    filterName = de->d_name;
    filterName.resize(filterName.size() - strlen(matchExtension));

    //create the filename with the path...
    filterFilePath  = _path.c_str();
    filterFilePath += '/';
    filterFilePath += de->d_name;

    dcwloginfof("Discovered a filter file: %s\n", filterFilePath.c_str());

    //validate the filter name dont already exist (defensive)
    try {
      for (FileFilterProfileList::const_iterator i = output.begin(); i != output.end(); i++) {
        if (filterName == i->GetName()) {
          dcwlogerrf("Ignoring traffic filter profile: %s\n", filterFilePath.c_str());
          throw "";
        }
      }
    }
    catch (...) {continue;}

    //add it to our traffic filter profiles
    output.push_back(dcw::FileTrafficFilterProfile(filterName.c_str(), filterFilePath.c_str()));
  }

  closedir(d);
}
