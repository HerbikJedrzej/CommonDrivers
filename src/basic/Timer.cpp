#include"Timer.hh"

namespace Drivers{

Timer::Timer(const double _freq, const uint32_t _resolution, const uint32_t _divider, void (*_delay)(uint32_t)):
    DriverIfc(_delay),
    cycle(0),
    resolution((1 + _resolution)),
    periodInUs(1000000 / (_freq / resolution / (1 + _divider))),
    notInitialisedCounter(0),
    counter(&notInitialisedCounter){}

void Timer::setCounterReference(volatile uint32_t& _counter){
    counter = &_counter;
}

void Timer::handleTimeEvent(DriverIfc*){
    cycle++;
}

Timer::TimeStamp Timer::getTimeStamp() const{
    return TimeStamp{cycle, *counter};
}

void Timer::updateSignalParams(const double _freq, const uint32_t _resolution, const uint32_t _divider){
    resolution = (1 + _resolution);
    periodInUs = 1000000 / (_freq / resolution / (1 + _divider));
}

unsigned int Timer::timeDiffInMicroSeconds(const Timer::TimeStamp& timeStamp) const{
    if(*counter < timeStamp.counter){
        const unsigned int counterDiff = resolution + *counter - timeStamp.counter;
        const unsigned int cyclesDiff = cycle - timeStamp.cycles - 1;
        return (cyclesDiff * periodInUs + double(counterDiff) / double(resolution) * periodInUs);
    }
    const unsigned int counterDiff = *counter - timeStamp.counter;
    const unsigned int cyclesDiff = cycle - timeStamp.cycles;
    return (cyclesDiff * periodInUs + double(counterDiff) / double(resolution) * periodInUs);
}

double Timer::timeDiffInSeconds(const TimeStamp& timeStamp) const{
    return
        (
            double(cycle - timeStamp.cycles)
            +
            (   double(*counter)
                -
                double(timeStamp.counter)
            )
            /
            double(resolution)
        )
        *
        double(periodInUs) / 1000000.0;
}

bool Timer::isAfter(const Timer::TimeStamp& timeStamp) const{
    if(timeStamp.cycles > cycle)
        return false;
    if((timeStamp.cycles < cycle) || (timeStamp.counter <= *counter))
        return true;
    return false;
}

}
