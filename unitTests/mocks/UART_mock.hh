#pragma once

#include <gmock/gmock.h>

#include "UART_Ifc.hh"

namespace Drivers {

class UartMock : public UART_Ifc
{
public:
    MOCK_METHOD(BusStatus, read, (uint8_t*, uint16_t), (override));
    MOCK_METHOD(BusStatus, write, (uint8_t*, uint16_t), (override));
    MOCK_METHOD(BusStatus, writeDMA, (uint8_t*, uint16_t, DriverIfc*), (override));
    MOCK_METHOD(BusStatus, writeDMAwithoutAlocate, (uint8_t*, uint16_t, DriverIfc*), (override));
    MOCK_METHOD(bool, isBussy, (), (override));

    UartMock():UART_Ifc([](uint32_t)->void {}) {}
};

}
