#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  char buf[10] = "Hi there\0"; //fine
  char hDroid[5] = { "12" }; //compiles right
  char hDroid[5] = { "1234" }; //fine..
  



  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
}
