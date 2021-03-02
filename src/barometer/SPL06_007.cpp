#include "SPL06_007.hh"
#include <OwnExceptions.hh>
#include <cmath>

namespace Drivers{

SPL06_007::SPL06_007(I2C_Ifc* _i2c, uint8_t _addr, bool& _dataReadyFlag, void (*_delay)(uint32_t), const InterruptInputList& _irqPin, Drivers::GPIO_Ifc* const _gpio):
    BarometerIfc(_dataReadyFlag, _delay),
    i2c(_i2c),
    addr(_addr),
    rawPreasure(0),
    rawTemperature(0),
    gpio(_gpio),
    irqPin(_irqPin),
    altitudeSum(0),
    stepingMeanCounter(0),
    mode(Mode::ContinuousPressureAndTemperatureMeasurement),
    preasureRate(Rate::_4_per_second),
    temperatureRate(Rate::_4_per_second),
    preasureOversampling(Oversampling::_64_times),
    temperatureOversampling(Oversampling::_1_times){
        for(uint8_t i = 0; i < stepingMeanSize; i++)
            stepingMeanBuffor[i] = 0;
    }

SPL06_007::~SPL06_007(){
    gpio->subscribe(irqPin, nullptr);
    i2c->stopWakingMe(this);
}

void SPL06_007::handleTimeEvent(DriverIfc*){}

constexpr double SPL06_007::compensationScaleFactors[8];

void SPL06_007::handleFinish(DriverIfc* drv){
    if(drv == gpio){
        uint8_t irqSource = getIrqSource() & (IrqSource::Preasure | IrqSource::Temperature);
        if(irqSource == IrqSource::Preasure){
            uint8_t data[3];
            i2c->read(addr, 0x00, false, data, 3);
            rawPreasure    = (uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | (uint32_t)data[2];
            rawPreasure = (rawPreasure & 1 << 23) ? (0xff000000 | rawPreasure) : rawPreasure;
        }else if(irqSource == IrqSource::Temperature){
            uint8_t data[3];
            i2c->read(addr, 0x03, false, data, 3);
            rawTemperature = (uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | (uint32_t)data[2];
            rawTemperature = (rawTemperature & 1 << 23) ? (0xff000000 | rawTemperature) : rawTemperature;
        }else{
            uint8_t data[6];
            i2c->read(addr, 0x00, false, data, 6);
            rawPreasure    = (uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | (uint32_t)data[2];
            rawTemperature = (uint32_t)data[3] << 16 | (uint32_t)data[4] << 8 | (uint32_t)data[5];
            rawPreasure    = (   rawPreasure & 1 << 23) ? (0xff000000 |    rawPreasure) : rawPreasure;
            rawTemperature = (rawTemperature & 1 << 23) ? (0xff000000 | rawTemperature) : rawTemperature;
        }

        double ftsc = (double)rawTemperature / compensationScaleFactors[temperatureOversampling];
        double fpsc = (double)rawPreasure /  compensationScaleFactors[preasureOversampling];
        temperature = (double)coefficients.c0 * 0.5 + (double)coefficients.c1 * ftsc;
        preasure = (double)coefficients.c00 +
            fpsc * ((double)coefficients.c10 +fpsc * ((double)coefficients.c20 + fpsc * (double)coefficients.c30)) +
            ftsc * (double)coefficients.c01 + (ftsc * fpsc * ((double)coefficients.c11 + fpsc * (double)coefficients.c21));
        double pK = preasure / msl_pressure;
        altitudeSum -= stepingMeanBuffor[stepingMeanCounter];
        stepingMeanBuffor[stepingMeanCounter] = (((pow(pK, (-(a * R) / g))) * T1) - T1) / a;
        altitudeSum += stepingMeanBuffor[stepingMeanCounter++];
        if(stepingMeanSize == stepingMeanCounter)
            stepingMeanCounter = 0;
        altitude = altitudeSum / stepingMeanSize;
    }
}

void SPL06_007::clearFifo(){
    uint8_t pData = 0x80;
    i2c->write(addr, 0x0C, false, &pData, 1);
}

bool SPL06_007::checkIcId(){
    uint8_t pData = 0;
    i2c->read(addr, 0x0D, false, &pData, 1);
    if(pData == 0x10)
        return true;
    return false;
}

bool SPL06_007::coefficientsReady(){
    uint8_t pData;
    i2c->read(addr, 0x08, false, &pData, 1);
    return ((pData & 0b10000000) != 0); 
}

bool SPL06_007::reset(){
    uint8_t pData = 0b00001001;
    i2c->write(addr, 0x0C, false, &pData, 1);
    delay(50);
    i2c->read(addr, 0x08, false, &pData, 1);
    return ((pData & 0b01000000) != 0); 
}

bool SPL06_007::init(){
    if(!i2c)
        THROW_invalid_argument("SPL06-007 i2c handler is null.");
    if(!gpio)
        THROW_invalid_argument("SPL06-007 gpio handler is null.");
    if(irqPin == InterruptInputList::endOfInterruptInputList)
        THROW_invalid_argument("SPL06-007 interrupt pin wrong configured.");
    while(i2c->isBussy());
    if(!checkIcId())
        THROW_invalid_argument("SPL06-007 returned wrong ID.");
    if(!reset())
        THROW_invalid_argument("SPL06-007 reset device failed.");
    delay(10);
    uint8_t data = uint8_t((preasureRate << 4) | preasureOversampling);
    i2c->write(addr, 0x06, false, &data, 1);
    data = uint8_t(0x80 | (temperatureRate << 4) | temperatureOversampling);
    i2c->write(addr, 0x07, false, &data, 1);
    data = uint8_t(mode);
    i2c->write(addr, 0x08, false, &data, 1);
    data = 0b10110000;
    if(temperatureOversampling > Oversampling::_8_times)
        data |= 0b00001000;
    if(preasureOversampling > Oversampling::_8_times)
        data |= 0b00000100;
    i2c->write(addr, 0x09, false, &data, 1);
    delay(10);
    if(!coefficientsReady())
        THROW_invalid_argument("SPL06-007 coefficients are not avaible.");
    getCoefficients();
    gpio->subscribe(irqPin, this);
    i2c->wakeMeUp(this);
    return true;
}

bool SPL06_007::fifoFull(){
    uint8_t pData;
    i2c->read(addr, 0x0B, false, &pData, 1);
    return ((pData & 2) != 0); 
}
bool SPL06_007::fifoEmpty(){
    uint8_t pData;
    i2c->read(addr, 0x0B, false, &pData, 1);
    return ((pData & 1) != 0); 
}

void SPL06_007::getCoefficients(){
    uint8_t buf[18];
    i2c->read(addr, 0x10, false, buf, 18);
    coefficients.c0 = (uint16_t)buf[0] << 4 | (uint16_t)buf[1] >> 4;
    coefficients.c0 = (coefficients.c0 & 1 << 11) ? (0xf000 | coefficients.c0) : coefficients.c0;
    coefficients.c1 = (uint16_t)(buf[1] & 0x0f) << 8 | (uint16_t)buf[2];
    coefficients.c1 = (coefficients.c1 & 1 << 11) ? (0xf000 | coefficients.c1) : coefficients.c1;
    coefficients.c00 = (uint32_t)buf[3] << 12 | (uint32_t)buf[4] << 4 | (uint16_t)buf[5] >> 4;
    coefficients.c00 = (coefficients.c00 & 1 << 19) ? (0xfff00000 | coefficients.c00) : coefficients.c00;
    coefficients.c10 = (uint32_t)(buf[5] & 0x0f) << 16 | (uint32_t)buf[6] << 8 | (uint32_t)buf[7];
    coefficients.c10 = (coefficients.c10 & 1 << 19) ? (0xfff00000 | coefficients.c10) : coefficients.c10;
    coefficients.c01 = (uint16_t)buf[8] << 8 | buf[9];
    coefficients.c11 = (uint16_t)buf[10] << 8 | buf[11];
    coefficients.c20 = (uint16_t)buf[12] << 8 | buf[13];
    coefficients.c21 = (uint16_t)buf[14] << 8 | buf[15];
    coefficients.c30 = (uint16_t)buf[16] << 8 | buf[17];
}

uint8_t SPL06_007::getIrqSource(){
    uint8_t data;
    i2c->read(addr, 0x0A, false, &data, 1);
    return (data & 0b111); 
}

}
