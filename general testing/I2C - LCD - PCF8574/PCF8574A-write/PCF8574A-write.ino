#include <ESP8266WiFi.h>
#include <Wire.h>
#define SDA_PIN D2
#define SCL_PIN D1
byte pin_address;
uint8_t pin_addr; 

void setup() {
  Serial.begin( 115200 );
  Serial.println();
  pin_address = 3;
  pin_addr = ( (uint8_t) 1 << pin_address );
  Serial.println( "writing to fourth pin" );
  Wire.begin( SDA_PIN , SCL_PIN );
  Wire.beginTransmission(0x38); //it's 0x38 since it's PCF8574A //not 0x20  //not 0xF0
  Wire.write(pin_addr);
  Wire.endTransmission();
}

void loop() {
  Serial.println("looping");  
  
  delay(2000);
  
  if( Serial.available() ) {
    Serial.read();  
  }
}
