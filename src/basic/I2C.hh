#pragma once

#include <stdint.h>
#include "I2C_Ifc.hh"

namespace Drivers{

class I2C : public I2C_Ifc
{
private:
    BusStatus(*writeDMAptr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
    BusStatus(*readDMAptr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
    BusStatus(*writePtr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    BusStatus(*readPtr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    bool(*isBussyPtr)();
    uint32_t timeOut = {100};
    uint16_t parse(const bool& MemAddSize);
public:
    I2C(
        BusStatus(*_writeDMAptr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
        , BusStatus(*_readDMAptr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
        , BusStatus(*_writePtr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
        , BusStatus(*_readPtr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
        , bool(*_isBussyPtr)()
        , void (*_delay)(uint32_t));
    ~I2C();
    BusStatus read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size);
    BusStatus write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size);
    BusStatus read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe);
    BusStatus write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe);
    bool isBussy() override;
};

}