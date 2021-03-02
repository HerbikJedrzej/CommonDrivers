#pragma once

#include "DriverIfc.hh"

namespace Drivers{

class Timer : public DriverIfc
{
public:
  struct TimeStamp{
    unsigned int cycles;
    uint32_t counter;
  };
  Timer(const double _freq, const uint32_t _resolution, const uint32_t _divider, void (*_delay)(uint32_t));
  void setCounterReference(volatile uint32_t& _counter);
  void handleTimeEvent(DriverIfc*) override;
  void updateSignalParams(const double _freq, const uint32_t _resolution, const uint32_t _divider);
  TimeStamp getTimeStamp() const;
  double timeDiffInSeconds(const TimeStamp& timeStamp) const;
  unsigned int timeDiffInMicroSeconds(const TimeStamp& timeStamp) const;
  bool isAfter(const TimeStamp& timeStamp) const;
protected:
  unsigned int cycle;
  uint32_t resolution;
  unsigned int periodInUs;
  uint32_t notInitialisedCounter;
  volatile uint32_t* counter;
private:
};

}