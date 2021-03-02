#pragma once

#include <stdint.h>
#include <GPIOconf.hh>
#include "SPI_Ifc.hh"
#include "GPIO.hh"

namespace Drivers{

class SPI : public SPI_Ifc
{
private:
    BusStatus(*readWriteDMAptr)(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
    BusStatus(*readWritePtr)(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
    bool(*isBussyPtr)();
    bool(*resetPtr)();
    GPIO& gpio;
    OutputList ssPinToToggle;
    uint32_t timeOut = {100};
    uint8_t delayMultiplier = {0};
public:
    SPI(
        GPIO& _gpio
        , BusStatus(*_readWriteDMA)(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
        , BusStatus(*_readWrite)(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
        , bool(*_isBussyPtr)()
        , bool(*_resetPtr)()
        , void (*_delay)(uint32_t));
    ~SPI();
    BusStatus readWrite(const OutputList& ssPin, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, const uint8_t multiplierDelayLoops = 0);
    BusStatus readWrite(const OutputList& ssPin, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, DriverIfc* _callMe, const uint8_t multiplierDelayLoops = 0);
    bool reset();
    void handleFinish(DriverIfc*) override;
    void handleError(DriverIfc*) override;
    void handleAbort(DriverIfc*) override;
    bool isBussy() override;
};

}
