#include "MPU6050.hh"
#include <OwnExceptions.hh>

namespace Drivers{

MPU6050::MPU6050(I2C_Ifc* _i2c, uint8_t _addr, bool& _akcAndGyroDataReadyFlag, bool& _magDataReadyFlag, void (*_delay)(uint32_t), const RottatedAxis& axis):
    IMUsensorIfc(_akcAndGyroDataReadyFlag, _magDataReadyFlag, axis, _delay),
    i2c(_i2c),
    addr(_addr)
    {}

MPU6050::~MPU6050(){}

bool MPU6050::reset(){
    uint8_t data = 0x80;
    return BusStatus::OK == i2c->write(addr, 0x6B, false, &data, 1);
}

bool MPU6050::init(){
    uint8_t data[1];
    while(i2c->isBussy());
    if(!checkIcId())
        return false;
    if(!reset())
        THROW_invalid_argument("MPU reset device failed.");
    data[0] = 3;
    delay(10);
    i2c->write(addr, 0x6B, false, data, 1);
    data[0] = 1;
    delay(10);
    i2c->write(addr, 0x1A, false, data, 1);
    data[0] = 0b00011000;
    delay(10);
    i2c->write(addr, 0x1B, false, data, 1);
    data[0] = 0b00010000;
    delay(10);
    i2c->write(addr, 0x1C, false, data, 1);
    delay(10);
    i2c->wakeMeUp(this);
    return true;
}

void MPU6050::handleTimeEvent(DriverIfc*){
    i2c->read(addr, 0x3B, false, sensorData, size, this);
}

#define M_PI 3.14159265358979323846 /* pi */

bool MPU6050::checkIcId(){
    uint8_t pData = 0;
    i2c->read(addr, 0x75, false, &pData, 1);
    if(pData == 0b01101000)
        return true;
    return false;
}

void MPU6050::handleFinish(DriverIfc*){
    akcelerometerAndGyroskopeDataReadyFlag = true;
    akcelerometr[0] = ((double)((int16_t)((sensorData[0] << 8) | sensorData[1]))) * 9.81 / 4096;
    akcelerometr[1] = ((double)((int16_t)((sensorData[2] << 8) | sensorData[3]))) * 9.81 / 4096;
    akcelerometr[2] = ((double)((int16_t)((sensorData[4] << 8) | sensorData[5]))) * 9.81 / 4096;
    temperatureMPU = (double)((int16_t)((sensorData[6] << 8) | sensorData[7]));
    gyroskope[0] = ((double)((int16_t)((sensorData[8] << 8) | sensorData[9]))) * M_PI * 11.1111111 / 32768;
    gyroskope[1] = ((double)((int16_t)((sensorData[10] << 8) | sensorData[11]))) * M_PI * 11.1111111 / 32768;
    gyroskope[2] = ((double)((int16_t)((sensorData[12] << 8) | sensorData[13]))) * M_PI * 11.1111111 / 32768;
    invert();
}

}