#include <UARTwithQueue.hh>
#include <gtest/gtest.h>

namespace{

class CallBackMock : Drivers::DriverIfc{
    bool errorWasCalled;
    bool abortWasCalled;
    bool finishWasCalled;
public:
    CallBackMock(void (*delay)(uint32_t)):
        DriverIfc(delay),
        errorWasCalled(false),
        abortWasCalled(false),
        finishWasCalled(false){}
    void handleFinish(DriverIfc*) override{
        finishWasCalled = true;
    }
    void handleError(DriverIfc*) override{
        errorWasCalled = true;
    }
    void handleAbort(DriverIfc*) override{
        abortWasCalled = true;
    }
    bool wasCallederror(){
        return errorWasCalled;
    }
    bool wasCalledabort(){
        return abortWasCalled;
    }
    bool wasCalledfinish(){
        return finishWasCalled;
    }
};

bool isBussy_return = false;
bool isBussy(){
    return isBussy_return;
}

uint8_t* read_Data = nullptr;
uint16_t read_Size = 0;
uint32_t read_Timeout = 100;
Drivers::BusStatus read_return = Drivers::BusStatus::OK;
Drivers::BusStatus read(uint8_t *Data, uint16_t Size, uint32_t Timeout){
    EXPECT_EQ(Size, read_Size);
    EXPECT_EQ(Timeout, read_Timeout);
    if(!read_Data){
        EXPECT_TRUE(false);
        return read_return;
    }
    for(uint16_t i = 0; i < Size; i++)
        Data[i] = read_Data[i];
    return read_return;
}

uint8_t* write_Data = nullptr;
uint16_t write_Size = 0;
uint32_t write_Timeout = 100;
Drivers::BusStatus write_return = Drivers::BusStatus::OK;
Drivers::BusStatus write(uint8_t *Data, uint16_t Size, uint32_t Timeout){
    EXPECT_EQ(Size, write_Size);
    EXPECT_EQ(Timeout, write_Timeout);
    if(!write_Data){
        EXPECT_TRUE(false);
        return write_return;
    }
    for(uint16_t i = 0; i < Size; i++)
        write_Data[i] = Data[i];
    return write_return;
}

uint8_t* writeDMA_Data = nullptr;
uint16_t writeDMA_Size = 0;
Drivers::BusStatus writeDMA_return = Drivers::BusStatus::OK;
Drivers::BusStatus writeDMA(uint8_t *Data, uint16_t Size){
    EXPECT_EQ(Size, writeDMA_Size);
    if(!writeDMA_Data){
        EXPECT_TRUE(false);
        return writeDMA_return;
    }
    isBussy_return = true;
    for(uint16_t i = 0; i < Size; i++)
        writeDMA_Data[i] = Data[i];
    return writeDMA_return;
}

uint32_t delayUART_value = 0;
void delayUART(uint32_t value){
    EXPECT_EQ(delayUART_value, value);
}

TEST(QueueUART_Test, readStatus){
    Drivers::QueueUART<5, 10> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = true;
    EXPECT_TRUE(uart.isBussy());
    isBussy_return = false;
    EXPECT_FALSE(uart.isBussy());
}

void compareArrays(uint8_t* array1, uint8_t* array2, uint16_t size, const int lineNumber){
    for(uint16_t i = 0; i < size; i++)
        EXPECT_EQ(array1[i], array2[i]) << "Array element number: " << i << " Line number: " << lineNumber;
}

TEST(QueueUART_Test, readBusy){
    Drivers::QueueUART<5, 10> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = true;
    uint8_t data[4] = {2, 6, 2, 7};
    read_Data = data;
    read_Size = 4;
    uint8_t localData[4];
    EXPECT_EQ(uart.read(localData, 4), Drivers::BusStatus::BUSY);
    EXPECT_NE(data[0], localData[0]);
    EXPECT_NE(data[1], localData[1]);
    EXPECT_NE(data[2], localData[2]);
    EXPECT_NE(data[3], localData[3]);
}

TEST(QueueUART_Test, readOK){
    Drivers::QueueUART<5, 10> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t data[4] = {6, 2, 7, 1};
    read_Data = data;
    read_Size = 4;
    uint8_t localData[4];
    EXPECT_EQ(uart.read(localData, 4), Drivers::BusStatus::OK);
    compareArrays(data, localData, 4, __LINE__);
}

TEST(QueueUART_Test, writeBusy){
    Drivers::QueueUART<5, 10> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = true;
    uint8_t data[5] = {1, 5, 7, 2, 4};
    write_Data = data;
    write_Size = 5;
    uint8_t localData[5] = {0, 0, 0, 0, 0};
    EXPECT_EQ(uart.write(localData, 5), Drivers::BusStatus::BUSY);
    EXPECT_NE(data[0], localData[0]);
    EXPECT_NE(data[1], localData[1]);
    EXPECT_NE(data[2], localData[2]);
    EXPECT_NE(data[3], localData[3]);
    EXPECT_NE(data[4], localData[4]);
}

TEST(QueueUART_Test, writeOK){
    Drivers::QueueUART<5, 10> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t data[5] = {5, 8, 21, 5, 6};
    write_Data = data;
    write_Size = 5;
    uint8_t localData[5] = {0, 0, 0, 0, 0};
    EXPECT_EQ(uart.write(localData, 5), Drivers::BusStatus::OK);
    compareArrays(data, localData, 5, __LINE__);
}

TEST(QueueUART_Test, writeDMA_single){
    Drivers::QueueUART<6, 10> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t data[6] = {5, 6, 2, 7, 1, 8};
    writeDMA_Data = data;
    writeDMA_Size = 6;
    uint8_t localData[6] = {0, 0, 0, 0, 0, 0};
    CallBackMock callBackMock(delayUART);
    EXPECT_EQ(uart.writeDMA(localData, 6, (Drivers::DriverIfc*)(&callBackMock)), Drivers::BusStatus::OK);
    uart.handleFinish(nullptr);
    EXPECT_TRUE(callBackMock.wasCalledfinish());
    compareArrays(data, localData, 6, __LINE__);
}

TEST(QueueUART_Test, writeDMA_toMuchData){
    Drivers::QueueUART<10, 4> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t data[5] = {1, 2, 3, 4, 5};
    writeDMA_Data = data;
    writeDMA_Size = 5;
    uint8_t localData[5] = {0, 0, 0, 0, 0};
    CallBackMock callBackMock(delayUART);
    EXPECT_ANY_THROW(uart.writeDMA(localData, 5, (Drivers::DriverIfc*)(&callBackMock)));
    EXPECT_FALSE(callBackMock.wasCalledfinish());
    EXPECT_NE(data[0], localData[0]);
    EXPECT_NE(data[1], localData[1]);
    EXPECT_NE(data[2], localData[2]);
    EXPECT_NE(data[3], localData[3]);
    EXPECT_NE(data[4], localData[4]);
}

TEST(QueueUART_Test, writeDMA_twice){
    Drivers::QueueUART<3, 8> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t localData[2][8] = {{8, 1, 7, 2, 6, 3, 5, 4}, {12, 21, 34, 43, 56, 65, 78, 87}};
    uint8_t data[2][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
    writeDMA_Size = 8;
    CallBackMock callBackMock1(delayUART);
    CallBackMock callBackMock2(delayUART);
    writeDMA_Data = data[0];
    EXPECT_EQ(uart.writeDMA(localData[0], 8, (Drivers::DriverIfc*)(&callBackMock1)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[1], 8, (Drivers::DriverIfc*)(&callBackMock2)), Drivers::BusStatus::OK);
    writeDMA_Data = data[1];
    isBussy_return = false;
    uart.handleFinish(nullptr);
    uart.handleFinish(nullptr);
    EXPECT_TRUE(callBackMock1.wasCalledfinish());
    EXPECT_TRUE(callBackMock2.wasCalledfinish());
    compareArrays(data[0], localData[0], 8, __LINE__);
    compareArrays(data[1], localData[1], 8, __LINE__);
}

TEST(QueueUART_Test, writeDMA_toMuchOperations){
    Drivers::QueueUART<2, 8> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t localData[3][8] = { {8, 1, 7, 2, 6, 3, 5, 4},
                                {12, 21, 34, 43, 56, 65, 78, 87},
                                {1, 1, 1, 1, 1, 1, 1, 1}};
    uint8_t data[3][8] = {  {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0}};
    writeDMA_Size = 8;
    CallBackMock callBackMock1(delayUART);
    CallBackMock callBackMock2(delayUART);
    CallBackMock callBackMock3(delayUART);
    writeDMA_Data = data[0];
    EXPECT_EQ(uart.writeDMA(localData[0], 8, (Drivers::DriverIfc*)(&callBackMock1)), Drivers::BusStatus::OK);
    writeDMA_Data = data[1];
    EXPECT_EQ(uart.writeDMA(localData[1], 8, (Drivers::DriverIfc*)(&callBackMock2)), Drivers::BusStatus::OK);
    EXPECT_ANY_THROW(uart.writeDMA(localData[2], 8, (Drivers::DriverIfc*)(&callBackMock3)));
    EXPECT_FALSE(callBackMock1.wasCalledfinish());
    compareArrays(data[0], localData[0], 8, __LINE__);
    EXPECT_NE(data[1][0], localData[1][0]);
    EXPECT_NE(data[1][1], localData[1][1]);
    EXPECT_NE(data[1][2], localData[1][2]);
    EXPECT_NE(data[1][3], localData[1][3]);
    EXPECT_NE(data[1][4], localData[1][4]);
    EXPECT_NE(data[1][5], localData[1][5]);
    EXPECT_NE(data[1][6], localData[1][6]);
    EXPECT_NE(data[1][7], localData[1][7]);
}

TEST(QueueUART_Test, writeDMA_add_operations_after_get_empty){
    Drivers::QueueUART<4, 8> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t localData[8][7] = { {8, 1, 7, 2, 6, 3, 5},
                                {7, 7, 7, 7, 7, 7, 7},
                                {6, 6, 6, 6, 6, 6, 6},
                                {5, 5, 5, 5, 5, 5, 5},
                                {4, 4, 4, 4, 4, 4, 4},
                                {3, 3, 3, 3, 3, 3, 3},
                                {2, 2, 2, 2, 2, 2, 2},
                                {1, 1, 1, 1, 1, 1, 1}};
    uint8_t data[8][7] = {  {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0}};
    writeDMA_Size = 7;
    CallBackMock callBackMock1(delayUART);
    CallBackMock callBackMock2(delayUART);
    CallBackMock callBackMock3(delayUART);
    CallBackMock callBackMock4(delayUART);
    CallBackMock callBackMock5(delayUART);
    CallBackMock callBackMock6(delayUART);
    CallBackMock callBackMock7(delayUART);
    CallBackMock callBackMock8(delayUART);
    writeDMA_Data = data[0];
    EXPECT_EQ(uart.writeDMA(localData[0], 7, (Drivers::DriverIfc*)(&callBackMock1)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[1], 7, (Drivers::DriverIfc*)(&callBackMock2)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[2], 7, (Drivers::DriverIfc*)(&callBackMock3)), Drivers::BusStatus::OK);
    writeDMA_Data = data[1];
    uart.handleFinish(nullptr);
    writeDMA_Data = data[2];
    uart.handleFinish(nullptr);
    uart.handleFinish(nullptr);
    writeDMA_Data = data[3];
    isBussy_return = false;
    EXPECT_EQ(uart.writeDMA(localData[3], 7, (Drivers::DriverIfc*)(&callBackMock4)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[4], 7, (Drivers::DriverIfc*)(&callBackMock5)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[5], 7, (Drivers::DriverIfc*)(&callBackMock6)), Drivers::BusStatus::OK);
    writeDMA_Data = data[4];
    uart.handleFinish(nullptr);
    writeDMA_Data = data[5];
    uart.handleError(nullptr);
    uart.handleFinish(nullptr);
    writeDMA_Data = data[6];
    isBussy_return = false;
    EXPECT_EQ(uart.writeDMA(localData[6], 7, (Drivers::DriverIfc*)(&callBackMock7)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[7], 7, (Drivers::DriverIfc*)(&callBackMock8)), Drivers::BusStatus::OK);
    isBussy_return = false;
    writeDMA_Data = data[7];
    uart.handleFinish(nullptr);
    uart.handleFinish(nullptr);
    EXPECT_TRUE(callBackMock1.wasCalledfinish());
    EXPECT_TRUE(callBackMock2.wasCalledfinish());
    EXPECT_TRUE(callBackMock3.wasCalledfinish());
    EXPECT_TRUE(callBackMock4.wasCalledfinish());
    EXPECT_TRUE(callBackMock5.wasCallederror());
    EXPECT_TRUE(callBackMock6.wasCalledfinish());
    EXPECT_TRUE(callBackMock7.wasCalledfinish());
    EXPECT_TRUE(callBackMock8.wasCalledfinish());
    compareArrays(data[0], localData[0], 7, __LINE__);
    compareArrays(data[1], localData[1], 7, __LINE__);
    compareArrays(data[2], localData[2], 7, __LINE__);
    compareArrays(data[3], localData[3], 7, __LINE__);
    compareArrays(data[4], localData[4], 7, __LINE__);
    compareArrays(data[5], localData[5], 7, __LINE__);
    compareArrays(data[6], localData[6], 7, __LINE__);
    compareArrays(data[7], localData[7], 7, __LINE__);
}

TEST(QueueUART_Test, writeDMA_add_operations_before_get_empty){
    Drivers::QueueUART<4, 8> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t localData[8][7] = { {8, 1, 7, 2, 6, 3, 5},
                                {7, 7, 7, 7, 7, 7, 7},
                                {6, 6, 6, 6, 6, 6, 6},
                                {5, 5, 5, 5, 5, 5, 5},
                                {4, 4, 4, 4, 4, 4, 4},
                                {3, 3, 3, 3, 3, 3, 3},
                                {2, 2, 2, 2, 2, 2, 2},
                                {1, 1, 1, 1, 1, 1, 1}};
    uint8_t data[8][7] = {  {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0}};
    writeDMA_Size = 7;
    CallBackMock callBackMock1(delayUART);
    CallBackMock callBackMock2(delayUART);
    CallBackMock callBackMock3(delayUART);
    CallBackMock callBackMock4(delayUART);
    CallBackMock callBackMock5(delayUART);
    CallBackMock callBackMock6(delayUART);
    CallBackMock callBackMock7(delayUART);
    CallBackMock callBackMock8(delayUART);
    writeDMA_Data = data[0];
    EXPECT_EQ(uart.writeDMA(localData[0], 7, (Drivers::DriverIfc*)(&callBackMock1)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[1], 7, (Drivers::DriverIfc*)(&callBackMock2)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[2], 7, (Drivers::DriverIfc*)(&callBackMock3)), Drivers::BusStatus::OK);
    writeDMA_Data = data[1];
    uart.handleFinish(nullptr);
    EXPECT_EQ(uart.writeDMA(localData[3], 7, (Drivers::DriverIfc*)(&callBackMock4)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[4], 7, (Drivers::DriverIfc*)(&callBackMock5)), Drivers::BusStatus::OK);
    writeDMA_Data = data[2];
    uart.handleFinish(nullptr);
    writeDMA_Data = data[3];
    uart.handleFinish(nullptr);
    EXPECT_EQ(uart.writeDMA(localData[5], 7, (Drivers::DriverIfc*)(&callBackMock6)), Drivers::BusStatus::OK);
    writeDMA_Data = data[4];
    uart.handleFinish(nullptr);
    writeDMA_Data = data[5];
    uart.handleFinish(nullptr);
    uart.handleFinish(nullptr);
    isBussy_return = false;
    writeDMA_Data = data[6];
    EXPECT_EQ(uart.writeDMA(localData[6], 7, (Drivers::DriverIfc*)(&callBackMock7)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[7], 7, (Drivers::DriverIfc*)(&callBackMock8)), Drivers::BusStatus::OK);
    writeDMA_Data = data[7];
    uart.handleFinish(nullptr);
    uart.handleFinish(nullptr);
    EXPECT_TRUE(callBackMock1.wasCalledfinish());
    EXPECT_TRUE(callBackMock2.wasCalledfinish());
    EXPECT_TRUE(callBackMock3.wasCalledfinish());
    EXPECT_TRUE(callBackMock4.wasCalledfinish());
    EXPECT_TRUE(callBackMock5.wasCalledfinish());
    EXPECT_TRUE(callBackMock6.wasCalledfinish());
    EXPECT_TRUE(callBackMock7.wasCalledfinish());
    EXPECT_TRUE(callBackMock8.wasCalledfinish());
    compareArrays(data[0], localData[0], 7, __LINE__);
    compareArrays(data[1], localData[1], 7, __LINE__);
    compareArrays(data[2], localData[2], 7, __LINE__);
    compareArrays(data[3], localData[3], 7, __LINE__);
    compareArrays(data[4], localData[4], 7, __LINE__);
    compareArrays(data[5], localData[5], 7, __LINE__);
    compareArrays(data[6], localData[6], 7, __LINE__);
    compareArrays(data[7], localData[7], 7, __LINE__);
}

TEST(QueueUART_Test, writeDMA_maxOperation){
    Drivers::QueueUART<2, 8> uart(read, write, writeDMA, isBussy, delayUART);
    isBussy_return = false;
    uint8_t localData[2][8] = {{8, 1, 7, 2, 6, 3, 5, 4}, {12, 21, 34, 43, 56, 65, 78, 87}};
    uint8_t data[2][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
    writeDMA_Size = 8;
    CallBackMock callBackMock1(delayUART);
    CallBackMock callBackMock2(delayUART);
    writeDMA_Data = data[0];
    EXPECT_EQ(uart.writeDMA(localData[0], 8, (Drivers::DriverIfc*)(&callBackMock1)), Drivers::BusStatus::OK);
    EXPECT_EQ(uart.writeDMA(localData[1], 8, (Drivers::DriverIfc*)(&callBackMock2)), Drivers::BusStatus::OK);
    writeDMA_Data = data[1];
    isBussy_return = false;
    uart.handleFinish(nullptr);
    uart.handleFinish(nullptr);
    EXPECT_TRUE(callBackMock1.wasCalledfinish());
    EXPECT_TRUE(callBackMock2.wasCalledfinish());
    compareArrays(data[0], localData[0], 8, __LINE__);
    compareArrays(data[1], localData[1], 8, __LINE__);
}

}