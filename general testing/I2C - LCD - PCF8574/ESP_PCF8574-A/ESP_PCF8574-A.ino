#include "ESP_PCF8574.h"

// #define PCF8574_ADDRESS (0x38)
uint8_t PCF8574_ADDRESS = 0x38;
uint8_t pin_addr;
ESP_PCF8574 pcf8574A;

void setup() {
  Serial.begin(115200);
  Serial.println();
  pcf8574A.begin( PCF8574_ADDRESS, SDA_PIN, SCL_PIN );      

  pin_addr = 0;
  if ( pcf8574A.setBit(pin_addr, false) ) {
    Serial.println("Bit is written successfully. Happy Writing.");
  } else {
    Serial.println("Couldn't write the bit");
  }

  pin_addr = 1;
  if ( pcf8574A.setBit(pin_addr, true) ) {
    Serial.println("Bit is written successfully. Happy Writing.");
  } else {
    Serial.println("Couldn't write the bit");
  }

  Serial.println("Now, let's try reading pins");
  pin_addr = 4;
}

void loop() {
  Serial.println("looping");      
  boolean pin_addr_state = pcf8574A.getBit(pin_addr); //pin_addr_state is not yet accountable unless we test isDataRead()
  if ( pcf8574A.isDataRead() ) {
    Serial.print("Bit is correctly read, its state is ");
    Serial.println( pin_addr_state );
  } else {
    Serial.println("Bit was not read.");
  }
  delay(4000);    
}



