#pragma once
#include <stdint.h>
#include "DriverIfc.hh"
#include "GPIO.hh"

namespace Drivers{

class SPI_Ifc : public DriverIfc
{
public:
    SPI_Ifc(void (*delay)(uint32_t)):DriverIfc(delay){}
    SPI_Ifc(SPI_Ifc&) = delete;
    SPI_Ifc(SPI_Ifc&&) = delete;
    virtual BusStatus readWrite(const OutputList& ssPin, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, const uint8_t multiplierDelayLoops = 0) = 0;
    virtual BusStatus readWrite(const OutputList& ssPin, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, DriverIfc* _callMe, const uint8_t multiplierDelayLoops = 0) = 0;
    virtual bool reset() = 0;
};

}