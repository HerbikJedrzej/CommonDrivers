#pragma once

#include <cstdio>

#include "UART_Ifc.hh"
#include "OwnExceptions.hh"

namespace Drivers {

class Logger : public DriverIfc {
    static Logger* instance; 
    UART_Ifc* uart;
    const unsigned int& timer;
    unsigned int transmittingStart;
    unsigned int transmittingStop;
    unsigned int used;
    const unsigned int maxDataBuffor;
    const unsigned int maxSingleMessageSize;
    char* buffer;
    char* singleMessageBuffer;

public:
    enum Level {
        FATAL=0,
        ERROR,
        WARNING,
        INIT,
        INFO,
        DEBUG
    };
    Level usedLevel;
    Logger(UART_Ifc* uart, const unsigned int& timer, const unsigned int maxDataBuffor, const unsigned int maxSingleMessageSize);
    ~Logger();
    static Logger& getInstance();
    void handleTimeEvent(DriverIfc*) override;
    void handleFinish(DriverIfc*) override;
    void handleError(DriverIfc*) override;
    void handleAbort(DriverIfc*) override;
    
    // -1 -> valL  < valR
    //  0 -> valL == valR
    //  1 -> valL  > valR
    int compareInRange(const unsigned int&,const unsigned int&);
    void log(const uint16_t& size);
    const char* getLevel(const Level& Level);

    template <typename... Types> 
    void log(Level level, const char* msg, Types... var){
        if(level > usedLevel)
            return;
        uint16_t size = snprintf(singleMessageBuffer, maxSingleMessageSize, "%s(%5u): ", getLevel(level), timer); 
        size += snprintf(&singleMessageBuffer[size], maxSingleMessageSize - size, msg, var...);
        if(size >= maxSingleMessageSize)
            THROW_out_of_range("Error log text is bigger than buffor.");
        singleMessageBuffer[size++] = '\n';
        log(size);
    }

    template <typename... Types> 
    void fatal(const char* msg, Types... var) { log(FATAL, msg, var...); }
    template <typename... Types> 
    void error(const char* msg, Types... var) { log(ERROR, msg, var...); }
    template <typename... Types> 
    void warning(const char* msg, Types... var) { log(WARNING, msg, var...); }
    template <typename... Types> 
    void init(const char* msg, Types... var) { log(INIT, msg, var...); }
    template <typename... Types> 
    void info(const char* msg, Types... var) { log(INFO, msg, var...); }
    template <typename... Types> 
    void debug(const char* msg, Types... var) { log(DEBUG, msg, var...); }
};

}