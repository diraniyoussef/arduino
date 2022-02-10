#include <ESP8266WiFi.h>
//#include "ESP_PCF8574.h"  //here I'm not using Kolban's library. Also found in https://github.com/nkolban/ESPLibs/tree/master/ArduinoLibs/ESP_PCF8574
#include <Wire.h>
#define SDA_PIN D2
#define SCL_PIN D1

byte pin_address;
uint8_t pin_addr; 
uint8_t PCF_addr = 56;//0x38 is 3 x 16 + 8 = 56

boolean isPinOn( uint8_t readByte , byte pin_address) {   
  uint8_t resultWithShiftAndAND = (uint8_t) 1 & ( readByte >> pin_address );
  uint8_t byte_1 = (uint8_t) 1;
  if ( byte_1 == resultWithShiftAndAND ) {
    return true;
  } else {
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  pin_address = 3;
  pin_addr = ( (uint8_t)1<<pin_address);
  
  Serial.println("reading pins");
  Wire.begin(SDA_PIN, SCL_PIN);
  //Wire.beginTransmission(0x38);//it's 0x38 since it's PCF8574A //not 0x20  //not 0xF0
  //Wire.write(pin_addr);
  //Wire.endTransmission();
  Wire.requestFrom( PCF_addr, 1 );//1 is 1 byte for receive from PCF, and it's really just 1 byte which NodeMCU needs.
  if( Wire.available() )    
  {
    uint8_t c = Wire.read();    // receive a byte as character
    Serial.print( isPinOn( c, 0 ) );     //0 is to know the first bit    
    Serial.print( isPinOn( c, 1 ) );     //1 to know the second bit    
    Serial.print( isPinOn( c, 2 ) );         
    Serial.print( isPinOn( c, 3 ) );         
    Serial.print( isPinOn( c, 4 ) );         
    Serial.print( isPinOn( c, 5 ) );         
    Serial.print( isPinOn( c, 6 ) );         
    Serial.print( isPinOn( c, 7 ) );         
  }
  
}

void loop() {
  Serial.println("looping");  
  
  delay(2000);
  
  if(Serial.available()){
    Serial.read();  
  }
}
