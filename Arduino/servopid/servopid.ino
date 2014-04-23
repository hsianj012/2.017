/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include <PID_v1.h>
#include <Servo.h>
#include <Wire.h>
#include <LSM303.h>

LSM303 compass;
float headingnew;
//Define Variables we'll be connecting to
double Setpoint, Input, Output;
Servo myservo;
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,2,5,1, DIRECT);
// myPID(in,out,set,kp,ki,kd,direct); what is direct?
void setup()
{
  //initialize the variables we're linked to
 Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>) {  -254,    -50,   -676};
  compass.m_max = (LSM303::vector<int16_t>){    +3,    -41,   -664};
  compass.read();
  headingnew = compass.heading();
  Input = headingnew;
  Setpoint = 0;

  //turn the PID on
  myservo.attach(9);
  myPID.SetMode(AUTOMATIC);
  
}

void loop()
{
  compass.read();
  headingnew = compass.heading();
  Serial.println(headingnew);
  Input = headingnew;
  myPID.Compute();
  myservo.write(Output);
  Serial.println(Output);
  delay(1000);
}


