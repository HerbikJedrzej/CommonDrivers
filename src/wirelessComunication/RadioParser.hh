#pragma once

#include "RadioIfc.hh"

/*
	deta length = 7
	bit 0:
		Main operation bit
		msb                                                                         lsb
		  7   |    6     |     5     |       4       |  3            |   2   |   1   |   0
		flyOn | engineOn | joyRight  |    joyLeft    | specialButton |  transmission number
	bit 1:
		Expected value for roll angle
		msb                       lsb
		7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
		          int8 value
	bit 2:
		Expected value for pitch angle
		msb                       lsb
		7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
		          int8 value
	bit 3:
		Expected value for yawl angle
		msb                       lsb
		7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
		          int8 value
	bit 4:
		Expected value for altitude incremetion (val / 200) = real altitude incremetion in meters
		msb    					  lsb
		7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
		          int8 value
	bit 5:
		Main special option number
		msb    					  lsb
		7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
				 uint8 value
	bit 6:
		Main special option value
		msb						  lsb
		7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
				 int8 value
*/

namespace Drivers{

enum RadioTrybe{
    RemoteControl,
    Drone,
    HalfDuplex,
    FullDuplex
};

class RadioParser
{
private:
	RadioIfc* const radio;
	const RadioTrybe trybe;
	uint16_t fromLastSucces = {0};
	bool fly                = {false};
	bool engineOn           = {false};
	uint8_t joyRTime	    = {false};
	uint8_t joyLTime	    = {false};
	uint8_t specialOptTime  = {false};
	int8_t roll	            = {0};
	int8_t pitch	        = {0};
	int8_t yawl		        = {0};
	int8_t altitude         = {0};
	uint8_t mainNumber      = {0};
	uint8_t mainNumberTime  = {0};
	int8_t mainValue        = {0};
	
	uint8_t transmissionNumber = {0};

	static constexpr uint8_t optionsInterval = {50};
	static constexpr uint8_t mainOptionInterval = {10};
	void decrementFilters();
	void timeEventForRemote();
	void timeEventForDrone();	
public:
    RadioParser(RadioIfc* const radio, const RadioTrybe trybe);
    ~RadioParser();
	void run();
	bool getFlyOnOption() const;
	void setFlyOnOption(const bool);
	bool getEngineOnOption() const;
	void setEngineOnOption(const bool);
	bool getJoyRightButtonOption() const;
	void setJoyRightButtonOption();
	bool getJoyLeftButtonOption() const;
	void setJoyLeftButtonOption();
	bool getSpecialButtonOption() const;
	void setSpecialButtonOption();
	int8_t getRollValue() const;
	void setRollValue(const int8_t);
	int8_t getPitchValue() const;
	void setPitchValue(const int8_t);
	int8_t getYawlValue() const;
	void setYawlValue(const int8_t);
	int8_t getAltitudeIncremetionValue() const;
	void setAltitudeIncremetionValue(const int8_t);
	uint8_t getMainOptionNumber() const;
	int8_t getMainOptionValue() const;
	void setMainOption(const uint8_t, const int8_t);
	uint8_t getTransmissionNumber() const;
};

}
