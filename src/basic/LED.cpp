#include "LED.hh"

namespace Drivers{

LED::LED(GPIO& _gpio, void (*_delay)(uint32_t)):
    DriverIfc(_delay),
    gpio(_gpio)
    {
        for(uint8_t i = 0; i < LedsList::end_of_leds; i++){
            outputList[i].output = OutputList::none;
            togglePeriod[i] = 0;
        }
        gpio.wakeMeUp(this);
    }

LED::~LED(){
    gpio.stopWakingMe(this);
}

void LED::addLED(const LedsList led, const OutputList output, const bool activeHigh){
    outputList[led] = {output, activeHigh};
}

bool LED::get(const LedsList& led){
    return gpio.pin(outputList[led].output);
}

void LED::set(const LedsList& led, bool on){
    gpio.pin(
        outputList[led].output,
        on?
            outputList[led].activeHigh
            :
            !outputList[led].activeHigh
        );
    togglePeriod[led] = 0;
}

void LED::toggle(const LedsList& led){
    const auto& pin = outputList[led].output;
    gpio.pin(pin, !gpio.pin(pin));
}

void LED::set(const LedsList& led, bool on, const uint8_t& toggleAfterLoops){
    gpio.pin(
        outputList[led].output,
        on?
            outputList[led].activeHigh
            :
            !outputList[led].activeHigh
        );
    togglePeriod[led] = toggleAfterLoops;
}

void LED::handleTimeEvent(DriverIfc*){
    for(uint8_t i = 0; i < LedsList::end_of_leds; i++){
        if(togglePeriod[i] == 0)
            continue;
        togglePeriod[i]--;
        if(togglePeriod[i] != 0)
            continue;
        const auto& pin = outputList[i].output;
        gpio.pin(pin, !gpio.pin(pin));
    }
}

}