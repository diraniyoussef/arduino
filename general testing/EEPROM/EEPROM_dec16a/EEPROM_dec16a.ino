#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define EEPROM_SIZE 100

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE); //size must be between 4 and 4096
  
  EEPROM.write(0, 'b');
  EEPROM.write(1, 'a');
  EEPROM.commit();

  Serial.println();
  Serial.printf("in EEPROM: %c\n", EEPROM.read(0));
  Serial.printf("in EEPROM: %c\n", EEPROM.read(0));
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
