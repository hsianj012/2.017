#include <SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/sleep.h>
#include <string>
#include <array>

// Ladyada's logger modified by Bill Greiman to use the SdFat library
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS Shield
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false
/* set to true to only log to SD when GPS has a fix, for debugging, keep it false */
#define LOG_FIXONLY false  

// Set the pins used
#define chipSelect 10
#define ledPin 13

File logfile;

///////////////////////
// Added extra variables
#define Rad_earth 6378100 // in m
double desiredLatitude;
double desiredLongitude;
double mydecimallatitude;
double mydecimallongitude;
double myDistanceToGo;

double desiredHeading; //from GPS
double currentHeading; //from compass
double relWindAngle; //from wind vane
//double flowDiff; // from anemometers
//double boatHeel; // from accelerometer

double sailTrim; //actuator variables
double rudderAngle;

boolean isLogging = true;
int logTime = 10000; //in ms
///////////////////////

// read a Hex value and return the decimal equivalent
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

// blink out an error code
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
  
  /////////////////////********************   GPS AND SD CARD INITIALIZATION   ******************************/////////////////////
  
  
  // for Leonardos, if you want to debug SD issues, uncomment this line
  // to see serial output
  //while (!Serial);
  
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
  
  ////////////////////////********************   OTHER INITIALIZATION PROCEDURES *********************************////////////////
  
   // *initialize other sensors here* 
 
  Serial.println("Ready!");
}

void logData(){
  
  /*
    Print statement:
    
    DESIRED HEADING [DEG], CURRENT HEADING [DEG], RUDDER ANGLE, REL WIND ANGLE [DEG], SAIL TRIM;
    
  */
  
  
  //    char *stringptr = GPS.lastNMEA();
  //    uint8_t stringsize = strlen(stringptr);
  //    if (stringsize != logfile.write((uint8_t *)stringptr, stringsize))    //write the string to the SD file
  //      error(4);
        
          
     //String bufferString = to_string(currentHeading);
     //const char *buffer = bufferString.c_str();
  //   char buffer[10];
  
  double data[] = {desiredHeading, currentHeading, rudderAngle, relWindAngle, sailTrim};
  int dataLength = 5;
  
  for(int i=0; i<dataLength; i++){     //iterate through array of data
    
    char *buffer =  new char[11];     // create buffer w/ space to store value and delimeter
    dtostrf(data[i], 4, 2, buffer);     //convert double to string and store in char array (buffer)
    buffer[9] = ',';     //change char before '/0' to delimeter between values
    
    if(i==(dataLength-1))    //if last value use the statement delimeter
      buffer[9] = ';';
      
    uint8_t len = strlen(buffer);
    
    if (len != logfile.write((uint8_t*)buffer, len))    //write the string to the SD file
      error(4);
    
  }
    
    logfile.flush();  //save data statement
    Serial.println();
  
}


void loop() {

  //////////////////**************** TIMED DATA LOGGING ****************//////////////////////////////////////////////////////
  if(millis()>logTime && isLogging==true){
    logfile.close();
    isLogging = false;
    Serial.println("Closing file");
    return;
  }
  
  else if(millis()>logTime && isLogging==false){
    Serial.println("Logging Done!!");
    return;
  }
  
  ///////////////////////////////////////**************************   GPS HEADING CALCULATION ******************///////////////////
  
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

    // Rad. lets log it!
    
    
    /////////////////////////
    // Added code to calculate heading and direction
    // CAREFUL!! You can't output all info to the serial port or it will freeze because
    // the baud rate is limited. So do be careful.
    
    // set desired location
    desiredLatitude = 41;
    desiredLongitude = -71.095161;
    
    Serial.print("dlat: ");  Serial.println(desiredLatitude,6);
    Serial.print("dlong: "); Serial.println(desiredLongitude,6);
    
    // convert latitude and longitude to helpful values
    // Ex: 4213.14 = 42 [degrees] + 13.14 [minutes] /60 = 42.219 [decimal degrees]
    // The extra ((GPS.lat=='N')*2-1) is 1 if north and -1 if south
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
    // cos(mydecimallatitude)*(desiredLongitude - mydecimallongitude) * Rad_earth is the "horizontal" part of the square
    // (desiredLatitude-mydecimallatitude) * Rad_earth is the vertical part of the square.
    // When calculating the heading you do atan(horizontal/vertical) so the Rad_earth cancels.
    // To get the heading in degrees you multiply by 180 and divide by 3.141592.
    desiredHeading = atan2(
          cos(mydecimallatitude/180.0*3.141592)*(desiredLongitude - mydecimallongitude),
          (desiredLatitude-mydecimallatitude))
          *180.0/3.141592;
    
    //try to keep heading always positive.
    if (desiredHeading <0)
        desiredHeading = desiredHeading + 360;
        
    // Calculate distance to go: use pythagorean theorem.
    myDistanceToGo = Rad_earth * 
          sqrt(
          pow((cos(mydecimallatitude/180.0*3.141592)*(desiredLongitude - mydecimallongitude)/180.0*3.141592),2.0)+
          pow((desiredLatitude-mydecimallatitude)/180.0*3.141592,2.0));
          
    Serial.print("H: ");Serial.println(desiredHeading,6); 
    Serial.print("D: ");Serial.println(myDistanceToGo,6);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

    Serial.println("Logging Data");
    
    // building statement
    logData(); // CAN ONLY TAKE PROPERLY ROUNDED VALUES
    // End added code
    ///////////////////////
  }
}


/* End code */
