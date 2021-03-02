#include "I2C.hh"

namespace Drivers{

I2C::I2C(
        BusStatus(*_writeDMAptr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
        , BusStatus(*_readDMAptr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
        , BusStatus(*_writePtr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
        , BusStatus(*_readPtr)(uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
        , bool(*_isBussyPtr)()
        , void (*_delay)(uint32_t)
        ):
            I2C_Ifc(_delay),
            writeDMAptr(_writeDMAptr),
            readDMAptr(_readDMAptr),
            writePtr(_writePtr),
            readPtr(_readPtr),
            isBussyPtr(_isBussyPtr)
            {}
   
I2C::~I2C(){}

bool I2C::isBussy(){
    return isBussyPtr();
}

uint16_t I2C::parse(const bool& MemAddSize){
    if(MemAddSize)return 2;
    return 1;
}

BusStatus I2C::read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size){
    return readPtr(DevAddress, MemAddress, parse(MemAddrIs16b), pData, Size, timeOut);
}

BusStatus I2C::write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size){
    return writePtr(DevAddress, MemAddress, parse(MemAddrIs16b), pData, Size, timeOut);
}

BusStatus I2C::read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe){
    BusStatus toReturn = readDMAptr(DevAddress, MemAddress, parse(MemAddrIs16b), pData, Size);
    if(toReturn == BusStatus::OK)
        callMe = _callMe;
    return toReturn;
}

BusStatus I2C::write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe){
    BusStatus toReturn = writeDMAptr(DevAddress, MemAddress, parse(MemAddrIs16b), pData, Size);
    if(toReturn == BusStatus::OK)
        callMe = _callMe;
    return toReturn;
}

}