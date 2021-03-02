#include <InterfacesConf_drivers_test.hh>
#include <cstdlib>

using drivers_test::spi;
using drivers_test::hspi;
using drivers_test::HAL_StatusTypeDef;
using drivers_test::HAL_SPI_StateTypeDef;

TEST(SPI_Test, readStatus){
    hspi.spiIsBussy = true;
    EXPECT_TRUE(spi.isBussy());
    hspi.spiIsBussy = false;
    EXPECT_FALSE(spi.isBussy());
}

TEST(SPI_Test, readWriteWithDMA){
    drivers_test::SPIIrqTest irqObj;
    uint8_t dataIn[10], dataOut[10];
    OutputList ssPin = OutputList::spiSlaveSelect;
    drivers_test::gpio.pin(OutputList::spiSlaveSelect, true);
    hspi.clear();
    for(unsigned int i = 0; i < 10; i++){
        hspi.dataIn.push_back(std::rand());
        dataOut[i] = std::rand();
    }
    irqObj.callMe();
    EXPECT_TRUE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_EQ(spi.readWrite(ssPin, dataOut, dataIn, 10, &irqObj), Drivers::BusStatus::OK);
    EXPECT_FALSE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    spi.handleFinish(nullptr);
    EXPECT_TRUE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(hspi.size, 10);
    EXPECT_FALSE(hspi.pinState);
    EXPECT_EQ(hspi.dataOut.size(), 10);
    EXPECT_TRUE(hspi.spiIsBussy);
    EXPECT_TRUE(hspi.dma);
    EXPECT_EQ(hspi.Timeout, 0);
    for(unsigned int i = 0; i < 10; i++){
        EXPECT_EQ(hspi.dataOut[i], dataOut[i]);
        EXPECT_EQ(hspi.dataIn[i], dataIn[i]);
    }
    hspi.clear();

    uint8_t dataIn2[100], dataOut2[100];
    drivers_test::gpio.pin(OutputList::spiSlaveSelect, true);
    for(unsigned int i = 0; i < 100; i++){
        hspi.dataIn.push_back(std::rand());
        dataOut2[i] = std::rand();
    }
    hspi.spiIsBussy = true;
    irqObj.callMe();
    EXPECT_TRUE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_TRUE(hspi.pinState);
    EXPECT_EQ(spi.readWrite(ssPin, dataOut2, dataIn2, 100, &irqObj), Drivers::BusStatus::BUSY);
    EXPECT_TRUE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_TRUE(hspi.pinState);
    spi.handleFinish(nullptr);
    EXPECT_TRUE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_TRUE(hspi.pinState);
    EXPECT_FALSE(irqObj.getWas());
    EXPECT_TRUE(irqObj.getWill());
    hspi.spiIsBussy = false;
    EXPECT_TRUE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_TRUE(hspi.pinState);
    EXPECT_EQ(spi.readWrite(ssPin, dataOut2, dataIn2, 92, &irqObj), Drivers::BusStatus::OK);
    EXPECT_FALSE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_FALSE(hspi.pinState);
    spi.handleFinish(nullptr);
    EXPECT_TRUE(drivers_test::gpio.pin(OutputList::spiSlaveSelect));
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(hspi.size, 92);
    EXPECT_FALSE(hspi.pinState);
    EXPECT_EQ(hspi.dataOut.size(), 92);
    EXPECT_TRUE(hspi.spiIsBussy);
    EXPECT_TRUE(hspi.dma);
    EXPECT_EQ(hspi.Timeout, 0);
    for(unsigned int i = 0; i < 92; i++){
        EXPECT_EQ(hspi.dataOut[i], dataOut2[i]);
        EXPECT_EQ(hspi.dataIn[i], dataIn2[i]);
    }
    hspi.clear();
}

TEST(SPI_Test, readWriteWithoutDMA){
    uint8_t dataIn[10], dataOut[10];
    OutputList ssPin = OutputList::spiSlaveSelect;
    drivers_test::gpio.pin(OutputList::spiSlaveSelect, true);
    hspi.clear();
    for(unsigned int i = 0; i < 10; i++){
        hspi.dataIn.push_back(std::rand());
        dataOut[i] = std::rand();
    }
    EXPECT_EQ(spi.readWrite(ssPin, dataOut, dataIn, 10), Drivers::BusStatus::OK);
    EXPECT_EQ(hspi.size, 10);
    EXPECT_FALSE(hspi.pinState);
    EXPECT_EQ(hspi.dataOut.size(), 10);
    EXPECT_FALSE(hspi.spiIsBussy);
    EXPECT_FALSE(hspi.dma);
    EXPECT_EQ(hspi.Timeout, 100);
    for(unsigned int i = 0; i < 10; i++){
        EXPECT_EQ(hspi.dataOut[i], dataOut[i]);
        EXPECT_EQ(hspi.dataIn[i], dataIn[i]);
    }
    hspi.clear();

    uint8_t dataIn2[100], dataOut2[100];
    drivers_test::gpio.pin(OutputList::spiSlaveSelect, true);
    for(unsigned int i = 0; i < 100; i++){
        hspi.dataIn.push_back(std::rand());
        dataOut2[i] = std::rand();
    }
    hspi.spiIsBussy = true;
    EXPECT_TRUE(hspi.pinState);
    EXPECT_EQ(spi.readWrite(ssPin, dataOut2, dataIn2, 100), Drivers::BusStatus::BUSY);
    hspi.spiIsBussy = false;
    EXPECT_TRUE(hspi.pinState);
    EXPECT_EQ(spi.readWrite(ssPin, dataOut2, dataIn2, 92), Drivers::BusStatus::OK);
    EXPECT_FALSE(hspi.pinState);
    EXPECT_EQ(hspi.size, 92);
    EXPECT_EQ(hspi.dataOut.size(), 92);
    EXPECT_FALSE(hspi.spiIsBussy);
    EXPECT_FALSE(hspi.dma);
    EXPECT_EQ(hspi.Timeout, 100);
    for(unsigned int i = 0; i < 92; i++){
        EXPECT_EQ(hspi.dataOut[i], dataOut2[i]);
        EXPECT_EQ(hspi.dataIn[i], dataIn2[i]);
    }
    hspi.clear();
}
