#include <Servo.hh>
#include <gtest/gtest.h>
#include <stdint.h>

TEST(Servo_Test, set){
    const double maxAngle= 150.0;
    uint16_t regValue = 1000;
    uint16_t regs[5];
    Drivers::Servo<uint16_t> servo(regs, 5, 20 * regValue, maxAngle, 5.0, 10.0);
    servo.set(0, 43.5);
    servo.set(1, 45.0);
    servo.set(2, 10.0);
    servo.set(3, 14.6);
    servo.set(4, 10.5);
    EXPECT_EQ(regs[0], regValue + uint16_t(regValue * 43.5 / maxAngle));
    EXPECT_EQ(regs[1], regValue + uint16_t(regValue * 45.0 / maxAngle));
    EXPECT_EQ(regs[2], regValue + uint16_t(regValue * 10.0 / maxAngle));
    EXPECT_EQ(regs[3], regValue + uint16_t(regValue * 14.6 / maxAngle));
    EXPECT_EQ(regs[4], regValue + uint16_t(regValue * 10.5 / maxAngle));
}

TEST(Servo_Test, setAbove100){
    const double maxAngle= 250.0;
    uint16_t regValue = 1000;
    uint16_t regs[5];
    Drivers::Servo<uint16_t> servo(regs, 5, 20 * regValue, maxAngle, 5.0, 10.0);
    servo.set(0, 43.5);
    servo.set(1, 455.0);
    servo.set(2, 250.0);
    servo.set(3, 14.6);
    servo.set(4, 100.5);
    EXPECT_EQ(regs[0], regValue + uint16_t(regValue * 43.5 / maxAngle));
    EXPECT_EQ(regs[1], 2 * regValue);
    EXPECT_EQ(regs[2], 2 * regValue);
    EXPECT_EQ(regs[3], regValue + uint16_t(regValue * 14.6 / maxAngle));
    EXPECT_EQ(regs[4], regValue + uint16_t(regValue * 100.5 / maxAngle));
}

TEST(Servo_Test, setBelow0){
    const double maxAngle= 50.0;
    uint16_t regValue = 1000;
    uint16_t regs[5];
    Drivers::Servo<uint16_t> servo(regs, 5, 20 * regValue, maxAngle, 5.0, 10.0);
    servo.set(0, 43.5);
    servo.set(1, -455.0);
    servo.set(2, 14.6);
    servo.set(3, -100.5);
    servo.set(4, 0.0);
    EXPECT_EQ(regs[0], regValue + regValue * 43.5 / maxAngle);
    EXPECT_EQ(regs[1], regValue);
    EXPECT_EQ(regs[2], regValue + regValue * 14.6 / maxAngle);
    EXPECT_EQ(regs[3], regValue);
    EXPECT_EQ(regs[4], regValue);
}

TEST(Servo_Test, outOfRange){
    const double maxAngle= 150.0;
    uint16_t regValue = 1000;
    uint16_t regs[7];
    Drivers::Servo<uint16_t> servo(regs, 7, 20 * regValue, maxAngle);
    EXPECT_NO_THROW(servo.set(0, 43.5));
    EXPECT_NO_THROW(servo.set(1, 455.0));
    EXPECT_NO_THROW(servo.set(2, 14.6));
    EXPECT_NO_THROW(servo.set(3, 100.5));
    EXPECT_NO_THROW(servo.set(4, 0.0));
    EXPECT_NO_THROW(servo.set(5, 0.0));
    EXPECT_NO_THROW(servo.set(6, 0.0));
    EXPECT_ANY_THROW(servo.set(7, 0.0));
}