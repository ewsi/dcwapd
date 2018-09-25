#include "./trafficfilterprofile.h"

using namespace dcw;



TrafficFilterProfile::TrafficFilterProfile(const char * const name) : _name(name) {
  //
}

TrafficFilterProfile::~TrafficFilterProfile() {
  //
}

TrafficFilterProfile::TrafficFilterProfile(const TrafficFilterProfile& rhv) :
  _name(rhv._name) {
  //
}

const char *TrafficFilterProfile::GetName() const {
  return _name.c_str();
}

