#define SELECT_PIN 4  //green
#define CLOCK_PIN 5   //yellow
#define DATA_PIN 6    //white

//READING DATA FUNCTION
int read_encoder(){    //is void neccessary inside brakcets
  unsigned int position1 = 0;

  //shift in our data  
  digitalWrite(SELECT_PIN, LOW);
  delay(1);            //lengthening delay
  byte d1 = shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);
  byte d2 = shiftIn(DATA_PIN, CLOCK_PIN, MSBFIRST);
  digitalWrite(SELECT_PIN, HIGH);
  
  //get our position variable - shifting bits
  position1 = d1;
  position1 = position1 << 8;
  position1 |= d2;
  position1 = position1 >> 6;
  
  return position1;
}

//SETUP
void setup() {                
  pinMode(SELECT_PIN,  OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);
  
   //digitalWrite(SELECT_PIN, LOW);
  //digitalWrite(CLOCK_PIN, LOW);
  Serial.begin(115200);
}

int reading;
float angle;

// the loop routine runs over and over again forever:
void loop() {
   reading = read_encoder();
   
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
      //Serial.println(reading);
   }
   
   delay(1000);
}
