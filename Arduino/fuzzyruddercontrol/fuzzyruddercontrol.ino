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

// Instanciando um objeto da biblioteca
Fuzzy* fuzzy = new Fuzzy();

//FuzzySet* left = new FuzzySet(0, -45, -45, -90);
FuzzySet* left = new FuzzySet(-90, -45, -45, 0);
FuzzySet* middle = new FuzzySet(-45, -15, 15, 45);
FuzzySet* right = new FuzzySet(0, 45, 45, 90);
FuzzySet* strongright = new FuzzySet(45, 100, 100, 180);
//FuzzySet* strongleft = new FuzzySet(-45, -100, -100, -180);
FuzzySet* strongleft = new FuzzySet(-180, -100, -100, -45);

//FuzzySet* leftturn = new FuzzySet(0, -20, -1000, -1000);
FuzzySet* leftturn = new FuzzySet(-1000, -1000, -20, 0);
FuzzySet* rightturn = new FuzzySet(0, 20, 1000, 1000);
FuzzySet* neutralturn = new FuzzySet(-20, 0, 0, 20);

  FuzzySet* strongleftrud = new FuzzySet(-150, -150, -150, -150);
  FuzzySet* leftrud = new FuzzySet(-50, -50, -50, -50);
  FuzzySet* neutralrud = new FuzzySet(0, 0, 0, 0);
  FuzzySet* rightrud = new FuzzySet(50, 50, 50, 50);
  FuzzySet* strongrightrud = new FuzzySet(150, 150, 150, 150);

float mydirection = 30;
float myturn = 0;

void setup(){
  Serial.begin(9600);
  
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
FuzzyRule* fuzzyRule5 = new FuzzyRule(4, ifheadstrongleftandturnneutral, thenstrongleftrud);
  fuzzy->addFuzzyRule(fuzzyRule5);
FuzzyRule* fuzzyRule6 = new FuzzyRule(6, ifheadstrongleftandturnright, thenstrongleftrud);
 fuzzy->addFuzzyRule(fuzzyRule6);
FuzzyRule* fuzzyRule7 = new FuzzyRule(7, ifheadmiddleandturnneutral, thenneutralrud);
  fuzzy->addFuzzyRule(fuzzyRule7);
FuzzyRule* fuzzyRule8 = new FuzzyRule(8, ifheadmiddleandturnleft, thenrightrud);
  fuzzy->addFuzzyRule(fuzzyRule8);
FuzzyRule* fuzzyRule9 = new FuzzyRule(9, ifheadmiddleandturnright, thenleftrud);
  fuzzy->addFuzzyRule(fuzzyRule9);  // I found my bug
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
  fuzzy->setInput(1, mydirection); //sets direction
  fuzzy->setInput(2, myturn); //sets turnspeed
  //fuzzy->setInput(input#, value):

  fuzzy->fuzzify();
  
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
  Serial.print(rightrud->getPertinence());
  Serial.print(leftrud->getPertinence());
  Serial.print(neutralrud->getPertinence());
  Serial.print(strongrightrud->getPertinence());
  Serial.print(strongleftrud->getPertinence());


  delay(100000);
}
