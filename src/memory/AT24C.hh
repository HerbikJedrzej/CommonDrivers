#pragma once

#include <cstdint>
#include "MemoryIfc.hh"
#include "I2C.hh"
#include "GPIO.hh"

namespace Drivers{

class Memory : public MemoryIfc
{
public:
	Memory(I2C_Ifc* _i2c, GPIO_Ifc* _gpio, const uint8_t& _memoryAdress, const OutputList& _writeProtectPin, void (*_delay)(uint32_t));
	~Memory();
	virtual bool write(uint16_t addr, uint8_t* data, uint16_t dataSize) override;
	virtual bool read(uint16_t addr, uint8_t* data, uint16_t dataSize) override;
	virtual void writeDMA(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish) override;
	virtual void writeDMAwithoutDataAlocate(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish) override;
	virtual bool readDMA(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish) override;
	virtual bool init(const RegPair* initTable = nullptr, uint16_t size = 0) override;
	virtual void lockMemory() override;
	virtual void unlockMemory() override;
	virtual SizeModel getSizeModel() override;
protected:
	bool checkMemory();
	SizeModel readSizeModel();
	bool checkAvaibleOfCell(const uint16_t& addr);
	bool checkFirstUsage();
	void handleFinish(DriverIfc*) override;
private:
	struct Cell{
		uint16_t addr = {0};
		uint8_t* data = {nullptr};
		uint16_t dataSize = {0};
		bool* finish = {nullptr};
		bool write = {true};
		bool alocateData = {true};
		Cell* next = {nullptr};
	};
	Cell* begin = {nullptr};
	Cell* end = {nullptr};
	bool removeCellfalg = {false};
	bool* doneFlag = {nullptr};
	SizeModel sizeModel = BytePagesNotDefined;
	I2C_Ifc* i2c;
	GPIO_Ifc* gpio;
	const uint8_t memoryAdress;
	const OutputList writeProtectPin;
	bool pinLogic = {true};

	bool isEmpty();
	void addCell(Cell* cell);
	void removeCell();
};

}
