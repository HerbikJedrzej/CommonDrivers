#pragma once

#include <stdint.h>
#include "UART_Ifc.hh"
#include <OwnExceptions.hh>

namespace Drivers{

template<unsigned int queueMaxSize, unsigned int maxDataSize>
class QueueUART : public UART_Ifc
{
private:
    struct QueueCell{
        uint8_t dataSize = {0};
        uint8_t dataCopy[maxDataSize];
        DriverIfc* callMe = {nullptr};
    };
    BusStatus(*readPtr)(uint8_t *Data, uint16_t Size, uint32_t Timeout);
    BusStatus(*writePtr)(uint8_t *Data, uint16_t Size, uint32_t Timeout);
    BusStatus(*writeDMAPtr)(uint8_t *Data, uint16_t Size);
    bool(*isBussyPtr)();
    QueueCell queue[queueMaxSize];
    uint8_t bufferPointerProcessing;
    uint8_t bufferPointer;
    uint32_t timeOut = {100};
public:
    QueueUART(
        BusStatus(*_read)(uint8_t *Data, uint16_t Size, uint32_t Timeout),
        BusStatus(*_write)(uint8_t *Data, uint16_t Size, uint32_t Timeout),
        BusStatus(*_writeDMA)(uint8_t *Data, uint16_t Size),
        bool(*_isBussyPtr)(),
        void (*_delay)(uint32_t));
    ~QueueUART();
    BusStatus read(uint8_t *Data, uint16_t Size) override;
    BusStatus write(uint8_t *Data, uint16_t Size) override;
    BusStatus writeDMA(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) override;
    BusStatus writeDMAwithoutAlocate(uint8_t *Data, uint16_t Size, DriverIfc* _callMe) override;
    void handleFinish(DriverIfc*) override;
    void handleError(DriverIfc*) override;
    void handleAbort(DriverIfc*) override;
    bool isBussy() override;
};


template<unsigned int queueMaxSize, unsigned int maxDataSize>
QueueUART<queueMaxSize, maxDataSize>::QueueUART(
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
            isBussyPtr(_isBussyPtr),
            bufferPointerProcessing(0),
            bufferPointer(0)
            {
                for(unsigned int i = 0; i < queueMaxSize; i++){
                    queue[i].dataSize = 0;
                    queue[i].callMe = nullptr;
                }
            }
   
template<unsigned int queueMaxSize, unsigned int maxDataSize>
QueueUART<queueMaxSize, maxDataSize>::~QueueUART(){}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
bool QueueUART<queueMaxSize, maxDataSize>::isBussy(){
    return isBussyPtr();
}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
BusStatus QueueUART<queueMaxSize, maxDataSize>::read(uint8_t* Data, uint16_t Size){
    if(isBussyPtr())
        return BusStatus::BUSY;
    return readPtr(Data, Size, timeOut);
}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
BusStatus QueueUART<queueMaxSize, maxDataSize>::write(uint8_t* Data, uint16_t Size){
    if(isBussyPtr())
        return BusStatus::BUSY;
    return writePtr(Data, Size, timeOut);
}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
BusStatus QueueUART<queueMaxSize, maxDataSize>::writeDMA(uint8_t* Data, uint16_t Size, DriverIfc* _callMe){
    return writeDMAwithoutAlocate(Data, Size, _callMe);
}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
BusStatus QueueUART<queueMaxSize, maxDataSize>::writeDMAwithoutAlocate(uint8_t* Data, uint16_t Size, DriverIfc* _callMe){
    if(Size > maxDataSize)
        THROW_out_of_range("UART data has bigger size than maxDataSize.");
    if(bufferPointer == bufferPointerProcessing)
        if(isBussy())
            THROW_out_of_range("UART data queue is overloaded.");
    for(uint16_t i = 0; i < Size; i++)
        queue[bufferPointer].dataCopy[i] = Data[i];
    queue[bufferPointer].dataSize = Size;
    queue[bufferPointer].callMe = _callMe;
    bufferPointer++;
    const bool busNotUsed = ((bufferPointer - 1) == bufferPointerProcessing);
    if(bufferPointer == queueMaxSize)
        bufferPointer = 0;
    if(busNotUsed)
        writeDMAPtr(queue[bufferPointerProcessing].dataCopy, queue[bufferPointerProcessing].dataSize);
    return BusStatus::OK;
}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
void QueueUART<queueMaxSize, maxDataSize>::handleFinish(DriverIfc*){
    DriverIfc* currentCall = queue[bufferPointerProcessing++].callMe;
    if(bufferPointerProcessing == queueMaxSize)
        bufferPointerProcessing = 0;
    if(bufferPointerProcessing != bufferPointer)
        writeDMAPtr(queue[bufferPointerProcessing].dataCopy, queue[bufferPointerProcessing].dataSize);
    if(currentCall != nullptr)
        currentCall->handleFinish(this);
    currentCall = nullptr;
}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
void QueueUART<queueMaxSize, maxDataSize>::handleError(DriverIfc*){
    DriverIfc* currentCall = queue[bufferPointerProcessing++].callMe;
    if(bufferPointerProcessing == queueMaxSize)
        bufferPointerProcessing = 0;
    if(bufferPointerProcessing != bufferPointer)
        writeDMAPtr(queue[bufferPointerProcessing].dataCopy, queue[bufferPointerProcessing].dataSize);
    if(currentCall != nullptr)
        currentCall->handleError(this);
    currentCall = nullptr;
}

template<unsigned int queueMaxSize, unsigned int maxDataSize>
void QueueUART<queueMaxSize, maxDataSize>::handleAbort(DriverIfc*){
    DriverIfc* currentCall = queue[bufferPointerProcessing++].callMe;
    if(bufferPointerProcessing == queueMaxSize)
        bufferPointerProcessing = 0;
    if(bufferPointerProcessing != bufferPointer)
        writeDMAPtr(queue[bufferPointerProcessing].dataCopy, queue[bufferPointerProcessing].dataSize);
    if(currentCall != nullptr)
        currentCall->handleAbort(this);
    currentCall = nullptr;
}

}
