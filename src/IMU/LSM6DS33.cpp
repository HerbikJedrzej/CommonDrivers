#include "LSM6DS33.hh"

namespace Drivers{

LSM6DS33::LSM6DS33(SPI_Ifc* _spi, OutputList _ssPinToToggle, bool& _akcAndGyroDataReadyFlag, bool& _magDataReadyFlag, void (*_delay)(uint32_t), const RottatedAxis& axis):
IMUsensorIfc(_akcAndGyroDataReadyFlag, _magDataReadyFlag, axis, _delay),
spi(_spi),
ssPinToToggle(_ssPinToToggle)
{}

LSM6DS33::~LSM6DS33(){}

bool LSM6DS33::init(){
    uint8_t initData[11];
    uint8_t checkData[11] = {0x90};
    initData[0] = 0x10;
    initData[1] = 0xAE; // A - ODR = 6.66 kHz // E - +/-8g, anti-aliasing filer 100 Hz
    initData[2] = 0x4C; // 6 - frequency of output data = 104 Hz // 2000 degres per second
    initData[3] = 0x04; // default
    initData[4] = 0x04; // I2C disable
    initData[5] = 0x60; // rounding mode
    initData[6] = 0x60; // data triger by latched, level sensitive and edge sensitive
    initData[7] = 0x4C;
    initData[8] = 0xA5;
    initData[9] = 0x38;
    initData[10] = 0x3F;
    while(spi->isBussy());
    if(!checkIcId())
        return false;
    delay(1);
    spi->readWrite(ssPinToToggle, initData, initData, 11);
    delay(1);
    spi->readWrite(ssPinToToggle, checkData, checkData, 11);
    if(
        (0xAE != checkData[1]) ||
        (0x4C != checkData[2]) ||
        (0x04 != checkData[3]) ||
        (0x04 != checkData[4]) ||
        (0x60 != checkData[5]) ||
        (0x60 != checkData[6]) ||
        ((0x4C & 0xf7) != (checkData[7] & 0xf7)) ||
        (0xA5 != checkData[8]) ||
        (0x38 != checkData[9]) ||
        (0x3F != checkData[10])
    )
        return false;
    delay(1);
    spi->wakeMeUp(this);
    return true;
}

void LSM6DS33::handleTimeEvent(DriverIfc*){
      akcelerometrData[0] = 0b10100010;
      spi->readWrite(ssPinToToggle, akcelerometrData, akcelerometrData, 13, this);
}

bool LSM6DS33::checkIcId(){
    uint8_t data[2] = {0x8f, 0x00};
    spi->readWrite(ssPinToToggle, data, data, 2);
    if(data[1] == 0b01101001)
        return true;
    return false;
}

#define M_PI 3.14159265358979323846 /* pi */

void LSM6DS33::handleFinish(DriverIfc*){
    akcelerometerAndGyroskopeDataReadyFlag = true;
	gyroskope[0] = ((int16_t)(( akcelerometrData[2] << 8 ) | akcelerometrData[1])) * M_PI * 11.1111111 / 32768;
	gyroskope[1] = ((int16_t)(( akcelerometrData[4] << 8 ) | akcelerometrData[3])) * M_PI * 11.1111111 / 32768;
	gyroskope[2] = ((int16_t)(( akcelerometrData[6] << 8 ) | akcelerometrData[5])) * M_PI * 11.1111111 / 32768;
	akcelerometr[0] = ((int16_t)(( akcelerometrData[8] << 8) | akcelerometrData[7])) * 9.81 / 4096;
	akcelerometr[1] = ((int16_t)(( akcelerometrData[10] << 8 ) | akcelerometrData[9])) * 9.81 / 4096;
	akcelerometr[2] = ((int16_t)(( akcelerometrData[12] << 8 ) | akcelerometrData[11])) * 9.81 / 4096;
    invert();
}

}