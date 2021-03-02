#include <MPU6050.hh>
#include <gtest/gtest.h>
#include <I2C_mock.hh>

void MPU6050_delay(uint32_t){
}

#define M_PI 3.14159265358979323846 /* pi */

std::vector<uint8_t> generateMPU6050Measurements(std::vector<double> akcl, std::vector<double> gyro, const double temp = 20.0){
    std::vector<uint8_t> toReturn;
    if(akcl.size() != 3)
        throw std::invalid_argument("Akcelerometer shoud have 3 values.");
    if(gyro.size() != 3)
        throw std::invalid_argument("Gyroskope shoud have 3 values.");
    int16_t tmp = (int16_t)(akcl[0] / 9.81 * 4096);
    toReturn.push_back(uint8_t(tmp >> 8));
    toReturn.push_back(uint8_t(tmp & 0xff));
    tmp = (int16_t)(akcl[1] / 9.81 * 4096);
    toReturn.push_back(uint8_t(tmp >> 8));
    toReturn.push_back(uint8_t(tmp & 0xff));
    tmp = (int16_t)(akcl[2] / 9.81 * 4096);
    toReturn.push_back(uint8_t(tmp >> 8));
    toReturn.push_back(uint8_t(tmp & 0xff));
    tmp = (int16_t)(temp / M_PI / 11.1111111 * 32768);
    toReturn.push_back(uint8_t(tmp >> 8));
    toReturn.push_back(uint8_t(tmp & 0xff));
    tmp = (int16_t)(gyro[0] / M_PI / 11.1111111 * 32768);
    toReturn.push_back(uint8_t(tmp >> 8));
    toReturn.push_back(uint8_t(tmp & 0xff));
    tmp = (int16_t)(gyro[1] / M_PI / 11.1111111 * 32768);
    toReturn.push_back(uint8_t(tmp >> 8));
    toReturn.push_back(uint8_t(tmp & 0xff));
    tmp = (int16_t)(gyro[2] / M_PI / 11.1111111 * 32768);
    toReturn.push_back(uint8_t(tmp >> 8));
    toReturn.push_back(uint8_t(tmp & 0xff));
    return toReturn;
}

TEST(MPU6050_Test, init){
    I2C_Mock i2c(MPU6050_delay);
    bool flag1, flag2;
    constexpr uint8_t addr = 0x25;
    Drivers::MPU6050 sensor(&i2c, addr, flag1, flag2, MPU6050_delay);
    i2c.setReadingBuffor({
        {addr, 0x0075, {0b01101000}}
    });
    EXPECT_TRUE(sensor.init());
    i2c.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addr, 0x006B, {0x80}},
        {addr, 0x006B, {3}},
        {addr, 0x001A, {1}},
        {addr, 0x001B, {0b00011000}},
        {addr, 0x001C, {0b00010000}},
    });
    EXPECT_TRUE(i2c.empty());
}

TEST(MPU6050_Test, measurement){
    I2C_Mock i2c(MPU6050_delay);
    bool flag1, flag2;
    constexpr uint8_t addr = 0x25;
    Drivers::MPU6050 sensor(&i2c, addr, flag1, flag2, MPU6050_delay);
    i2c.setReadingBuffor({{addr, 0x0075, {0b01101000}}});
    sensor.init();
    EXPECT_FALSE(i2c.empty());
    i2c.setReadingBuffor({{addr, 0x003B, generateMPU6050Measurements({ -6.4, 1.7, 0.14}, {-0.4, 0.0, 11.14})}});
    i2c.handleTimeEvent(nullptr);
    i2c.handleFinish(nullptr);
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(i2c.empty());
}

TEST(MPU6050_Test, invertedX){
    I2C_Mock i2c(MPU6050_delay);
    bool flag1, flag2;
    constexpr uint8_t addr = 0x25;
    Drivers::MPU6050 sensor(&i2c, addr, flag1, flag2, MPU6050_delay, Drivers::IMUsensorIfc::RottatedAxis::X);
    i2c.setReadingBuffor({{addr, 0x0075, {0b01101000}}});
    sensor.init();
    EXPECT_FALSE(i2c.empty());
    i2c.setReadingBuffor({{addr, 0x003B, generateMPU6050Measurements({ -6.4, -1.7, -0.14}, {-0.4, -0.0, -11.14})}});
    i2c.handleTimeEvent(nullptr);
    i2c.handleFinish(nullptr);
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(i2c.empty());
}

TEST(MPU6050_Test, invertedY){
    I2C_Mock i2c(MPU6050_delay);
    bool flag1, flag2;
    constexpr uint8_t addr = 0x25;
    Drivers::MPU6050 sensor(&i2c, addr, flag1, flag2, MPU6050_delay, Drivers::IMUsensorIfc::RottatedAxis::Y);
    i2c.setReadingBuffor({{addr, 0x0075, {0b01101000}}});
    sensor.init();
    EXPECT_FALSE(i2c.empty());
    i2c.setReadingBuffor({{addr, 0x003B, generateMPU6050Measurements({ 6.4, 1.7, -0.14}, {0.4, 0.0, -11.14})}});
    i2c.handleTimeEvent(nullptr);
    i2c.handleFinish(nullptr);
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(i2c.empty());
}

TEST(MPU6050_Test, invertedZ){
    I2C_Mock i2c(MPU6050_delay);
    bool flag1, flag2;
    constexpr uint8_t addr = 0x25;
    Drivers::MPU6050 sensor(&i2c, addr, flag1, flag2, MPU6050_delay, Drivers::IMUsensorIfc::RottatedAxis::Z);
    i2c.setReadingBuffor({{addr, 0x0075, {0b01101000}}});
    sensor.init();
    EXPECT_FALSE(i2c.empty());
    i2c.setReadingBuffor({{addr, 0x003B, generateMPU6050Measurements({ 6.4, -1.7, 0.14}, {0.4, 0.0, 11.14})}});
    i2c.handleTimeEvent(nullptr);
    i2c.handleFinish(nullptr);
    EXPECT_NEAR(sensor.akcelerometr[0], -6.39 , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[1],  1.7  , 0.01);
    EXPECT_NEAR(sensor.akcelerometr[2],  0.14 , 0.01);
    EXPECT_NEAR(sensor.gyroskope[0],    -0.4  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[1],     0.0  , 0.01);
    EXPECT_NEAR(sensor.gyroskope[2],     11.14, 0.01);
    EXPECT_TRUE(i2c.empty());
}
