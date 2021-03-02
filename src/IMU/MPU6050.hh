#pragma once

#include <stdint.h>
#include <GPIOconf.hh>
#include "DriverIfc.hh"
#include "I2C.hh"
#include "IMUsensorIfc.hh"

namespace Drivers{

class MPU6050 : public IMUsensorIfc
{
private:
    I2C_Ifc* i2c;
    uint8_t addr;
    static constexpr uint8_t size = {14};
    uint8_t sensorData[size];
    double temperatureMPU = {0};
    void handleTimeEvent(DriverIfc*) override;
    void handleFinish(DriverIfc*) override;
    bool checkIcId();
    bool reset();
public:
    MPU6050(I2C_Ifc* _i2c, uint8_t _addr, bool& _akcAndGyroDataReadyFlag, bool& _magDataReadyFlag, void (*_delay)(uint32_t), const RottatedAxis& axis = none);
    ~MPU6050();
    bool init() override;
};

}
