#include "SPI.hh"

namespace Drivers{

SPI::SPI(
        GPIO& _gpio
        , BusStatus(*_readWriteDMA)(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
        , BusStatus(*_readWrite)(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
        , bool(*_isBussyPtr)()
        , bool(*_resetPtr)()
        , void (*_delay)(uint32_t)
        ):
            SPI_Ifc(_delay),
            readWriteDMAptr(_readWriteDMA),
            readWritePtr(_readWrite),
            isBussyPtr(_isBussyPtr),
            resetPtr(_resetPtr),
            gpio(_gpio)
            {}
   
SPI::~SPI(){}

bool SPI::isBussy(){
    return isBussyPtr();
}

bool SPI::reset(){
    return resetPtr();
}

BusStatus SPI::readWrite(const OutputList& ssPin, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, const uint8_t multiplierDelayLoops){
    if(isBussyPtr())
        return BusStatus::BUSY;
    gpio.pin(ssPin, false);
    delayLoops(multiplierDelayLoops);
    BusStatus toReturn = readWritePtr(pTxData, pRxData, Size, timeOut);
    delayLoops(multiplierDelayLoops);
    gpio.pin(ssPin, true);
    return toReturn;
}

BusStatus SPI::readWrite(const OutputList& ssPin, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, DriverIfc* _callMe, const uint8_t multiplierDelayLoops){
    if(isBussyPtr())
        return BusStatus::BUSY;
    gpio.pin(ssPin, false);
    ssPinToToggle = ssPin;
    delayLoops(multiplierDelayLoops);
    BusStatus toReturn = readWriteDMAptr(pTxData, pRxData, Size);
    if(toReturn != BusStatus::OK)
        gpio.pin(ssPin, true);
    else{
        callMe = _callMe;
        delayMultiplier = multiplierDelayLoops;
    }
    return toReturn;
}

void SPI::handleFinish(DriverIfc*){
    delayLoops(delayMultiplier);
    delayMultiplier = 0;
    gpio.pin(ssPinToToggle, true);
    if(callMe != nullptr)
        callMe->handleFinish(this);
    callMe = nullptr;
}

void SPI::handleError(DriverIfc*){
    gpio.pin(ssPinToToggle, true);
    if(callMe != nullptr)
        callMe->handleError(this);
    callMe = nullptr;
}

void SPI::handleAbort(DriverIfc*){
    gpio.pin(ssPinToToggle, true);
    if(callMe != nullptr)
        callMe->handleAbort(this);
    callMe = nullptr;
}

}