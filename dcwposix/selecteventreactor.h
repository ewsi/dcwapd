#ifndef SELECTEVENTREACTOR_H_INCLUDED
#define SELECTEVENTREACTOR_H_INCLUDED


#include "dcw/eventreactor.h"

#include <map>
#include <set>

namespace dcwposix {

class SelectEventReactor : public dcw::EventReactor {
  SelectEventReactor(const SelectEventReactor&); //no copy

public:
  struct SelectableIOProvider : public virtual EventReactor::IOProvider {
    virtual ~SelectableIOProvider() {}
    virtual int GetSelectableFd() const = 0;
  };

  SelectEventReactor();
  virtual ~SelectEventReactor();

  virtual void Run();
  virtual void Stop();
  virtual void RegisterIOSubscriber(IOSubscriber& sub, EventReactor::IOProvider& pub);
  virtual void UnegisterIOSubscriber(IOSubscriber& sub);
  virtual void ArmTimer(Timer& timer, unsigned seconds);
  virtual void DisarmTimer(Timer& timer);
  virtual void SleepSec(const unsigned seconds) const;
  virtual void SleepMs(const unsigned milliseconds) const;

private:
  typedef std::set<SelectableIOProvider *> IOPubSet;
  typedef std::map<IOSubscriber *, IOPubSet> IOSubMap;

  void updateNfds();

  int      _nfds;
  bool     _stopRequested;
  IOSubMap _ioSubs;
};

} // namespace dcwposix


#endif //#ifndef SELECTEVENTREACTOR_H_INCLUDED
