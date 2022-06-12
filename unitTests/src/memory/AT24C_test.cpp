#include <gtest/gtest.h>
#include <cstdlib>
#include <AT24C.hh>
#include <GPIO_mock.hh>
#include <I2C_mock.hh>

namespace MemoryTestSpcae{
    void delay(uint32_t){}
    const Drivers::Memory::RegPair initTable[] = {
        {0x0010, 0x29},
        {0x0011, 0x28},
        {0x0012, 0x27},
        {0x0014, 0x25},
        {0x0017, 0x22},
        {0x0018, 0x21},
        {0x0019, 0x20}
    };
    const uint16_t tableSize = sizeof(initTable)/sizeof(initTable[0]);
}

using Drivers::MemoryIfc;

TEST(AT24C_Test, init){
    I2C_Mock i2c(MemoryTestSpcae::delay);
    GPIO_Mock gpio(MemoryTestSpcae::delay);
    const uint8_t addr = 0xA0;
    Drivers::Memory memory(&i2c, &gpio, addr, OutputList::MemoryWriteProtect, MemoryTestSpcae::delay);
    i2c.setReadingBuffor({
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0xac}},
        {addr, 0xFFFF, {0}},
        {addr, 0x7FFF, {0xac}},
        {addr, 0x7FFF, {0}},
        {addr, 0x3FFF, {0xac}},
        {addr, 0x3FFF, {0}},
        {addr, 0x1FFF, {0xac}},
        {addr, 0x1FFF, {0}},
        {addr, 0x17FF, {0xac}},
        {addr, 0x17FF, {0}},
    });
    EXPECT_TRUE(memory.init());
    i2c.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},   
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0b10100101}},
        {addr, 0xFFFF, {0xac}},
        {addr, 0x7FFF, {0b10100101}},
        {addr, 0x7FFF, {0xac}},
        {addr, 0x3FFF, {0b10100101}},
        {addr, 0x3FFF, {0xac}},
        {addr, 0x1FFF, {0b10100101}},
        {addr, 0x1FFF, {0xac}},
        {addr, 0x17FF, {0b10100101}},
        {addr, 0x17FF, {0xac}},
    });
    EXPECT_EQ(MemoryIfc::BytePagesNotDefined, memory.getSizeModel());
    EXPECT_TRUE(i2c.empty());
}

TEST(AT24C_Test, initSize0){
    I2C_Mock i2c(MemoryTestSpcae::delay);
    GPIO_Mock gpio(MemoryTestSpcae::delay);
    const uint8_t addr = 0xA0;
    Drivers::Memory memory(&i2c, &gpio, addr, OutputList::MemoryWriteProtect, MemoryTestSpcae::delay);
    i2c.setReadingBuffor({
        {addr, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0xac}},
        {addr, 0xFFFF, {0b10100101}},
    });
    EXPECT_TRUE(memory.init(MemoryTestSpcae::initTable, 0));
    i2c.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},   
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0b10100101}},
        {addr, 0xFFFF, {0xac}},
    });
    EXPECT_TRUE(i2c.empty());   
}

TEST(AT24C_Test, initSizeOfTable){
    I2C_Mock i2c(MemoryTestSpcae::delay);
    GPIO_Mock gpio(MemoryTestSpcae::delay);
    const uint8_t addr = 0xA0;
    Drivers::Memory memory(&i2c, &gpio, addr, OutputList::MemoryWriteProtect, MemoryTestSpcae::delay);
    i2c.setReadingBuffor({
        {addr, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0xac}},
        {addr, 0xFFFF, {0b10100101}},
    });
    EXPECT_TRUE(memory.init(MemoryTestSpcae::initTable, MemoryTestSpcae::tableSize));
    i2c.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},   
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0b10100101}},
        {addr, 0xFFFF, {0xac}},
        {addr, MemoryTestSpcae::initTable[0].addr, {MemoryTestSpcae::initTable[0].value}},
        {addr, MemoryTestSpcae::initTable[1].addr, {MemoryTestSpcae::initTable[1].value}},
        {addr, MemoryTestSpcae::initTable[2].addr, {MemoryTestSpcae::initTable[2].value}},
        {addr, MemoryTestSpcae::initTable[3].addr, {MemoryTestSpcae::initTable[3].value}},
        {addr, MemoryTestSpcae::initTable[4].addr, {MemoryTestSpcae::initTable[4].value}},
        {addr, MemoryTestSpcae::initTable[5].addr, {MemoryTestSpcae::initTable[5].value}},
        {addr, MemoryTestSpcae::initTable[6].addr, {MemoryTestSpcae::initTable[6].value}},
    });
    EXPECT_TRUE(i2c.empty());   
}

TEST(AT24C_Test, reinit){
    I2C_Mock i2c(MemoryTestSpcae::delay);
    GPIO_Mock gpio(MemoryTestSpcae::delay);
    const uint8_t addr = 0xA0;
    Drivers::Memory memory(&i2c, &gpio, addr, OutputList::MemoryWriteProtect, MemoryTestSpcae::delay);
    i2c.setReadingBuffor({
        {addr, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0xac}},
        {addr, 0xFFFF, {0b10100101}},
    });
    EXPECT_TRUE(memory.init(MemoryTestSpcae::initTable, MemoryTestSpcae::tableSize));
    i2c.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},   
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0b10100101}},
        {addr, 0xFFFF, {0xac}},
        {addr, MemoryTestSpcae::initTable[0].addr, {MemoryTestSpcae::initTable[0].value}},
        {addr, MemoryTestSpcae::initTable[1].addr, {MemoryTestSpcae::initTable[1].value}},
        {addr, MemoryTestSpcae::initTable[2].addr, {MemoryTestSpcae::initTable[2].value}},
        {addr, MemoryTestSpcae::initTable[3].addr, {MemoryTestSpcae::initTable[3].value}},
        {addr, MemoryTestSpcae::initTable[4].addr, {MemoryTestSpcae::initTable[4].value}},
        {addr, MemoryTestSpcae::initTable[5].addr, {MemoryTestSpcae::initTable[5].value}},
        {addr, MemoryTestSpcae::initTable[6].addr, {MemoryTestSpcae::initTable[6].value}},
    });
    EXPECT_TRUE(i2c.empty());
    EXPECT_EQ(MemoryIfc::BytePages256, memory.getSizeModel());
    i2c.setReadingBuffor({
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0xac}},
        {addr, 0xFFFF, {0}},
        {addr, 0x7FFF, {0xad}},
        {addr, 0x7FFF, {0b10100101}},
    });
    EXPECT_TRUE(memory.init(MemoryTestSpcae::initTable, MemoryTestSpcae::tableSize));
    i2c.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addr, 0x0000, {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26}},   
        {addr, 0x0000, {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff}},
        {addr, 0xFFFF, {0b10100101}},
        {addr, 0xFFFF, {0xac}},
        {addr, 0x7FFF, {0b10100101}},
        {addr, 0x7FFF, {0xad}},
    });
    EXPECT_TRUE(i2c.empty());
    EXPECT_EQ(MemoryIfc::BytePages128, memory.getSizeModel());
}
