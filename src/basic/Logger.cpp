#include "Logger.hh"

namespace Drivers {

Logger* Logger::instance = nullptr; 

Logger::Logger(UART_Ifc* uart, const unsigned int& timer, const unsigned int maxDataBuffor, const unsigned int maxSingleMessageSize):
    DriverIfc([](uint32_t)-> void {}),
    uart(uart),
    timer(timer),
    transmittingStart(0),
    transmittingStop(0),
    used(0),
    maxDataBuffor(maxDataBuffor),
    maxSingleMessageSize(maxSingleMessageSize),
    buffer(new char[maxDataBuffor]),
    singleMessageBuffer(new char[maxSingleMessageSize]),
    usedLevel(WARNING) {
        Logger::instance = this;
        if(buffer < (void*)0)
            THROW_out_of_range("Cannot allocate memory for logger buffer.");
    }

Logger::~Logger() {
    Logger::instance = nullptr;
    delete[] singleMessageBuffer;
    delete[] buffer;
}

Logger& Logger::getInstance() {
    return *instance;
}

void Logger::handleTimeEvent(DriverIfc*) {
}

void Logger::handleError(DriverIfc* d) {
    handleFinish(d);
}

void Logger::handleAbort(DriverIfc* d) {
    handleFinish(d);
}

const char* Logger::getLevel(const Level& level) {
    switch (level) {
    case Level::FATAL:
        return "FATAL";
    case Level::ERROR:
        return "ERROR";
    case Level::WARNING:
        return "WARNING";
    case Level::INIT:
        return "INIT";
    case Level::INFO:
        return "INFO";
    case Level::DEBUG:
        return "DEBUG";
    default:
        THROW_invalid_argument("Unknown log level");
    }
}

void Logger::log(const uint16_t& size) {
    const unsigned int acctualUsed = (used + size)%maxDataBuffor;
    if(compareInRange(used, transmittingStart) == -1 && compareInRange(acctualUsed, transmittingStart) != -1)
        THROW_out_of_range("End of logger buffer.");
    for(int i = 0; i < size; i++) {
        buffer[(used + i)%maxDataBuffor] = singleMessageBuffer[i];
    }
    used = acctualUsed;
    if(!uart->isBussy()) {
        handleFinish(this);
    }
}

void Logger::handleFinish(DriverIfc*) {
    if(transmittingStop == maxDataBuffor){
        transmittingStop = 0;
    }
    transmittingStart = transmittingStop;
    if(used < transmittingStart)
        transmittingStop = maxDataBuffor;
    else if(used == transmittingStart)
        return;
    else
        transmittingStop = used;
    uart->writeDMAwithoutAlocate((uint8_t*)&buffer[transmittingStart], uint16_t(transmittingStop - transmittingStart), this); 
}

int Logger::compareInRange(const unsigned int& val1,const unsigned int& val2) {
    if(val1 == val2)
        return 0;
    int diff1 = 0;
    int diff2 = 0;
    if(val1 > val2) {
        diff1 = val1 - val2;
        diff2 = val2 + maxDataBuffor - val1;
    } else {
        diff1 = val1 + maxDataBuffor - val2;
        diff2 = val2 - val1;
    }
    return diff1 < diff2? 1 : -1;
}

}