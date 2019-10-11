
#include "./filters.h"


#include "dcw/filetrafficfilterprofile.h"
#include "dcwposix/filterdirscanner.h"
#include "dcwlinux/macremapper_driver.h"
#include "ccspwrapper/tr181_config_provider.h"

namespace {

//two for one...
//this works perfect for this because both consumers
//(tr181 and dcw config) delete when they are done
struct SingleFilter :
  public ::dcw::FileTrafficFilterProfile,
  public ::ccspwrapper::Tr181ConfigProvider {

  SingleFilter(const ::dcw::FileTrafficFilterProfile& rhv) :
    ::dcw::FileTrafficFilterProfile(rhv) {}
  virtual ~SingleFilter() {}
  virtual void GetValue(const char * const name, ::ccspwrapper::Tr181Scalar& value) {
    //theres only one value... the name...
    value = this->GetName();
  }
};

} // namespace


Filters::Filters() {
  _scanPath = "/var/run/dcw/filters";
}

Filters::~Filters() {
  //
}

const char *Filters::GetScanPath() const {
  return _scanPath.c_str();
}

void Filters::SetScanPath(const char * const scanPath) {
  _scanPath = scanPath;
}


void Filters::PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
  //
  ::dcwlinux::APConfigurationProvider::CFTFPList filters;
  InstanciateCFileTrafficFilterProfiles(filters);
  for (::dcwlinux::APConfigurationProvider::CFTFPList::const_iterator i = filters.begin(); i != filters.end(); i++) {
    collection.push_back((SingleFilter*)*i);
  }
}

void Filters::InstanciateCFileTrafficFilterProfiles(::dcwlinux::APConfigurationProvider::CFTFPList& output) const {
  try {
    ::dcwposix::FilterdirScanner::FileFilterProfileList ffpl;
    ::dcwposix::FilterdirScanner dirScanner(_scanPath.c_str());
    dirScanner.Scan(ffpl);
    for(::dcwposix::FilterdirScanner::FileFilterProfileList::const_iterator i = ffpl.begin(); i != ffpl.end(); i++) {
      if (!::dcwlinux::MacRemapperDriver::ValidateFilter(*i)) continue;

      //note: caller (either dcw config or ccsp/tr181) will cleanup
      output.push_back(new SingleFilter(*i));
    }
  }
  catch (...) {
    //probably should care more if this fails...
  }
}

