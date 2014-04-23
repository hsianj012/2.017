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

	myCompass.setOutputMode(0);
	Serial.print("Output mode: ");
	Serial.println(myCompass.getOutputMode());
}
void loop()
{
	heading = myCompass.getHeading();
	Serial.println(heading);
	delay(100);
}