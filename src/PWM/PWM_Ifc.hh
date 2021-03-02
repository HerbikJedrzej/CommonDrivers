#pragma once

#include <cstdint>

namespace Drivers{

class PWM_Ifc{
public:
    virtual uint8_t size() = 0;
    virtual void set(const double percent, const uint8_t& engine = 0) = 0;
};

}
