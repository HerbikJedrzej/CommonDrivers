#pragma once

#include <OwnExceptions.hh>

namespace Drivers{

template<class T>
class ESC
{
private:
    T* const engines;
    const uint8_t sizeOfEngines;
    const T baseCounterValue;
public:
    ESC(T* const _engines, const uint8_t _size, const T _counterMax):
        engines(_engines),
        sizeOfEngines(_size),
        baseCounterValue(_counterMax / 20){}

    ~ESC(){}

    uint8_t size(){
        return sizeOfEngines;
    }

    virtual void set(const uint8_t& engine, const double percent){
        if(engine >= sizeOfEngines)
            THROW_out_of_range("Requested engine out of the list.");
        if(percent <= 0.0)
            engines[engine] = baseCounterValue;
        else if(percent >= 100.0)
            engines[engine] = 2 * baseCounterValue;
        else
            engines[engine] = baseCounterValue * (percent / 100.0) + baseCounterValue;
    }
};

}
