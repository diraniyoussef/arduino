#include <ESP8266WiFi.h>
//this is also dimmming.

void setup() {
  // put your setup code here, to run once:
  pinMode(D3, OUTPUT);
  analogWrite(D3, 1023);
  delay(15000);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i=0; i < 1023; i++){
    analogWrite(D3, i);
    delay(20);    
  }
    
}
