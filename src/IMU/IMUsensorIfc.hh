#pragma once

#include <stdint.h>
#include "DriverIfc.hh"

namespace Drivers{

class IMUsensorIfc : public DriverIfc
{
protected:
    bool& akcelerometerAndGyroskopeDataReadyFlag;
    bool& magnetometerDataReadyFlag;
    void handleTimeEvent(DriverIfc*) = 0;
    void handleFinish(DriverIfc*) = 0;
public:
    enum RottatedAxis{
        X = 0,
        Y,
        Z,
        none
    };
    const RottatedAxis invertedAxis;
    IMUsensorIfc(bool& _akcAndGyroDataReadyFlag, bool& _magDataReadyFlag, const RottatedAxis& axis, void (*_delay)(uint32_t)):
        DriverIfc(_delay),
        akcelerometerAndGyroskopeDataReadyFlag(_akcAndGyroDataReadyFlag),
        magnetometerDataReadyFlag(_magDataReadyFlag),
        invertedAxis(axis){}
    ~IMUsensorIfc() = default;
    virtual bool init() = 0;
    double akcelerometr[3];
    double gyroskope[3];
    double* magnetometer = {nullptr};
protected:
    void invert(){
        if(invertedAxis == RottatedAxis::none)
            return;
        for(uint8_t i = 0; i < 3; i++){
            if(i == invertedAxis)
                continue;
            akcelerometr[i] = -akcelerometr[i];
            gyroskope[i] = -gyroskope[i];
            if(magnetometer != nullptr)
                magnetometer[i] = -magnetometer[i];
        }
    }
};

}
