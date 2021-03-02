#include <BothChannelEngineDriver.hh>
#include <GPIO_mock.hh>
#include <gtest/gtest.h>
#include <stdint.h>

TEST(BothChannelEngineDriver_Test, set){
    GPIO_Mock gpio([](uint32_t)->void{});
    uint16_t maxValue = 1000;
    uint16_t reg;
    gpio.outputs[OutputList::outPinA1] = true;
    gpio.outputs[OutputList::outPinB2] = true;
    Drivers::BothChannelEngineDriver<uint16_t> servo(&gpio, OutputList::outPinA1, OutputList::outPinB2, &reg, maxValue);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    servo.set(10.5);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue * 10.5 / 100);
    servo.set(25.0);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue * 25.0 / 100);
}

TEST(BothChannelEngineDriver_Test, setWithDriveOffset){
    GPIO_Mock gpio([](uint32_t)->void{});
    uint16_t maxValue = 1000;
    uint16_t reg;
    gpio.outputs[OutputList::outPinA1] = true;
    gpio.outputs[OutputList::outPinB2] = true;
    Drivers::BothChannelEngineDriver<uint16_t> servo(&gpio, OutputList::outPinA1, OutputList::outPinB2, &reg, maxValue, maxValue / 2);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    servo.set(11.0);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, (maxValue / 2) * (1.0 + 11.0 / 100.0));
    servo.set(100.0);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue);
    servo.set(50.0);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, (maxValue / 2) * (1.0 + 50.0 / 100.0));
    servo.set(-50.0);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, (maxValue / 2) * (1.0 + 50.0 / 100.0));
    servo.set(0.0);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, 0);
}

TEST(BothChannelEngineDriver_Test, setAbove100){
    GPIO_Mock gpio([](uint32_t)->void{});
    uint16_t maxValue = 1000;
    uint16_t reg;
    gpio.outputs[OutputList::outPinA1] = true;
    gpio.outputs[OutputList::outPinB2] = true;
    Drivers::BothChannelEngineDriver<uint16_t> servo(&gpio, OutputList::outPinA1, OutputList::outPinB2, &reg, maxValue);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    servo.set(10.5);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue * 10.5 / 100);
    servo.set(110.0);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue);
}

TEST(BothChannelEngineDriver_Test, setBelow0){
    GPIO_Mock gpio([](uint32_t)->void{});
    uint16_t maxValue = 1000;
    uint16_t reg;
    gpio.outputs[OutputList::outPinA1] = true;
    gpio.outputs[OutputList::outPinB2] = true;
    Drivers::BothChannelEngineDriver<uint16_t> servo(&gpio, OutputList::outPinA1, OutputList::outPinB2, &reg, maxValue);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    servo.set(-10.5);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue * 10.5 / 100);
    servo.set(-25.0);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue * 25.0 / 100);
}

TEST(BothChannelEngineDriver_Test, setBelow100){
    GPIO_Mock gpio([](uint32_t)->void{});
    uint16_t maxValue = 1000;
    uint16_t reg;
    gpio.outputs[OutputList::outPinA1] = true;
    gpio.outputs[OutputList::outPinB2] = true;
    Drivers::BothChannelEngineDriver<uint16_t> servo(&gpio, OutputList::outPinA1, OutputList::outPinB2, &reg, maxValue);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    servo.set(-10.5);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue * 10.5 / 100);
    servo.set(-125.0);
    EXPECT_FALSE(gpio.outputs[OutputList::outPinA1]);
    EXPECT_TRUE(gpio.outputs[OutputList::outPinB2]);
    EXPECT_EQ(reg, maxValue);
}
