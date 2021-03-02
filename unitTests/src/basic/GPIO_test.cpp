#include <gtest/gtest.h>
#include <InterfacesConf_drivers_test.hh>
#include <cstdlib>

using namespace::drivers_test;
using drivers_test::GpioIrqTest;

namespace drivers_test{
    extern GPIO_TypeDef* GPIOC;
}

TEST(GPIO_Test, InputPins){
    EXPECT_FALSE(gpio.pin(InputList::inPinC14));
    EXPECT_FALSE(gpio.pin(InputList::inPinC15));
    drivers_test::GPIOC->IDR = 0b1000000000000000;
    EXPECT_FALSE(gpio.pin(InputList::inPinC14));
    EXPECT_TRUE(gpio.pin(InputList::inPinC15));
    drivers_test::GPIOC->IDR |= 0b0100000000000000;
    EXPECT_TRUE(gpio.pin(InputList::inPinC14));
    EXPECT_TRUE(gpio.pin(InputList::inPinC15));
    drivers_test::GPIOC->IDR &= 0b0111111111111111;
    EXPECT_TRUE(gpio.pin(InputList::inPinC14));
    EXPECT_FALSE(gpio.pin(InputList::inPinC15));
}

TEST(GPIO_Test, IrqPins){
    GpioIrqTest testObj;
    EXPECT_TRUE(testObj.getWas());
    EXPECT_FALSE(testObj.getWill());

    testObj.callMe();
        gpio.setChangedPin(InterruptInputList::irqPinA11);
        gpio.handleFinish(nullptr);
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinC1);
        gpio.handleFinish(nullptr);
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinB12);
        gpio.handleFinish(nullptr);
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

    gpio.subscribe(InterruptInputList::irqPinB12, &testObj);

        gpio.setChangedPin(InterruptInputList::irqPinB11);
        gpio.handleFinish(nullptr);
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinB12);
        gpio.handleFinish(nullptr);
    EXPECT_TRUE(testObj.getWas());
    EXPECT_FALSE(testObj.getWill());

    testObj.callMe();
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinB13);
        gpio.handleFinish(nullptr);
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinB12);
        gpio.handleFinish(nullptr);
    EXPECT_TRUE(testObj.getWas());
    EXPECT_FALSE(testObj.getWill());

    testObj.callMe();
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

    gpio.subscribe(InterruptInputList::irqPinC10, &testObj);

        gpio.setChangedPin(InterruptInputList::irqPinB12);
        gpio.handleFinish(nullptr);
    EXPECT_TRUE(testObj.getWas());
    EXPECT_FALSE(testObj.getWill());

    testObj.callMe();
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinC10);
        gpio.handleFinish(nullptr);
    EXPECT_TRUE(testObj.getWas());
    EXPECT_FALSE(testObj.getWill());

    testObj.callMe();
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

    gpio.subscribe(InterruptInputList::irqPinB12, nullptr);

        gpio.setChangedPin(InterruptInputList::irqPinB12);
        gpio.handleFinish(nullptr);
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinC1);
        gpio.handleFinish(nullptr);
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());

        gpio.setChangedPin(InterruptInputList::irqPinC10);
        gpio.handleFinish(nullptr);
    EXPECT_TRUE(testObj.getWas());
    EXPECT_FALSE(testObj.getWill());

    testObj.callMe();
    EXPECT_FALSE(testObj.getWas());
    EXPECT_TRUE(testObj.getWill());
}

TEST(GPIO_Test, OutputPins){
    gpio.pin(OutputList::outPinB9, true);
    gpio.pin(OutputList::outPinC0, true);
    EXPECT_FALSE(gpio.pin(OutputList::outPinA1));
    EXPECT_FALSE(gpio.pin(OutputList::outPinC15));
    EXPECT_FALSE(gpio.pin(OutputList::outPinE3));
    EXPECT_FALSE(gpio.pin(OutputList::outPinD5));
    EXPECT_FALSE(gpio.pin(OutputList::outPinB2));
    EXPECT_FALSE(gpio.pin(OutputList::outPinA10));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinB2, true);
    EXPECT_FALSE(gpio.pin(OutputList::outPinA1));
    EXPECT_FALSE(gpio.pin(OutputList::outPinC15));
    EXPECT_FALSE(gpio.pin(OutputList::outPinE3));
    EXPECT_FALSE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_FALSE(gpio.pin(OutputList::outPinA10));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinE3, true);
    EXPECT_FALSE(gpio.pin(OutputList::outPinA1));
    EXPECT_FALSE(gpio.pin(OutputList::outPinC15));
    EXPECT_TRUE(gpio.pin(OutputList::outPinE3));
    EXPECT_FALSE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_FALSE(gpio.pin(OutputList::outPinA10));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinC15, true);
    EXPECT_FALSE(gpio.pin(OutputList::outPinA1));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC15));
    EXPECT_TRUE(gpio.pin(OutputList::outPinE3));
    EXPECT_FALSE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_FALSE(gpio.pin(OutputList::outPinA10));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinD5, true);
    EXPECT_FALSE(gpio.pin(OutputList::outPinA1));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC15));
    EXPECT_TRUE(gpio.pin(OutputList::outPinE3));
    EXPECT_TRUE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_FALSE(gpio.pin(OutputList::outPinA10));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinB9, false);
    EXPECT_FALSE(gpio.pin(OutputList::outPinA1));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC15));
    EXPECT_TRUE(gpio.pin(OutputList::outPinE3));
    EXPECT_TRUE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_FALSE(gpio.pin(OutputList::outPinA10));
    EXPECT_FALSE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinA10, true);
    EXPECT_FALSE(gpio.pin(OutputList::outPinA1));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC15));
    EXPECT_TRUE(gpio.pin(OutputList::outPinE3));
    EXPECT_TRUE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_TRUE(gpio.pin(OutputList::outPinA10));
    EXPECT_FALSE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinA1, true);
    EXPECT_TRUE(gpio.pin(OutputList::outPinA1));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC15));
    EXPECT_TRUE(gpio.pin(OutputList::outPinE3));
    EXPECT_TRUE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_TRUE(gpio.pin(OutputList::outPinA10));
    EXPECT_FALSE(gpio.pin(OutputList::outPinB9));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinC0, false);
    EXPECT_TRUE(gpio.pin(OutputList::outPinA1));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC15));
    EXPECT_TRUE(gpio.pin(OutputList::outPinE3));
    EXPECT_TRUE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_TRUE(gpio.pin(OutputList::outPinA10));
    EXPECT_FALSE(gpio.pin(OutputList::outPinB9));
    EXPECT_FALSE(gpio.pin(OutputList::outPinC0));
    gpio.pin(OutputList::outPinE3, false);
    gpio.pin(OutputList::outPinC0, true);
    gpio.pin(OutputList::outPinC0, false);
    EXPECT_TRUE(gpio.pin(OutputList::outPinA1));
    EXPECT_TRUE(gpio.pin(OutputList::outPinC15));
    EXPECT_FALSE(gpio.pin(OutputList::outPinE3));
    EXPECT_TRUE(gpio.pin(OutputList::outPinD5));
    EXPECT_TRUE(gpio.pin(OutputList::outPinB2));
    EXPECT_TRUE(gpio.pin(OutputList::outPinA10));
    EXPECT_FALSE(gpio.pin(OutputList::outPinB9));
    EXPECT_FALSE(gpio.pin(OutputList::outPinC0));
}