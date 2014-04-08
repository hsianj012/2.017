/*
 * Ahmad Byagowi (ahmadexp@gmail.com)
 * Date : Aug 31, 2013
 * All code (c)2013 Ahmad Byagowi Consultant. all rights reserved
*/

//changed pin numbers
int cs  = 4;  //green
int dio = 6;  //white
int clk = 5;  //yellow

void init_as5134(void){    //changed order of write and pinMode
  pinMode(cs,  OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(dio, INPUT);
  digitalWrite(cs, LOW);  //why is this low not high?...
  digitalWrite(clk, LOW);
}
int read_as5134(void){
  int clock_p;
  unsigned int data=0;
  digitalWrite(cs,LOW);
  digitalWrite(clk,LOW);
  digitalWrite(cs,HIGH);
  delay(1);
  for(clock_p=21;clock_p--;clock_p>0){  //counts down from 21 to 9
    digitalWrite(clk,LOW);
    delay(1);
    digitalWrite(clk,HIGH);
    delay(1);
    if(clock_p<9){
      if(digitalRead(dio))
        data+=(1<<(clock_p));
    }
  }
  digitalWrite(clk,LOW);
  digitalWrite(cs,LOW);
  return data;
}

// the setup routine runs once when you press reset:
void setup() {                
  init_as5134();
  Serial.begin(115200);
}

//variables to keep track of position
int reading = 0;
float angle = 0;

// the loop routine runs over and over again forever:
void loop() {
  
   reading = read_as5134();
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
