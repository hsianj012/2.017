int curHead = 0.0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly: 
  curHead = curHead+1;
  Serial.println(curHead);
}
