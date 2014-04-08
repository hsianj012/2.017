//from http://reprap.org/wiki/Magnetic_Rotary_Encoder_1.0
//sad times no analog pin on this encoder :(
#define ANALOG_PIN 0

int reading = 0;
float angle = 0;

void setup()
{
   Serial.begin(19200);
   Serial.println("Started");
}

void loop()
{
   reading = analogRead(ANALOG_PIN);
   angle = ((float)reading / 1024.0) * 360.0;
   
   Serial.print("Reading: ");
   Serial.print(reading, DEC);
   Serial.print(" Angle: ");
   Serial.println((int)angle, DEC);
   
   delay(1000);
}
