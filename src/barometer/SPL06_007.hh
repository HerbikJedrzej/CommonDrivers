#pragma once

#include <stdint.h>
#include <GPIOconf.hh>
#include "I2C.hh"
#include "GPIO.hh"
#include "BarometerIfc.hh"

namespace Drivers{

class SPL06_007 : public BarometerIfc{
private:
    I2C_Ifc* i2c;
    uint8_t addr;
    void handleTimeEvent(DriverIfc*) final;
    void handleFinish(DriverIfc*) final;
    bool checkIcId();
    bool coefficientsReady();
    bool reset();
    void getCoefficients();
    void clearFifo();
    bool fifoFull();
    bool fifoEmpty();
    int32_t rawPreasure;
    int32_t rawTemperature;
    Drivers::GPIO_Ifc* const gpio;
    const InterruptInputList irqPin;
    static constexpr uint8_t stepingMeanSize = {7};
    double altitudeSum;
    uint8_t stepingMeanCounter;
    double stepingMeanBuffor[stepingMeanSize];
    static constexpr double dt = {0.01};
    static constexpr double T1 = {15.0 + 273.15};
    static constexpr double a = {-6.5 / 1000.0};
    static constexpr double g = {9.80665};
    static constexpr double R = {287.05};
    static constexpr double msl_pressure = {101325.0};
    enum IrqSource{
        Preasure = 1,
        Temperature = 2,
        Fifo = 4,
    };
    static constexpr double compensationScaleFactors[8] = {
        524288,
        1572864,
        3670016,
        7864320,
        253952,
        516096,
        1040384,
        2088960
    };
    uint8_t getIrqSource();
    enum Mode{
        Idle = 0,
        PressureMeasurement,
        TemperatureMeasurement,
        ContinuousPressureMeasurement = 5,
        ContinuousTemperatureMeasurement,
        ContinuousPressureAndTemperatureMeasurement
    } mode;
    enum Rate{
        _1_per_second = 0,
        _2_per_second = 1,
        _4_per_second = 2,
        _8_per_second = 3,
        _16_per_second = 4,
        _32_per_second = 5,
        _64_per_second = 6,
        _128_per_second = 7
    } preasureRate, temperatureRate;
    enum Oversampling{
        _1_times = 0,
        _2_times = 1,
        _4_times = 2,
        _8_times = 3,
        _16_times = 4,
        _32_times = 5,
        _64_times = 6,
        _128_times = 7        
    } preasureOversampling, temperatureOversampling;
public:
    struct Coefficients{
        // double c0 = 199.0;
        // double c1 = 3843.0;
        // double c00 = 76972.0;
        // double c10 = 999157.0;
        // double c01 = 63152.0;
        // double c11 = 1251.0;
        // double c20 = 58257.0;
        // double c21 = 83.0;
        // double c30 = 64724.0;
        int16_t c0, c1;
        int32_t c00, c10;
        int16_t c01, c11, c20, c21, c30;
    } coefficients;
    SPL06_007(I2C_Ifc* _i2c, uint8_t _addr, bool& _dataReadyFlag, void (*_delay)(uint32_t), const InterruptInputList& _irqPin, Drivers::GPIO_Ifc* const _gpio);
    ~SPL06_007();
    bool init() final;
};

}
