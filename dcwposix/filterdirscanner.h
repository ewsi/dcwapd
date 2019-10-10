#ifndef FILTERDIRSCANNER_H_INCLUDED
#define FILTERDIRSCANNER_H_INCLUDED

#include "dcw/filetrafficfilterprofile.h"

#include <list>
#include <string>


namespace dcwposix {

class FilterdirScanner {
  FilterdirScanner(const FilterdirScanner&); //no copy

  const std::string _path;

public:
  explicit FilterdirScanner(const char * const path);
  ~FilterdirScanner();


  typedef std::list<dcw::FileTrafficFilterProfile> FileFilterProfileList;
  void Scan(FileFilterProfileList& output);
};

} //namespace dcwposix {

#endif //#ifndef FILTERDIRSCANNER_H_INCLUDED
