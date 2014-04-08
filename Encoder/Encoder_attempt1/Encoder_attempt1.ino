#define SELECT_PIN 4 //green
#define CLOCK_PIN 5  //yellow
#define DATA_PIN 6   //white

void setup()
{
  //setup our pins
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(SELECT_PIN, OUTPUT);

  //give some default values
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(SELECT_PIN, HIGH);

  Serial.begin(19200);
}

//variables to keep track of position
int reading = 0;
float angle = 0;

void loop()
{
   reading = readPosition();
   
   if (reading >= 0)
   {
      angle = ((float)reading / 1024.0) * 360.0;

      Serial.print("Reading: ");
      Serial.print(reading, DEC);
      Serial.print(" Angle: ");
      Serial.println((int)angle, DEC);
   }
   else
   {
      Serial.print("Error: ");
      Serial.println(reading);
   }
   
   delay(1000);
}

//read the current angular position
int readPosition()
{
  unsigned int position1 = 0;     //RENAMING POSITION SINCE IT IS HIGHLIGHTED

  //shift in our data  
  digitalWrite(SELECT_PIN, LOW);
  delay(1);    //changeded from delayMicroseconds(1)
  byte d1 = shiftInB(DATA_PIN, CLOCK_PIN);
  byte d2 = shiftInB(DATA_PIN, CLOCK_PIN);
  digitalWrite(SELECT_PIN, HIGH);

  //get our position variable
  position1 = d1;
  position1 = position1 << 8; 
  position1 |= d2;
 
  position1 = position1 >> 6;

  
  //check the offset compensation flag: 1 == started up
  if (!(d2 & B00100000))
    position1 = -1;

  //check the cordic overflow flag: 1 = error
  if (d2 & B00010000)
    position1 = -2;

  //check the linearity alarm: 1 = error
  if (d2 & B00001000)
    position1 = -3;

  //check the magnet range: 11 = error
  if ((d2 & B00000110) == B00000110)
    position1 = -4;

  return position1;
}

//read in a byte of data from the digital input of the board.
byte shiftInB(byte data_pin, byte clock_pin)  //chnaged name to shiftInB
{
  byte data = 0;

  for (int i=7; i>=0; i--)
  {
    digitalWrite(clock_pin, LOW);
    delay(1);              //changeded from delayMicroseconds(1)
    digitalWrite(clock_pin, HIGH);
    delay(1);              //changeded from delayMicroseconds(1)

    byte bit1 = digitalRead(data_pin);  //renamed bit because it is highlighted
    data |= (bit1 << i);
  }

  Serial.print("byte: ");
  Serial.println(data, BIN);

  return data;
}
