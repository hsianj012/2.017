#include <Arduino.h>
#include <Wire.h>
#include <HMC6352.h>

// void constructor
HMC6352::HMC6352()
{
	address_ = 0x21;	// this is 0x42 shifted once to the right
};	

// user-specified address
HMC6352::HMC6352(int address)
{
	address_ = address;
};

float HMC6352::getHeading()
{
	Wire.beginTransmission(address_);
	Wire.write(c_getDataStandby_);	
	Wire.endTransmission();

	delay(7);

	Wire.requestFrom(address_, 2);

	byte highByte	= Wire.read();
	byte lowByte	= Wire.read();

	return((highByte*256+lowByte)/10.0);
}

int HMC6352::getOperationalMode()
{
	Wire.beginTransmission(address_);
	Wire.write(c_readFromRAM_);	
	Wire.write(c_operationalModeAddressRAM_);
	Wire.endTransmission();

	delay(1);

	Wire.requestFrom(address_, 1);

	int modeValue = Wire.read();

	return(modeValue&3);
}

int HMC6352::getOutputMode()
{
	Wire.beginTransmission(address_);
	Wire.write(c_readFromRAM_);	
	Wire.write(c_outputModeAddressRAM_);
	Wire.endTransmission();

	delay(1);

	Wire.requestFrom(address_, 1);

	int modeValue = Wire.read();

	return(modeValue&7);
}

void HMC6352::setOutputMode(const int& mode)
{
	int setMode = mode;
	// sanity check on the input
	if (setMode > 4 || setMode < 0) 
	{
		setMode = 0;	// fall-back to heading measurements
	}
	Wire.beginTransmission(address_);
	Wire.write(c_writeToRAM_);				// command
	Wire.write(c_outputModeAddressRAM_);	// argument 1 - address
	Wire.write(setMode);					// argument 2 - data
	Wire.endTransmission();

	delay(1);
}