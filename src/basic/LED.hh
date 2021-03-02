#pragma once

#include <stdint.h>
#include "GPIO.hh"

enum LedsList{
  initFailure,
  wrongHWID,
  batteryOut,
  optionProcess,
  angleGreaterThan15deg,
  end_of_leds
};

namespace Drivers{

class LED : public DriverIfc
{
  struct LedPair{
    OutputList output;
    bool activeHigh;
  };
public:
  LED(GPIO& _gpio, void (*_delay)(uint32_t));
  LED(LED&) = delete;
  LED(LED&&) = delete;
  ~LED();
  void addLED(const LedsList led, const OutputList output, const bool activeHigh);
  void handleTimeEvent(DriverIfc*) final;
  bool get(const LedsList& led);
  void set(const LedsList& led, bool on);
  void set(const LedsList& led, bool on, const uint8_t& toggleAfterLoops);
  void toggle(const LedsList& led);
protected:
  GPIO& gpio;
  LedPair outputList[LedsList::end_of_leds];
  uint8_t togglePeriod[LedsList::end_of_leds];
};

}
