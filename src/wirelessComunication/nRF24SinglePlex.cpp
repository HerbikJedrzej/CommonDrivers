#include "nRF24SinglePlex.hh"
#include "nRF24MemoryMap.hh"
#include <stdexcept>
#include <OwnExceptions.hh>

#define CALL_MEMBER_FN(object,ptrToMember)  if(ptrToMember!=nullptr)(object->*(ptrToMember))()

namespace Drivers{

nRF24SinglePlex::nRF24SinglePlex(const uint8_t _dataSize, const uint8_t _key, unsigned int& _time, bool transmitterMode, SPI_Ifc* _spi, GPIO_Ifc* _gpio, const OutputList& _cePin, const OutputList& _csnPin, void (*_delay)(uint32_t)):
	nRF24SinglePlex(_dataSize, _key, _time, transmitterMode, _spi, _gpio, _cePin, _csnPin, InterruptInputList::endOfInterruptInputList, _delay){}

nRF24SinglePlex::nRF24SinglePlex(const uint8_t _dataSize, const uint8_t _key, unsigned int& _time, bool transmitterMode, SPI_Ifc* _spi, GPIO_Ifc* _gpio, const OutputList& _cePin, const OutputList& _csnPin, const InterruptInputList& _irqPin, void (*_delay)(uint32_t)):
	DriverIfc(_delay),
	RadioIfc(_key, _time),
	sizeOfData(_dataSize),
	gpio(_gpio),
	delayMultiplier(13),
	realSizeOfData(_dataSize + 2),
	data(new uint8_t[realSizeOfData]),
	correcData(new uint8_t[sizeOfData]),
	spi(_spi),
	cePin(_cePin),
	csnPin(_csnPin),
	irqPin(_irqPin),
	radioisTransmitter(transmitterMode),
	nextInterrupOperation(nullptr){
		for(uint8_t i = 0; i < sizeOfData; i++)
			correcData[i] = 0;
	}

nRF24SinglePlex::~nRF24SinglePlex(){
	delete [] data;
	delete [] correcData;
	gpio->subscribe(irqPin, nullptr);
	gpio->stopWakingMe(this);
}

void nRF24SinglePlex::reciverPreparation(){
	keyIsCorrect = (((data[1] & 0xf0) | (data[realSizeOfData - 1] & 0x0f)) == key);
	if(keyIsCorrect){
		for(uint8_t i = 1; i < sizeOfData; i++)
			correcData[i] = data[i + 1];
		correcData[0] = (data[1] & 0x0f) | (data[realSizeOfData - 1] & 0xf0);
	}
	reciveData();    
}

void nRF24SinglePlex::transmitterPreparation(){
	for(uint8_t i = 1; i < sizeOfData; i++)
		data[i + 1] = correcData[i];
	data[1] = (key & 0xf0) | (correcData[0] & 0x0f);
	data[realSizeOfData - 1] = (correcData[0] & 0xf0) | (key & 0x0f);
    sendData();
}

void nRF24SinglePlex::init(){
	initIC();
	gpio->wakeMeUp(this);
	if(irqPin != InterruptInputList::endOfInterruptInputList)
		gpio->subscribe(irqPin, this);
}

void nRF24SinglePlex::handleTimeEvent(DriverIfc*){
	if(radioisTransmitter)
		transmitterPreparation();
	else
		reciverPreparation();
}

uint8_t& nRF24SinglePlex::setTx(unsigned int i){
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	if(!radioisTransmitter)
		THROW_invalid_argument("Radio in in reciving mode.");
	return correcData[i];
}

uint8_t& nRF24SinglePlex::setRx(unsigned int i){
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	if(radioisTransmitter)
		THROW_invalid_argument("Radio in in transsmiting mode.");
	return correcData[i];
}

uint8_t nRF24SinglePlex::getTx(unsigned int i) const{
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	if(!radioisTransmitter)
		THROW_invalid_argument("Radio in in reciving mode.");
	return correcData[i];
}

uint8_t nRF24SinglePlex::getRx(unsigned int i) const{
	if(i > sizeOfData)
		THROW_out_of_range("Trying to get more radio data than declrated.");
	if(radioisTransmitter)
		THROW_invalid_argument("Radio in in transsmiting mode.");
	return correcData[i];
}

void nRF24SinglePlex::handleFinish(DriverIfc* driver){
	if(driver == spi){
		CALL_MEMBER_FN(this, nextInterrupOperation);
		return;
	}else if(driver == gpio){
		isAckTransmittedPast = true;
	}
}

void nRF24SinglePlex::handleError(DriverIfc*){
}

void nRF24SinglePlex::handleAbort(DriverIfc*){
}

bool nRF24SinglePlex::isKyeCorrect(){
	return keyIsCorrect;
}

bool nRF24SinglePlex::isAckCorrect(){
	return isAckTransmitted;
}

bool nRF24SinglePlex::isComunicationCorrect(){
	if(radioisTransmitter)
		return isAckCorrect();
	else
		return isKyeCorrect();
}

void nRF24SinglePlex::ACK(){
	uint8_t readData[2] = {0x07, 0xFF};
	uint8_t writeData[2] = {0x27, 0x70};
	if(irqPin == InterruptInputList::endOfInterruptInputList){
		spi->readWrite(csnPin, readData, readData, 2, 1);
		isAckTransmitted = ((readData[1] & 0x20) !=0);
	}
	else
		isAckTransmitted = isAckTransmittedPast;
	isAckTransmittedPast = false;
	spi->readWrite(csnPin, writeData, writeData, 2, 1);
}

void nRF24SinglePlex::sendData(){
	ACK();
	nextInterrupOperation = &nRF24SinglePlex::execudeSending;
	data[0] = FLUSH_TX;
	spi->readWrite(csnPin, data, data, 1, this, 1);
}
void nRF24SinglePlex::execudeSending(){
	nextInterrupOperation = nullptr;
	data[0] = W_TX_PAYLOAD;
	spi->readWrite(csnPin, data, data, realSizeOfData, this, 1);
}
void nRF24SinglePlex::reciveData(){
	nextInterrupOperation = &nRF24SinglePlex::reset;
	data[0] = R_RX_PAYLOAD;
	for(int i = 1; i < realSizeOfData; i++)
		data[i] = NOP;
	spi->readWrite(csnPin, data, data, realSizeOfData, this, 1);
}

void nRF24SinglePlex::reset(){
	nextInterrupOperation = nullptr;
}

void nRF24SinglePlex::setRegister(uint8_t reg, uint8_t data){
    uint8_t tmp[2] = {reg, data};
    tmp[0]+=0x20;
    spi->readWrite(csnPin, tmp, tmp, 2, delayMultiplier);
}

void nRF24SinglePlex::switchMode(){
	gpio->pin(cePin, false);
	delayLoops();
	if(radioisTransmitter)
	    setRegister(CONFIG, (irqPin == InterruptInputList::endOfInterruptInputList)? 0x7E : 0x5E);
	else
    	setRegister(CONFIG, 0x7F);
	delayLoops();
	gpio->pin(cePin, true);
}

void nRF24SinglePlex::initIC(){
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
	switchMode();
    delay(100);
}

}
