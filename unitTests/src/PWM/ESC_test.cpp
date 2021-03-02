#include <ESC.hh>
#include <gtest/gtest.h>
#include <stdint.h>

TEST(ESC_Test, set){
    uint16_t regValue = 1000;
    uint16_t regs[5];
    Drivers::ESC<uint16_t> esc(regs, 5, 20 * regValue);
    esc.set(0, 43.5);
    esc.set(1, 45.0);
    esc.set(2, 10.0);
    esc.set(3, 14.6);
    esc.set(4, 10.5);
    EXPECT_EQ(regs[0], regValue + regValue * 43.5 / 100.0);
    EXPECT_EQ(regs[1], regValue + regValue * 45.0 / 100.0);
    EXPECT_EQ(regs[2], regValue + regValue * 10.0 / 100.0);
    EXPECT_EQ(regs[3], regValue + regValue * 14.6 / 100.0);
    EXPECT_EQ(regs[4], regValue + regValue * 10.5 / 100.0);
}

TEST(ESC_Test, setAbove100){
    uint16_t regValue = 1000;
    uint16_t regs[5];
    Drivers::ESC<uint16_t> esc(regs, 5, 20 * regValue);
    esc.set(0, 43.5);
    esc.set(1, 455.0);
    esc.set(2, 100.0);
    esc.set(3, 14.6);
    esc.set(4, 100.5);
    EXPECT_EQ(regs[0], regValue + regValue * 43.5 / 100.0);
    EXPECT_EQ(regs[1], 2 * regValue);
    EXPECT_EQ(regs[2], 2 * regValue);
    EXPECT_EQ(regs[3], regValue + regValue * 14.6 / 100.0);
    EXPECT_EQ(regs[4], 2 * regValue);
}

TEST(ESC_Test, setBelow0){
    uint16_t regValue = 1000;
    uint16_t regs[5];
    Drivers::ESC<uint16_t> esc(regs, 5, 20 * regValue);
    esc.set(0, 43.5);
    esc.set(1, -455.0);
    esc.set(2, 14.6);
    esc.set(3, -100.5);
    esc.set(4, 0.0);
    EXPECT_EQ(regs[0], regValue + regValue * 43.5 / 100.0);
    EXPECT_EQ(regs[1], regValue);
    EXPECT_EQ(regs[2], regValue + regValue * 14.6 / 100.0);
    EXPECT_EQ(regs[3], regValue);
    EXPECT_EQ(regs[4], regValue);
}

TEST(ESC_Test, outOfRange){
    uint16_t regValue = 1000;
    uint16_t regs[7];
    Drivers::ESC<uint16_t> esc(regs, 7, 20 * regValue);
    EXPECT_NO_THROW(esc.set(0, 43.5));
    EXPECT_NO_THROW(esc.set(1, 455.0));
    EXPECT_NO_THROW(esc.set(2, 14.6));
    EXPECT_NO_THROW(esc.set(3, 100.5));
    EXPECT_NO_THROW(esc.set(4, 0.0));
    EXPECT_NO_THROW(esc.set(5, 0.0));
    EXPECT_NO_THROW(esc.set(6, 0.0));
    EXPECT_ANY_THROW(esc.set(7, 0.0));
}