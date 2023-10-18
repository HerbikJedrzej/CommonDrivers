#include<iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <string>
#include <thread>
#include <exception>

#include "Logger.hh"
#include "UART_mock.hh"

#define UART_BUSSY .WillOnce(Return(true))
#define UART_NOT_BUSSY .WillOnce(Return(false))
#define UART_BUSSY_CALLS(TIMES) (TIMES)
#define UART_NOT_BUSSY_CALLS(TIMES) (TIMES)

using Drivers::Logger;
using Drivers::UartMock;
using Drivers::BusStatus;

using namespace ::testing;

class LoggerTest : public Test {
    std::shared_ptr<Logger> sut;
    std::shared_ptr<UartMock> uart;
    uint8_t* logMsg;
    uint16_t logMsgSize;
    unsigned int timer = 0;

protected:
    UartMock& getUart() {
        return *uart.get();
    }

    void checkLogMessage(const char* msg, bool addNewLine = true) {
        std::string text = (char*)logMsg;
        std::string expected = msg;
        if(addNewLine)
            expected += "\n";
        EXPECT_STREQ(expected.c_str(), text.substr(0,logMsgSize).c_str());
    }

    void incrementTimer() {
        timer++;
    }

    void setLogLevel(Logger::Level level) {
        sut->usedLevel = level;
    }

    void SetUp() override {
        timer = 0;
        uart = std::make_shared<UartMock>();
        sut = std::make_shared<Logger>(uart.get(), timer, 250, 40);
        EXPECT_CALL(getUart(), read(_, _)).Times(0);
        EXPECT_CALL(getUart(), write(_, _)).Times(0);
        EXPECT_CALL(getUart(), writeDMA(_, _, _)).Times(0);
        ON_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).WillByDefault(DoAll(SaveArg<0>(&logMsg), SaveArg<1>(&logMsgSize)));
    }

    void TearDown() override {
        sut.reset();
        uart.reset();
        logMsgSize = 0;
    }
};

TEST_F(LoggerTest, BaseLog){
    EXPECT_CALL(getUart(), isBussy()).Times(UART_BUSSY_CALLS(3))
        UART_NOT_BUSSY
        UART_BUSSY
        UART_NOT_BUSSY;
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(2);

    Logger::getInstance().fatal("Hello %s", "Test");
    checkLogMessage("FATAL(    0): Hello Test");
    incrementTimer();
    Logger::getInstance().error("Hello %s", "Test");
    checkLogMessage("FATAL(    0): Hello Test");
    incrementTimer();
    Logger::getInstance().warning("Hello %s", "Test");
    checkLogMessage("ERROR(    1): Hello Test\nWARNING(    2): Hello Test");
    incrementTimer();
}

TEST_F(LoggerTest, CheckEachLogLevelAndLevelMechanism){
    constexpr int calls = 9;
    EXPECT_CALL(getUart(), isBussy()).Times(UART_BUSSY_CALLS(calls))
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY;
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(calls);

    Logger::getInstance().fatal("Hello %s", "Test");
    checkLogMessage("FATAL(    0): Hello Test");
    incrementTimer();
    Logger::getInstance().warning("Hello %s", "Test");
    checkLogMessage("WARNING(    1): Hello Test");
    incrementTimer();
    Logger::getInstance().error("Hello %s", "Test");
    checkLogMessage("ERROR(    2): Hello Test");
    incrementTimer();
    Logger::getInstance().init("Hello %s", "Test");
    incrementTimer();
    Logger::getInstance().info("Hello %s", "Test");
    incrementTimer();
    Logger::getInstance().debug("Hello %s", "Test");
    incrementTimer();

    setLogLevel(Logger::Level::DEBUG);

    Logger::getInstance().fatal("Hello %s", "Test");
    checkLogMessage("FATAL(    6): Hello Test");
    incrementTimer();
    Logger::getInstance().warning("Hello %s", "Test");
    checkLogMessage("WARNING(    7): Hello Test");
    incrementTimer();
    Logger::getInstance().error("Hello %s", "Test");
    checkLogMessage("ERROR(    8): Hello Test");
    incrementTimer();
    Logger::getInstance().init("Hello %s", "Test");
    checkLogMessage("INIT(    9): Hello Test");
    incrementTimer();
    Logger::getInstance().info("Hello %s", "Test");
    checkLogMessage("INFO(   10): Hello Test");
    incrementTimer();
    Logger::getInstance().debug("Hello %s", "Test");
    checkLogMessage("DEBUG(   11): Hello Test");
}

TEST_F(LoggerTest, AddToQueueWhenBussy){
    EXPECT_CALL(getUart(), isBussy()).Times(UART_BUSSY_CALLS(3))
        UART_BUSSY
        UART_BUSSY
        UART_BUSSY;
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).Times(0);
    incrementTimer();
    Logger::getInstance().fatal("Hello %s", "Bussy 1");
    incrementTimer();
    Logger::getInstance().fatal("Hello %s", "Bussy 2");
    incrementTimer();
    Logger::getInstance().debug("Hello %s", "Not expected");
    incrementTimer();
    Logger::getInstance().fatal("Hello %s", "Even more bussy");
    incrementTimer();

    EXPECT_CALL(getUart(), isBussy()).Times(0);
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(1);
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("FATAL(    1): Hello Bussy 1\nFATAL(    2): Hello Bussy 2\nFATAL(    4): Hello Even more bussy");

    incrementTimer();

    EXPECT_CALL(getUart(), isBussy()).Times(0);
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(0);
    Logger::getInstance().handleFinish(nullptr);

    checkLogMessage("FATAL(    1): Hello Bussy 1\nFATAL(    2): Hello Bussy 2\nFATAL(    4): Hello Even more bussy");
}

TEST_F(LoggerTest, AddToQueueWhenNotBussy){
    EXPECT_CALL(getUart(), isBussy()).Times(UART_NOT_BUSSY_CALLS(3))
        UART_NOT_BUSSY
        UART_NOT_BUSSY
        UART_NOT_BUSSY;
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).Times(3);
    incrementTimer();
    Logger::getInstance().fatal("Hello %s", "Bussy 1");
    checkLogMessage("FATAL(    1): Hello Bussy 1");

    incrementTimer();
    Logger::getInstance().fatal("Hello %s", "Bussy 2");
    checkLogMessage("FATAL(    2): Hello Bussy 2");

    incrementTimer();
    Logger::getInstance().debug("Hello %s", "Not expected");

    incrementTimer();
    Logger::getInstance().fatal("Hello %s", "Even more bussy");
    checkLogMessage("FATAL(    4): Hello Even more bussy");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("FATAL(    4): Hello Even more bussy");
}

TEST_F(LoggerTest, AddToQueueWhenNotBussyButPerformingSlow){
    EXPECT_CALL(getUart(), isBussy()).Times(UART_NOT_BUSSY_CALLS(2) + UART_BUSSY_CALLS(6))
        UART_NOT_BUSSY
        UART_BUSSY
        UART_BUSSY
        UART_NOT_BUSSY
        UART_BUSSY
        UART_BUSSY
        UART_BUSSY
        UART_BUSSY;
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).Times(2);

    incrementTimer();

    Logger::getInstance().fatal("Hello %s", "Not Bussy");
    checkLogMessage("FATAL(    1): Hello Not Bussy");
    incrementTimer();
    Logger::getInstance().error("Hello %s", "Bussy 1");
    Logger::getInstance().fatal("Hello %s", "Bussy 2");
    incrementTimer();
    checkLogMessage("FATAL(    1): Hello Not Bussy");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("ERROR(    2): Hello Bussy 1\nFATAL(    2): Hello Bussy 2");
    Logger::getInstance().handleFinish(nullptr);
    Logger::getInstance().handleFinish(nullptr);

    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(1);

    Logger::getInstance().fatal("Hello %s", "Bussy Begi 1");
    checkLogMessage("FATAL(    3): Hello Bussy Begi 1");
    Logger::getInstance().handleFinish(nullptr);

    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(0);

    Logger::getInstance().error("Hello %s", "Bussy 3");
    incrementTimer();
    Logger::getInstance().error("Hello %s", "Bussy 4");
    Logger::getInstance().fatal("Hello %s", "Bussy Begi 2");
    Logger::getInstance().warning("Hello %s", "Bussy 5");

    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(1);
    checkLogMessage("FATAL(    3): Hello Bussy Begi 1");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("ERROR(    3): Hello Bussy 3\nERROR(    4): Hello Bussy 4\nFATAL(    4): Hello Bussy Begi 2\nWARNING(    4): Hello Bussy 5");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("ERROR(    3): Hello Bussy 3\nERROR(    4): Hello Bussy 4\nFATAL(    4): Hello Bussy Begi 2\nWARNING(    4): Hello Bussy 5");

    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, &Logger::getInstance())).Times(0);
    Logger::getInstance().handleFinish(nullptr);
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("ERROR(    3): Hello Bussy 3\nERROR(    4): Hello Bussy 4\nFATAL(    4): Hello Bussy Begi 2\nWARNING(    4): Hello Bussy 5");
}

TEST_F(LoggerTest, TooSmallSingleBuffer) {
    EXPECT_CALL(getUart(), isBussy()).Times(UART_BUSSY_CALLS(1))
        UART_BUSSY;
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).Times(1);
    incrementTimer();

    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("FATAL(    1): Test generic test: qwerty");

    incrementTimer();
    try {
        Logger::getInstance().fatal("Test generic test: %s", "qwertyu");
        FAIL() << "Shouldn't reach";
    } catch(std::out_of_range& e) {
        EXPECT_STREQ(e.what(), "Error log text is bigger than buffor.");
    }
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("FATAL(    1): Test generic test: qwerty");
}

TEST_F(LoggerTest, UseAllBuffer) {
    EXPECT_CALL(getUart(), isBussy()).Times(UART_BUSSY_CALLS(8))
        UART_BUSSY UART_BUSSY UART_BUSSY UART_BUSSY
        UART_BUSSY UART_BUSSY UART_BUSSY UART_BUSSY;
    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).Times(1);

    incrementTimer();
    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("FATAL(    1): Test generic test: qwerty");
    incrementTimer();
    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    incrementTimer();
    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    incrementTimer();
    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    incrementTimer();
    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    incrementTimer();
    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    incrementTimer();
    try {
        Logger::getInstance().fatal("Test generic test: %s", "qwerty");
        FAIL() << "Exception expected";
    } catch(std::out_of_range& e) {
        EXPECT_STREQ("End of logger buffer.", e.what());
    }

    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).Times(1);
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("FATAL(    2): Test generic test: qwerty\n"\
        "FATAL(    3): Test generic test: qwerty\n"\
        "FATAL(    4): Test generic test: qwerty\n"\
        "FATAL(    5): Test generic test: qwerty\n"\
        "FATAL(    6): Test generic test: qwerty");

    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    incrementTimer();
    try {
        Logger::getInstance().fatal("Test generic test: %s", "qwerty");
        FAIL() << "Exception expected";
    } catch(std::out_of_range& e) {
        EXPECT_STREQ("End of logger buffer.", e.what());
    }

    EXPECT_CALL(getUart(), writeDMAwithoutAlocate(_, _, _)).Times(2);
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("FATAL(    ", false);
    Logger::getInstance().fatal("Test generic test: %s", "qwerty");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("7): Test generic test: qwerty\nFATAL(    8): Test generic test: qwerty");
    Logger::getInstance().handleFinish(nullptr);
    checkLogMessage("7): Test generic test: qwerty\nFATAL(    8): Test generic test: qwerty");
}

TEST_F(LoggerTest, compareInRange) {
    constexpr unsigned int maxBuff = 250;
    EXPECT_EQ(1, Logger::getInstance().compareInRange(4, 2));
    EXPECT_EQ(0, Logger::getInstance().compareInRange(3, 3));
    EXPECT_EQ(-1, Logger::getInstance().compareInRange(2, 4));

    EXPECT_EQ(1, Logger::getInstance().compareInRange(maxBuff - 1, maxBuff - 2));
    EXPECT_EQ(0, Logger::getInstance().compareInRange(maxBuff - 1, maxBuff - 1));
    EXPECT_EQ(-1, Logger::getInstance().compareInRange(maxBuff - 2,maxBuff - 1));

    EXPECT_EQ(1, Logger::getInstance().compareInRange(2, maxBuff-2));
    EXPECT_EQ(-1, Logger::getInstance().compareInRange(maxBuff-2, 1));

    EXPECT_EQ(1, Logger::getInstance().compareInRange(maxBuff/4 * 3 - 1, maxBuff/4 + 1));
    EXPECT_EQ(-1, Logger::getInstance().compareInRange(maxBuff/4 * 3 + 1, maxBuff/4 - 1));
}

namespace {

class EmptyUart: public Drivers::UART_Ifc {
    bool working = true;
    uint8_t *msg = nullptr;
    unsigned int done = 0;
    uint16_t size = 0;
    std::thread t;

    void doSomething() {
        try {
            while(working) {
                if(msg != nullptr) {
                    std::string((char*)msg).substr(0,size).find("ERROR");
                    msg = nullptr;
                    done++;
                    Logger::getInstance().handleFinish(nullptr);
                }
            }
        } catch(std::out_of_range& e) {
            FAIL() << ("Exception: " + std::string(e.what())) << std::endl;
        } catch (...) {
            FAIL() << "Exception" << std::endl;
        }
    }
public:
    EmptyUart():
        UART_Ifc([](uint32_t)->void {}),
        t(&EmptyUart::doSomething, this) {}

    void stop() {
        working = false;
        t.join();
    }

    BusStatus read(uint8_t*, uint16_t) override {
        throw std::invalid_argument("Shouldn't be used");
    }
    BusStatus write(uint8_t*, uint16_t) override {
        throw std::invalid_argument("Shouldn't be used");
    }
    BusStatus writeDMA(uint8_t*, uint16_t, DriverIfc*) override {
        throw std::invalid_argument("Shouldn't be used");
    }
    BusStatus writeDMAwithoutAlocate(uint8_t* _msg, uint16_t _size, DriverIfc*) override {
        if(msg != nullptr)
            return BusStatus::BUSY;
        size = _size;
        msg = _msg;
        return BusStatus::OK;
    }
    
    bool isBussy() override {
        return msg != nullptr;
    }
};

}

TEST_F(LoggerTest, ParallerFailed) {
    using namespace std::chrono_literals;
    EmptyUart uart;
    unsigned int timer = 0;
    constexpr unsigned int maxMessageSize = 32;
    Logger sut(&uart, timer, 10*maxMessageSize - maxMessageSize/2, maxMessageSize);
    std::this_thread::sleep_for(200ms);
    try {
        for(timer = 0; timer < 100000; timer++) {
            Logger::getInstance().error("Some error: %5d", timer);
        }
        FAIL() << "Notexpected" << std::endl;
    } catch(std::out_of_range& e) {
        ASSERT_TRUE(true) << ("Exception2: " + std::string(e.what())) << std::endl;
    } catch (...) {
        FAIL() << "Exception2" << std::endl;
    }
    std::this_thread::sleep_for(10ms);
    uart.stop();
}

TEST_F(LoggerTest, ParallerPass) {
    using namespace std::chrono_literals;
    EmptyUart uart;
    unsigned int timer = 0;
    constexpr unsigned int maxMessageSize = 32;
    Logger sut(&uart, timer, 10000*maxMessageSize - maxMessageSize/2, maxMessageSize);
    std::this_thread::sleep_for(200ms);
    try {
        for(timer = 0; timer < 1000; timer++) {
            std::this_thread::sleep_for(1ms);
            Logger::getInstance().error("Some error: %5d", timer);
        }
    } catch(std::out_of_range& e) {
        FAIL() << ("Exception2: " + std::string(e.what())) << std::endl;
    } catch (...) {
        FAIL() << "Exception2" << std::endl;
    }
    std::this_thread::sleep_for(10ms);
    uart.stop();
}
