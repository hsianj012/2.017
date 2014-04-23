int pwm_signal = A1;

void setup()
{
  Serial.begin(19200);
  pinMode(pwm_signal, INPUT);
}

float value = 0;
float angle = 0;
float avgAngle = 0;
float prevAngle = 0;

void loop()
{
  value = analogRead(pwm_signal);
  angle = (value/1024.0)*360.0;
  Serial.print("Reading value: ");
  Serial.print(value);
  Serial.print("Angle: ");
  Serial.print(angle);  
  
  //averaging
  //weight past values in relation to currnet value
  //more weight on current value - greater repsonse to change
  //0 to 360 jump
  avgAngle = 0.99*prevAngle +0.01*angle;
  prevAngle = angle;
  Serial.print("Average: ");
  Serial.println(avgAngle);  
}



