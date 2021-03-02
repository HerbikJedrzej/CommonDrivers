#include <gtest/gtest.h>
#include <InterfacesConf_drivers_test.hh>
#include <cstdlib>


namespace DriverIfcTest{

class TestClass : public Drivers::DriverIfc{
    bool wasRunned = {false};
public:
    static unsigned int commonID;
    TestClass():
        DriverIfc(drivers_test::HAL_Delay)
        {
            DriverIfc::loopsToWait = 100;
            // manageTestObj.wakeMeUp(this);
        }
    ~TestClass(){}
    void handleTimeEvent(Drivers::DriverIfc*) override{
        wasRunned = true;
    }
    void reset(){
        wasRunned = false;
    }
    bool getState(){
        return wasRunned;
    }
};

TEST(DriverIfc_Test, singleObjToWakeUp){
    Drivers::DriverIfc manageTestObj(drivers_test::HAL_Delay);
    TestClass testObj;
    manageTestObj.wakeMeUp(&testObj);
    EXPECT_FALSE(testObj.getState());
    manageTestObj.handleTimeEvent(nullptr);
    EXPECT_TRUE(testObj.getState());
    testObj.reset();
    EXPECT_FALSE(testObj.getState());
    manageTestObj.handleTimeEvent(nullptr);
    EXPECT_TRUE(testObj.getState());
}

TEST(DriverIfc_Test, multipleObjToWakeUp){
    constexpr uint8_t size = 10;
    Drivers::DriverIfc manageTestObj(drivers_test::HAL_Delay);
    std::vector<TestClass> testObj(size);
    for(uint8_t i = 0; i < size; i++){
        manageTestObj.wakeMeUp(&testObj[i]);
        EXPECT_FALSE(testObj[i].getState()) << "Failure for obj no. " << (unsigned int)i << ".";
    }
    for(uint8_t i = 0; i < size; i++){
        manageTestObj.handleTimeEvent(nullptr);
        EXPECT_TRUE(testObj[i].getState()) << "Failure for obj no. " << (unsigned int)i << ".";
        testObj[i].reset();
        EXPECT_FALSE(testObj[i].getState()) << "Failure for obj no. " << (unsigned int)i << ".";
        manageTestObj.handleTimeEvent(nullptr);
        EXPECT_TRUE(testObj[i].getState()) << "Failure for obj no. " << (unsigned int)i << ".";
    }
    for(uint8_t i = 0; i < size; i++){
        testObj[i].reset();
        EXPECT_FALSE(testObj[i].getState()) << "Failure for obj no. " << (unsigned int)i << ".";
        manageTestObj.stopWakingMe(&testObj[i]);
    }
    for(uint8_t i = 0; i < size; i++){
        EXPECT_FALSE(testObj[i].getState()) << "Failure for obj no. " << (unsigned int)i << ".";
        manageTestObj.handleTimeEvent(nullptr);
        EXPECT_FALSE(testObj[i].getState()) << "Failure for obj no. " << (unsigned int)i << ".";
    }
}

}