#pragma once

#include <stdint.h>

namespace Drivers{

enum BusStatus{
  OK       = 0,
  ERR      = 1,
  BUSY     = 2,
  TIMEOUT  = 3
};

class DriverIfc
{
public:
  DriverIfc(void (*_delay)(uint32_t));
  DriverIfc(DriverIfc&) = delete;
  DriverIfc(DriverIfc&&) = delete;
  virtual ~DriverIfc();
  virtual void handleTimeEvent(DriverIfc*);
  virtual void handleFinish(DriverIfc*);
  virtual void handleError(DriverIfc*);
  virtual void handleAbort(DriverIfc*);
  virtual void wakeMeUp(DriverIfc* _wakeMeUpPtr);
  virtual void stopWakingMe(DriverIfc* _wakeMeUpPtr);
  virtual bool isBussy();
  static uint16_t loopsToWait;
protected:
	void delayLoops();
	void delayLoops(const uint8_t multiplier);
  uint8_t wakeMeUpPtrFind(DriverIfc* _wakeMeUpPtr);
  void addToWakeMeUpPtr(DriverIfc* _wakeMeUpPtr);
  void removeToWakeMeUpPtr(const uint8_t id);
  void (*delay)(uint32_t) = nullptr;
  DriverIfc* callMe = nullptr;
  uint8_t wakeMeUpPtrSize = {0};
  DriverIfc** wakeMeUpPtr = nullptr;
};

}
