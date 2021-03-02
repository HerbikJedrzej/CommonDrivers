#include "UART.hh"

namespace Drivers{

UART::UART(
        BusStatus(*_read)(uint8_t *Data, uint16_t Size, uint32_t Timeout),
        BusStatus(*_write)(uint8_t *Data, uint16_t Size, uint32_t Timeout),
        BusStatus(*_writeDMA)(uint8_t *Data, uint16_t Size),
        bool(*_isBussyPtr)(),
        void (*_delay)(uint32_t)
        ):
            UART_Ifc(_delay),
            readPtr(_read),
            writePtr(_write),
            writeDMAPtr(_writeDMA),
            isBussyPtr(_isBussyPtr)
            {}
   
UART::~UART(){}

bool UART::isBussy(){
    return isBussyPtr();
}

BusStatus UART::read(uint8_t* Data, uint16_t Size){
    if(isBussyPtr())
        return BusStatus::BUSY;
    return readPtr(Data, Size, timeOut);;
}

BusStatus UART::write(uint8_t* Data, uint16_t Size){
    if(isBussyPtr())
        return BusStatus::BUSY;
    return writePtr(Data, Size, timeOut);;
}

BusStatus UART::writeDMA(uint8_t* Data, uint16_t Size, DriverIfc* _callMe){
    if(isBussyPtr())
        return BusStatus::BUSY;
    if(!dataCopy)
        delete [] dataCopy;
    dataCopy = new uint8_t[Size];
    for(uint8_t i = 0; i < Size; i++)
        dataCopy[i] = Data[i];
    callMe = _callMe;
    BusStatus toReturn = writeDMAPtr(dataCopy, Size);
    if(toReturn != BusStatus::OK){
        delete [] dataCopy;
        dataCopy = nullptr;
        callMe = nullptr;
    }
    return toReturn;
}

BusStatus UART::writeDMAwithoutAlocate(uint8_t* Data, uint16_t Size, DriverIfc* _callMe){
    if(isBussyPtr())
        return BusStatus::BUSY;
    if(!Data)
        return BusStatus::ERR;
    callMe = _callMe;
    BusStatus toReturn = writeDMAPtr(Data, Size);
    if(toReturn != BusStatus::OK)
        callMe = nullptr;
    return toReturn;
}

void UART::handleFinish(DriverIfc*){
    if(callMe != nullptr)
        callMe->handleFinish(this);
    callMe = nullptr;
}

void UART::handleError(DriverIfc*){
    if(callMe != nullptr)
        callMe->handleError(this);
    callMe = nullptr;
}

void UART::handleAbort(DriverIfc*){
    if(callMe != nullptr)
        callMe->handleAbort(this);
    callMe = nullptr;
}

}