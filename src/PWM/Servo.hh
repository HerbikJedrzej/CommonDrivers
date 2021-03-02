#pragma once

#include <OwnExceptions.hh>

namespace Drivers{

template<class T>
class Servo
{
private:
    T* const engines;
    const uint8_t sizeOfEngines;
    const T minCounterValue;
    const T operatingRange;
    const double maxAngle;
public:
    Servo(T* const _engines, const uint8_t _size, const T _counterMax, const double _maxAngle, const double minPercentOfPWM = 4.5, const double maxPercentOfPWM = 10.5):
        engines(_engines),
        sizeOfEngines(_size),
        minCounterValue(_counterMax * (minPercentOfPWM / 100.0)),
        operatingRange(_counterMax * (maxPercentOfPWM / 100.0) - minCounterValue),
        maxAngle(_maxAngle){}

    ~Servo(){}

    uint8_t size(){
        return sizeOfEngines;
    }

    void set(const uint8_t& engine, const double angle){
        if(engine >= sizeOfEngines)
            THROW_out_of_range("Requested engine out of the list.");
        if(angle <= 0.0)
            engines[engine] = minCounterValue;
        else if(angle >= maxAngle)
            engines[engine] = minCounterValue + operatingRange;
        else
            engines[engine] = minCounterValue + (angle / maxAngle) * operatingRange;
    }

};

}
