#include <PWM.hh>
#include <gtest/gtest.h>
#include <stdint.h>

TEST(PWM_Test, set){
    uint16_t maxValue = 40000;
    uint16_t regs[5];
    Drivers::PWM<uint16_t> pwm(regs, 5, maxValue);
    pwm.set(0, 43.5);
    pwm.set(1, 45.0);
    pwm.set(2, 10.0);
    pwm.set(3, 14.6);
    pwm.set(4, 10.5);
    EXPECT_EQ(regs[0], maxValue * 43.5 / 100.0);
    EXPECT_EQ(regs[1], maxValue * 45.0 / 100.0);
    EXPECT_EQ(regs[2], maxValue * 10.0 / 100.0);
    EXPECT_EQ(regs[3], maxValue * 14.6 / 100.0);
    EXPECT_EQ(regs[4], maxValue * 10.5 / 100.0);
}

TEST(PWM_Test, setAbove100){
    uint16_t maxValue = 40000;
    uint16_t regs[5];
    Drivers::PWM<uint16_t> pwm(regs, 5, maxValue);
    pwm.set(0, 43.5);
    pwm.set(1, 455.0);
    pwm.set(2, 100.0);
    pwm.set(3, 14.6);
    pwm.set(4, 100.5);
    EXPECT_EQ(regs[0], maxValue * 43.5 / 100.0);
    EXPECT_EQ(regs[1], maxValue);
    EXPECT_EQ(regs[2], maxValue);
    EXPECT_EQ(regs[3], maxValue * 14.6 / 100.0);
    EXPECT_EQ(regs[4], maxValue);
}

TEST(PWM_Test, setBelow0){
    uint16_t maxValue = 40000;
    uint16_t regs[5];
    Drivers::PWM<uint16_t> pwm(regs, 5, maxValue);
    pwm.set(0, 43.5);
    pwm.set(1, -455.0);
    pwm.set(2, 14.6);
    pwm.set(3, -100.5);
    pwm.set(4, 0.0);
    EXPECT_EQ(regs[0], maxValue * 43.5 / 100.0);
    EXPECT_EQ(regs[1], 0);
    EXPECT_EQ(regs[2], maxValue * 14.6 / 100.0);
    EXPECT_EQ(regs[3], 0);
    EXPECT_EQ(regs[4], 0);
}

TEST(PWM_Test, outOfRange){
    uint16_t maxValue = 40000;
    uint16_t regs[7];
    Drivers::PWM<uint16_t> pwm(regs, 7, maxValue);
    EXPECT_NO_THROW(pwm.set(0, 43.5));
    EXPECT_NO_THROW(pwm.set(1, 455.0));
    EXPECT_NO_THROW(pwm.set(2, 14.6));
    EXPECT_NO_THROW(pwm.set(3, 100.5));
    EXPECT_NO_THROW(pwm.set(4, 0.0));
    EXPECT_NO_THROW(pwm.set(5, 0.0));
    EXPECT_NO_THROW(pwm.set(6, 0.0));
    EXPECT_ANY_THROW(pwm.set(7, 0.0));
}