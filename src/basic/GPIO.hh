#pragma once

#include "GPIO_ifc.hh"

namespace Drivers{

class GPIO : public GPIO_Ifc
{
public:
  GPIO(void(*write)(OutputList, bool), bool(*read)(OutputList), bool(*readInput)(InputList), void (*_delay)(uint32_t));
  ~GPIO();
  void pin(OutputList, bool state) const override;
  bool pin(OutputList) const override;
  bool pin(InputList) const override;
  void setChangedPin(InterruptInputList pin) override;
  void subscribe(InterruptInputList pin, DriverIfc* driver) override;
  void handleFinish(DriverIfc*) override;
protected:
  void(*set)(OutputList, bool);
  bool(*get)(OutputList);
  bool(*getInput)(InputList);
  InterruptInputList changedPin = InterruptInputList::endOfInterruptInputList;
  DriverIfc* callMe[InterruptInputList::endOfInterruptInputList];
private:
};

}