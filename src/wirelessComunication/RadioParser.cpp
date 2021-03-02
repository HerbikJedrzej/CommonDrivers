#include "RadioParser.hh"
#include <OwnExceptions.hh>

namespace Drivers{

RadioParser::RadioParser(Drivers::RadioIfc* const _radio, const RadioTrybe _trybe):
radio(_radio),
trybe(_trybe){
}

RadioParser::~RadioParser(){
}

void RadioParser::decrementFilters(){
    if(joyRTime > 0)
        joyRTime--;
    if(joyLTime > 0)
        joyLTime--;
    if(specialOptTime > 0)
        specialOptTime--;
    if(mainNumberTime > 0)
        mainNumberTime--;
}

void RadioParser::timeEventForRemote(){
    uint8_t tmpBite = transmissionNumber & 0x07;
    if(engineOn){
        if(fly)
            tmpBite |= 0b11000000;
        else
            tmpBite |= 0b01000000;
    }
    if(joyRTime > 0)
        tmpBite |= 0b00100000;
    if(joyLTime > 0)
        tmpBite |= 0b00010000;
    if(specialOptTime > 0)
        tmpBite |= 0b00001000;
    radio->setTx(0) = tmpBite;
    if(mainNumberTime == 0){
        mainNumber = 0;
        mainValue = 0;
    }
    radio->setTx(5) = mainNumber;
    radio->setTx(6) = mainValue;
}

void RadioParser::timeEventForDrone(){
    if(fromLastSucces > 10){
        fly      = (fromLastSucces > 15)? false : fly;
        engineOn           = (fromLastSucces > 15)? false : engineOn;
        joyRTime           = 0;
        joyLTime           = 0;
        specialOptTime     = 0;
        roll               = 0;
        pitch              = 0;
        yawl               = 0;
        altitude           = -100;
        mainNumber         = 0;
        mainNumberTime     = 0;
        mainValue          = 0;
        if(fromLastSucces > 6000 && !engineOn)
            THROW_out_of_range("Missing transmision more than 1 minute.");
    }
    if(!radio->isComunicationCorrect()){
        fromLastSucces++;
        return;
    }
    else
        fromLastSucces = 0;

    engineOn = ((radio->getRx(0) & 0b01000000) != 0);
    fly      = ((radio->getRx(0) & 0b11000000) == 0b11000000);
    if(((radio->getRx(0) & 0b00100000) != 0) && (joyRTime == 0))
        joyRTime = 50;
    if(((radio->getRx(0) & 0b00010000) != 0) && (joyLTime == 0))
        joyLTime = 50;
    if(((radio->getRx(0) & 0b00001000) != 0) && (specialOptTime == 0))
        specialOptTime = 50;
    transmissionNumber = (radio->getRx(0) & 0b00000111);
    roll     = radio->getRx(1);
    pitch    = radio->getRx(2);
    yawl     = radio->getRx(3);
    altitude = radio->getRx(4);
    if((radio->getRx(5) > 0) && (mainNumberTime == 0)){
        mainNumberTime = 10;
        mainNumber = radio->getRx(5);
        mainValue  = radio->getRx(6);
    }
    else{
        mainNumber = 0;
        mainValue  = 0;
    }
}	

void RadioParser::run(){
    transmissionNumber++;
    if(transmissionNumber == 8)
        transmissionNumber = 0;
    decrementFilters();
    if(trybe == RadioTrybe::RemoteControl)
        timeEventForRemote();
    else if(trybe == RadioTrybe::Drone)
        timeEventForDrone();
    else
        THROW_invalid_argument("Not implemented parser type");
}

///////////////////////////////////////////////////////////////////
///////////////////////// Parsing methods /////////////////////////
///////////////////////////////////////////////////////////////////

bool RadioParser::getFlyOnOption() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getFlyOnOption.");
    return fly;
}

void RadioParser::setFlyOnOption(const bool val){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setFlyOnOption.");
    fly = val && engineOn;
}

bool RadioParser::getEngineOnOption() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getEngineOnOption.");
    return engineOn;
}

void RadioParser::setEngineOnOption(const bool val){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setEngineOnOption.");
    engineOn = val;
    fly = fly && engineOn;
}

bool RadioParser::getJoyRightButtonOption() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getJoyRightButtonOption.");
    return joyRTime == optionsInterval;
}

void RadioParser::setJoyRightButtonOption(){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setJoyRightButtonOption.");
    if(joyRTime == 0)
        joyRTime = optionsInterval;
}

bool RadioParser::getJoyLeftButtonOption() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getJoyLeftButtonOption.");
    return joyLTime == optionsInterval;
}

void RadioParser::setJoyLeftButtonOption(){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setJoyLeftButtonOption.");
    if(joyLTime == 0)
        joyLTime = optionsInterval;
}

bool RadioParser::getSpecialButtonOption() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getSpecialButtonOption.");
    return specialOptTime == optionsInterval;
}

void RadioParser::setSpecialButtonOption(){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setSpecialButtonOption.");
    if(specialOptTime == 0)
        specialOptTime = optionsInterval;
}

int8_t RadioParser::getRollValue() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getRollValue.");
    return roll;
}

void RadioParser::setRollValue(const int8_t val){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setRollValue.");
    radio->setTx(1) = val;
}

int8_t RadioParser::getPitchValue() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getPitchValue.");
    return pitch;
}

void RadioParser::setPitchValue(const int8_t val){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setPitchValue.");
    radio->setTx(2) = val;
}

int8_t RadioParser::getYawlValue() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getYawlValue.");
    return yawl;
}

void RadioParser::setYawlValue(const int8_t val){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setYawlValue.");
    radio->setTx(3) = val;
}

int8_t RadioParser::getAltitudeIncremetionValue() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getAltitudeIncremetionValue.");
    return altitude;
}

void RadioParser::setAltitudeIncremetionValue(const int8_t val){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setAltitudeIncremetionValue.");
    radio->setTx(4) = val;
}

uint8_t RadioParser::getMainOptionNumber() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getMainOptionNumber.");
    return mainNumber;
}

int8_t RadioParser::getMainOptionValue() const{
    if(trybe == RadioTrybe::RemoteControl)
        THROW_invalid_argument("Wrong mode for getMainOptionValue.");
    return mainValue;
}

void RadioParser::setMainOption(const uint8_t num, const int8_t val){
    if(trybe == RadioTrybe::Drone)
        THROW_invalid_argument("Wrong mode for setMainOptionNumber.");
    if(mainNumberTime == 0){
        mainNumberTime = mainOptionInterval;
        mainNumber = num;
        mainValue = val;
    }
}

uint8_t RadioParser::getTransmissionNumber() const{
    return transmissionNumber;
}

}
