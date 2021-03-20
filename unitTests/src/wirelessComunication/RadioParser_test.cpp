#include <gtest/gtest.h>
#include <RadioParser.hh>
#include <cstdlib>
#include <vector>
#include <memory>

namespace RadioParserTest{

enum RadioBitTest{
    mainOperationBit,
	rollaAngle,
	pitchaAngle,
	yawlaAngle,
	altitudeIncremetion,
	optionNumber,
	optionValue
};

class Radio_testhelper : public Drivers::RadioIfc{
    const uint8_t testKey = {0x67};
    unsigned int testTime = {0};
    uint8_t* const dataIn;
    uint8_t* const dataOut;
    const unsigned int dataSize;
public:
    Radio_testhelper(uint8_t* const _dataIn, uint8_t* const _dataOut, const unsigned int _dataSize):
        Drivers::RadioIfc(testKey, testTime),
        dataIn(_dataIn),
        dataOut(_dataOut),
        dataSize(_dataSize)
        {}
	void init() override{};
	void handleTimeEvent(Drivers::DriverIfc*) override{};
    bool isKyeCorrectTest = {true};
    bool isAckCorrectTest = {true};
    bool isComunicationCorrectTest = {true};
    bool isKyeCorrect() override{
        return isKyeCorrectTest;
    };
	bool isAckCorrect() override{
        return isAckCorrectTest;
    };
	bool isComunicationCorrect() override{
        return isComunicationCorrectTest;
    };

    uint8_t& setTx(unsigned int i){
        if(!dataOut)
            throw std::invalid_argument("Wrong mode for setTx.");
        if(i > dataSize)
            throw std::out_of_range("Trying to get more radio data than declrated.");
        return dataOut[i];
    }

    uint8_t& setRx(unsigned int i){
        if(!dataIn)
            throw std::invalid_argument("Wrong mode for setRx.");
        if(i > dataSize)
            throw std::out_of_range("Trying to get more radio data than declrated.");
        return dataIn[i];
    }

    uint8_t getTx(unsigned int i) const{
        if(!dataOut)
            throw std::invalid_argument("Wrong mode for getTx.");
        if(i > dataSize)
            throw std::out_of_range("Trying to get more radio data than declrated.");
        return dataOut[i];
    }

    uint8_t getRx(unsigned int i) const{
        if(!dataIn)
            throw std::invalid_argument("Wrong mode for getRx.");
        if(i > dataSize)
            throw std::out_of_range("Trying to get more radio data than declrated.");
        return dataIn[i];
    }
}; 

class TimeIterator{
    uint8_t transsmisionCounter = {0};
    Drivers::RadioParser& waker1;
    Drivers::RadioParser& waker2;
public:
    TimeIterator(Drivers::RadioParser& r1, Drivers::RadioParser& r2):
    waker1(r1),
    waker2(r2){};
    void increment(){
        waker2.run();
        waker1.run();
        transsmisionCounter++;
        if(transsmisionCounter == 8)
            transsmisionCounter = 0;
    }
    uint8_t trID(){
        return transsmisionCounter & 0x07;
    }
};

///////////////////////////////////////////////////////////////////
///////////////////////  Test data parsing  ///////////////////////
///////////////////////////////////////////////////////////////////

TEST(RadioParser_Test, set_get_EngineOnOption){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setEngineOnOption(true) );
    EXPECT_ANY_THROW( parserDrone.setEngineOnOption(true) );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getEngineOnOption()) );
    EXPECT_ANY_THROW( EXPECT_FALSE(parserRemoteControl.getEngineOnOption()) );
    for(uint8_t i = 0; i < 54; i++){
        waker.increment();
        EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getEngineOnOption()) );
        EXPECT_ANY_THROW( EXPECT_TRUE(parserRemoteControl.getEngineOnOption()) );
        EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b01000000 | waker.trID());
    }
    EXPECT_NO_THROW(  parserRemoteControl.setEngineOnOption(false) );
    for(uint8_t i = 0; i < 54; i++){
        waker.increment();
        EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getEngineOnOption()) );
        EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00000000 | waker.trID());
    }
}

TEST(RadioParser_Test, set_get_FlyOnOption){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setFlyOnOption(true) );
    EXPECT_ANY_THROW( parserDrone.setFlyOnOption(true) );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getFlyOnOption()) );
    EXPECT_ANY_THROW( EXPECT_FALSE(parserRemoteControl.getFlyOnOption()) );
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getFlyOnOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00000000 | waker.trID());
    EXPECT_NO_THROW(  parserRemoteControl.setEngineOnOption(true) );
    EXPECT_NO_THROW(  parserRemoteControl.setFlyOnOption(true) );
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getFlyOnOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b11000000 | waker.trID());
    EXPECT_NO_THROW(  parserRemoteControl.setEngineOnOption(false) );
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getFlyOnOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00000000 | waker.trID());
    EXPECT_NO_THROW(  parserRemoteControl.setEngineOnOption(true) );
    EXPECT_NO_THROW(  parserRemoteControl.setFlyOnOption(true) );
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getFlyOnOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b11000000 | waker.trID());
    EXPECT_NO_THROW(  parserRemoteControl.setFlyOnOption(false) );
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getFlyOnOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b01000000 | waker.trID());
    EXPECT_NO_THROW(  parserRemoteControl.setEngineOnOption(false) );
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getFlyOnOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00000000 | waker.trID());
}

TEST(RadioParser_Test, get_set_JoyRightButtonOption){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setJoyRightButtonOption() );
    EXPECT_ANY_THROW( parserDrone.setJoyRightButtonOption() );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyRightButtonOption()) );
    EXPECT_ANY_THROW( EXPECT_FALSE(parserRemoteControl.getJoyRightButtonOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0);
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00100000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getJoyRightButtonOption()) );
    for(uint8_t i = 2; i < 50; i++){
        waker.increment();
        if(i == 20)
            parserRemoteControl.setJoyRightButtonOption();
        EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00100000 | waker.trID());
        EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyRightButtonOption()) );
    }
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00000000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyRightButtonOption()) );
    EXPECT_NO_THROW(  parserRemoteControl.setJoyRightButtonOption() );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyRightButtonOption()) );
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00100000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getJoyRightButtonOption()) );
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00100000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyRightButtonOption()) );
}

TEST(RadioParser_Test, get_set_JoyLeftButtonOption){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setJoyLeftButtonOption() );
    EXPECT_ANY_THROW( parserDrone.setJoyLeftButtonOption() );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyLeftButtonOption()) );
    EXPECT_ANY_THROW( EXPECT_FALSE(parserRemoteControl.getJoyLeftButtonOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0);
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00010000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getJoyLeftButtonOption()) );
    for(uint8_t i = 2; i < 50; i++){
        waker.increment();
        if(i == 20)
            parserRemoteControl.setJoyLeftButtonOption();
        EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00010000 | waker.trID());
        EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyLeftButtonOption()) );
    }
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00000000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyLeftButtonOption()) );
    EXPECT_NO_THROW(  parserRemoteControl.setJoyLeftButtonOption() );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyLeftButtonOption()) );
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00010000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getJoyLeftButtonOption()) );
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00010000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getJoyLeftButtonOption()) );
}

TEST(RadioParser_Test, get_set_SpecialButtonOption){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setSpecialButtonOption() );
    EXPECT_ANY_THROW( parserDrone.setSpecialButtonOption() );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getSpecialButtonOption()) );
    EXPECT_ANY_THROW( EXPECT_FALSE(parserRemoteControl.getSpecialButtonOption()) );
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0);
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00001000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getSpecialButtonOption()) );
    for(uint8_t i = 2; i < 50; i++){
        waker.increment();
        if(i == 20)
            parserRemoteControl.setSpecialButtonOption();
        EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00001000 | waker.trID());
        EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getSpecialButtonOption()) );
    }
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00000000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getSpecialButtonOption()) );
    EXPECT_NO_THROW(  parserRemoteControl.setSpecialButtonOption() );
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getSpecialButtonOption()) );
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00001000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_TRUE(parserDrone.getSpecialButtonOption()) );
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::mainOperationBit], 0b00001000 | waker.trID());
    EXPECT_NO_THROW(  EXPECT_FALSE(parserDrone.getSpecialButtonOption()) );
}

TEST(RadioParser_Test, get_set_RollValue){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setRollValue(-75) );
    EXPECT_ANY_THROW( parserDrone.setRollValue(6) );
    EXPECT_NO_THROW(  EXPECT_NE(parserDrone.getRollValue(), -75) );
    EXPECT_ANY_THROW( EXPECT_EQ(parserRemoteControl.getRollValue(), -75) );
    EXPECT_EQ(dataRtoD[RadioBitTest::rollaAngle], uint8_t(-75));
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getRollValue(), -75) );
    EXPECT_EQ(dataRtoD[RadioBitTest::rollaAngle], uint8_t(-75));
    EXPECT_NO_THROW(  parserRemoteControl.setRollValue(68) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getRollValue(), -75) );
    EXPECT_EQ(dataRtoD[RadioBitTest::rollaAngle], 68);
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getRollValue(), 68) );
    EXPECT_EQ(dataRtoD[RadioBitTest::rollaAngle], 68);
}

TEST(RadioParser_Test, get_set_PitchValue){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setPitchValue(56) );
    EXPECT_ANY_THROW( parserDrone.setPitchValue(6) );
    EXPECT_NO_THROW(  EXPECT_NE(parserDrone.getPitchValue(), 56) );
    EXPECT_ANY_THROW( EXPECT_EQ(parserRemoteControl.getPitchValue(), 56) );
    EXPECT_EQ(dataRtoD[RadioBitTest::pitchaAngle], 56);
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getPitchValue(), 56) );
    EXPECT_EQ(dataRtoD[RadioBitTest::pitchaAngle], 56);
    EXPECT_NO_THROW(  parserRemoteControl.setPitchValue(-32) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getPitchValue(), 56) );
    EXPECT_EQ(dataRtoD[RadioBitTest::pitchaAngle], uint8_t(-32));
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getPitchValue(), -32) );
    EXPECT_EQ(dataRtoD[RadioBitTest::pitchaAngle], uint8_t(-32));
}

TEST(RadioParser_Test, get_set_YawlValue){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setYawlValue(73) );
    EXPECT_ANY_THROW( parserDrone.setYawlValue(6) );
    EXPECT_NO_THROW(  EXPECT_NE(parserDrone.getYawlValue(), 73) );
    EXPECT_ANY_THROW( EXPECT_EQ(parserRemoteControl.getYawlValue(), 73) );
    EXPECT_EQ(dataRtoD[RadioBitTest::yawlaAngle], 73);
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getYawlValue(), 73) );
    EXPECT_EQ(dataRtoD[RadioBitTest::yawlaAngle], 73);
    EXPECT_NO_THROW(  parserRemoteControl.setYawlValue(-98) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getYawlValue(), 73) );
    EXPECT_EQ(dataRtoD[RadioBitTest::yawlaAngle], uint8_t(-98));
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getYawlValue(), -98) );
    EXPECT_EQ(dataRtoD[RadioBitTest::yawlaAngle], uint8_t(-98));
}

TEST(RadioParser_Test, get_set_AltitudeIncremetionValue){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setAltitudeIncremetionValue(16) );
    EXPECT_ANY_THROW( parserDrone.setAltitudeIncremetionValue(6) );
    EXPECT_NO_THROW(  EXPECT_NE(parserDrone.getAltitudeIncremetionValue(), 16) );
    EXPECT_ANY_THROW( EXPECT_EQ(parserRemoteControl.getAltitudeIncremetionValue(), 16) );
    EXPECT_EQ(dataRtoD[RadioBitTest::altitudeIncremetion], 16);
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getAltitudeIncremetionValue(), 16) );
    EXPECT_EQ(dataRtoD[RadioBitTest::altitudeIncremetion], 16);
    EXPECT_NO_THROW(  parserRemoteControl.setAltitudeIncremetionValue(-67) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getAltitudeIncremetionValue(), 16) );
    EXPECT_EQ(dataRtoD[RadioBitTest::altitudeIncremetion], uint8_t(-67));
    waker.increment();
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getAltitudeIncremetionValue(), -67) );
    EXPECT_EQ(dataRtoD[RadioBitTest::altitudeIncremetion], uint8_t(-67));
}

TEST(RadioParser_Test, get_set_MainOptionNumber_MainOptionValue){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    EXPECT_NO_THROW(  parserRemoteControl.setMainOption(56, -25) );
    EXPECT_ANY_THROW( parserDrone.setMainOption(6, 47) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionNumber(), 0) );
    EXPECT_ANY_THROW( EXPECT_EQ(parserRemoteControl.getMainOptionNumber(), 56) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionValue(), 0) );
    EXPECT_ANY_THROW( EXPECT_EQ(parserRemoteControl.getMainOptionValue(), 56) );
    EXPECT_EQ(dataRtoD[RadioBitTest::optionNumber], 0);
    EXPECT_EQ(dataRtoD[RadioBitTest::optionValue], 0);
    waker.increment();
    EXPECT_EQ(dataRtoD[RadioBitTest::optionNumber], 56);
    EXPECT_EQ(dataRtoD[RadioBitTest::optionValue], uint8_t(-25));
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionNumber(), 56) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionValue(), -25) );
    for(uint8_t i = 2; i < 10; i++){
        waker.increment();
        if(i == 8)
            parserRemoteControl.setMainOption(221, -122);
        EXPECT_EQ(dataRtoD[RadioBitTest::optionNumber], 56);
        EXPECT_EQ(dataRtoD[RadioBitTest::optionValue], uint8_t(-25));
        EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionNumber(), 0) );
        EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionValue(), 0) );
    }
    waker.increment();    
    EXPECT_EQ(dataRtoD[RadioBitTest::optionNumber], 0);
    EXPECT_EQ(dataRtoD[RadioBitTest::optionValue], 0);
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionNumber(), 0) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionValue(), 0) );
    EXPECT_NO_THROW( parserRemoteControl.setMainOption(45, 78) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionNumber(), 0) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionValue(), 0) );
    EXPECT_EQ(dataRtoD[RadioBitTest::optionNumber], 0);
    EXPECT_EQ(dataRtoD[RadioBitTest::optionValue], 0);
    waker.increment();    
    EXPECT_EQ(dataRtoD[RadioBitTest::optionNumber], 45);
    EXPECT_EQ(dataRtoD[RadioBitTest::optionValue], 78);
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionNumber(), 45) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionValue(), 78) );
    waker.increment();    
    EXPECT_EQ(dataRtoD[RadioBitTest::optionNumber], 45);
    EXPECT_EQ(dataRtoD[RadioBitTest::optionValue], 78);
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionNumber(), 0) );
    EXPECT_NO_THROW(  EXPECT_EQ(parserDrone.getMainOptionValue(), 0) );
}

/////////////////////////////////////////////////////////////////////
/////////////////////  Tests of time behaviour  /////////////////////
/////////////////////////////////////////////////////////////////////

TEST(RadioParser_Test, fly_engine_stop){
    uint8_t dataRtoD[7] = {0, 0, 0, 0, 0, 0, 0};
    Radio_testhelper radio1(dataRtoD, nullptr, 7);
    Radio_testhelper radio2(nullptr, dataRtoD, 7);
    Drivers::RadioParser parserDrone(&radio1, Drivers::RadioMode::Drone);
    Drivers::RadioParser parserRemoteControl(&radio2, Drivers::RadioMode::RemoteControl);
    TimeIterator waker(parserDrone, parserRemoteControl);
    parserRemoteControl.setEngineOnOption(true);
    parserRemoteControl.setFlyOnOption(true);
    parserRemoteControl.setJoyLeftButtonOption();
    parserRemoteControl.setRollValue(-24);
    parserRemoteControl.setPitchValue(124);
    parserRemoteControl.setYawlValue(4);
    parserRemoteControl.setAltitudeIncremetionValue(0);
    parserRemoteControl.setMainOption(68, -4);
    EXPECT_EQ(dataRtoD[0], 0);
    EXPECT_EQ(dataRtoD[1], uint8_t(-24));
    EXPECT_EQ(dataRtoD[2], 124);
    EXPECT_EQ(dataRtoD[3], 4);
    EXPECT_EQ(dataRtoD[4], 0);
    EXPECT_EQ(dataRtoD[5], 0);
    EXPECT_EQ(dataRtoD[6], 0);
    EXPECT_FALSE(parserDrone.getEngineOnOption());
    EXPECT_FALSE(parserDrone.getFlyOnOption());
    EXPECT_FALSE(parserDrone.getJoyRightButtonOption());
    EXPECT_FALSE(parserDrone.getJoyLeftButtonOption());
    EXPECT_FALSE(parserDrone.getSpecialButtonOption());
    EXPECT_EQ(parserDrone.getRollValue(), 0);
    EXPECT_EQ(parserDrone.getPitchValue(), 0);
    EXPECT_EQ(parserDrone.getYawlValue(), 0);
    EXPECT_EQ(parserDrone.getAltitudeIncremetionValue(), 0);
    EXPECT_EQ(parserDrone.getMainOptionNumber(), 0);
    EXPECT_EQ(parserDrone.getMainOptionValue(), 0);
    waker.increment();
    EXPECT_EQ(dataRtoD[0], 0b11010001);
    EXPECT_EQ(dataRtoD[1], uint8_t(-24));
    EXPECT_EQ(dataRtoD[2], 124);
    EXPECT_EQ(dataRtoD[3], 4);
    EXPECT_EQ(dataRtoD[4], 0);
    EXPECT_EQ(dataRtoD[5], 68);
    EXPECT_EQ(dataRtoD[6], uint8_t(-4));
    EXPECT_TRUE(parserDrone.getEngineOnOption());
    EXPECT_TRUE(parserDrone.getFlyOnOption());
    EXPECT_FALSE(parserDrone.getJoyRightButtonOption());
    EXPECT_TRUE(parserDrone.getJoyLeftButtonOption());
    EXPECT_FALSE(parserDrone.getSpecialButtonOption());
    EXPECT_EQ(parserDrone.getRollValue(), -24);
    EXPECT_EQ(parserDrone.getPitchValue(), 124);
    EXPECT_EQ(parserDrone.getYawlValue(), 4);
    EXPECT_EQ(parserDrone.getAltitudeIncremetionValue(), 0);
    EXPECT_EQ(parserDrone.getMainOptionNumber(), 68);
    EXPECT_EQ(parserDrone.getMainOptionValue(), -4);
    parserRemoteControl.setEngineOnOption(false);
    parserRemoteControl.setJoyRightButtonOption();
    parserRemoteControl.setRollValue(24);
    parserRemoteControl.setPitchValue(54);
    parserRemoteControl.setYawlValue(14);
    parserRemoteControl.setAltitudeIncremetionValue(-10);
    parserRemoteControl.setMainOption(168, 44);
    radio1.isComunicationCorrectTest = false;
    waker.increment();
    EXPECT_EQ(dataRtoD[0], 0b00110010);
    EXPECT_EQ(dataRtoD[1], 24);
    EXPECT_EQ(dataRtoD[2], 54);
    EXPECT_EQ(dataRtoD[3], 14);
    EXPECT_EQ(dataRtoD[4], uint8_t(-10));
    EXPECT_EQ(dataRtoD[5], 68);
    EXPECT_EQ(dataRtoD[6], uint8_t(-4));
    for(uint8_t i = 0; i < 11; i++){
        ASSERT_TRUE(parserDrone.getEngineOnOption());
        ASSERT_TRUE(parserDrone.getFlyOnOption());
        ASSERT_FALSE(parserDrone.getJoyRightButtonOption());
        ASSERT_FALSE(parserDrone.getJoyLeftButtonOption());
        ASSERT_FALSE(parserDrone.getSpecialButtonOption());
        ASSERT_EQ(parserDrone.getRollValue(), -24) << "Execution nr. " << uint16_t(i) << std::endl;
        ASSERT_EQ(parserDrone.getPitchValue(), 124);
        ASSERT_EQ(parserDrone.getYawlValue(), 4);
        ASSERT_EQ(parserDrone.getAltitudeIncremetionValue(), 0);
        ASSERT_EQ(parserDrone.getMainOptionNumber(), 68);
        ASSERT_EQ(parserDrone.getMainOptionValue(), -4);
        ASSERT_EQ(parserDrone.getTransmissionNumber(), waker.trID());
        ASSERT_EQ(parserRemoteControl.getTransmissionNumber(), waker.trID());
        waker.increment();
    }
    for(uint16_t i = 0; i < 5; i++){
        ASSERT_TRUE(parserDrone.getEngineOnOption());
        ASSERT_TRUE(parserDrone.getFlyOnOption()) << "Execution nr. " << uint16_t(i) << std::endl;
        ASSERT_FALSE(parserDrone.getJoyRightButtonOption());
        ASSERT_FALSE(parserDrone.getJoyLeftButtonOption());
        ASSERT_FALSE(parserDrone.getSpecialButtonOption());
        ASSERT_EQ(parserDrone.getRollValue(), 0);
        ASSERT_EQ(parserDrone.getPitchValue(), 0);
        ASSERT_EQ(parserDrone.getYawlValue(), 0);
        ASSERT_EQ(parserDrone.getAltitudeIncremetionValue(), -100);
        ASSERT_EQ(parserDrone.getMainOptionNumber(), 0);
        ASSERT_EQ(parserDrone.getMainOptionValue(), 0);
        ASSERT_EQ(parserDrone.getTransmissionNumber(), waker.trID());
        ASSERT_EQ(parserRemoteControl.getTransmissionNumber(), waker.trID());
        waker.increment();
    }
    for(uint16_t i = 1; i < 5985; i++){
        ASSERT_FALSE(parserDrone.getEngineOnOption()) << "Execution nr. " << uint16_t(i) << std::endl;
        ASSERT_FALSE(parserDrone.getFlyOnOption());
        ASSERT_FALSE(parserDrone.getJoyRightButtonOption());
        ASSERT_FALSE(parserDrone.getJoyLeftButtonOption());
        ASSERT_FALSE(parserDrone.getSpecialButtonOption());
        ASSERT_EQ(parserDrone.getRollValue(), 0);
        ASSERT_EQ(parserDrone.getPitchValue(), 0);
        ASSERT_EQ(parserDrone.getYawlValue(), 0);
        ASSERT_EQ(parserDrone.getAltitudeIncremetionValue(), -100);
        ASSERT_EQ(parserDrone.getMainOptionNumber(), 0);
        ASSERT_EQ(parserDrone.getMainOptionValue(), 0);
        ASSERT_EQ(parserDrone.getTransmissionNumber(), waker.trID());
        ASSERT_EQ(parserRemoteControl.getTransmissionNumber(), waker.trID());
        waker.increment();
    }
    EXPECT_FALSE(parserDrone.getEngineOnOption());
    EXPECT_FALSE(parserDrone.getFlyOnOption());
    EXPECT_FALSE(parserDrone.getJoyRightButtonOption());
    EXPECT_FALSE(parserDrone.getJoyLeftButtonOption());
    EXPECT_FALSE(parserDrone.getSpecialButtonOption());
    EXPECT_EQ(parserDrone.getRollValue(), 0);
    EXPECT_EQ(parserDrone.getPitchValue(), 0);
    EXPECT_EQ(parserDrone.getYawlValue(), 0);
    EXPECT_EQ(parserDrone.getAltitudeIncremetionValue(), -100);
    EXPECT_EQ(parserDrone.getMainOptionNumber(), 0);
    EXPECT_EQ(parserDrone.getMainOptionValue(), 0);
    EXPECT_ANY_THROW(waker.increment());
}

}