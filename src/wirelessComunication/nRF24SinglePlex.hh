#pragma once

#include <stdlib.h>
#include "GPIO.hh"
#include "SPI.hh"
#include "DriverIfc.hh"
#include <GPIOconf.hh>
#include "RadioIfc.hh"

/*
	bit 0:
		Used to operation on ic's registers
	bit 1:
		Control bit.
		msb                                       lsb
		 7  |  6  |  5  |  4  |  3  |  2  |  1  |  0
         msb transmission key |      lsb data[0]    
	bit 2, 3, 4, ...
		data[1, 2, 3, ...]
	last bit:
		Main operation bit
		msb                                lsb
		  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0
		      msb data[0]      |  lsb transmission key 
*/

namespace Drivers{

class nRF24SinglePlex : public DriverIfc, public RadioIfc
{
public:
	nRF24SinglePlex(const uint8_t _dataSize, const uint8_t _key, unsigned int& _time, bool transmitterMode, SPI_Ifc* _spi, GPIO_Ifc* _gpio, const OutputList& _cePin, const OutputList& _csnPin, void (*_delay)(uint32_t));
	nRF24SinglePlex(const uint8_t _dataSize, const uint8_t _key, unsigned int& _time, bool transmitterMode, SPI_Ifc* _spi, GPIO_Ifc* _gpio, const OutputList& _cePin, const OutputList& _csnPin, const InterruptInputList& _irqPin, void (*_delay)(uint32_t));
	virtual ~nRF24SinglePlex();
	uint8_t getTx(unsigned int i) const;
	uint8_t getRx(unsigned int i) const;
	uint8_t& setTx(unsigned int i);
	uint8_t& setRx(unsigned int i);
	virtual bool isComunicationCorrect() override;
	void init() override;
protected:
	virtual bool isKyeCorrect() override;
	virtual bool isAckCorrect() override;
	void reciverPreparation();
	void transmitterPreparation();
	void initIC();
	void switchMode();
	const uint8_t sizeOfData;
	GPIO_Ifc* gpio;
	bool isAckTransmitted = {false};
	bool isAckTransmittedPast = {false};
	const uint8_t delayMultiplier;
	const uint8_t realSizeOfData;
	uint8_t* data;
	uint8_t* correcData;
	SPI_Ifc* spi;
	const OutputList cePin;
	const OutputList csnPin;
	const InterruptInputList irqPin;
	bool keyIsCorrect = {false};
	const bool radioisTransmitter;
private:
	void (nRF24SinglePlex::*nextInterrupOperation)(void);
	void handleFinish(DriverIfc*) override;
	void handleError(DriverIfc*) override;
	void handleAbort(DriverIfc*) override;
	void setRegister(uint8_t reg, uint8_t data);
	void handleTimeEvent(DriverIfc*) override;

	void ACK();
	void sendData();
	void execudeSending();
	void reciveData();
	void reset();

};

}
