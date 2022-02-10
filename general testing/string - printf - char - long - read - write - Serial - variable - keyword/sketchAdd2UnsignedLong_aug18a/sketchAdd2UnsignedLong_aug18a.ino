#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

unsigned long valMax = 4294967295;
//unsigned long val1 = 4294967290;
//unsigned long val2 = 2;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.disconnect();
//I tried simply adding. It worked but in general it made the esp8266 to reset with a stack error. Perhaps it's an overflow error.
  Serial.printf("\n %lu", add2UnsignedLong(4294967290,8));  
}

unsigned long add2UnsignedLong(unsigned long val1, unsigned long val2){
  unsigned long val3;
  if (val1 < val2){ //this isn't necessary but ok.
    val3 = val1;
    val1 = val2;
    val2 = val3;
  }
  val3 = val1;
  while(val3 < valMax && val2 > 0){
    val3 = val3 + 1;
    val2 = val2 - 1;  
    Serial.printf("\n new values are %lu and %lu", val3, val2);     
  }
  if ( val3 == valMax)
    val3 = val2;
  return val3;
}

void loop() {
  // put your main code here, to run repeatedly:

}
