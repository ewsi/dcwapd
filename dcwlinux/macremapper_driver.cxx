#include "./macremapper_driver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>

#include <macremapper_filter_config.h>
#include <macremapper_ioctl.h>
#include <mrm_filter_conf_parser.h>

#include "dcw/dcwlog.h"
#include "dcw/cfiletrafficfilterprofile.h"

#include "./brctlnetwork.h"

#include <exception>

using namespace dcwlinux;

namespace {
struct DriverOpenFailedException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to Open The Driver";
  }
};
struct DriverWipeFailedException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to Wipe the Driver";
  }
};
struct FilterNameTooLongException : public std::exception {
  virtual const char* what() const throw() {
    return "Filter Name Too Long";
  }
};
struct FilterFileLoadFailException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to Load Filter File";
  }
};
struct DriverSetFilterFailedException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to Load the Filter Into the Driver";
  }
};
struct TooManyDataChannelDestinationsException : public std::exception {
  virtual const char* what() const throw() {
    return "Too Many Data Channels for Replacement Destination Provided!";
  }
};
struct RemapFailedException : public std::exception {
  virtual const char* what() const throw() {
    return "Device Filter Remap Failed!";
  }
};
struct RemapDeleteFailedException : public std::exception {
  virtual const char* what() const throw() {
    return "Device Filter Remap Delete Failed!";
  }
};
struct DumpFailedException : public std::exception {
  virtual const char* what() const throw() {
    return "Failed to Dump Driver!";
  }
};
};

const char _driverFilename[] = "/proc/macremapctl";

MacRemapperDriver::MacRemapperDriver() {
  dcwlogdbgf("Attempting to open the MAC address remapper driver (MRM): %s\n", _driverFilename);
  _fd = open(_driverFilename, O_RDWR);
  if (_fd == -1) {
    throw DriverOpenFailedException();
  }
  try {
    WipeConfiguration();
  }
  catch (...) {
    close(_fd);
    throw;
  }

  dcwlogdbgf("Successfully opened MAC address remapper driver (MRM) as FD# %d\n", _fd);
}

MacRemapperDriver::~MacRemapperDriver() {
  try {
    WipeConfiguration();
  }
  catch (...) {}

  close(_fd);
  dcwlogdbgf("Closed the MAC address remapper driver (MRM) FD# %d\n", _fd);
}

void MacRemapperDriver::Dump() const {
  char linebuf[2048];
  FILE *f;

  //re-opening the driver each time for a dump guarantees
  //us a refreshed output as the driver generates this
  //report only once per open() on the first read() call
  f = fopen(_driverFilename, "r");
  if (f == NULL) {
    throw DumpFailedException();
  }

  //simply dump the driver's line based output into our logger functions
  dcwlogdbgf("%s\n", "MRM Driver Dump:");
  while (fgets(linebuf, sizeof(linebuf), f) != NULL) {
    dcwlogdbgf("  %.*s", static_cast<int>(sizeof(linebuf)), linebuf);
  }

  fclose(f);
}

bool MacRemapperDriver::ValidateFilter(const ::dcw::CFileTrafficFilterProfile& filter) {
  struct mrm_filter_config filterconf;
  return filter_file_loadf(&filterconf, ::dcw::AutoCFTFP(filter)) == 0;
}

void MacRemapperDriver::ParseAndLoadFilter(const ::dcw::CFileTrafficFilterProfile& filter) {
  struct mrm_filter_config filterconf;

  dcwloginfof("Loading MRM filter \"%s\" into the kernel\n", filter.GetName());

  if (strlen(filter.GetName()) >= sizeof(filterconf.name)) {
    throw FilterNameTooLongException();
  }

  //the filter_file_load() function comes from "mrmctl"
  if (filter_file_loadf(&filterconf, ::dcw::AutoCFTFP(filter)) != 0) {
    throw FilterFileLoadFailException();
  }
  strncpy(filterconf.name, filter.GetName(), sizeof(filterconf.name));

  if (ioctl(_fd, MRM_SETFILTER, &filterconf) == -1) {
    throw DriverSetFilterFailedException();
  }
}

void MacRemapperDriver::ApplyClientTrafficPolicy(const dcw::MacAddress& primaryAddr, const dcw::TrafficPolicy& policy) {
  struct mrm_remap_entry re;
  ::dcw::TrafficPolicy::DataChannelMap dataChannels;

  dcwloginfof("Applying MRM remap for device %s using traffic filter profile: %s\n", primaryAddr.ToString().c_str(), policy.trafficFilterProfile->GetName());

  //first filter out the unbonded data channels (NULL BasicChannel*)
  for (::dcw::TrafficPolicy::DataChannelMap::const_iterator i = policy.dataChannels.begin() ; i != policy.dataChannels.end(); i++) {
    if (i->second != NULL) {
      dataChannels[i->first] = i->second;
    }
  }

  //did we get a policy application request for zero bonded data channels?
  if (dataChannels.empty()) {
    //client has no bonded data channels...
    //remove any mapping...
    dcwlogdbgf("Requested application of a zero bonded data channel traffic policy for %s. Removing instead...\n", primaryAddr.ToString().c_str());
    RemoveClientTrafficPolicy(primaryAddr);
    return;
  }

  //ensure we dont have too many replacement destinations
  if (dataChannels.size() > MRM_MAX_REPLACE) {
    throw TooManyDataChannelDestinationsException();
  }

  //populate our remap ioctl()
  memset(&re, 0, sizeof(re));
  strncpy(re.filter_name, policy.trafficFilterProfile->GetName(), sizeof(re.filter_name));
  memcpy(re.match_macaddr, primaryAddr.Value, sizeof(re.match_macaddr));

  //add each of the replacements into the remap ioctl() list...
  for (::dcw::TrafficPolicy::DataChannelMap::const_iterator channel = dataChannels.begin(); channel != dataChannels.end(); ++channel) {
    //copy over the replacement MAC address...
    const ::dcw::MacAddress& dest = channel->first;
    memcpy(re.replace[re.replace_count].macaddr, dest.Value, sizeof(re.replace[re.replace_count].macaddr));

    //do we have an interface to remap to?
    const BrctlChannel * const btctlChannel = dynamic_cast<const BrctlChannel*>(channel->second);
    if (btctlChannel != NULL) {
      if (btctlChannel->GetIfName() != NULL) {
        strncpy(re.replace[re.replace_count].ifname, btctlChannel->GetIfName(), sizeof(re.replace[re.replace_count].ifname));
      }
    }
    
    //increment the replacement count...
    ++re.replace_count;
  }

  //send it to the driver...
  if (ioctl(_fd, MRM_SETREMAP, &re) == -1) {
    throw RemapFailedException();
  }
}

void MacRemapperDriver::RemoveClientTrafficPolicy(const dcw::MacAddress& primaryAddr) {
  struct mrm_remap_entry re;

  dcwloginfof("Removing any MRM remap for device %s\n", primaryAddr.ToString().c_str());
  
  memcpy(re.match_macaddr, primaryAddr.Value, sizeof(re.match_macaddr));
  if (ioctl(_fd, MRM_DELETEREMAP, &re) == -1) {
    //if the ioctl() comes back with "EINVAL" error,
    //the driver is telling us that the primary
    //MAC address was not found in its remap list
    //no need to throw an exception because we are
    //in the expected state after this function call
    //meaning that the client traffic policy was removed
    //(it just didnt exist before)
    if (errno != EINVAL) {
      //errno aint "EINVAL"; something else failed...
      //throw an exception
      throw RemapDeleteFailedException();
    }
    dcwlogwarnf("Failed to remove MRM remap for %s due to no existing remap entry", primaryAddr.ToString().c_str());
  }
}


void MacRemapperDriver::WipeConfiguration() {
  dcwlogdbgf("%s\n", "Wiping MAC address remapper (MRM)...");
  if (ioctl(_fd, MRM_WIPERUNCONF) == -1) {
    throw DriverWipeFailedException();
  }
  dcwloginfof("%s\n", "MAC address remapper configuration wiped");
}

