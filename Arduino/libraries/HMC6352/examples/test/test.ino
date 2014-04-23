#include <Wire.h>
#include "HMC6352.h"

HMC6352 myCompass;
float heading;

void setup()
{
	Serial.begin(9600);
	delay(10);
	Wire.begin();
	delay(10);
	Serial.println("RESET!");

    // read EEPROM
    Serial.println("Dumping content of sensor configuration (from EEPROM):");
	for(byte i = 0; i< 9; i++)
	{
		Wire.beginTransmission(0x21);
		Wire.write(0x72);	
		Wire.write(i);
		Wire.endTransmission();

		delay(1);

		Wire.requestFrom(0x21, 1);

		int data = Wire.read();
		Serial.print("Address ");
        Serial.print(i);
        Serial.print(": ");
		Serial.println(data);
	}    

	Serial.print("Operational mode: ");
	Serial.println(myCompass.getOperationalMode());
	Serial.print("Output mode: ");
	Serial.println(myCompass.getOutputMode());
	Serial.println("Setting output mode to Raw Magnetometer X (1)");
	Serial.println("(power-cycle the sensor to revert to heading)");
	Serial.println("(or use a setOutputMode call to restore it)");
	myCompass.setOutputMode(1);
	Serial.print("Output mode: ");
	Serial.println(myCompass.getOutputMode());
}
void loop()
{
	heading = myCompass.getHeading();
	Serial.println(heading);
	delay(100);
}