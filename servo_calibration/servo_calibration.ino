// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
 
void setup() 
{ 
  myservo.attach(45);  // attaches the servo on pin 9 to the servo object 
} 
 
 
void loop() 
{ 
  for(i=0; i<60;i++){
    myservo.write(i);   // waits 15ms for the servo to reach the position 
  } 
} 
