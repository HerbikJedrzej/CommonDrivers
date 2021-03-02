#pragma once
#include <InterfacesConf_drivers_test.hh>
#include <vector>
#include <gtest/gtest.h>

struct I2CmockCell{
    uint8_t devAddr;
    uint16_t regAddr;
    std::vector<uint8_t> data;
};

class I2C_Mock : public Drivers::I2C_Ifc{
    std::vector<I2CmockCell> readedData;
    std::vector<I2CmockCell> writedData;
    unsigned int resets;
    bool resetValue;
    Drivers::BusStatus returnedBusStatus;
public:
    I2C_Mock(void (*delay)(uint32_t)):
        I2C_Ifc(delay),
        readedData(),
        writedData(),
        resets(0),
        resetValue(true),
        returnedBusStatus(Drivers::BusStatus::OK){
            callMe = nullptr;
        }
    
    Drivers::BusStatus read(uint8_t DevAddress, uint16_t MemAddress, bool, uint8_t *pData, uint16_t Size) override{
        if(readedData.empty())
            throw std::out_of_range("Too less rows in I2C test.");
        const auto cell = readedData.front();
        readedData.erase(readedData.begin());
        EXPECT_EQ(DevAddress, cell.devAddr);
        EXPECT_EQ(MemAddress, cell.regAddr);
        if(cell.data.size() != Size)
            throw std::out_of_range("Diffrent data size in I2C test. Expected = " + std::to_string(Size) + ", acctual = " + std::to_string(cell.data.size()));
        for(unsigned int i = 0; i < Size; i++)
            pData[i] = cell.data[i];
        return returnedBusStatus;
    }

    Drivers::BusStatus write(uint8_t DevAddress, uint16_t MemAddress, bool, uint8_t *pData, uint16_t Size) override{
        I2CmockCell tmp{DevAddress, MemAddress, {}};
        for(unsigned int i = 0; i < Size; i++)
            tmp.data.push_back(pData[i]);
        writedData.push_back(tmp);
        return returnedBusStatus;
    }

    Drivers::BusStatus read(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe) override{
        if(callMe != nullptr)
            throw std::out_of_range("In mock reading callMe == nullptr");
            // return Drivers::BusStatus::BUSY;
        callMe = _callMe;
        return read(DevAddress, MemAddress, MemAddrIs16b, pData, Size);
    }

    Drivers::BusStatus write(uint8_t DevAddress, uint16_t MemAddress, bool MemAddrIs16b, uint8_t *pData, uint16_t Size, DriverIfc* _callMe) override{
        if(callMe != nullptr)
            throw std::out_of_range("In mock writing callMe == nullptr");
            // return Drivers::BusStatus::BUSY;
        callMe = _callMe;
        return write(DevAddress, MemAddress, MemAddrIs16b, pData, Size);
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
    void checkWritedBuffor(const char* name, const std::vector<I2CmockCell>& correctAnswers){
        ASSERT_EQ(correctAnswers.size(), writedData.size()) << "in scenario \"" + std::string(name) + "\".";
        for(unsigned int i = 0; i < correctAnswers.size(); i++){
            ASSERT_EQ(correctAnswers[i].data.size(), writedData[i].data.size()) << "in scenario \"" + std::string(name) << "\". i = " << i;
            ASSERT_EQ(correctAnswers[i].devAddr, writedData[i].devAddr) << "in scenario \"" + std::string(name) << "\". i = " << i;
            ASSERT_EQ(correctAnswers[i].regAddr, writedData[i].regAddr) << "in scenario \"" + std::string(name) << "\". i = " << i;
            for(unsigned int j = 0; j < correctAnswers[i].data.size(); j++)
                EXPECT_EQ(correctAnswers[i].data[j], writedData[i].data[j]) << "in scenario \"" + std::string(name) << "\". i = " << i << " j = " << j;
        }
        writedData.clear();
    }
    void setReadingBuffor(const std::vector<I2CmockCell>& vec){
        for(const auto& row : vec)
            readedData.push_back(row);
    }
};
