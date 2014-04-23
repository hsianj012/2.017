// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
Servo myservo2;               // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
int pos2 = 0;
int analogPin = 0;
int val = 0;
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(10); 
  Serial.begin(9600);
} 
 
 
void loop() 
{ 
  val = analogRead(analogPin);
  Serial.println(val-360);
  delay(1000);
  
  pos = val - 360;
//  myservo.write(pos);
//  myservo2.write(pos);
  // the pot reads from 360 to 540
//  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
//  {                                  // in steps of 1 degree 
//    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//    myservo2.write(2*pos);
//    delay(15);                       // waits 15ms for the servo to reach the position 
 // } 
 // for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
 // {                                
 //   myservo.write(pos);              // tell servo to go to position in variable 'pos' 
 //   myservo2.write(2*pos);
//   delay(15);                       // waits 15ms for the servo to reach the position 
 // } 
} 
