#include "nRF24HalfDuplex.hh"
#include "nRF24MemoryMap.hh"
#include <stdexcept>
#include <OwnExceptions.hh>

#define CALL_MEMBER_FN(object,ptrToMember)  if(ptrToMember!=nullptr)(object->*(ptrToMember))()

namespace Drivers{

nRF24HalfDuplex::nRF24HalfDuplex(const uint8_t _dataSize, const uint8_t _key, unsigned int& _time, bool transmitterMode, SPI_Ifc* _spi, GPIO_Ifc* _gpio, const OutputList& _cePin, const OutputList& _csnPin, const InterruptInputList& _irqPin, void (*_delay)(uint32_t)):
	DriverIfc(_delay),
	RadioIfc(_key, _time),
	sizeOfData(_dataSize),
	gpio(_gpio),
	delayMultiplier(13),
	realSizeOfData(_dataSize + 2),
	data(new uint8_t[realSizeOfData]),
	dataTx(new uint8_t[sizeOfData]),
	dataRx(new uint8_t[sizeOfData]),
	spi(_spi),
	cePin(_cePin),
	csnPin(_csnPin),
	irqPin(_irqPin),
	transmisionMaster(transmitterMode),
	radioisTransmitter(transmitterMode),
	nextInterrupOperation(nullptr){
		for(uint8_t i = 0; i < sizeOfData; i++){
			dataTx[i] = 0;
			dataRx[i] = 0;
		}
	}

nRF24HalfDuplex::~nRF24HalfDuplex(){
	delete [] data;
	delete [] dataTx;
	delete [] dataRx;
	gpio->subscribe(irqPin, nullptr);
	gpio->stopWakingMe(this);
}

void nRF24HalfDuplex::transmitterPreparation(){
	for(uint8_t i = 1; i < sizeOfData; i++)
		data[i + 1] = dataTx[i];
	data[1] = (key & 0xf0) | (dataTx[0] & 0x08) | (transmissionNumber & 0x07);
	data[realSizeOfData - 1] = (dataTx[0] & 0xf0) | (key & 0x0f);
    sendData();
}

void nRF24HalfDuplex::init(){
	if(irqPin == InterruptInputList::endOfInterruptInputList)
		THROW_invalid_argument("Radio irq pin initialised by endOfInterruptInputList.");
	initIC();
	gpio->wakeMeUp(this);
	gpio->subscribe(irqPin, this);
}

void nRF24HalfDuplex::sendRecive(){
	if(radioisTransmitter)
		transmitterPreparation();
	else
		reciveData();
}

void nRF24HalfDuplex::handleTimeEvent(DriverIfc*){
	isAckTransmitted = isAckTransmittedPast;
	isAckTransmittedPast = false;
	keyIsCorrect = keyIsCorrectPast;
	keyIsCorrectPast = false;
	transmissionNumber++;
	if(transmissionNumber >= 12)
		transmissionNumber = 0;
	if(transmissionNumber < 8)
		sendRecive();
	else if(transmissionNumber == 8)
		switchMode(!transmisionMaster);
	else if(transmissionNumber == 11)
		switchMode(transmisionMaster);
	else
		sendRecive();
}

uint8_t& nRF24HalfDuplex::setTx(unsigned int i){
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	return dataTx[i];
}

uint8_t& nRF24HalfDuplex::setRx(unsigned int i){
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	return dataRx[i];
}

uint8_t nRF24HalfDuplex::getTx(unsigned int i) const{
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	return dataTx[i];
}

uint8_t nRF24HalfDuplex::getRx(unsigned int i) const{
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	return dataRx[i];
}

void nRF24HalfDuplex::handleFinish(DriverIfc* driver){
	if(driver == spi){
		CALL_MEMBER_FN(this, nextInterrupOperation);
		return;
	}else if(driver == gpio){
		isAckTransmittedPast = subsequentReadedACK;
		subsequentReadedACK = true;
	}
}

void nRF24HalfDuplex::handleError(DriverIfc*){
}

void nRF24HalfDuplex::handleAbort(DriverIfc*){
}

bool nRF24HalfDuplex::isKyeCorrect(){
	return keyIsCorrect;
}

bool nRF24HalfDuplex::isAckCorrect(){
	return isAckTransmitted;
}

bool nRF24HalfDuplex::isComunicationCorrect(){
	if(radioisTransmitter)
		return isAckCorrect();
	else
		return isKyeCorrect();
}

void nRF24HalfDuplex::sendData(){
	uint8_t writeData[2] = {0x27, 0x70};
	spi->readWrite(csnPin, writeData, writeData, 2, 1);
	nextInterrupOperation = &nRF24HalfDuplex::execudeSending;
	data[0] = FLUSH_TX;
	spi->readWrite(csnPin, data, data, 1, this, 1);
}
void nRF24HalfDuplex::execudeSending(){
	nextInterrupOperation = nullptr;
	data[0] = W_TX_PAYLOAD;
	spi->readWrite(csnPin, data, data, realSizeOfData, this, 1);
}
void nRF24HalfDuplex::reciveData(){
	nextInterrupOperation = &nRF24HalfDuplex::reset;
	data[0] = R_RX_PAYLOAD;
	for(int i = 1; i < realSizeOfData; i++)
		data[i] = NOP;
	spi->readWrite(csnPin, data, data, realSizeOfData, this, 1);
}

void nRF24HalfDuplex::reset(){
	nextInterrupOperation = nullptr;
	keyIsCorrectPast = (((data[1] & 0xf0) | (data[realSizeOfData - 1] & 0x0f)) == key);
	if(keyIsCorrectPast){
		for(uint8_t i = 1; i < sizeOfData; i++)
			dataRx[i] = data[i + 1];
		dataRx[0] = (data[1] & 0x0f) | (data[realSizeOfData - 1] & 0xf0);
		if(!transmisionMaster)
			transmissionNumber = data[1] & 0x07;
	}
}

void nRF24HalfDuplex::setRegister(uint8_t reg, uint8_t data){
    uint8_t tmp[2] = {reg, data};
    tmp[0]+=0x20;
    spi->readWrite(csnPin, tmp, tmp, 2, delayMultiplier);
}

void nRF24HalfDuplex::switchMode(const bool& transmitter){
	radioisTransmitter = transmitter;
	subsequentReadedACK = false;
	gpio->pin(cePin, false);
	delayLoops();
	if(radioisTransmitter)
	    setRegister(CONFIG, 0x5E);
	else
    	setRegister(CONFIG, 0x7F);
	delayLoops();
	gpio->pin(cePin, true);
}

void nRF24HalfDuplex::initIC(){
	uint8_t* val = new uint8_t[realSizeOfData]{RX_ADDR_P0 + 0x20};
	bool miniDroneMode = true;
    delay(100);
    setRegister(EN_AA, 0x01);
    setRegister(EN_RXADDR, 0x01);
    setRegister(SETUP_AW, 0x03);
    setRegister(RF_CH, 0x01);
    setRegister(RF_SETUP, miniDroneMode? 0x0e : 0x07);
    setRegister(SETUP_RETR, 0x2F);
	for(uint8_t i = 1; i < realSizeOfData; i++)
		val[i] = key;
	spi->readWrite(csnPin, val, val, realSizeOfData, delayMultiplier);
	val[0] = TX_ADDR + 0x20;
	for(uint8_t i = 1; i < realSizeOfData; i++)
		val[i] = key;
	spi->readWrite(csnPin, val, val, realSizeOfData, delayMultiplier);
    setRegister(RX_PW_P0, realSizeOfData - 1);
	delete [] val;
	switchMode(radioisTransmitter);
    delay(100);
}

}
