#ifndef MACREMAPPER_DRIVER_H_INCLUDED
#define MACREMAPPER_DRIVER_H_INCLUDED

#include "dcw/trafficsorter.h"

namespace dcw {
  class CFileTrafficFilterProfile;
} // namespace dcw

namespace dcwlinux {

class MacRemapperDriver : public dcw::TrafficSorter {
  MacRemapperDriver(const MacRemapperDriver&); //no copy

public:
  MacRemapperDriver();
  virtual ~MacRemapperDriver();

  void Dump() const;

  static bool ValidateFilter(const ::dcw::CFileTrafficFilterProfile& filter);
  void ParseAndLoadFilter(const ::dcw::CFileTrafficFilterProfile& filter);

  virtual void ApplyClientTrafficPolicy(const dcw::MacAddress& primaryAddr, const dcw::TrafficPolicy& policy);
  virtual void RemoveClientTrafficPolicy(const dcw::MacAddress& primaryAddr);

private:

  void WipeConfiguration();

  int _fd;
};

} // namespace dcwlinux



#endif //#ifndef MACREMAPPER_DRIVER_H_INCLUDED
