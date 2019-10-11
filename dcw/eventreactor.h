#ifndef EVENTREACTOR_H_INCLUDED
#define EVENTREACTOR_H_INCLUDED

namespace dcw {


struct EventReactor {
  struct Timer {
    virtual ~Timer() {}
    virtual void OnFire() = 0;
  };

  struct IOProvider {
    virtual ~IOProvider() {}
  };

  struct IOSubscriber {
    virtual ~IOSubscriber() {}
    virtual void OnIOReady(IOProvider& iop) = 0;
  };
  virtual ~EventReactor() {}
  virtual void Run() = 0;
  virtual void Stop() = 0;
  virtual void RegisterIOSubscriber(IOSubscriber& sub, IOProvider& pub) = 0;
  virtual void UnegisterIOSubscriber(IOSubscriber& sub) = 0;
  virtual void ArmTimer(Timer& timer, unsigned seconds) = 0;
  virtual void DisarmTimer(Timer& timer) = 0;
  virtual void SleepSec(const unsigned seconds) const = 0;
  virtual void SleepMs(const unsigned milliseconds) const = 0;
};

} // namespace dcw

#endif //#ifndef EVENTREACTOR_H_INCLUDED
