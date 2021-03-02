#include <gtest/gtest.h>
#include "InterfacesConf_drivers_test.hh"
#include <LED.hh>
#include <cstdlib>

using drivers_test::gpio;

TEST(LED_Test, init){
    Drivers::LED led(gpio, drivers_test::HAL_Delay);
    led.addLED(LedsList::initFailure, OutputList::ledPin1, true);
    led.addLED(LedsList::wrongHWID, OutputList::ledPin2, false);
    led.set(LedsList::initFailure, true);
    led.set(LedsList::wrongHWID, true);
    EXPECT_TRUE(gpio.pin(OutputList::ledPin1));
    EXPECT_FALSE(gpio.pin(OutputList::ledPin2));
    led.toggle(LedsList::initFailure);
    led.toggle(LedsList::wrongHWID);
    EXPECT_FALSE(gpio.pin(OutputList::ledPin1));
    EXPECT_TRUE(gpio.pin(OutputList::ledPin2));
    led.set(LedsList::initFailure, true);
    led.set(LedsList::wrongHWID, false);
    EXPECT_TRUE(gpio.pin(OutputList::ledPin1));
    EXPECT_TRUE(gpio.pin(OutputList::ledPin2));
}

TEST(LED_Test, toggleWithDelay){
    Drivers::LED led(gpio, drivers_test::HAL_Delay);
    constexpr unsigned int size = 128;
    led.addLED(LedsList::initFailure, OutputList::ledPin1, true);
    led.addLED(LedsList::wrongHWID, OutputList::ledPin2, false);
    led.set(LedsList::initFailure, true);
    led.set(LedsList::wrongHWID, false);

    EXPECT_TRUE(gpio.pin(OutputList::ledPin1));
    EXPECT_TRUE(gpio.pin(OutputList::ledPin2));

    led.set(LedsList::initFailure, false, size);
    led.set(LedsList::wrongHWID, true, size);
    for(unsigned int i = 0; i < size; i++){
        EXPECT_FALSE(gpio.pin(OutputList::ledPin1)) << "Iteration no. " << i << ".";
        EXPECT_FALSE(gpio.pin(OutputList::ledPin2)) << "Iteration no. " << i << ".";
        led.handleTimeEvent(nullptr);
    }
    EXPECT_TRUE(gpio.pin(OutputList::ledPin1));
    EXPECT_TRUE(gpio.pin(OutputList::ledPin2));
}

TEST(LED_Test, interruptToggleWithDelay){
    Drivers::LED led(gpio, drivers_test::HAL_Delay);
    constexpr unsigned int size = 128;
    constexpr unsigned int interruptTime = 10;
    led.addLED(LedsList::initFailure, OutputList::ledPin1, true);
    led.addLED(LedsList::wrongHWID, OutputList::ledPin2, false);
    led.set(LedsList::initFailure, true);
    led.set(LedsList::wrongHWID, false);

    EXPECT_TRUE(gpio.pin(OutputList::ledPin1));
    EXPECT_TRUE(gpio.pin(OutputList::ledPin2));

    led.set(LedsList::initFailure, false, size);
    led.set(LedsList::wrongHWID, true, size);
    for(unsigned int i = 0; i < interruptTime; i++){
        EXPECT_FALSE(gpio.pin(OutputList::ledPin1)) << "Iteration no. " << i << ".";
        EXPECT_FALSE(gpio.pin(OutputList::ledPin2)) << "Iteration no. " << i << ".";
        led.handleTimeEvent(nullptr);
    }
    led.set(LedsList::initFailure, true);
    led.set(LedsList::wrongHWID, false);
    for(unsigned int i = 0; i < size; i++){
        EXPECT_TRUE(gpio.pin(OutputList::ledPin1));
        EXPECT_TRUE(gpio.pin(OutputList::ledPin2));
        led.handleTimeEvent(nullptr);
    }

    led.set(LedsList::initFailure, false, size);
    led.set(LedsList::wrongHWID, true, size);
    for(unsigned int i = 0; i < interruptTime; i++){
        EXPECT_FALSE(gpio.pin(OutputList::ledPin1)) << "Iteration no. " << i << ".";
        EXPECT_FALSE(gpio.pin(OutputList::ledPin2)) << "Iteration no. " << i << ".";
        led.handleTimeEvent(nullptr);
    }
    led.set(LedsList::initFailure, false);
    led.set(LedsList::wrongHWID, true);
    for(unsigned int i = 0; i < size; i++){
        EXPECT_FALSE(gpio.pin(OutputList::ledPin1)) << "Iteration no. " << i << ".";
        EXPECT_FALSE(gpio.pin(OutputList::ledPin2)) << "Iteration no. " << i << ".";
        led.handleTimeEvent(nullptr);
    }
}
