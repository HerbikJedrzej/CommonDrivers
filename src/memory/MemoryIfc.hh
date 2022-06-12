#pragma once

#include <cstdint>
#include "DriverIfc.hh"
#include "I2C.hh"
#include "GPIO.hh"

namespace Drivers{

class MemoryIfc : public DriverIfc
{
public:
	struct RegPair{
		uint16_t addr;
		uint8_t value;
	};
	enum SizeModel{
		BytePagesNotDefined = 0,
		BytePages256 = 0xFFFF,
		BytePages128 = 0x7FFF,
		BytePages64 = 0x3FFF,
		BytePages32 = 0x1FFF,
		BytePages16 = 0x17FF,
	};
	MemoryIfc(void (*delay)(uint32_t)):DriverIfc(delay){}
	virtual bool write(uint16_t addr, uint8_t* data, uint16_t dataSize) = 0;
	virtual bool read(uint16_t addr, uint8_t* data, uint16_t dataSize) = 0;
	virtual void writeDMA(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish) = 0;
	virtual void writeDMAwithoutDataAlocate(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish) = 0;
	virtual bool readDMA(uint16_t addr, uint8_t* data, uint16_t dataSize, bool* finish) = 0;
	virtual bool init(const RegPair* initTable = nullptr, uint16_t size = 0) = 0;
	virtual void lockMemory() = 0;
	virtual void unlockMemory() = 0;
	virtual SizeModel getSizeModel() = 0;
};

}
