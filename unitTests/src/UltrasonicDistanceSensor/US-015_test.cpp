#include <gtest/gtest.h>
#include <US-015.hh>
#include <GPIO_mock.hh>

using Drivers::Timer;

#define EXPECT_NEAR_PRECISION 0.0001

namespace {
    void US_015_Test_delay(uint32_t){}

    double calculateDistanceInMiliMeters(const unsigned int & timeDiffInMicroSeconds){
        return timeDiffInMicroSeconds * 0.001 * 340 / 2;
    }
    void nextLoop(Timer& timer, GPIO_Mock& gpio){
        timer.handleTimeEvent(nullptr);
        gpio.handleTimeEvent(nullptr);
    }
    void startUltrasonicMeasure(GPIO_Mock& gpio){
        gpio.inputs[InputList::Echo] = true;
        gpio.setChangedPin(InterruptInputList::EchoIrq);
        gpio.handleFinish(nullptr);
    }
    void finishUltrasonicMeasrue(GPIO_Mock& gpio){
        gpio.inputs[InputList::Echo] = false;
        gpio.setChangedPin(InterruptInputList::EchoIrq);
        gpio.handleFinish(nullptr);
    }
}

TEST(US_015_Test, notInit){
    GPIO_Mock gpio(US_015_Test_delay);
    uint32_t counter = 0;
    Timer timer(100000000, 10000, 10, US_015_Test_delay);
    Timer::TimeStamp timeStamp = timer.getTimeStamp();
    Drivers::US_015 sensor(timer, &gpio, OutputList::Trig, InputList::Echo, InterruptInputList::EchoIrq, US_015_Test_delay);
    timer.setCounterReference(counter);
    gpio.irqs[InterruptInputList::EchoIrq] = nullptr;
    gpio.inputs[InputList::Echo] = false;
    EXPECT_DOUBLE_EQ(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0.0);
    EXPECT_NEAR(sensor.getDistanceInMilimeters(), 0.0, EXPECT_NEAR_PRECISION);
    startUltrasonicMeasure(gpio);
    counter = 1000;
    finishUltrasonicMeasrue(gpio);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NEAR(sensor.getDistanceInMilimeters(), 0.0, EXPECT_NEAR_PRECISION);
    timeStamp = timer.getTimeStamp();
    startUltrasonicMeasure(gpio);
    nextLoop(timer, gpio);
    finishUltrasonicMeasrue(gpio);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NEAR(sensor.getDistanceInMilimeters(), 0.0, EXPECT_NEAR_PRECISION);
    nextLoop(timer, gpio);
    timeStamp = timer.getTimeStamp();
    startUltrasonicMeasure(gpio);
    nextLoop(timer, gpio);
    counter = 9000;
    finishUltrasonicMeasrue(gpio);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NEAR(sensor.getDistanceInMilimeters(), 0.0, EXPECT_NEAR_PRECISION);
    timeStamp = timer.getTimeStamp();
    startUltrasonicMeasure(gpio);
    nextLoop(timer, gpio);
    counter = 1000;
    finishUltrasonicMeasrue(gpio);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NEAR(sensor.getDistanceInMilimeters(), 0.0, EXPECT_NEAR_PRECISION);
    timeStamp = timer.getTimeStamp();
}

TEST(US_015_Test, init){
    GPIO_Mock gpio(US_015_Test_delay);
    uint32_t counter = 0;
    Timer timer(100000000, 10000, 10, US_015_Test_delay);
    Timer::TimeStamp timeStamp = timer.getTimeStamp();
    Drivers::US_015 sensor(timer, &gpio, OutputList::Trig, InputList::Echo, InterruptInputList::EchoIrq, US_015_Test_delay);
    timer.setCounterReference(counter);
    gpio.irqs[InterruptInputList::EchoIrq] = nullptr;
    gpio.inputs[InputList::Echo] = false;
    sensor.init();
    EXPECT_DOUBLE_EQ(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0.0);
    EXPECT_NEAR(sensor.getDistanceInMilimeters(), 0.0, EXPECT_NEAR_PRECISION);
    startUltrasonicMeasure(gpio);
    counter = 1000;
    finishUltrasonicMeasrue(gpio);
    EXPECT_NEAR(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), sensor.getDistanceInMilimeters(), EXPECT_NEAR_PRECISION);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NE(sensor.getDistanceInMilimeters(), 0);
    timeStamp = timer.getTimeStamp();
    startUltrasonicMeasure(gpio);
    nextLoop(timer, gpio);
    finishUltrasonicMeasrue(gpio);
    EXPECT_NEAR(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), sensor.getDistanceInMilimeters(), EXPECT_NEAR_PRECISION);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NE(sensor.getDistanceInMilimeters(), 0);
    nextLoop(timer, gpio);
    timeStamp = timer.getTimeStamp();
    startUltrasonicMeasure(gpio);
    nextLoop(timer, gpio);
    counter = 9000;
    finishUltrasonicMeasrue(gpio);
    EXPECT_NEAR(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), sensor.getDistanceInMilimeters(), EXPECT_NEAR_PRECISION);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NE(sensor.getDistanceInMilimeters(), 0);
    timeStamp = timer.getTimeStamp();
    startUltrasonicMeasure(gpio);
    nextLoop(timer, gpio);
    counter = 1000;
    finishUltrasonicMeasrue(gpio);
    EXPECT_NEAR(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), sensor.getDistanceInMilimeters(), EXPECT_NEAR_PRECISION);
    EXPECT_NE(calculateDistanceInMiliMeters(timer.timeDiffInMicroSeconds(timeStamp)), 0);
    EXPECT_NE(sensor.getDistanceInMilimeters(), 0);
    timeStamp = timer.getTimeStamp();
}
