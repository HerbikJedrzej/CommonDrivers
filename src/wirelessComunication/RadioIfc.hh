#pragma once

#include <stdlib.h>
#include "DriverIfc.hh"

namespace Drivers{

class RadioIfc
{
public:
	RadioIfc(const uint8_t _key, unsigned int& _time):
		key(_key),
		time(_time)
		{}
	virtual ~RadioIfc() = default;
	virtual bool isKyeCorrect() = 0;
	virtual bool isAckCorrect() = 0;
	virtual bool isComunicationCorrect() = 0;
	virtual void init() = 0;
	virtual uint8_t getTx(unsigned int i) const = 0;
	virtual uint8_t getRx(unsigned int i) const = 0;
	virtual uint8_t& setTx(unsigned int i) = 0;
	virtual uint8_t& setRx(unsigned int i) = 0;
protected:
	virtual void handleTimeEvent(DriverIfc*) = 0;
	const uint8_t key;
	unsigned int& time;
};

}
