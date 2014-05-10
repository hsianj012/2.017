#include <SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>


Adafruit_GPS GPS(&Serial1); //using MEGA tx1, rx1 hardware serial instead of SoftSerial
LSM303 compass; //initializing compass on SDA(20) and SCL(21)

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false
/* set to true to only log to SD when GPS has a fix, for debugging, keep it false */
#define LOG_FIXONLY false  

// Set the pins used
#define chipSelect 10
#define ledPin 13
#define windVaneSignal A2
#define rudServo 44
#define trimServo 45 

File logfile;
Servo rudderServo;
Servo sailTrimServo;

///////////////////////
// Added extra variables
#define Rad_earth 6378100 // in m
float desiredLatitude = 0.0;
float desiredLongitude = 0.0;
float mydecimallatitude = 0.0;
float mydecimallongitude = 0.0;
float myDistanceToGo = 0.0;

float desiredHeading = 5.0; //from planner
float currentHeading = 9999.9; //from compass 9999.9 is a dummy variable to initialize the filter w/o 
float rudPrevError = 0.0;
float rudIntegral = 0.0;
float relWindAngle_trim = 9999.9; //from wind vane
//float relWindAngle_rud = 0.0;
//float relWindAngle_plan = 0.0;

float u_sailTrim = 110; //actuator variables
float u_rudder = 100; 



boolean isLogging = true;
unsigned long logTime = 60000; //in ms
unsigned long lStart = 0; //in ms
const int logRate = 10; //data is logged every [logRate] iterations of the loop; for a loop speed of 0.02, 5 logs/sec
int logNumber = 0; 
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
  
  //delay(10000); //give time to close boat electronics before starting test
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("\r\nUltimate GPSlogger Shield");
  pinMode(ledPin, OUTPUT);

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(windVaneSignal, INPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect, 11, 12, 13)) {
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "5_4LOG00.TXT");
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
  
  Wire.begin();
  compass.init();
  compass.enableDefault();
   
   /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>) {  -254,    -50,   -676};
  compass.m_max = (LSM303::vector<int16_t>){    +3,    -41,   -664};
 
  rudderServo.attach(rudServo);
  sailTrimServo.attach(trimServo);
  
  Serial.println("Sensors enabled...");

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
///////////////////////////////////////**************************GPS LOCATION UPDATE *********************////////////////////////
  
  // [implement] update current GPS location and filter using RP knowledge
  
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
 
 /*   
    if(millis()<60000)
      desiredHeading = 180.0;
    else if(millis() < 120000)
      desiredHeading = 90.0;
    else
      desiredHeading = 0;
*/
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    


  }
 
////////////////////////////////////************************UPDATE SENSORS AND RUN CONTROLLERS***************/////////////////////
  float loopRate = getLoopRate();
  updateWindDirection(loopRate); //use to test wind sensor
  updateCurrentHeading(loopRate); //take in compass data every time step and apply filter
  //[implement] updateTurningRate()/getRotationRate: used for fuzzy logic
  
  //[implement] updateDesiredHeading(): initiate planner method once every minute or 30 sec
  
  rudderController(loopRate);             // run w/ every loop iteration recieving filtered data and response at x Hz
  sailTrimController();                   // run w/ every loop iteration recieving filtered data and response at y Hz
  
  if(logNumber==logRate){ //logs data every [logRate] iterations of the loop to avoid slowing down processor
    logData();
    logNumber = 0;
  }
  else
    logNumber++;
}







///////////////////////////*******************DATA LOGGER METHOD****************************************//////////////////////////
void logData(){
  // logs every second w/ GPS
  /*
    Print statement:
    
    DESIRED HEADING [DEG], CURRENT HEADING [DEG], RUDDER ANGLE, REL WIND ANGLE [DEG], SAIL TRIM;
    
  */
  int dataLength = 5;
  float data[5] = {desiredHeading, currentHeading, u_rudder, relWindAngle_trim, u_sailTrim,};
  
  for(int i=0; i<dataLength; i++){     //iterate through array of data
    
    //char *buffer =  new char[11];     // create buffer w/ space to store value and delimeter
    char bufferArray[7];
    char * bufferPointer;
    bufferPointer = bufferArray;

    dtostrf(data[i], -8, 3, bufferPointer);     //convert float to string and store in char array (buffer)
    bufferArray[7] = ',';     //change char before '/0' to delimeter between values
    
    if(i==(dataLength-1))    //if last value use the statement delimeter
      bufferArray[7] = ';';
      
    uint8_t len = strlen(bufferPointer);
    if (len != logfile.write((uint8_t*)bufferPointer, len))    //write the string to the SD file
      error(4);
    
  }
    
    logfile.flush();  //save data statement
  
}

////////////////////////**************************PLANNER METHOD*************************////////////////////////////////////////
void updateDesiredHeading(){
  // 1 min timescale
  
  
  
}

////////////////////////**************************WIND SENSOR METHOD***************************///////////////////////////////////
void updateWindDirection(float deltaT){
  float angle = (int)((analogRead(windVaneSignal)/1024.0)*360.0+110)%360;
  
  // [implement] offset so that 0 deg is actual pointing to bow
  
  // apply filters and separate out into 3 data streams
  float f_cutoff_trim = 0.2; //cutoff fequency for sail trim controller, change this to adjust response time of controller
//  float f_cutoff_rud = 0.5; //cutoff fequency for modifying desired heading for rudder controller, change this to adjust response time of controller
//  float f_cutoff_plan = 1/60.0; //cutoff fequency for planner, change this to adjust averaging for the planner
//  
  float alpha_trim = deltaT/((1/(2*PI*f_cutoff_trim))+deltaT);
//  float alpha_rud = deltaT/((1/(2*PI*f_cutoff_rud))+deltaT);
//  float alpha_plan = deltaT/((1/(2*PI*f_cutoff_plan))+deltaT);
  
  if(relWindAngle_trim == 9999.9){
   relWindAngle_trim = angle;
   return;
  } 
  relWindAngle_trim = alpha_trim*angle+(1-alpha_trim)*relWindAngle_trim;
//  relWindAngle_rud = alpha_rud*angle+(1-alpha_rud)*relWindAngle_rud;
//  relWindAngle_plan = alpha_plan*angle+(1-alpha_plan)*relWindAngle_plan;
  

//  Serial.print("Wind Angle (desired heading adjustment filtered): ");
//  Serial.println(relWindAngle_rud);
//  Serial.print("Wind Angle (planner filtered): ");
//  Serial.println(relWindAngle_plan);  

  
}

////////////////////////**************************COMPASS HEADING METHOD***************************///////////////////////////////

void updateCurrentHeading(float deltaT){

  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.
  
  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.
  
  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use
  
  compass.heading((LSM303::vector<int>){0, 0, 1});
  
  to use the +Z axis as a reference.
  */
  
  float f_cutoff = 0.75; //cutoff fequency for rudder controller, change this to adjust response time of controller
  float alpha = deltaT/((1/(2*PI*f_cutoff))+deltaT);
  
  compass.read();
  //(LSM303::vector<int>){1, 0, 0}
  if(currentHeading == 9999.9){
    currentHeading = compass.heading(); //16 to compensate for offset in compass error
    return;
  }
  currentHeading = alpha*compass.heading()+(1-alpha)*currentHeading; //-180 ??

//  Serial.print("Filtered Current Heading: ");
//  Serial.println(currentHeading);
  //[implement] change heading reference angle
}

////////////////////////**************************RUDDER CONTROLLER***************************///////////////////////////////////

void rudderController(float deltaT){
    // CALIBRATION INFO (RC boat): pwm 50-150 (team boat): pwm 115 center, 60-160
    desiredHeading = 180.0; // FOR TESTING PURPOSES ONLY!!!!
   
    float kp = 0.6; //for kp = 0.5 saturation reached at error = +/- 90 deg
                    //incrementing kp by 0.1 moves saturation value ~10 deg
    float ki = 0.01;
    float kd = 0.0;
   
    float error = (desiredHeading - currentHeading); //valid if error <= 180 && error >=-180
    
    if(error > 180.0) //fixing 0/360deg discontinuity
      error = desiredHeading - (currentHeading + 360);
  
    else if (error < -180.0)
      error = (desiredHeading + 360) - currentHeading;
    
    rudIntegral = constrain((rudIntegral + error*deltaT),-25/ki,25/ki); 
    //constraint prevents indefinite windup; 25 is the max pwm contribution of the integral controller
    
    u_rudder = constrain(-1*(kp*error + ki*rudIntegral + kd*(error-rudPrevError)/deltaT)+115,60,160); 
    //keeps control action within acceptable values for servo pwm; controller+100 is compensation for servo PWM offset
      
//    Serial.print("desired heading: ");
//    Serial.println(desiredHeading);
//    Serial.print("current heading: ");
//    Serial.println(currentHeading);
//    Serial.print("actuator: ");
//    Serial.println(u_rudder);
    rudPrevError = error;
    rudderServo.write(u_rudder);


}

////////////////////////**************************SAIL TRIM CONTROLLER***************************///////////////////////////////////

void sailTrimController(){
  // CALIBRATION INFO (RC boat): pwm 50 - 130 (team boat): pwm 70-110, 70 = "close haul" 90 = "beam reach" 110="downwind run"
  
  if(relWindAngle_trim < 50 || relWindAngle_trim > 310)
    u_sailTrim = 70;
  
  else if(relWindAngle_trim < 180)
    u_sailTrim = map(relWindAngle_trim,50,180,70,110);

  else
    u_sailTrim = map(relWindAngle_trim,180,310,110,70);
  

  sailTrimServo.write(u_sailTrim);

}

////////////////////////////////**********************CALCULATE DELTA T OF LOOP/SENSOR READ RATE******************/////////////////
float getLoopRate(){
  float dT = 0.001*(millis()-lStart);
  lStart = millis();
  return dT;
}
  
/* End code */
