#pragma once
#include <InterfacesConf_drivers_test.hh>
#include <vector>
#include <gtest/gtest.h>

class SPI_Mock : public Drivers::SPI_Ifc{
    std::vector<std::vector<uint8_t>> readedData;
    std::vector<std::vector<uint8_t>> writedData;
    unsigned int resets;
    bool resetValue;
    Drivers::BusStatus returnedBusStatus;
public:
    SPI_Mock(void (*delay)(uint32_t)):
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
