#pragma once
#include <stdint.h>
#include "DriverIfc.hh"

namespace Drivers{

class UART_Ifc : public DriverIfc
{
public:
    UART_Ifc(void (*delay)(uint32_t)):DriverIfc(delay){}
    UART_Ifc(UART_Ifc&) = delete;
    UART_Ifc(UART_Ifc&&) = delete;
    virtual BusStatus read(uint8_t *Data, uint16_t Size) = 0;
    virtual BusStatus write(uint8_t *Data, uint16_t Size) = 0;
    virtual BusStatus writeDMA(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) = 0;
    virtual BusStatus writeDMAwithoutAlocate(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) = 0;
    virtual bool isBussy() = 0;
};

}