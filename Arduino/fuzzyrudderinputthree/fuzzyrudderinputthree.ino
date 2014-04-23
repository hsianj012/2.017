#include <iostream>
#include "FuzzyRule.h"
#include "FuzzyComposition.h"
#include "Fuzzy.h"
#include "FuzzyRuleConsequent.h"
#include "FuzzyOutput.h"
#include "FuzzyInput.h"
#include "FuzzyIO.h"
#include "FuzzySet.h"
#include "FuzzyRuleAntecedent.h"
#include <Wire.h>
#include <Servo.h>
//#include "HMC6352.h"
 

//HMC6352 myCompass;
//float heading1;
//float heading2;
//Servo myservo;

float heading360;

int pos = 0;
int gyroPin = A0;   //Gyro is connected to analog pin 0
int gyroPintwo = A1;
float gyroVoltage = 3.3;         //Gyro is running at 5V
float gyroZeroVoltage = 1.27;  //Gyro is zeroed at 2.5V
float gyroSensitivity = .0083;  //Our example gyro is 7mV/deg/sec


// Instanciando um objeto da biblioteca
Fuzzy* fuzzy = new Fuzzy();

//FuzzySet* left = new FuzzySet(0, -45, -45, -90);
FuzzySet* left = new FuzzySet(-90, -45, -45, 0);
FuzzySet* middle = new FuzzySet(-45, -15, 15, 45);
FuzzySet* right = new FuzzySet(0, 45, 45, 90);
FuzzySet* strongright = new FuzzySet(45, 90, 90, 180);
//FuzzySet* strongleft = new FuzzySet(-45, -100, -100, -180);
FuzzySet* strongleft = new FuzzySet(-180, -90, -90, -45);

//FuzzySet* leftturn = new FuzzySet(0, -20, -1000, -1000);
FuzzySet* leftturn = new FuzzySet(-50, -50, -20, 0);
FuzzySet* rightturn = new FuzzySet(0, 20, 50, 50);
FuzzySet* neutralturn = new FuzzySet(-20, 0, 0, 20);

  FuzzySet* strongleftrud = new FuzzySet(-15, -15, -15, -15);
  FuzzySet* leftrud = new FuzzySet(-5, -5, -5, -5);
  FuzzySet* neutralrud = new FuzzySet(0, 0, 0, 0);
  FuzzySet* rightrud = new FuzzySet(5, 5, 5, 5);
  FuzzySet* strongrightrud = new FuzzySet(15, 15, 15, 15);

float mydirection = 0;
float difference = 0;
float myturn = 0;
 
void setup(){
    Serial.begin(9600);

 // Serial.begin(9600);
  
  delay(10);
	Wire.begin();
	delay(10);
	Serial.println("RESET!");

//	myCompass.setOutputMode(0);
//	Serial.print("Output mode: ");
///	Serial.println(myCompass.getOutputMode());

   //     myservo.attach(9);
  
  // FuzzyInput
  FuzzyInput* desheading = new FuzzyInput(1);
  desheading->addFuzzySet(left);
  desheading->addFuzzySet(right);
  desheading->addFuzzySet(strongright);
  desheading->addFuzzySet(strongleft);
  desheading->addFuzzySet(middle);



  fuzzy->addFuzzyInput(desheading);

  // FuzzyInput
  FuzzyInput* turn = new FuzzyInput(2);
  turn->addFuzzySet(leftturn);
  turn->addFuzzySet(rightturn);
  turn->addFuzzySet(neutralturn);

  fuzzy->addFuzzyInput(turn);

  // FuzzyOutput
  // adicionando speed como output também
  FuzzyOutput* rudderchange = new FuzzyOutput(1);

//
  rudderchange->addFuzzySet(strongleftrud);
  rudderchange->addFuzzySet(leftrud);
  rudderchange->addFuzzySet(neutralrud);
  rudderchange->addFuzzySet(rightrud);
  rudderchange->addFuzzySet(strongrightrud);

  fuzzy->addFuzzyOutput(rudderchange);

  // Building FuzzyRule
  FuzzyRuleAntecedent* ifheadleftandturnleft = new FuzzyRuleAntecedent();
  ifheadleftandturnleft ->joinWithAND(left, leftturn);
  FuzzyRuleAntecedent* ifheadleftandturnright = new FuzzyRuleAntecedent();
  ifheadleftandturnright ->joinWithAND(left, rightturn);
  FuzzyRuleAntecedent* ifheadleftandturnneutral = new FuzzyRuleAntecedent();
  ifheadleftandturnneutral ->joinWithAND(left, neutralturn);

  FuzzyRuleAntecedent* ifheadstrongrightandturnleft = new FuzzyRuleAntecedent();
  ifheadstrongrightandturnleft ->joinWithAND(strongright, leftturn);
  FuzzyRuleAntecedent* ifheadstrongrightandturnright= new FuzzyRuleAntecedent();
  ifheadstrongrightandturnright ->joinWithAND(strongright, rightturn);
  FuzzyRuleAntecedent* ifheadstrongrightandturnneutral= new FuzzyRuleAntecedent();
  ifheadstrongrightandturnneutral ->joinWithAND(strongright, neutralturn);

  FuzzyRuleAntecedent* ifheadstrongleftandturnleft = new FuzzyRuleAntecedent();
  ifheadstrongleftandturnleft ->joinWithAND(strongleft, leftturn);
  FuzzyRuleAntecedent* ifheadstrongleftandturnright= new FuzzyRuleAntecedent();
  ifheadstrongleftandturnright ->joinWithAND(strongleft, rightturn);
  FuzzyRuleAntecedent* ifheadstrongleftandturnneutral= new FuzzyRuleAntecedent();
  ifheadstrongleftandturnneutral ->joinWithAND(strongleft, neutralturn);

  FuzzyRuleAntecedent* ifheadrightandturnleft = new FuzzyRuleAntecedent();
  ifheadrightandturnleft ->joinWithAND(right, leftturn);
  FuzzyRuleAntecedent* ifheadrightandturnright= new FuzzyRuleAntecedent();
  ifheadstrongrightandturnright ->joinWithAND(right, rightturn);
  FuzzyRuleAntecedent* ifheadrightandturnneutral= new FuzzyRuleAntecedent();
  ifheadrightandturnneutral ->joinWithAND(right, neutralturn);

  FuzzyRuleAntecedent* ifheadmiddleandturnleft = new FuzzyRuleAntecedent();
  ifheadmiddleandturnleft ->joinWithAND(middle, leftturn);
  FuzzyRuleAntecedent* ifheadmiddleandturnright= new FuzzyRuleAntecedent();
  ifheadmiddleandturnright ->joinWithAND(middle, rightturn);
  FuzzyRuleAntecedent* ifheadmiddleandturnneutral= new FuzzyRuleAntecedent();
  ifheadmiddleandturnneutral ->joinWithAND(middle, neutralturn);

FuzzyRuleConsequent* thenneutralrud= new FuzzyRuleConsequent();
  thenneutralrud ->addOutput(neutralrud);
FuzzyRuleConsequent* thenstrongrightrud= new FuzzyRuleConsequent();
  thenstrongrightrud ->addOutput(strongrightrud);
FuzzyRuleConsequent* thenstrongleftrud= new FuzzyRuleConsequent();
  thenstrongleftrud ->addOutput(strongleftrud);
FuzzyRuleConsequent* thenleftrud= new FuzzyRuleConsequent();
  thenleftrud ->addOutput(leftrud);
FuzzyRuleConsequent* thenrightrud= new FuzzyRuleConsequent();
  thenrightrud ->addOutput(rightrud);
  
  FuzzyRule* fuzzyRule1 = new FuzzyRule(1, ifheadleftandturnleft, thenneutralrud);
 fuzzy->addFuzzyRule(fuzzyRule1);
 FuzzyRule* fuzzyRule2 = new FuzzyRule(2, ifheadleftandturnneutral, thenleftrud);
  fuzzy->addFuzzyRule(fuzzyRule2);
FuzzyRule* fuzzyRule3 = new FuzzyRule(3, ifheadleftandturnright, thenstrongleftrud);
 fuzzy->addFuzzyRule(fuzzyRule3);
FuzzyRule* fuzzyRule4 = new FuzzyRule(4, ifheadstrongleftandturnleft, thenleftrud);
  fuzzy->addFuzzyRule(fuzzyRule4);
FuzzyRule* fuzzyRule5 = new FuzzyRule(5, ifheadstrongleftandturnneutral, thenstrongleftrud);
  fuzzy->addFuzzyRule(fuzzyRule5);
FuzzyRule* fuzzyRule6 = new FuzzyRule(6, ifheadstrongleftandturnright, thenstrongleftrud);
 fuzzy->addFuzzyRule(fuzzyRule6);
FuzzyRule* fuzzyRule7 = new FuzzyRule(7, ifheadmiddleandturnneutral, thenneutralrud);
  fuzzy->addFuzzyRule(fuzzyRule7);
FuzzyRule* fuzzyRule8 = new FuzzyRule(8, ifheadmiddleandturnleft, thenrightrud);
  fuzzy->addFuzzyRule(fuzzyRule8);
FuzzyRule* fuzzyRule9 = new FuzzyRule(9, ifheadmiddleandturnright, thenleftrud);
  fuzzy->addFuzzyRule(fuzzyRule9);
FuzzyRule* fuzzyRule10 = new FuzzyRule(10, ifheadrightandturnleft, thenstrongrightrud);
  fuzzy->addFuzzyRule(fuzzyRule10);
FuzzyRule* fuzzyRule11= new FuzzyRule(11, ifheadrightandturnneutral, thenrightrud);
  fuzzy->addFuzzyRule(fuzzyRule11);
  FuzzyRule* fuzzyRule12 = new FuzzyRule(12, ifheadrightandturnright, thenneutralrud);
  fuzzy->addFuzzyRule(fuzzyRule12);
FuzzyRule* fuzzyRule13 = new FuzzyRule(13, ifheadstrongrightandturnneutral, thenstrongrightrud);
  fuzzy->addFuzzyRule(fuzzyRule13);
FuzzyRule* fuzzyRule14 = new FuzzyRule(14, ifheadstrongrightandturnright, thenrightrud);
  fuzzy->addFuzzyRule(fuzzyRule14);
FuzzyRule* fuzzyRule15 = new FuzzyRule(15, ifheadstrongrightandturnleft, thenstrongrightrud);
  fuzzy->addFuzzyRule(fuzzyRule15);

}

void loop(){
  
//  heading1 = myCompass.getHeading();
//  Serial.print("heading1:");
//  Serial.println(heading1);
//  delay(10000);
//  heading2 = myCompass.getHeading();
//        Serial.print("heading2:");
 //     Serial.println(heading2);
      
 //  Serial.print("Difference:");
//   difference = heading2-heading1;
 //  myturn = difference/10;
   float gyroRate = (analogRead(gyroPin) * gyroVoltage) / 1023;
  float gyroRatetwo = (analogRead(gyroPintwo) * gyroVoltage) / 1023;
 // float gyroRate = analogRead(gyroPin);
 // float gyroRatetwo = analogRead(gyroPintwo);
  //This line finds the voltage offset from sitting still
  gyroRate -= gyroZeroVoltage;
//gyroRatetwo -= gyroZeroVoltage;
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
  //Serial.println(gyroRate);
  //Serial.println(gyroRatetwo);
  myturn = gyroRatetwo;
  fuzzy->setInput(1, mydirection); //sets direction
  fuzzy->setInput(2, myturn); //sets turnspeed
  //fuzzy->setInput(input#, value):
//delay(10000);
  fuzzy->fuzzify();
  
  Serial.println("");
  Serial.print("Set Direction");
  Serial.print(mydirection);
  Serial.print("Set turnspeed");
  Serial.println(myturn);
  
  Serial.print("Turn: left");
  Serial.print(leftturn->getPertinence());
  Serial.print(", middle");
  Serial.println(neutralturn->getPertinence());
  Serial.print(", right");
  Serial.println(rightturn->getPertinence());
  
  Serial.print("Direction: strongleft");
  Serial.println(strongleft->getPertinence());
  Serial.print(", left ");
  Serial.print(left->getPertinence());
  Serial.print(", middle ");
  Serial.println(middle->getPertinence());
  Serial.print(", right ");
  Serial.print(right->getPertinence());
  Serial.print(", strongright ");
  Serial.println(strongright->getPertinence());

  float outputone = fuzzy->defuzzify(1);
   
  Serial.print("Rudderangle: ");
  Serial.println(outputone);
  Serial.println(", Output Pertinence ");
    Serial.print(", right ");
  Serial.print(rightrud->getPertinence());
    Serial.print(", left ");
  Serial.print(leftrud->getPertinence());
    Serial.print(", middle ");
  Serial.print(neutralrud->getPertinence());
    Serial.print(", strongright ");
  Serial.print(strongrightrud->getPertinence());
      Serial.print(", strongleft ");
  Serial.print(strongleftrud->getPertinence());
  Serial.println("");
  bool wasTheRule1Fired = fuzzy->isFiredRule(1);
 // Serial.print("rule 1");
  //Serial.println(wasTheRule1Fired);
   // bool wasTheRule2Fired = fuzzy->isFiredRule(2);
   // Serial.print("rule 2");
 // Serial.println(wasTheRule2Fired);
  //bool wasTheRule3Fired = fuzzy->isFiredRule(3);
 // Serial.print("rule 3");
 // Serial.println(wasTheRule3Fired);
 // bool wasTheRule4Fired = fuzzy->isFiredRule(4);
 // Serial.print("rule 4");
 // Serial.println(wasTheRule4Fired);
  //bool wasTheRule5Fired = fuzzy->isFiredRule(5);
   // Serial.print("rule 5");
 // Serial.println(wasTheRule5Fired);
 // bool wasTheRule6Fired = fuzzy->isFiredRule(6);
  //  Serial.print("rule 6");
  //Serial.println(wasTheRule6Fired);
 // bool wasTheRule7Fired = fuzzy->isFiredRule(7);
  //  Serial.print("rule 7");
  //Serial.println(wasTheRule7Fired);
 // bool wasTheRule8Fired = fuzzy->isFiredRule(8);
   // Serial.print("rule 8");
 // Serial.println(wasTheRule8Fired);
 // bool wasTheRule9Fired = fuzzy->isFiredRule(9);
  //  Serial.print("rule 9");
  //Serial.println(wasTheRule9Fired);
  //bool wasTheRule10Fired = fuzzy->isFiredRule(10);
   // Serial.print("rule 10");
 /// Serial.println(wasTheRule10Fired);
 // bool wasTheRule11Fired = fuzzy->isFiredRule(11);
  //  Serial.print("rule 11");
 // Serial.println(wasTheRule11Fired);
  //bool wasTheRule12Fired = fuzzy->isFiredRule(12);
   // Serial.print("rule 12");
 // Serial.println(wasTheRule12Fired);
 // bool wasTheRule13Fired = fuzzy->isFiredRule(13);
  //  Serial.print("rule 13");
 // Serial.println(wasTheRule13Fired);
 // bool wasTheRule14Fired = fuzzy->isFiredRule(14);
   // Serial.print("rule 14");
 /// Serial.println(wasTheRule14Fired);
 // bool wasTheRule15Fired = fuzzy->isFiredRule(15);
   // Serial.print("rule 15");
 // Serial.println(wasTheRule15Fired);
  
  // myservo.write(outputone);


  delay(10);
  
  
}
