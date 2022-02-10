#include <ESP8266WiFi.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  
  Serial.println("Flash size is");
  Serial.println( ESP.getFlashChipSize() );
}

void loop() {
  // put your main code here, to run repeatedly:

}
