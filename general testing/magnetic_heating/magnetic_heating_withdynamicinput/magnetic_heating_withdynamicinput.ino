#include <ESP8266WiFi.h>

//int outPin = 8;

//unsigned int charge_delay = 50;
//unsigned int discharge_delay = 50;
//int charge_delay = 50;
//int discharge_delay = 50;
uint32 charge_delay = 50;
uint32 discharge_delay = 50;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();  
  pinMode( D8 , OUTPUT);   
  Serial.println("Starting");  
}


char c = 'r';
boolean isChargingNotDischarging = false;
String s = "";

void loop() {
  // put your main code here, to run repeatedly:

  while( Serial.available() > 0 ) { //does not block
    c = Serial.read(); //does not block
    
    if( c == 'c' ) { //c for charging
      isChargingNotDischarging = true;      
    } else if( c == 'd' ) { //d for discharging
      isChargingNotDischarging = false;
    } else if( c != 'r' ) { //r for resume
      s += c;     
    } else if( c == 'r' ) {
      //now let's convert this string onto a number
      int number = s.toInt();
      
      if( isChargingNotDischarging ) {
        charge_delay = number;
        Serial.printf("New charging delay is %u\n", charge_delay );
      } else {
        discharge_delay = number;
        Serial.printf("New discharging delay is %u\n", discharge_delay );
      }
      s = "";      
    }
  }
  
  if( c == 'r' ) {
    digitalWrite( D8 , HIGH); // sets the pin on
    
    delayMicroseconds(charge_delay);      // use delay() when needed to delay in the order of seconds.
    digitalWrite(D8, LOW);  // sets the pin off
    delayMicroseconds(discharge_delay);      // pauses for 50 microseconds
    
  }
}
