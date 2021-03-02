#pragma once

#include <stdint.h>
#include "DriverIfc.hh"

namespace Drivers{

class BarometerIfc : public DriverIfc
{
protected:
    bool& dataReadyFlag;
    void handleTimeEvent(DriverIfc*) = 0;
    void handleFinish(DriverIfc*) = 0;
public:
    BarometerIfc(bool& _dataReadyFlag, void (*_delay)(uint32_t)):
        DriverIfc(_delay),
        dataReadyFlag(_dataReadyFlag){}
    ~BarometerIfc() = default;
    virtual bool init() = 0;
    double preasure;
    double temperature;
    double altitude;
protected:
};

}
