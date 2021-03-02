#pragma once

#include <stdint.h>
#include <GPIOconf.hh>
#include "DriverIfc.hh"
#include "SPI.hh"
#include "IMUsensorIfc.hh"

namespace Drivers{

class LSM6DS33 : public IMUsensorIfc
{
private:
    SPI_Ifc* spi;
    OutputList ssPinToToggle;
    uint8_t akcelerometrData[13];
    void handleTimeEvent(DriverIfc*) override;
    void handleFinish(DriverIfc*) override;
    bool checkIcId();
public:
    LSM6DS33(SPI_Ifc* _spi, OutputList _ssPinToToggle, bool& _akcAndGyroDataReadyFlag, bool& _magDataReadyFlag, void (*_delay)(uint32_t), const RottatedAxis& axis = none);
    ~LSM6DS33();
    bool init() override;
};

}
