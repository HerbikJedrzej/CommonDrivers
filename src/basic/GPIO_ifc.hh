#pragma once

#include "DriverIfc.hh"
#include <GPIOconf.hh>

namespace Drivers{

class GPIO_Ifc : public DriverIfc
{
public:
  GPIO_Ifc(void (*_delay)(uint32_t)):DriverIfc(_delay){}
  virtual void pin(OutputList, bool state) const = 0;
  virtual bool pin(OutputList) const = 0;
  virtual bool pin(InputList) const = 0;
  virtual void setChangedPin(InterruptInputList pin) = 0;
  virtual void subscribe(InterruptInputList pin, DriverIfc* driver) = 0;
};

}