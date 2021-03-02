#include "AT24C.hh"

namespace Drivers{

Memory::Memory(I2C_Ifc* _i2c, GPIO_Ifc* _gpio, const uint8_t& _memoryAdress, const OutputList& _writeProtectPin, void (*_delay)(uint32_t)):
	DriverIfc(_delay),
	i2c(_i2c),
	gpio(_gpio),
	memoryAdress(_memoryAdress),
	writeProtectPin(_writeProtectPin)
	{
	}

Memory::~Memory(){}

bool Memory::isEmpty(){
	return ((begin == nullptr) && (end == nullptr));
}

void Memory::addCell(Cell* cell){
	if(end != nullptr)
		end->next = cell;
	else
		begin = cell;
	end = cell;
}

bool Memory::init(const RegPair* initTable, const uint16_t size){
	uint8_t data[10]      = {0xac, 0x53, 0x35, 0xba, 0xc8, 0xc2, 0xaa, 0x26, 0xff, 0x00};
	uint8_t dataClear[10] = {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff};
	uint8_t dataCheck[10] = {0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00};
	while(!read(0x00, dataCheck, 10));
	const bool firstUseOfMemory = (
		dataCheck[0] != 0x00 ||
		dataCheck[1] != 0xff ||
		dataCheck[2] != 0x00 ||
		dataCheck[3] != 0xff ||
		dataCheck[4] != 0x00 ||
		dataCheck[5] != 0xff ||
		dataCheck[6] != 0x00 ||
		dataCheck[7] != 0xff ||
		dataCheck[8] != 0x00 ||
		dataCheck[9] != 0xff
	);
	while(!write(0x00, data, 10));
	delay(2);
	while(!read(0x00, dataCheck, 10));
	const bool first = (
		dataCheck[0] == 0xac &&
		dataCheck[1] == 0x53 &&
		dataCheck[2] == 0x35 &&
		dataCheck[3] == 0xba &&
		dataCheck[4] == 0xc8 &&
		dataCheck[5] == 0xc2 &&
		dataCheck[6] == 0xaa &&
		dataCheck[7] == 0x26 &&
		dataCheck[8] == 0xff &&
		dataCheck[9] == 0x00
	);
	while(!write(0x00, dataClear, 10));
	delay(2);
	while(!read(0x00, dataCheck, 10));
	const bool second = (
		dataCheck[0] == 0x00 &&
		dataCheck[1] == 0xff &&
		dataCheck[2] == 0x00 &&
		dataCheck[3] == 0xff &&
		dataCheck[4] == 0x00 &&
		dataCheck[5] == 0xff &&
		dataCheck[6] == 0x00 &&
		dataCheck[7] == 0xff &&
		dataCheck[8] == 0x00 &&
		dataCheck[9] == 0xff
	);
	if(firstUseOfMemory && first && second){
		uint8_t val;
		for(uint16_t i = 0; i < size; i++){
			val = initTable[i].value;
			if(!write(initTable[i].addr, &val, 1))
				return false;
		}
		return true;
	}
	return (first && second);
}

void Memory::lockMemory(){
	gpio->pin(writeProtectPin, pinLogic);
}

void Memory::unlockMemory(){
	gpio->pin(writeProtectPin, !pinLogic);
}

void Memory::removeCell(){
	Cell* tmp = begin;
	if(begin == end){
		end = nullptr;
		begin = nullptr;
	}
	else
		begin = tmp->next;
	if(tmp != nullptr){
		if(tmp->alocateData)
			delete [] tmp->data;
		delete tmp;
	}
}

bool Memory::write(uint16_t addr, uint8_t* data, uint16_t dataSize){
	bool toReturn = (BusStatus::OK == i2c->write(memoryAdress, addr, true, data, dataSize));
	return toReturn;
}

bool Memory::read(uint16_t addr, uint8_t* data, uint16_t dataSize){
	bool toReturn = (BusStatus::OK == i2c->read(memoryAdress, addr, true, data, dataSize));
	return toReturn;
}

void Memory::writeDMA(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish){
	Cell* cell = new Cell{addr, new uint8_t[dataSize], dataSize, finish, true, true};
	if(finish != nullptr)
		*finish = false;
	for(uint16_t i = 0; i < dataSize; i++)
		cell->data[i] = data[i];
	addCell(cell);
	if(isEmpty() && !i2c->isBussy()){
		doneFlag = finish;
		removeCellfalg = true;
		i2c->write(memoryAdress, addr, true, cell->data, dataSize, this);
	}
}

void Memory::writeDMAwithoutDataAlocate(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish){
	Cell* cell = nullptr;
	if(finish != nullptr)
		*finish = false;
	if(isEmpty() && !i2c->isBussy()){
		doneFlag = finish;
		i2c->write(memoryAdress, addr, true, data, dataSize, this);
	}
	else{
		cell = new Cell{addr, data, dataSize, finish, true, false};
		addCell(cell);
		if(!i2c->isBussy()){
			doneFlag = finish;
			removeCellfalg = true;
			i2c->write(memoryAdress, addr, true, cell->data, dataSize, this);
		}
	}
}

bool Memory::readDMA(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish){
	if(finish != nullptr)
		*finish = false;
	if(isEmpty() && !i2c->isBussy()){
		doneFlag = finish;
		i2c->read(memoryAdress, addr, true, data, dataSize, this);
		return true;
	}
	return false;
}

void Memory::handleFinish(DriverIfc*){
	if(doneFlag != nullptr)
		*doneFlag = true;
	doneFlag = nullptr;
	if(removeCellfalg){
		removeCellfalg = false;
		removeCell();
	}
	if(isEmpty())
		return;
	doneFlag = begin->finish;
	removeCellfalg = true;
	i2c->write(memoryAdress, begin->addr, true, begin->data, begin->dataSize, this);
}

}