#include <InterfacesConf_drivers_test.hh>
#include <cstdlib>

using drivers_test::uart;
using drivers_test::huart;
using drivers_test::HAL_StatusTypeDef;
using drivers_test::HAL_UART_StateTypeDef;

TEST(UART_Test, readStatus){
    huart.uartIsBussy = true;
    EXPECT_TRUE(uart.isBussy());
    huart.uartIsBussy = false;
    EXPECT_FALSE(uart.isBussy());
}

TEST(UART_Test, writeWithDMA){
    drivers_test::UARTIrqTest irqObj;
    uint8_t dataOut[10];
    huart.clear();
    for(unsigned int i = 0; i < 10; i++){
        dataOut[i] = std::rand();
    }
    irqObj.callMe();
    EXPECT_EQ(uart.writeDMA(dataOut, 10, &irqObj), Drivers::BusStatus::OK);
    uart.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(huart.size, 10);
    EXPECT_EQ(huart.dataOut.size(), 10);
    EXPECT_TRUE(huart.uartIsBussy);
    EXPECT_TRUE(huart.dma);
    EXPECT_EQ(huart.Timeout, 0);
    for(unsigned int i = 0; i < 10; i++){
        EXPECT_EQ(huart.dataOut[i], dataOut[i]);
    }
    huart.clear();

    uint8_t dataOut2[100];
    for(unsigned int i = 0; i < 100; i++){
        dataOut2[i] = std::rand();
    }
    huart.uartIsBussy = true;
    irqObj.callMe();
    EXPECT_EQ(uart.writeDMA(dataOut2, 100, &irqObj), Drivers::BusStatus::BUSY);
    uart.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWas());
    EXPECT_TRUE(irqObj.getWill());
    huart.uartIsBussy = false;
    EXPECT_EQ(uart.writeDMA(dataOut2, 92, &irqObj), Drivers::BusStatus::OK);
    uart.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(huart.size, 92);
    EXPECT_EQ(huart.dataOut.size(), 92);
    EXPECT_TRUE(huart.uartIsBussy);
    EXPECT_TRUE(huart.dma);
    EXPECT_EQ(huart.Timeout, 0);
    for(unsigned int i = 0; i < 92; i++){
        EXPECT_EQ(huart.dataOut[i], dataOut2[i]);
    }
    huart.clear();
}

TEST(UART_Test, writeWithDMAwithoutAlocate){
    drivers_test::UARTIrqTest irqObj;
    uint8_t dataOut[10];
    huart.clear();
    for(unsigned int i = 0; i < 10; i++){
        dataOut[i] = std::rand();
    }
    irqObj.callMe();
    EXPECT_EQ(uart.writeDMAwithoutAlocate(dataOut, 10, &irqObj), Drivers::BusStatus::OK);
    uart.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(huart.size, 10);
    EXPECT_EQ(huart.dataOut.size(), 10);
    EXPECT_TRUE(huart.uartIsBussy);
    EXPECT_TRUE(huart.dma);
    EXPECT_EQ(huart.Timeout, 0);
    for(unsigned int i = 0; i < 10; i++){
        EXPECT_EQ(huart.dataOut[i], dataOut[i]);
    }
    huart.clear();

    uint8_t dataOut2[100];
    for(unsigned int i = 0; i < 100; i++){
        dataOut2[i] = std::rand();
    }
    huart.uartIsBussy = true;
    irqObj.callMe();
    EXPECT_EQ(uart.writeDMAwithoutAlocate(dataOut2, 100, &irqObj), Drivers::BusStatus::BUSY);
    uart.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWas());
    EXPECT_TRUE(irqObj.getWill());
    huart.uartIsBussy = false;
    EXPECT_EQ(uart.writeDMAwithoutAlocate(dataOut2, 92, &irqObj), Drivers::BusStatus::OK);
    uart.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(huart.size, 92);
    EXPECT_EQ(huart.dataOut.size(), 92);
    EXPECT_TRUE(huart.uartIsBussy);
    EXPECT_TRUE(huart.dma);
    EXPECT_EQ(huart.Timeout, 0);
    for(unsigned int i = 0; i < 92; i++){
        EXPECT_EQ(huart.dataOut[i], dataOut2[i]);
    }
    huart.clear();
}

TEST(UART_Test, write){
    uint8_t dataOut[10];
    huart.clear();
    for(unsigned int i = 0; i < 10; i++){
        dataOut[i] = std::rand();
    }
    EXPECT_EQ(uart.write(dataOut, 10), Drivers::BusStatus::OK);
    EXPECT_EQ(huart.size, 10);
    EXPECT_EQ(huart.dataOut.size(), 10);
    EXPECT_FALSE(huart.uartIsBussy);
    EXPECT_FALSE(huart.dma);
    EXPECT_EQ(huart.Timeout, 100);
    for(unsigned int i = 0; i < 10; i++){
        EXPECT_EQ(huart.dataOut[i], dataOut[i]);
    }
    huart.clear();

    uint8_t dataOut2[100];
    for(unsigned int i = 0; i < 100; i++){
        huart.dataIn.push_back(std::rand());
        dataOut2[i] = std::rand();
    }
    huart.uartIsBussy = true;
    EXPECT_EQ(uart.write(dataOut2, 100), Drivers::BusStatus::BUSY);
    huart.uartIsBussy = false;
    EXPECT_EQ(uart.write(dataOut2, 92), Drivers::BusStatus::OK);
    EXPECT_EQ(huart.size, 92);
    EXPECT_EQ(huart.dataOut.size(), 92);
    EXPECT_FALSE(huart.uartIsBussy);
    EXPECT_FALSE(huart.dma);
    EXPECT_EQ(huart.Timeout, 100);
    for(unsigned int i = 0; i < 92; i++){
        EXPECT_EQ(huart.dataOut[i], dataOut2[i]);
    }
    huart.clear();
}

TEST(UART_Test, read){
    uint8_t dataIn[10];
    huart.clear();
    for(unsigned int i = 0; i < 10; i++){
        huart.dataIn.push_back(std::rand());
    }
    EXPECT_EQ(uart.read(dataIn, 10), Drivers::BusStatus::OK);
    EXPECT_EQ(huart.size, 10);
    EXPECT_EQ(huart.dataIn.size(), 10);
    EXPECT_FALSE(huart.uartIsBussy);
    EXPECT_FALSE(huart.dma);
    EXPECT_EQ(huart.Timeout, 100);
    for(unsigned int i = 0; i < 10; i++){
        EXPECT_EQ(huart.dataIn[i], dataIn[i]);
    }
    huart.clear();

    uint8_t dataIn2[100];
    for(unsigned int i = 0; i < 100; i++){
        huart.dataIn.push_back(std::rand());
    }
    huart.uartIsBussy = true;
    EXPECT_EQ(uart.read(dataIn2, 100), Drivers::BusStatus::BUSY);
    huart.uartIsBussy = false;
    EXPECT_EQ(uart.read(dataIn2, 92), Drivers::BusStatus::OK);
    EXPECT_EQ(huart.size, 92);
    EXPECT_FALSE(huart.uartIsBussy);
    EXPECT_FALSE(huart.dma);
    EXPECT_EQ(huart.Timeout, 100);
    for(unsigned int i = 0; i < 92; i++){
        EXPECT_EQ(huart.dataIn[i], dataIn2[i]);
    }
    huart.clear();
}
