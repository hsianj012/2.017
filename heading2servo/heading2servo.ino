// This program reads the heading off the compass and set the motor to 

//THis is a comment

// NOTES ON SETUP:
// make sure compass is connected inputs A4 and A5

#include <Wire.h>
#include <Servo.h>
#include "HMC6352.h"

HMC6352 myCompass;
float heading1;
float heading2;
Servo myservo;

float heading360;

int pos = 0;


// this setup code is taken from the hmc6352 heading file
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

        myservo.attach(9);
}
void loop()
{
        // retrieve and print the current heading
	heading1 = myCompass.getHeading();
	Serial.print("heading1:");
        Serial.println(heading1);

        // Have the position of the servo match the heading
        // used 360-heading because servo and heading are in opposite directions
        heading360 = 360-heading1;
//        heading2 = myCompass.getHeading();
//        Serial.print("heading2:");
//        Serial.println(heading2);
//        
//        Serial.print("Difference:");
//        Serial.println(heading2-heading1);
//        Serial.println("");
//        

//      Test the having servo move when compass moves
        myservo.write(heading360);
        
       
        delay(100);
}
