#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define EEPROM_SIZE 12

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE); //size must be between 4 and 4096 
  Serial.println();
  
  for (int i = 0; i < EEPROM_SIZE; i++)
    Serial.printf("in EEPROM: %c\n", EEPROM.read(i));
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
