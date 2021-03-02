# CommonDrivers
This is repository with base drivers to use on microprocesors after minimum interface adaptation.

Every driver which want to use interrupts has to be derived class of Drivers::DriverIfc (file: src/DriverIfc.hh)
  virtual void handleTimeEvent(DriverIfc*)
    - interrupts function used when want to sygnalize counters event
  virtual void handleFinish(DriverIfc*)
    - interrupts function used when want to sygnalize finished task event (for example end of i2c or SPI transaction). 
  virtual void handleError(DriverIfc*)
    - interrupts function used when want to sygnalize failed task event (for example error in i2c or SPI transaction). 
  virtual void handleAbort(DriverIfc*)
    - interrupts function used when want to sygnalize aborted task event (for example aborted i2c or SPI transaction). \

All of those functions should be used in interrupts functions to sygnalise events in base drivers.
In interrupts functions they should be called with nullptr argument.
Base not overriden handle method implementation of DriverIfc calls drivers which order a task finished by event.
Theirs handle method are called by base driver with pointer of base driver.
That makes drivers could recognise source of intterupts (for example UART or GPIO driver). 

  virtual void wakeMeUp(DriverIfc* _wakeMeUpPtr) - if not overriden then driver in _wakeMeUpPtr pointer recive handleTimeEvent.
  virtual void stopWakingMe(DriverIfc* _wakeMeUpPtr) - if not overriden then driver in _wakeMeUpPtr pointer stop reciving handleTimeEvent.
  virtual bool isBussy() - if return true then driver is bussy.

This liblary needs few files defined to work.
Examples of those files are in unitTest directory.
Those files are:
- GPIOconf.hh - Defines output pins, input pins, pins interrupts list and ADC device list.
- InterfacesConf.hh - drivers instances with method accesing to hw (Interrupts functions).
- OwnExceptions.hh - interface of exceptions throwing which could be addaptated to microprocesor.
                     Has two versions (two header files):
                     In unitTest directory is version for tests (used on platform which support exceptions throwing).
                     In main directory is version to use on microprocesor platform (used on platform which dosen't support exceptions throwing).