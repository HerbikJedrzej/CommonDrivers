#include "US-015.hh"
#include <OwnExceptions.hh>

namespace Drivers{

US_015::US_015(const Timer& _timer, GPIO_Ifc* const _gpio, const OutputList& _triger, const InputList& _echo, const InterruptInputList& _echoIrq, void(*_delay)(uint32_t)):
    DriverIfc(_delay),
    timer(_timer),
    gpio(_gpio),
    triger(_triger),
    echo(_echo),
    echoIrq(_echoIrq){}

double US_015::getDistanceInMilimeters(){
    return timeDiff * 0.17; // (timeDiff * 1000000)s * 340 m/s / 2
}

void US_015::init(){
    if(!gpio)
        THROW_invalid_argument("GPIO driver for ultrasonic sensor is nullptr.");
    gpio->subscribe(echoIrq, this);
    gpio->wakeMeUp(this);
}

void US_015::handleFinish(DriverIfc*){
    if(gpio->pin(echo))
        timeStamp = timer.getTimeStamp();
    else
        timeDiff = timer.timeDiffInMicroSeconds(timeStamp);
}

void US_015::handleTimeEvent(DriverIfc*){
    gpio->pin(triger, true);
    delayLoops();
    gpio->pin(triger, false);
}

}