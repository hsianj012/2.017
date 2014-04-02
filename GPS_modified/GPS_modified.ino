#include <SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/sleep.h>



SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
#define GPSECHO  true
/* set to true to only log to SD when GPS has a fix, for debugging, keep it false */
#define LOG_FIXONLY true  

// Set the pins used
#define chipSelect 10
#define ledPin 13

File logfile;

#define Rad_earth 6378100 // in m
double desiredlatitude;
double desiredlongitude;
double myHeading;
double mydecimallatitude;
double mydecimallongitude;
double myDistanceToGo;
double compassHeading = 180.0;
boolean isLogging = true;

uint8_t parseHex(char c) {
  if (c < '0')
    return 0;
  if (c <= '9')
    return c - '0';
  if (c < 'A')
    return 0;
  if (c <= 'F')
    return (c - 'A')+10;
}

void error(uint8_t errno) {
/*
  if (SD.errorCode()) {
    putstring("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
  }
  */
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

void setup() { 
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  
  Serial.begin(115200);
  Serial.println("\r\nUltimate GPSlogger Shield");
  pinMode(ledPin, OUTPUT);

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  //if (!SD.begin(chipSelect, 11, 12, 13)) {
  if (!SD.begin(chipSelect)) {      // if you're using an UNO, you can use this line instead
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "GPSLOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); Serial.println(filename);
  
  logfile.println("DESIRED HEADING [deg]      CURRENT HEADING [deg]"); ////TEST
  logfile.println();
  
  // connect to the GPS at the desired rate
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For logging data, we don't suggest using anything but either RMC only or RMC+GGA
  // to keep the log files at a reasonable size
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 or 5 Hz update rate

  // Turn off updates on antenna status, if the firmware permits it
  GPS.sendCommand(PGCMD_NOANTENNA);
  
  Serial.println("Ready!");
}

void loop() {
  if(millis()>10000&&isLogging){
    logfile.close();
    isLogging=false;
    return;
  }
  else if(millis()>10000&&!isLogging){
    Serial.println("Logging Done!!!");
    return;
  }
  char c = GPS.read();
  if (GPSECHO)
     if (c)   Serial.print(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    // Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
        
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
    
    // Sentence parsed! 
    Serial.println("OK");
    if (LOG_FIXONLY && !GPS.fix) {
        Serial.print("No Fix");
        return;
    }
   
    // set desired location
    desiredlatitude = 41;
    desiredlongitude = -71.095161;
    
    Serial.print("dlat: ");  Serial.println(desiredlatitude,6);
    Serial.print("dlong: "); Serial.println(desiredlongitude,6);
    
    mydecimallatitude  = (GPS.latitude  - fmod(GPS.latitude, 100.0)  + fmod(GPS.latitude,  100.0)/60.0*100.0)/100.0 * ((GPS.lat=='N')*2-1);
    mydecimallongitude = (GPS.longitude - fmod(GPS.longitude, 100.0) + fmod(GPS.longitude, 100.0)/60.0*100.0)/100.0 * ((GPS.lon=='E')*2-1);
    
    // Serial.println("decimallatitude");
    // Serial.print(mydecimallatitude);
    // Serial.println();
    // Serial.println("decimallongitude");
    // Serial.print(mydecimallongitude);
    // Serial.println();
    
    //Serial.print("latitude: ");         Serial.println(GPS.latitude,6);
    Serial.print("lat: ");  Serial.println(mydecimallatitude,6);
    //Serial.print("longitude: ");        Serial.println(GPS.lon);
    Serial.print("long: "); Serial.println(mydecimallongitude,6);
    
    // calculate heading
    // cos(mydecimallatitude)*(desiredlongitude - mydecimallongitude) * Rad_earth is the "horizontal" part of the square
    // (desiredlatitude-mydecimallatitude) * Rad_earth is the vertical part of the square.
    // When calculating the heading you do atan(horizontal/vertical) so the Rad_earth cancels.
    // To get the heading in degrees you multiply by 180 and divide by 3.141592.
    myHeading = atan2(
          cos(mydecimallatitude/180.0*3.141592)*(desiredlongitude - mydecimallongitude),
          (desiredlatitude-mydecimallatitude))
          *180.0/3.141592;
    
    //try to keep heading always positive.
    if (myHeading <0)
        myHeading = myHeading + 360;
        
    // Calculate distance to go: use pythagorean theorem.
    myDistanceToGo = Rad_earth * 
          sqrt(
          pow((cos(mydecimallatitude/180.0*3.141592)*(desiredlongitude - mydecimallongitude)/180.0*3.141592),2.0)+
          pow((desiredlatitude-mydecimallatitude)/180.0*3.141592,2.0));
          
    Serial.print("H: ");Serial.println(myHeading,6); 
    Serial.print("D: ");Serial.println(myDistanceToGo,6);
    
/////////////DATA LOGGING OPERATIONS/////////////////////////////////////////////////////////////////////////////////////////////

    Serial.println("LOGGING DATA!!!!");
    /*
    char *stringptr = GPS.lastNMEA();
    uint8_t stringsize = strlen(stringptr);
    if (stringsize != logfile.write((uint8_t *)stringptr, stringsize))    //write the string to the SD file
      error(4);
    if (strstr(stringptr, "RMC"))   logfile.flush();
    */
    
    logfile.print(myHeading);
    logfile.print("   ");
    logfile.println(compassHeading);
    logfile.flush();
    Serial.println();

    // End added code
    ///////////////////////
  }
}


/* End code */
