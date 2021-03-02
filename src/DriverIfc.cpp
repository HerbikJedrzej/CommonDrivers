#include "DriverIfc.hh"

namespace Drivers{

DriverIfc::DriverIfc(void (*_delay)(uint32_t)){
    delay = _delay;
}

DriverIfc::~DriverIfc()
{
    if(wakeMeUpPtr != nullptr)
        delete [] wakeMeUpPtr;
}

void DriverIfc::handleTimeEvent(DriverIfc*){
    for(uint8_t i = 0; i < wakeMeUpPtrSize; i++)
        wakeMeUpPtr[i]->handleTimeEvent(this);
}

void DriverIfc::handleFinish(DriverIfc*){
    if(callMe == nullptr)
        return;
    DriverIfc* tmp = callMe;
    callMe = nullptr;
    tmp->handleFinish(this);
}

void DriverIfc::handleError(DriverIfc*){
    if(callMe == nullptr)
        return;
    DriverIfc* tmp = callMe;
    callMe = nullptr;
    tmp->handleError(this);
}

void DriverIfc::handleAbort(DriverIfc*){
    if(callMe == nullptr)
        return;
    DriverIfc* tmp = callMe;
    callMe = nullptr;
    tmp->handleAbort(this);
}

uint8_t DriverIfc::wakeMeUpPtrFind(DriverIfc* _wakeMeUpPtr){
    for(uint8_t i = 0; i < wakeMeUpPtrSize; i++)
        if(wakeMeUpPtr[i] == _wakeMeUpPtr)
            return i;
    return wakeMeUpPtrSize;
}

void DriverIfc::removeToWakeMeUpPtr(const uint8_t id){
    if(id >= wakeMeUpPtrSize)
        return;
    wakeMeUpPtrSize--;
    DriverIfc** wakeMeUpPtrCopy = nullptr;
    if(wakeMeUpPtrSize > 0){
        wakeMeUpPtrCopy = new DriverIfc* [wakeMeUpPtrSize];
        for(uint8_t i = 0; i < id; i++)
            wakeMeUpPtrCopy[i] = wakeMeUpPtr[i];
        for(uint8_t i = id; i < wakeMeUpPtrSize; i++)
            wakeMeUpPtrCopy[i] = wakeMeUpPtr[i + 1];
    }
    delete [] wakeMeUpPtr;
    wakeMeUpPtr = wakeMeUpPtrCopy;
}

void DriverIfc::addToWakeMeUpPtr(DriverIfc* _wakeMeUpPtr){
    DriverIfc** wakeMeUpPtrCopy = new DriverIfc* [wakeMeUpPtrSize + 1];
    for(uint8_t i = 0; i < wakeMeUpPtrSize; i++)
        wakeMeUpPtrCopy[i] = wakeMeUpPtr[i];
    wakeMeUpPtrCopy[wakeMeUpPtrSize] = _wakeMeUpPtr;
    wakeMeUpPtrSize++;
    if(wakeMeUpPtr)
        delete [] wakeMeUpPtr;
    wakeMeUpPtr = wakeMeUpPtrCopy;
}

void DriverIfc::stopWakingMe(DriverIfc* _wakeMeUpPtr){
    if(!_wakeMeUpPtr)
        return;
    removeToWakeMeUpPtr(wakeMeUpPtrFind(_wakeMeUpPtr));
}

void DriverIfc::wakeMeUp(DriverIfc* _wakeMeUpPtr){
    if(!_wakeMeUpPtr)
        return;
    if(wakeMeUpPtrFind(_wakeMeUpPtr) == wakeMeUpPtrSize)
        addToWakeMeUpPtr(_wakeMeUpPtr);
}

uint16_t DriverIfc::loopsToWait = 50;

void DriverIfc::delayLoops(){
	for(volatile uint16_t i = 0; i < loopsToWait; i++);
}

void DriverIfc::delayLoops(const uint8_t multiplier){
	for(uint8_t i = 0; i < multiplier; i++)
        delayLoops();
}

bool DriverIfc::isBussy(){
    return false;
}

}