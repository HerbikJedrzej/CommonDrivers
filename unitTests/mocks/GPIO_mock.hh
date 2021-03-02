#pragma once
#include <InterfacesConf_drivers_test.hh>
#include <memory>

class GPIO_Mock : public Drivers::GPIO_Ifc{
    InterruptInputList changedPin = InterruptInputList::endOfInterruptInputList;
public:
    std::map<InputList, bool> inputs;
    mutable std::map<OutputList, bool> outputs;
    std::map<InterruptInputList, DriverIfc*> irqs;
    GPIO_Mock(void (*delay)(uint32_t)):GPIO_Ifc(delay){}
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