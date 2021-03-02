#pragma once

#include <stdint.h>
#include "DriverIfc.hh"

namespace Drivers{

class I2C_Ifc : public DriverIfc
{
public:
    I2C_Ifc(void (*delay)(uint32_t)):DriverIfc(delay){}
    virtual BusStatus read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size) = 0;
    virtual BusStatus write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size) = 0;
    virtual BusStatus read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe) = 0;
    virtual BusStatus write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe) = 0;
};

}