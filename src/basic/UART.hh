#pragma once

#include <stdint.h>
#include "UART_Ifc.hh"

namespace Drivers{

class UART : public UART_Ifc
{
private:
    BusStatus(*readPtr)(uint8_t *Data, uint16_t Size, uint32_t Timeout);
    BusStatus(*writePtr)(uint8_t *Data, uint16_t Size, uint32_t Timeout);
    BusStatus(*writeDMAPtr)(uint8_t *Data, uint16_t Size);
    bool(*isBussyPtr)();
    uint32_t timeOut = {100};
    uint8_t* dataCopy = {nullptr};
public:
    UART(
        BusStatus(*_read)(uint8_t *Data, uint16_t Size, uint32_t Timeout),
        BusStatus(*_write)(uint8_t *Data, uint16_t Size, uint32_t Timeout),
        BusStatus(*_writeDMA)(uint8_t *Data, uint16_t Size),
        bool(*_isBussyPtr)(),
        void (*_delay)(uint32_t));
    ~UART();
    BusStatus read(uint8_t *Data, uint16_t Size) override;
    BusStatus write(uint8_t *Data, uint16_t Size) override;
    BusStatus writeDMA(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) override;
    BusStatus writeDMAwithoutAlocate(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) override;
    void handleFinish(DriverIfc*) override;
    void handleError(DriverIfc*) override;
    void handleAbort(DriverIfc*) override;
    bool isBussy() override;
};

}
