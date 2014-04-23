void setup() {
  // put your setup code here, to run once:
#define led 9 
pinMode(led, OUTPUT);
Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly: 
{ Serial.write("Hello, world!"); 
 delay(1000); 

} 
}
