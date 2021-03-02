#pragma once

#include <DriverIfc.hh>
#include <Timer.hh>
#include <GPIO_ifc.hh>

namespace Drivers{

class US_015 : public DriverIfc{
public:
    US_015(const Timer& _timer, GPIO_Ifc* const _gpio, const OutputList& _triger, const InputList& _echo, const InterruptInputList& _echoIrq, void(*_delay)(uint32_t));
    void handleFinish(DriverIfc*) override;
    void handleTimeEvent(DriverIfc*) override;
    double getDistanceInMilimeters();
    void init();
private:
    const Timer& timer;
    GPIO_Ifc* const gpio;
    const OutputList triger;
    const InputList echo;
    const InterruptInputList echoIrq;
    Timer::TimeStamp timeStamp;
    double timeDiff;
};

}