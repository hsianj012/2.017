/* Keep track of gyro angle over time
 * Connect Gyro to Analog Pin 0
 *
 * Sketch by eric barch / ericbarch.com
 * v. 0.1 - simple serial output
 *
 */

int gyroPin = A0;   //Gyro is connected to analog pin 0
int gyroPintwo = A1;
float gyroVoltage = 3.3;         //Gyro is running at 5V
float gyroZeroVoltage = 1.27;  //Gyro is zeroed at 2.5V
float gyroSensitivity = .0083;  //Our example gyro is 7mV/deg/sec
float rotationThreshold = 1;   //Minimum deg/sec to keep track of - helps with gyro drifting

float currentAngle = 0;          //Keep track of our current angle

void setup() {
  Serial.begin (9600);
}

void loop() {
  //This line converts the 0-1023 signal to 0-5V
  float gyroRate = (analogRead(gyroPin) * gyroVoltage) / 1023;
  float gyroRatetwo = (analogRead(gyroPintwo) * gyroVoltage) / 1023;
 // float gyroRate = analogRead(gyroPin);
 // float gyroRatetwo = analogRead(gyroPintwo);
  //This line finds the voltage offset from sitting still
  gyroRate -= gyroZeroVoltage;
gyroRatetwo -= gyroZeroVoltage;
  //This line divides the voltage we found by the gyro's sensitivity
  gyroRate /= gyroSensitivity;
  gyroRatetwo /= gyroSensitivity;

  //Ignore the gyro if our angular velocity does not meet our threshold
 //if (gyroRatetwo >= rotationThreshold || gyroRatetwo <= -rotationThreshold) {
    //This line divides the value by 100 since we are running in a 10ms loop (1000ms/10ms)
  //  gyroRatetwo /= 100;
//currentAngle = gyroRatetwo;
 // }
   gyroRatetwo /= 1;
   gyroRatetwo -= 96.77;
//currentAngle += gyroRatetwo;
  //Keep our angle between 0-359 degrees
 // if (currentAngle < 0)
  //  currentAngle += 360;
 // else if (currentAngle > 359)
   // currentAngle -= 360;

  //DEBUG
  Serial.println(gyroRate);
  Serial.println(gyroRatetwo);

  delay(10);
}
