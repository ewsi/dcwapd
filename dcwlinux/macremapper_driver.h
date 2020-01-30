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
  ~MacRemapperDriver() override;

  void Dump() const;

  static bool ValidateFilter(const ::dcw::CFileTrafficFilterProfile& filter);
  void ParseAndLoadFilter(const ::dcw::CFileTrafficFilterProfile& filter);

  void ApplyClientTrafficPolicy(const dcw::MacAddress& primaryAddr, const dcw::TrafficPolicy& policy) override;
  void RemoveClientTrafficPolicy(const dcw::MacAddress& primaryAddr) override;

private:

  void WipeConfiguration();

  int _fd;
};

} // namespace dcwlinux



#endif //#ifndef MACREMAPPER_DRIVER_H_INCLUDED
