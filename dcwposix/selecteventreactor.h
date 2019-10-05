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
  ~SelectEventReactor() override;

  void Run() override;
  void Stop() override;
  void RegisterIOSubscriber(IOSubscriber& sub, EventReactor::IOProvider& pub) override;
  void UnegisterIOSubscriber(IOSubscriber& sub) override;
  void ArmTimer(Timer& timer, unsigned seconds) override;
  void DisarmTimer(Timer& timer) override;
  void SleepSec(const unsigned seconds) const override;
  void SleepMs(const unsigned milliseconds) const override;

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
