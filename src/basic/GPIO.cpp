#include"GPIO.hh"

namespace Drivers{

void GPIO::pin(OutputList pin, bool state) const{
    set(pin, state);
}

bool GPIO::pin(OutputList pin) const{
    return get(pin);
}

bool GPIO::pin(InputList pin) const{
    return getInput(pin);
}

GPIO::GPIO(void(*write)(OutputList, bool), bool(*read)(OutputList), bool(*readInput)(InputList), void (*_delay)(uint32_t)):
GPIO_Ifc(_delay)
{
    set = write;
    get = read;
    getInput = readInput;
    for(uint8_t i = 0; i < InterruptInputList::endOfInterruptInputList; i++)
        callMe[i] = nullptr;
}

GPIO::~GPIO(){}

void GPIO::subscribe(InterruptInputList pin, DriverIfc* driver){
    callMe[pin] = driver;
}

void GPIO::setChangedPin(InterruptInputList pin){
    changedPin = pin;
}

void GPIO::handleFinish(DriverIfc*){
    if(changedPin == InterruptInputList::endOfInterruptInputList)
        return;
    const InterruptInputList tmp = changedPin;
    changedPin = InterruptInputList::endOfInterruptInputList;
    if(callMe[tmp] != nullptr)
        callMe[tmp]->handleFinish(this);
}

}
