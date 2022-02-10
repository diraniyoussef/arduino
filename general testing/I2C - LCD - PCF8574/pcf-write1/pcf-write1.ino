#include "ESP_PCF8574.h"
#include <ESP8266WiFi.h>

//D1 and D2 are naturally SCL and SDA
//D7 is reserved as connectFailureNotifierPin

uint8_t address_PCF = 0x38;

void setup()
{  
  Serial.begin(115200);  
  //Serial.println();  
  ESP_PCF8574 pcf;
  pcf.begin( address_PCF , D2 , D1 );

  pcf.setBit( 0 , false );
  pcf.setBit( 1 , false );
  pcf.setBit( 2 , false );
  pcf.setBit( 3 , false );
  pcf.setBit( 4 , false );
  pcf.setBit( 5 , false );
  pcf.setBit( 6 , true );
  pcf.setBit( 7 , true );
  
  Serial.println( "the end" );
}

void loop()
{

}



