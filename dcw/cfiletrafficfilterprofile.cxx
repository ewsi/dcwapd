
#include "./cfiletrafficfilterprofile.h"


using namespace dcw;


CFileTrafficFilterProfile::CFileTrafficFilterProfile(const char * const name) :
  TrafficFilterProfile(name) {
  //
}

CFileTrafficFilterProfile::~CFileTrafficFilterProfile() {
  //
}

CFileTrafficFilterProfile::CFileTrafficFilterProfile(const CFileTrafficFilterProfile& rhv) :
  TrafficFilterProfile(rhv) {
  //
}

AutoCFTFP::AutoCFTFP(const CFileTrafficFilterProfile& cftfp) :
  _openFile(cftfp.fopen()) {
  //
}

AutoCFTFP::~AutoCFTFP() {
  fclose(_openFile);
}

AutoCFTFP::operator ::FILE*() const {
  return _openFile;
}
