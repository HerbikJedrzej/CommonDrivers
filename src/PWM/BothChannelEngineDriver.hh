#pragma once

#include <OwnExceptions.hh>
#include <GPIO_ifc.hh>
#include "PWM_Ifc.hh"

namespace Drivers{

template<class T>
class BothChannelEngineDriver : public PWM_Ifc{
private:
    GPIO_Ifc* const gpio;
    const OutputList enableForward;
    const OutputList enableBackward;
    T* const engineRegister;
    const T driveOffset;
    const T maxCounter;
public:
    BothChannelEngineDriver(GPIO_Ifc* const _gpio, const OutputList _enableForward, const OutputList _enableBackward, T* const _engineRegister, const T _counterMax, const T _driveOffset = 0):
        gpio(_gpio),
        enableForward(_enableForward),
        enableBackward(_enableBackward),
        engineRegister(_engineRegister),
        driveOffset(_driveOffset),
        maxCounter(_counterMax - driveOffset){}

    ~BothChannelEngineDriver(){}

    uint8_t size() override{
        return 1;
    }
    
    void set(const double percent, const uint8_t& = 0) override{
        const bool backward = (percent < 0.0);
        const double percentValue = (backward)? percent * -1.0 : percent;
        T driveValue = (percentValue > 100.0)? 
            maxCounter
            :
            T(double(maxCounter) * percentValue / 100.0);

        *engineRegister = (driveValue < 0.1)? 0 : driveValue + driveOffset;
        if(*engineRegister == 0){
            gpio->pin(enableForward, false);
            gpio->pin(enableBackward, false);
        }
        else{
            gpio->pin(enableForward, !backward);
            gpio->pin(enableBackward, backward);
        }
    }
};

}
