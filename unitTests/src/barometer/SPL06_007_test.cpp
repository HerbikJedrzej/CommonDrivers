#include <gtest/gtest.h>
#include <cstdlib>
#include <SPL06_007.hh>
#include <GPIO_mock.hh>
#include <I2C_mock.hh>

#define SPL06_007_ADDR 0xEC
namespace {
I2CmockCell getRealCoefficients(){
    return I2CmockCell{SPL06_007_ADDR, 0x10, {
        0xc,
        0x7f,
        0x3,
        0x12,
        0xca,
        0xcf,
        0x3e,
        0xf5,
        0xf6,
        0xb0,
        0x4,
        0xe3,
        0xe3,
        0x91,
        0x0,
        0x53,
        0xfc,
        0xd4
    }};
}

I2CmockCell getRealPreassure(){
    return I2CmockCell{SPL06_007_ADDR, 0x00, {
        0xf8,
        0x1d,
        0xaa
    }};
}

I2CmockCell getRealTemperature(){
    return I2CmockCell{SPL06_007_ADDR, 0x03, {
        0x2,
        0x52,
        0x2
    }};
}

std::vector<I2CmockCell> getReadInitSequence(){
    return std::vector<I2CmockCell>{
        {SPL06_007_ADDR, 0x0D, {0x10}},
        {SPL06_007_ADDR, 0x08, {0b01000000}},
        {SPL06_007_ADDR, 0x08, {0b10000000}},
        getRealCoefficients()
    };
}
std::vector<I2CmockCell> getWriteInitSequence(){
    uint8_t temperatureOversampling = 0;
    uint8_t preasureOversampling = 6;
    uint8_t preasureRate = 2;
    uint8_t temperatureRate = 2;
    uint8_t data = 0b10110000;
    if(temperatureOversampling > 3)
        data |= 0b00001000;
    if(preasureOversampling > 3)
        data |= 0b00000100;
    return std::vector<I2CmockCell>{
        {SPL06_007_ADDR, 0x0C, {0b00001001}},
        {SPL06_007_ADDR, 0x06, {uint8_t((preasureRate << 4) | preasureOversampling)}},
        {SPL06_007_ADDR, 0x07, {uint8_t(0x80 | (temperatureRate << 4) | temperatureOversampling)}},
        {SPL06_007_ADDR, 0x08, {7}},
        {SPL06_007_ADDR, 0x09, {data}}
    };
}
}

TEST(SPL06_007_Test, init){
    I2C_Mock i2c([](uint32_t)->void{});
    GPIO_Mock gpio([](uint32_t)->void{});
    bool flag;
    Drivers::SPL06_007 barometer(&i2c, SPL06_007_ADDR, flag, [](uint32_t)->void{}, InterruptInputList::SPL06_007_IrqPin, &gpio);
    EXPECT_TRUE(i2c.empty());
    i2c.setReadingBuffor(getReadInitSequence());
    barometer.init();
    i2c.checkWritedBuffor("Init case", getWriteInitSequence());
    EXPECT_TRUE(i2c.empty());
}

TEST(SPL06_007_Test, getTemperature){
    I2C_Mock i2c([](uint32_t)->void{});
    GPIO_Mock gpio([](uint32_t)->void{});
    bool flag;
    Drivers::SPL06_007 barometer(&i2c, SPL06_007_ADDR, flag, [](uint32_t)->void{}, InterruptInputList::SPL06_007_IrqPin, &gpio);
    EXPECT_TRUE(i2c.empty());
    i2c.setReadingBuffor(getReadInitSequence());
    barometer.init();
    i2c.checkWritedBuffor("Init case", getWriteInitSequence());
    EXPECT_TRUE(i2c.empty());
    i2c.setReadingBuffor({
        {SPL06_007_ADDR, 0x0A, {2}},
        getRealTemperature()
    });
    gpio.setChangedPin(InterruptInputList::SPL06_007_IrqPin);
    gpio.handleFinish(nullptr);
    EXPECT_TRUE(i2c.empty());
    EXPECT_NEAR(barometer.getTemperature(), 26.1192, 0.0001);
}

TEST(SPL06_007_Test, getPreassure){
    I2C_Mock i2c([](uint32_t)->void{});
    GPIO_Mock gpio([](uint32_t)->void{});
    bool flag;
    Drivers::SPL06_007 barometer(&i2c, SPL06_007_ADDR, flag, [](uint32_t)->void{}, InterruptInputList::SPL06_007_IrqPin, &gpio);
    EXPECT_TRUE(i2c.empty());
    i2c.setReadingBuffor(getReadInitSequence());
    barometer.init();
    i2c.checkWritedBuffor("Init case", getWriteInitSequence());
    EXPECT_TRUE(i2c.empty());
    i2c.setReadingBuffor({
        {SPL06_007_ADDR, 0x0A, {1}},
        getRealPreassure()
    });
    gpio.setChangedPin(InterruptInputList::SPL06_007_IrqPin);
    gpio.handleFinish(nullptr);
    EXPECT_TRUE(i2c.empty());
    EXPECT_NEAR(barometer.getPreasure(), 99819.4499, 0.0001);
}

TEST(SPL06_007_Test, getAltitude){
    I2C_Mock i2c([](uint32_t)->void{});
    GPIO_Mock gpio([](uint32_t)->void{});
    bool flag;
    Drivers::SPL06_007 barometer(&i2c, SPL06_007_ADDR, flag, [](uint32_t)->void{}, InterruptInputList::SPL06_007_IrqPin, &gpio);
    EXPECT_TRUE(i2c.empty());
    i2c.setReadingBuffor(getReadInitSequence());
    barometer.init();
    i2c.checkWritedBuffor("Init case", getWriteInitSequence());
    EXPECT_TRUE(i2c.empty());
    for(uint8_t i = 1; i <= 7; i++){
        i2c.setReadingBuffor({
            {SPL06_007_ADDR, 0x0A, {2}},
            getRealTemperature()
        });
        gpio.setChangedPin(InterruptInputList::SPL06_007_IrqPin);
        gpio.handleFinish(nullptr);
        EXPECT_TRUE(i2c.empty());
        EXPECT_NEAR(barometer.getAltitude(), 2331.1342 * i / 7, 0.0001);
    }
}

TEST(SPL06_007_Test, checkCoefficients){
    I2C_Mock i2c([](uint32_t)->void{});
    GPIO_Mock gpio([](uint32_t)->void{});
    bool flag;
    Drivers::SPL06_007 barometer(&i2c, SPL06_007_ADDR, flag, [](uint32_t)->void{}, InterruptInputList::SPL06_007_IrqPin, &gpio);
    EXPECT_TRUE(i2c.empty());
    i2c.setReadingBuffor(getReadInitSequence());
    barometer.init();
    i2c.checkWritedBuffor("Init case", getWriteInitSequence());
    EXPECT_TRUE(i2c.empty());
    const auto& c = barometer.coefficients;
    EXPECT_EQ(uint16_t(c.c0),  0x00c7);
    EXPECT_EQ(uint16_t(c.c1),  0xff03);
    EXPECT_EQ(uint32_t(c.c00), 0x12cac);
    EXPECT_EQ(uint32_t(c.c10), 0xffff3ef5);
    EXPECT_EQ(uint16_t(c.c01), 0xf6b0);
    EXPECT_EQ(uint16_t(c.c11), 0x04e3);
    EXPECT_EQ(uint16_t(c.c20), 0xe391);
    EXPECT_EQ(uint16_t(c.c21), 0x0053);
    EXPECT_EQ(uint16_t(c.c30), 0xfcd4);
}
