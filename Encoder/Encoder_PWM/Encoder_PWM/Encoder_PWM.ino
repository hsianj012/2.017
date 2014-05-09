#include <Servo.h>

#define trimServo 45
#define pwm_signal A2


float f_cutoff = 0.2;
float relWindAngle_trim = 9999.9;
long lStart = 0;
Servo sailTrimServo;


void setup()
{
  Serial.begin(19200);
  pinMode(pwm_signal, INPUT);
  sailTrimServo.attach(trimServo);
}



void loop()
{
  int pwm;
  float angle = (int)((analogRead(pwm_signal)/1024.0)*360.0+110)%360;
  //Serial.print("Reading value: ");
  //Serial.print(value);
  
  float deltaT = getLoopRate();
  float alpha_trim = deltaT/((1/(2*PI*f_cutoff))+deltaT);
  
  if(relWindAngle_trim == 9999.9){
   relWindAngle_trim = angle;
   return;
  } 
  relWindAngle_trim = alpha_trim*angle+(1-alpha_trim)*relWindAngle_trim;
  

  
  if(relWindAngle_trim < 50 || relWindAngle_trim > 310){
    sailTrimServo.write(70);
    pwm = 70;
  }
  else if(relWindAngle_trim < 180){
    pwm = map(relWindAngle_trim,50,180,70,110);
    sailTrimServo.write(pwm);
  }
  else{
    pwm = map(relWindAngle_trim,180,310,110,70);
    sailTrimServo.write(pwm);
  }
    

  
  //averaging
  //weight past values in relation to currnet value
  //more weight on current value - greater repsonse to change
  //0 to 360 jump
//  avgAngle = 0.99*prevAngle +0.01*angle;
//  prevAngle = angle;
//  Serial.print("Average: ");
//  Serial.println(avgAngle);  
}

float getLoopRate(){
  float dT = 0.001*(millis()-lStart);
  lStart = millis();
  return dT;
}



