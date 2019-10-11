#ifndef TRAFFICFILTERPROFILE_H_INCLUDED
#define TRAFFICFILTERPROFILE_H_INCLUDED

#include <string>

namespace dcw {

class TrafficFilterProfile {
public:
  explicit TrafficFilterProfile(const char * const name);
  virtual ~TrafficFilterProfile();
  TrafficFilterProfile(const TrafficFilterProfile& rhv); //copy ok for now; may go away in the future if this inherid from this class... tbd...

  const char * GetName() const;

private:
  std::string _name;

};

} // namespace dcw

#endif //#ifndef TRAFFICFILTERPROFILE_H_INCLUDED
