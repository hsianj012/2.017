int pwm_signal = A1;

void setup()
{
  Serial.begin(19200);
  pinMode(pwm_signal, INPUT);
}

float value = 0;
float angle = 0;
void loop()
{
  value = analogRead(pwm_signal);
  angle = (value/1024.0)*360.0;
  Serial.print("Reading value: ");
  Serial.println(value);
  Serial.print("Angle: ");
  Serial.println(angle);  
  
  delay(1000);
}

