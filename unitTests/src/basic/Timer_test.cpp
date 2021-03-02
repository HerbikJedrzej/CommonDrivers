#include <gtest/gtest.h>
#include <Timer.hh>
#include <cstdlib>

namespace {
    void delayTimerTest(uint32_t){}
}

TEST(Timer_Test, setCounterReference){
    Drivers::Timer timer(100000, 100, 10, delayTimerTest);
    uint32_t counter = 0;
    EXPECT_EQ(timer.getTimeStamp().counter, 0);
    EXPECT_EQ(timer.getTimeStamp().cycles, 0);
    timer.handleTimeEvent(nullptr);
    EXPECT_EQ(timer.getTimeStamp().counter, 0);
    EXPECT_EQ(timer.getTimeStamp().cycles, 1);
    timer.setCounterReference(counter);
    EXPECT_EQ(timer.getTimeStamp().counter, 0);
    EXPECT_EQ(timer.getTimeStamp().cycles, 1);
    timer.handleTimeEvent(nullptr);
    EXPECT_EQ(timer.getTimeStamp().counter, 0);
    EXPECT_EQ(timer.getTimeStamp().cycles, 2);
    counter++;
    EXPECT_EQ(timer.getTimeStamp().counter, 1);
    EXPECT_EQ(timer.getTimeStamp().cycles, 2);
    counter = 125;
    timer.handleTimeEvent(nullptr);
    timer.handleTimeEvent(nullptr);
    EXPECT_EQ(timer.getTimeStamp().counter, 125);
    EXPECT_EQ(timer.getTimeStamp().cycles, 4);
}

TEST(Timer_Test, isAfter){
    Drivers::Timer timer(100000, 100, 10, delayTimerTest);
    uint32_t counter = 0;
    timer.setCounterReference(counter);
    Drivers::Timer::TimeStamp stamp{5,52};
    EXPECT_FALSE(timer.isAfter(stamp));
    for(unsigned int i = 0; i < 5; i++){
        timer.handleTimeEvent(nullptr);
        EXPECT_FALSE(timer.isAfter(stamp));
    }
    timer.handleTimeEvent(nullptr);
    EXPECT_TRUE(timer.isAfter(stamp));
    stamp.cycles++;
    EXPECT_FALSE(timer.isAfter(stamp));
    counter = 51;
    EXPECT_FALSE(timer.isAfter(stamp));
    counter++;
    EXPECT_TRUE(timer.isAfter(stamp));
}

TEST(Timer_Test, timeDiffInSeconds){
    constexpr double freq = 100000.0;
    constexpr double resolution = 100.0;
    constexpr double divide = 10.0;
    Drivers::Timer timer(freq, resolution - 1, divide -1, delayTimerTest);
    uint32_t counter = 11;
    timer.setCounterReference(counter);
    timer.handleTimeEvent(nullptr);
    Drivers::Timer::TimeStamp stamp{1, 11};
    EXPECT_DOUBLE_EQ(timer.timeDiffInSeconds(stamp), 0.0);
    stamp.counter = 2;
    EXPECT_DOUBLE_EQ(timer.timeDiffInSeconds(stamp), 1.0 / freq * resolution * divide * (9 / resolution));
    timer.handleTimeEvent(nullptr);
    EXPECT_DOUBLE_EQ(timer.timeDiffInSeconds(stamp), 1.0 / freq * resolution * divide * (1 + 9 / resolution));
    stamp.counter = 15;
    EXPECT_DOUBLE_EQ(timer.timeDiffInSeconds(stamp), 1.0 / freq * resolution * divide * (1 - 4 / resolution));
}

TEST(Timer_Test, timeDiffInMicroSeconds){
    constexpr double freq = 10000.0;
    constexpr double resolution = 1000.0;
    constexpr double divide = 25.0;
    Drivers::Timer timer(freq, resolution - 1, divide -1, delayTimerTest);
    uint32_t counter = 11;
    timer.setCounterReference(counter);
    timer.handleTimeEvent(nullptr);
    Drivers::Timer::TimeStamp stamp{1, 11};
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 0);
    stamp.counter = 2;
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 1000000 / freq * resolution * divide * (9 / resolution));
    timer.handleTimeEvent(nullptr);
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 1000000 / freq * resolution * divide * (resolution + 9) / resolution);
    stamp.counter = 15;
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 1000000 / freq * resolution * divide * (1 - 4 / resolution));
}

TEST(Timer_Test, timeDiffInMicroSecondsWithChangedParams){
    constexpr double freq = 10000.0;
    constexpr double resolution = 1000.0;
    constexpr double divide = 25.0;
    Drivers::Timer timer(freq, resolution - 1, divide -1, delayTimerTest);
    timer.updateSignalParams(freq, resolution / 2 - 1, divide -1);
    uint32_t counter = 11;
    timer.setCounterReference(counter);
    timer.handleTimeEvent(nullptr);
    Drivers::Timer::TimeStamp stamp{1, 11};
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 0);
    stamp.counter = 2;
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 1000000 / freq * resolution / 2 * divide * (9 / (resolution / 2)));
    timer.handleTimeEvent(nullptr);
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 1000000 / freq * resolution / 2 * divide * (resolution / 2 + 9) / (resolution / 2));
    stamp.counter = 15;
    EXPECT_EQ(timer.timeDiffInMicroSeconds(stamp), 1000000 / freq * resolution / 2 * divide * (1 - 4 / (resolution / 2)));
}
