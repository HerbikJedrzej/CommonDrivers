#pragma once

#include <OwnExceptions.hh>

namespace Drivers{

template<class T>
class PWM
{
private:
    T* const engines;
    const uint8_t sizeOfEngines;
    const T maxCounter;
public:
    PWM(T* const _engines, const uint8_t _size, const T _counterMax):
        engines(_engines),
        sizeOfEngines(_size),
        maxCounter(_counterMax){}

    ~PWM(){}

    uint8_t size(){
        return sizeOfEngines;
    }

    void set(const uint8_t& engine, const double percent){
        if(engine >= sizeOfEngines)
            THROW_out_of_range("Requested engine out of the list.");
        if(percent < 0.0)
            engines[engine] = 0;
        else if(percent > 100.0)
            engines[engine] = maxCounter;
        else
            engines[engine] = T(double(maxCounter) * percent / 100.0);
    }

};

}
