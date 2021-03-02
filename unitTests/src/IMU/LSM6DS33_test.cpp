#include <LSM6DS33.hh>
#include <gtest/gtest.h>
#include <GPIO_mock.hh>
#include <SPI_mock.hh>

void LSM6DS33_delay(uint32_t){
}

#define M_PI 3.14159265358979323846 /* pi */

std::vector<uint8_t> generateMeasurements(std::vector<double> akcl, std::vector<double> gyro, std::vector<double>){
    std::vector<uint8_t> toReturn;
    if(akcl.size() != 3)
        throw std::invalid_argument("Akcelerometer shoud have 3 values.");
    if(gyro.size() != 3)
        throw std::invalid_argument("Gyroskope shoud have 3 values.");
    toReturn.push_back(0);
    int16_t tmp = (int16_t)(gyro[0] / M_PI / 11.1111111 * 32768);
    toReturn.push_back(uint8_t(tmp & 0xff));
    toReturn.push_back(uint8_t(tmp >> 8));
    tmp = (int16_t)(gyro[1] / M_PI / 11.1111111 * 32768);
    toReturn.push_back(uint8_t(tmp & 0xff));
    toReturn.push_back(uint8_t(tmp >> 8));
    tmp = (int16_t)(gyro[2] / M_PI / 11.1111111 * 32768);
    toReturn.push_back(uint8_t(tmp & 0xff));
    toReturn.push_back(uint8_t(tmp >> 8));
    tmp = (int16_t)(akcl[0] / 9.81 * 4096);
    toReturn.push_back(uint8_t(tmp & 0xff));
    toReturn.push_back(uint8_t(tmp >> 8));
    tmp = (int16_t)(akcl[1] / 9.81 * 4096);
    toReturn.push_back(uint8_t(tmp & 0xff));
    toReturn.push_back(uint8_t(tmp >> 8));
    tmp = (int16_t)(akcl[2] / 9.81 * 4096);
    toReturn.push_back(uint8_t(tmp & 0xff));
    toReturn.push_back(uint8_t(tmp >> 8));
    return toReturn;
}

TEST(LSM6DS33_Test, checkID){
    SPI_Mock spi(LSM6DS33_delay);
    GPIO_Mock gpio(LSM6DS33_delay);
    bool flag1, flag2;
    Drivers::LSM6DS33 sensor(&spi, OutputList::spiSlaveSelect, flag1, flag2, LSM6DS33_delay);
    spi.setReadingBuffor({
        {0x00, 0b00101001},
    });
    EXPECT_FALSE(sensor.init());
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {0x8f, 0x00},
    });
    EXPECT_TRUE(spi.empty());
}

TEST(LSM6DS33_Test, init){
    SPI_Mock spi(LSM6DS33_delay);
    GPIO_Mock gpio(LSM6DS33_delay);
    bool flag1, flag2;
    Drivers::LSM6DS33 sensor(&spi, OutputList::spiSlaveSelect, flag1, flag2, LSM6DS33_delay);
    spi.setReadingBuffor({
        {0x00, 0b01101001},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x10, 0xAE, 0x4C, 0x04, 0x04, 0x60, 0x60, 0x4C, 0xA5, 0x38, 0x3F}
    });
    EXPECT_TRUE(sensor.init());
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {0x8f, 0x00},
        {0x10, 0xAE, 0x4C, 0x04, 0x04, 0x60, 0x60, 0x4C, 0xA5, 0x38, 0x3F},
        {0x90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    });
    EXPECT_TRUE(spi.empty());
}

TEST(LSM6DS33_Test, measurement){
    SPI_Mock spi(LSM6DS33_delay);
    GPIO_Mock gpio(LSM6DS33_delay);
    bool flag1, flag2;
    Drivers::LSM6DS33 sensor(&spi, OutputList::spiSlaveSelect, flag1, flag2, LSM6DS33_delay);
    spi.setReadingBuffor({
        {0x00, 0b01101001},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x10, 0xAE, 0x4C, 0x04, 0x04, 0x60, 0x60, 0x4C, 0xA5, 0x38, 0x3F}
    });
    sensor.init();
    EXPECT_FALSE(spi.empty());
    spi.setReadingBuffor({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    EXPECT_FALSE(spi.empty());
    spi.setReadingBuffor({generateMeasurements({ -6.4, 1.7, 0.14}, {-0.4, 0.0, 11.14}, {})});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {{0b10100010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(spi.empty());
}

TEST(LSM6DS33_Test, invertedX){
    SPI_Mock spi(LSM6DS33_delay);
    GPIO_Mock gpio(LSM6DS33_delay);
    bool flag1, flag2;
    Drivers::LSM6DS33 sensor(&spi, OutputList::spiSlaveSelect, flag1, flag2, LSM6DS33_delay, Drivers::IMUsensorIfc::RottatedAxis::X);
    spi.setReadingBuffor({
        {0x00, 0b01101001},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x10, 0xAE, 0x4C, 0x04, 0x04, 0x60, 0x60, 0x4C, 0xA5, 0x38, 0x3F}
    });
    sensor.init();
    EXPECT_FALSE(spi.empty());
    spi.setReadingBuffor({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    EXPECT_FALSE(spi.empty());
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[0], 0.0);
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[1], 0.0);
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[2], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[0], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[1], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[2], 0.0);
    spi.setReadingBuffor({generateMeasurements({ -6.4, -1.7, -0.14}, {-0.4, -0.0, -11.14}, {})});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {{0b10100010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(spi.empty());
}

TEST(LSM6DS33_Test, invertedY){
    SPI_Mock spi(LSM6DS33_delay);
    GPIO_Mock gpio(LSM6DS33_delay);
    bool flag1, flag2;
    Drivers::LSM6DS33 sensor(&spi, OutputList::spiSlaveSelect, flag1, flag2, LSM6DS33_delay, Drivers::IMUsensorIfc::RottatedAxis::Y);
    spi.setReadingBuffor({
        {0x00, 0b01101001},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x10, 0xAE, 0x4C, 0x04, 0x04, 0x60, 0x60, 0x4C, 0xA5, 0x38, 0x3F}
    });
    sensor.init();
    EXPECT_FALSE(spi.empty());
    spi.setReadingBuffor({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    EXPECT_FALSE(spi.empty());
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[0], 0.0);
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[1], 0.0);
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[2], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[0], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[1], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[2], 0.0);
    spi.setReadingBuffor({generateMeasurements({ 6.4, 1.7, -0.14}, {0.4, 0.0, -11.14}, {})});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {{0b10100010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(spi.empty());
}

TEST(LSM6DS33_Test, invertedZ){
    SPI_Mock spi(LSM6DS33_delay);
    GPIO_Mock gpio(LSM6DS33_delay);
    bool flag1, flag2;
    Drivers::LSM6DS33 sensor(&spi, OutputList::spiSlaveSelect, flag1, flag2, LSM6DS33_delay, Drivers::IMUsensorIfc::RottatedAxis::Z);
    spi.setReadingBuffor({
        {0x00, 0b01101001},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x10, 0xAE, 0x4C, 0x04, 0x04, 0x60, 0x60, 0x4C, 0xA5, 0x38, 0x3F}
    });
    sensor.init();
    EXPECT_FALSE(spi.empty());
    spi.setReadingBuffor({{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    EXPECT_FALSE(spi.empty());
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[0], 0.0);
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[1], 0.0);
    EXPECT_DOUBLE_EQ(sensor.akcelerometr[2], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[0], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[1], 0.0);
    EXPECT_DOUBLE_EQ(sensor.gyroskope[2], 0.0);
    spi.setReadingBuffor({generateMeasurements({ 6.4, -1.7, 0.14}, {0.4, 0.0, 11.14}, {})});
    spi.handleTimeEvent(nullptr);
    spi.handleFinish(nullptr);
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {{0b10100010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(spi.empty());
}
