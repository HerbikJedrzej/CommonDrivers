#include <gtest/gtest.h>
#include <InterfacesConf_drivers_test.hh>
#include <nRF24MemoryMap.hh>
#include <nRF24SinglePlex.hh>
#include <cstdlib>
#include <vector>
#include <utility>
#include <memory>

using drivers_test::spi;
using drivers_test::gpio;
using drivers_test::hspi;
using drivers_test::HAL_StatusTypeDef;
using drivers_test::HAL_SPI_StateTypeDef;
using drivers_test::Registersmap;
using drivers_test::postTestInCounter;
using drivers_test::postTestOutCounter;
using drivers_test::radioRegsIn;
using drivers_test::radioRegsOut;
using drivers_test::testIn;
using drivers_test::testOut;

namespace nRF24SinglePlexTest{

unsigned int localTime = 0;
using Reg = uint8_t;
using Val = uint8_t;

class RadioSPI_Test : public Drivers::SPI_Ifc{
    std::vector<std::vector<uint8_t>> readedData;
    std::vector<std::vector<uint8_t>> writedData;
    unsigned int resets;
    bool resetValue;
    Drivers::BusStatus returnedBusStatus;
public:
    RadioSPI_Test(void (*delay)(uint32_t)):
        SPI_Ifc(delay),
        readedData(),
        writedData(),
        resets(0),
        resetValue(true),
        returnedBusStatus(Drivers::BusStatus::OK){
            callMe = nullptr;
        }
    Drivers::BusStatus readWrite(const OutputList&, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, const uint8_t = 0) override{
        if(readedData.empty())
            throw std::out_of_range("Too less rows in radio SPI test.");
        const auto vec = readedData.front();
        readedData.erase(readedData.begin());
        if(vec.size() != Size)
            throw std::out_of_range("Diffrent data size in radio SPI test. Expected = " + std::to_string(Size) + ", acctual = " + std::to_string(vec.size()));
        std::vector<uint8_t> tmp;
        for(unsigned int i = 0; i < Size; i++){
            tmp.push_back(pTxData[i]);
            pRxData[i] = vec[i];
        }
        writedData.push_back(tmp);
        return returnedBusStatus;
    }
    Drivers::BusStatus readWrite(const OutputList& ssPin, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, DriverIfc* _callMe, const uint8_t multiplierDelayLoops = 0) override{
        if(callMe != nullptr)
            throw std::out_of_range("dupa");
            // return Drivers::BusStatus::BUSY;
        callMe = _callMe;
        return readWrite(ssPin, pTxData, pRxData, Size, multiplierDelayLoops);
    }
    bool reset() override{
        resets++;
        return resetValue;
    }
    bool empty(){
        const bool toReturn = writedData.empty() && readedData.empty() && (callMe == nullptr);
        writedData.clear();
        readedData.clear();
        callMe = nullptr;
        return toReturn;
    }
    void setReset(const bool toReturn){
        resetValue = toReturn;
    }
    void setReturnedBusStatus(const Drivers::BusStatus toReturn){
        returnedBusStatus = toReturn;
    }
    unsigned int getResets(){
        const auto tmp = resets;
        resets = 0;
        return tmp;
    }
    void checkWritedBuffor(const char* name, const std::vector<std::vector<uint8_t>>& correctAnswers){
        ASSERT_EQ(correctAnswers.size(), writedData.size()) << "in scenario \"" + std::string(name) + "\".";
        for(unsigned int i = 0; i < correctAnswers.size(); i++){
            ASSERT_EQ(correctAnswers[i].size(), writedData[i].size()) << "in scenario \"" + std::string(name) << "\". i = " << i;
            for(unsigned int j = 0; j < correctAnswers[i].size(); j++)
                EXPECT_EQ(correctAnswers[i][j], writedData[i][j]) << "in scenario \"" + std::string(name) << "\". i = " << i << " j = " << j;
        }
        writedData.clear();
    }
    void setReadingBuffor(const std::vector<std::vector<uint8_t>>& vec){
        for(const auto& row : vec)
            readedData.push_back(row);
    }
};

class RadioGPIO_Test : public Drivers::GPIO_Ifc{
    InterruptInputList changedPin = InterruptInputList::endOfInterruptInputList;
public:
    std::map<InputList, bool> inputs;
    mutable std::map<OutputList, bool> outputs;
    std::map<InterruptInputList, DriverIfc*> irqs;
    RadioGPIO_Test(void (*delay)(uint32_t)):GPIO_Ifc(delay){}
    void pin(OutputList i, bool state) const override{
        outputs[i] = state;
    }
    bool pin(OutputList i) const override{
        return outputs.at(i);
    }
    bool pin(InputList i) const override{
        return inputs.at(i);
    }
    void setChangedPin(InterruptInputList pin) override{
        changedPin = pin;
    }
    void subscribe(InterruptInputList pin, DriverIfc* driver) override{
        irqs[pin] = driver;
    }
    void handleFinish(DriverIfc*){
        if(changedPin == InterruptInputList::endOfInterruptInputList)
            return;
        const InterruptInputList tmp = changedPin;
        changedPin = InterruptInputList::endOfInterruptInputList;
        if(irqs.at(tmp) != nullptr)
            irqs.at(tmp)->handleFinish(this);
    }
};

void nRF24SinglePlex_delay(uint32_t){
}

TEST(nRF24SinglePlex_Test, readStatus){
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);    
    Drivers::nRF24SinglePlex radio(5, 0x4a, localTime, true, &spi, &gpio, OutputList::radioCE, OutputList::radioCSN, InterruptInputList::radioIRQ, nRF24SinglePlex_delay);
    EXPECT_NO_THROW(radio.setTx(1));
    EXPECT_NO_THROW(radio.getTx(1));
    EXPECT_ANY_THROW(radio.setRx(1));
    EXPECT_ANY_THROW(radio.getRx(1));
}

TEST(nRF24SinglePlex_Test, radioInitTransmitter){
    unsigned int localTime = 0;
    constexpr uint8_t addRegBackoff = 0x20;
    constexpr uint8_t key = 0b01011010;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    uint8_t size = 4;
    Drivers::nRF24SinglePlex radio(
        size,
        key,
        localTime,
        true,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        nRF24SinglePlex_delay
    );
    radio.init();
    EXPECT_EQ(gpio.irqs.size(), 0);
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addRegBackoff + EN_AA      , 0x01},
        {addRegBackoff + EN_RXADDR  , 0x01},
        {addRegBackoff + SETUP_AW   , 0x03},
        {addRegBackoff + RF_CH      , 0x01},
        {addRegBackoff + RF_SETUP   , 0x0E},
        {addRegBackoff + SETUP_RETR , 0x2F},
        {addRegBackoff + RX_ADDR_P0 , key, key, key, key, key},
        {addRegBackoff + TX_ADDR    , key, key, key, key, key},
        {addRegBackoff + RX_PW_P0   , 0x05},
        {addRegBackoff + CONFIG     , 0x7E},
    });
    EXPECT_TRUE(spi.empty());
}

TEST(nRF24SinglePlex_Test, radioInitTransmitterWithIrqMode){
    unsigned int localTime = 0;
    constexpr uint8_t addRegBackoff = 0x20;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    uint8_t size = 4;
    constexpr uint8_t key = 0b01011010;
    Drivers::nRF24SinglePlex radio(
        size,
        key,
        localTime,
        true,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        InterruptInputList::radioIRQ,
        nRF24SinglePlex_delay
    );
    radio.init();
    ASSERT_EQ(gpio.irqs.size(), 1);
    ASSERT_NE(gpio.irqs.find(InterruptInputList::radioIRQ), gpio.irqs.end());
    EXPECT_EQ(gpio.irqs.at(InterruptInputList::radioIRQ), &radio);
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addRegBackoff + EN_AA      , 0x01},
        {addRegBackoff + EN_RXADDR  , 0x01},
        {addRegBackoff + SETUP_AW   , 0x03},
        {addRegBackoff + RF_CH      , 0x01},
        {addRegBackoff + RF_SETUP   , 0x0E},
        {addRegBackoff + SETUP_RETR , 0x2F},
        {addRegBackoff + RX_ADDR_P0 , key, key, key, key, key},
        {addRegBackoff + TX_ADDR    , key, key, key, key, key},
        {addRegBackoff + RX_PW_P0   , 0x05},
        {addRegBackoff + CONFIG     , 0x5E},
    });
    EXPECT_TRUE(spi.empty());
}

TEST(nRF24SinglePlex_Test, radioInitReciver){
    unsigned int localTime = 0;
    constexpr uint8_t addRegBackoff = 0x20;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    constexpr uint8_t key = 0b01011010;
    Drivers::nRF24SinglePlex radio(
        7,
        key,
        localTime,
        false,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        nRF24SinglePlex_delay
    );
    radio.init();
    EXPECT_EQ(gpio.irqs.size(), 0);
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
        {addRegBackoff + EN_AA      , 0x01},
        {addRegBackoff + EN_RXADDR  , 0x01},
        {addRegBackoff + SETUP_AW   , 0x03},
        {addRegBackoff + RF_CH      , 0x01},
        {addRegBackoff + RF_SETUP   , 0x0E},
        {addRegBackoff + SETUP_RETR , 0x2F},
        {addRegBackoff + RX_ADDR_P0 , key, key, key, key, key, key, key, key},
        {addRegBackoff + TX_ADDR    , key, key, key, key, key, key, key, key},
        {addRegBackoff + RX_PW_P0   , 0x08},
        {addRegBackoff + CONFIG     , 0x7F},
    });
    EXPECT_TRUE(spi.empty());
}

const std::vector<uint8_t> prepareTransmittedData(std::vector<uint8_t> data, const uint8_t key){
    std::vector<uint8_t> toReturn;
    toReturn.push_back(W_TX_PAYLOAD);
    toReturn.push_back((key & 0xf0) | (data[0] & 0x0f));
    for(unsigned int i = 1; i < data.size(); i++)
        toReturn.push_back(data[i]);
    toReturn.push_back((data[0] & 0xf0) | (key & 0x0f));
    return toReturn;
}

TEST(nRF24SinglePlex_Test, radioTransmitterMode){
    constexpr uint8_t key = 0b01011010;
    unsigned int localTime = 0;
    std::vector<uint8_t> dataToSend;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    Drivers::nRF24SinglePlex radio(
        6,
        key,
        localTime,
        true,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        nRF24SinglePlex_delay
    );
    radio.init();
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    EXPECT_FALSE(spi.empty());
    for(unsigned int i = 0; i < 25; i++){
        dataToSend.clear();
        for(uint8_t i = 0; i < 6; i++)
            dataToSend.push_back(std::rand() % 256);
        for(unsigned int i = 0; i < dataToSend.size(); i++)
            radio.setTx(i) = dataToSend[i];
        spi.setReadingBuffor({
            {0,0},
            {0,0},
            {0},
            {0,0,0,0,0,0,0,0}
        });
    	gpio.handleTimeEvent(nullptr);
        spi.handleFinish(nullptr);
        spi.handleFinish(nullptr);
        spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
            {0x07, 0xFF},
            {0x27, 0x70},
            {FLUSH_TX},
            prepareTransmittedData(dataToSend, key)
        });
        EXPECT_TRUE(spi.empty());
        EXPECT_TRUE(gpio.pin(OutputList::radioCE));
        localTime++;
    }
}

TEST(nRF24SinglePlex_Test, radioReciverMode){
    constexpr uint8_t key = 0b01011010;
    unsigned int localTime = 0;
    std::vector<uint8_t> dataToSend;
    std::vector<uint8_t> previousDataToSend;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    Drivers::nRF24SinglePlex radio(
        4,
        key,
        localTime,
        false,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        nRF24SinglePlex_delay
    );
    radio.init();
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    EXPECT_FALSE(spi.empty());
    for(unsigned int i = 0; i < 25; i++){
        previousDataToSend = dataToSend;
        dataToSend.clear();
        for(uint8_t j = 0; j < 4; j++)
            dataToSend.push_back(std::rand() % 256);
        spi.setReadingBuffor({prepareTransmittedData(dataToSend, key)});
    	gpio.handleTimeEvent(nullptr);
        spi.handleFinish(nullptr);
        if(i != 0){
            for(unsigned int j = 0; j < previousDataToSend.size(); j++)
                EXPECT_EQ(radio.getRx(j), previousDataToSend[j]) << " j = " << j;
        }
        spi.checkWritedBuffor(__PRETTY_FUNCTION__, {{R_RX_PAYLOAD, NOP, NOP, NOP, NOP, NOP}});
        EXPECT_TRUE(spi.empty());
        EXPECT_TRUE(gpio.pin(OutputList::radioCE));
        localTime++;
    }
}

TEST(nRF24SinglePlex_Test, recivedACK){
    constexpr uint8_t key = 0b01011010;
    unsigned int localTime = 0;
    std::vector<uint8_t> dataToSend;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    Drivers::nRF24SinglePlex radio(
        6,
        key,
        localTime,
        true,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        nRF24SinglePlex_delay
    );
    radio.init();
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    EXPECT_FALSE(spi.empty());
    for(unsigned int i = 0; i < 25; i++){
        dataToSend.clear();
        for(uint8_t i = 0; i < 6; i++)
            dataToSend.push_back(std::rand() % 256);
        for(unsigned int i = 0; i < dataToSend.size(); i++)
            radio.setTx(i) = dataToSend[i];
        if(localTime % 2 == 0)
            spi.setReadingBuffor({
                {0, 0x20},
                {0,0},
                {0},
                {0,0,0,0,0,0,0,0}
            });
        else
            spi.setReadingBuffor({
                {0, 0},
                {0,0},
                {0},
                {0,0,0,0,0,0,0,0}
            }); 
    	gpio.handleTimeEvent(nullptr);
        spi.handleFinish(nullptr);
        spi.handleFinish(nullptr);
        spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
            {0x07, 0xFF},
            {0x27, 0x70},
            {FLUSH_TX},
            prepareTransmittedData(dataToSend, key)
        });
        if(localTime % 2 == 0)
            EXPECT_TRUE(radio.isComunicationCorrect());
        else
            EXPECT_FALSE(radio.isComunicationCorrect());
        EXPECT_TRUE(spi.empty());
        EXPECT_TRUE(gpio.pin(OutputList::radioCE));
        localTime++;
    }
}

TEST(nRF24SinglePlex_Test, recivedACKWithIrqMode){
    constexpr uint8_t key = 0b01011010;
    unsigned int localTime = 0;
    std::vector<uint8_t> dataToSend;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    Drivers::nRF24SinglePlex radio(
        6,
        key,
        localTime,
        true,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        InterruptInputList::radioIRQ,
        nRF24SinglePlex_delay
    );
    radio.init();
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    EXPECT_FALSE(spi.empty());
    for(unsigned int i = 0; i < 25; i++){
        dataToSend.clear();
        for(uint8_t i = 0; i < 6; i++)
            dataToSend.push_back(std::rand() % 256);
        for(unsigned int i = 0; i < dataToSend.size(); i++)
            radio.setTx(i) = dataToSend[i];
        spi.setReadingBuffor({
            {0,0},
            {0},
            {0,0,0,0,0,0,0,0}
        });
        if(localTime % 2 == 0){
            gpio.setChangedPin(InterruptInputList::radioIRQ);
            gpio.handleFinish(nullptr);
        }
    	gpio.handleTimeEvent(nullptr);
        spi.handleFinish(nullptr);
        spi.handleFinish(nullptr);
        spi.checkWritedBuffor(__PRETTY_FUNCTION__, {
            {0x27, 0x70},
            {FLUSH_TX},
            prepareTransmittedData(dataToSend, key)
        });
        if(localTime % 2 == 0)
            EXPECT_TRUE(radio.isComunicationCorrect());
        else
            EXPECT_FALSE(radio.isComunicationCorrect());
        EXPECT_TRUE(spi.empty());
        EXPECT_TRUE(gpio.pin(OutputList::radioCE));
        localTime++;
    }
}

TEST(nRF24SinglePlex_Test, keyCorrect){
    constexpr uint8_t key1 = 0b01011010;
    constexpr uint8_t key2 = 0b01001010;    
    unsigned int localTime = 0;
    std::vector<uint8_t> dataToSend;
    std::vector<uint8_t> previousDataToSend;
    RadioSPI_Test spi(nRF24SinglePlex_delay);
    RadioGPIO_Test gpio(nRF24SinglePlex_delay);
    spi.setReadingBuffor({
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0},
        {0, 0},
    });
    Drivers::nRF24SinglePlex radio(
        4,
        key1,
        localTime,
        false,
        &spi,
        &gpio,
        OutputList::radioCE,
        OutputList::radioCSN,
        InterruptInputList::radioIRQ,
        nRF24SinglePlex_delay
    );
    radio.init();
    EXPECT_TRUE(gpio.pin(OutputList::radioCE));
    EXPECT_FALSE(spi.empty());
    for(unsigned int i = 0; i < 25; i++){
        if(localTime % 2 != 0)
            previousDataToSend = dataToSend;
        dataToSend.clear();
        for(uint8_t j = 0; j < 4; j++)
            dataToSend.push_back(std::rand() % 256);
        spi.setReadingBuffor({prepareTransmittedData(dataToSend, (localTime % 2 == 0)? key1 : key2)});
    	gpio.handleTimeEvent(nullptr);
        spi.handleFinish(nullptr);
        if(i != 0){
            for(unsigned int j = 0; j < previousDataToSend.size(); j++)
                EXPECT_EQ(radio.getRx(j), previousDataToSend[j]) << " j = " << j;
        }
        spi.checkWritedBuffor(__PRETTY_FUNCTION__, {{R_RX_PAYLOAD, NOP, NOP, NOP, NOP, NOP}});
        EXPECT_TRUE(spi.empty());
        EXPECT_TRUE(gpio.pin(OutputList::radioCE));
        if(localTime % 2 != 0)
            EXPECT_TRUE(radio.isComunicationCorrect());
        else
            EXPECT_FALSE(radio.isComunicationCorrect());
        localTime++;
    }
}

}