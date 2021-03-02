#include <gtest/gtest.h>
#include <InterfacesConf_drivers_test.hh>
#include <cstdlib>

using drivers_test::i2c;
using drivers_test::hi2c;
using drivers_test::HAL_StatusTypeDef;
using drivers_test::HAL_I2C_StateTypeDef;

TEST(I2C_Test, readStatus){
    hi2c.i2cIsBussy = true;
    EXPECT_TRUE(i2c.isBussy());
    hi2c.i2cIsBussy = false;
    EXPECT_FALSE(i2c.isBussy());
}

TEST(I2C_Test, writeWithDMA){
    drivers_test::I2CIrqTest irqObj;
    uint8_t data[10];
    for(unsigned int i = 0; i < 10; i++)
        data[i] = std::rand();
    hi2c.clear();
    irqObj.callMe();
    EXPECT_EQ(i2c.write(0x36, 0x3512, true, data, 10, &irqObj), Drivers::BusStatus::OK);
    i2c.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(hi2c.DevAddress, 0x36);
    EXPECT_EQ(hi2c.MemAddSize, 2);
    EXPECT_EQ(hi2c.MemAddress, 0x3512);
    EXPECT_EQ(hi2c.data.size(), 10);
    EXPECT_TRUE(hi2c.i2cIsBussy);
    EXPECT_TRUE(hi2c.dma);
    EXPECT_EQ(hi2c.Timeout, 0);
    for(unsigned int i = 0; i < 10; i++)
        EXPECT_EQ(hi2c.data[i], data[i]);

    uint8_t data2[100];
    for(unsigned int i = 0; i < 100; i++)
        data2[i] = std::rand();
    hi2c.clear();
    hi2c.i2cIsBussy = true;
    irqObj.callMe();
    EXPECT_EQ(i2c.write(0x08, 0x3001, false, data2, 100, &irqObj), Drivers::BusStatus::BUSY);
    i2c.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWas());
    EXPECT_TRUE(irqObj.getWill());
    hi2c.i2cIsBussy = false;
    EXPECT_EQ(i2c.write(0x98, 0x3211, false, data2, 92, &irqObj), Drivers::BusStatus::OK);
    i2c.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(hi2c.DevAddress, 0x98);
    EXPECT_EQ(hi2c.MemAddSize, 1);
    EXPECT_EQ(hi2c.MemAddress, 0x3211);
    EXPECT_EQ(hi2c.data.size(), 92);
    EXPECT_TRUE(hi2c.i2cIsBussy);
    EXPECT_TRUE(hi2c.dma);
    EXPECT_EQ(hi2c.Timeout, 0);
    for(unsigned int i = 0; i < 92; i++)
        EXPECT_EQ(hi2c.data[i], data2[i]);
}

TEST(I2C_Test, readWithDMA){
    drivers_test::I2CIrqTest irqObj;
    uint8_t data[10];
    hi2c.clear();
    irqObj.callMe();
    for(unsigned int i = 0; i < 10; i++)
        hi2c.data.push_back(std::rand());
    EXPECT_EQ(i2c.read(0x16, 0x3212, true, data, 10, &irqObj), Drivers::BusStatus::OK);
    i2c.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(hi2c.DevAddress, 0x16);
    EXPECT_EQ(hi2c.MemAddSize, 2);
    EXPECT_EQ(hi2c.MemAddress, 0x3212);
    EXPECT_EQ(hi2c.data.size(), 10);
    EXPECT_TRUE(hi2c.dma);
    EXPECT_TRUE(hi2c.i2cIsBussy);
    EXPECT_EQ(hi2c.Timeout, 0);
    for(unsigned int i = 0; i < 10; i++)
        EXPECT_EQ(hi2c.data[i], data[i]);

    uint8_t data2[100];
    hi2c.clear();
    irqObj.callMe();
    for(unsigned int i = 0; i < 92; i++)
        hi2c.data.push_back(std::rand());
    hi2c.i2cIsBussy = true;
    EXPECT_EQ(i2c.read(0x08, 0x3001, false, data2, 100, &irqObj), Drivers::BusStatus::BUSY);
    i2c.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWas());
    EXPECT_TRUE(irqObj.getWill());
    hi2c.i2cIsBussy = false;
    EXPECT_EQ(i2c.read(0x08, 0x3001, false, data2, 100, &irqObj), Drivers::BusStatus::OK);
    i2c.handleFinish(nullptr);
    EXPECT_FALSE(irqObj.getWill());
    EXPECT_TRUE(irqObj.getWas());
    EXPECT_EQ(hi2c.DevAddress, 0x08);
    EXPECT_EQ(hi2c.MemAddSize, 1);
    EXPECT_EQ(hi2c.MemAddress, 0x3001);
    EXPECT_EQ(hi2c.data.size(), 92);
    EXPECT_TRUE(hi2c.dma);
    EXPECT_TRUE(hi2c.i2cIsBussy);
    EXPECT_EQ(hi2c.Timeout, 0);
    for(unsigned int i = 0; i < 92; i++)
        EXPECT_EQ(hi2c.data[i], data2[i]);
    for(unsigned int i = 93; i < 100; i++)
        EXPECT_EQ(data2[i], 255);
}

TEST(I2C_Test, writeWithoutDMA){
    uint8_t data[10];
    for(unsigned int i = 0; i < 10; i++)
        data[i] = std::rand();
    hi2c.clear();
    EXPECT_EQ(i2c.write(0x36, 0x3512, true, data, 10), Drivers::BusStatus::OK);
    EXPECT_EQ(hi2c.DevAddress, 0x36);
    EXPECT_EQ(hi2c.MemAddSize, 2);
    EXPECT_EQ(hi2c.MemAddress, 0x3512);
    EXPECT_EQ(hi2c.data.size(), 10);
    EXPECT_FALSE(hi2c.dma);
    EXPECT_FALSE(hi2c.i2cIsBussy);
    EXPECT_EQ(hi2c.Timeout, 100);
    for(unsigned int i = 0; i < 10; i++)
        EXPECT_EQ(hi2c.data[i], data[i]);

    uint8_t data2[100];
    for(unsigned int i = 0; i < 100; i++)
        data2[i] = std::rand();
    hi2c.clear();
    hi2c.i2cIsBussy = true;
    EXPECT_EQ(i2c.write(0x08, 0x3001, false, data2, 100), Drivers::BusStatus::BUSY);
    hi2c.i2cIsBussy = false;
    EXPECT_EQ(i2c.write(0x98, 0x3211, false, data2, 92), Drivers::BusStatus::OK);
    EXPECT_EQ(hi2c.DevAddress, 0x98);
    EXPECT_EQ(hi2c.MemAddSize, 1);
    EXPECT_EQ(hi2c.MemAddress, 0x3211);
    EXPECT_EQ(hi2c.data.size(), 92);
    EXPECT_FALSE(hi2c.dma);
    EXPECT_FALSE(hi2c.i2cIsBussy);
    EXPECT_EQ(hi2c.Timeout, 100);
    for(unsigned int i = 0; i < 92; i++)
        EXPECT_EQ(hi2c.data[i], data2[i]);
}

TEST(I2C_Test, readWithoutDMA){
    uint8_t data[10];
    hi2c.clear();
    for(unsigned int i = 0; i < 10; i++)
        hi2c.data.push_back(std::rand());
    EXPECT_EQ(i2c.read(0x16, 0x3212, true, data, 10), Drivers::BusStatus::OK);
    EXPECT_EQ(hi2c.DevAddress, 0x16);
    EXPECT_EQ(hi2c.MemAddSize, 2);
    EXPECT_EQ(hi2c.MemAddress, 0x3212);
    EXPECT_EQ(hi2c.data.size(), 10);
    EXPECT_FALSE(hi2c.dma);
    EXPECT_FALSE(hi2c.i2cIsBussy);
    EXPECT_EQ(hi2c.Timeout, 100);
    for(unsigned int i = 0; i < 10; i++)
        EXPECT_EQ(hi2c.data[i], data[i]);

    uint8_t data2[100];
    hi2c.clear();
    for(unsigned int i = 0; i < 92; i++)
        hi2c.data.push_back(std::rand());
    hi2c.i2cIsBussy = true;
    EXPECT_EQ(i2c.read(0x08, 0x3001, false, data2, 100), Drivers::BusStatus::BUSY);
    hi2c.i2cIsBussy = false;
    EXPECT_EQ(i2c.read(0x08, 0x3001, false, data2, 100), Drivers::BusStatus::OK);
    EXPECT_EQ(hi2c.DevAddress, 0x08);
    EXPECT_EQ(hi2c.MemAddSize, 1);
    EXPECT_EQ(hi2c.MemAddress, 0x3001);
    EXPECT_EQ(hi2c.data.size(), 92);
    EXPECT_FALSE(hi2c.dma);
    EXPECT_FALSE(hi2c.i2cIsBussy);
    EXPECT_EQ(hi2c.Timeout, 100);
    for(unsigned int i = 0; i < 92; i++)
        EXPECT_EQ(hi2c.data[i], data2[i]);
    for(unsigned int i = 93; i < 100; i++)
        EXPECT_EQ(data2[i], 255);
}