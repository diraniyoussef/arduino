#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define EEPROM_SIZE 100

void setup() {
  Serial.begin(115200);
  Serial.println();
  char charBuffer[] = {'0', '1', '2', '5', '6','\0', '7'};

  Serial.print("Size of buffer according to sizeof is: ");
  int a = sizeof(charBuffer);
  Serial.print(a);
  Serial.println();

  a = String(charBuffer).length(); //using this without having '\0' in the char buffer is very WRONG
  Serial.print("Size of buffer according to String length is: ");  
  Serial.print(a);
  Serial.println(); 

  String b = "";
  Serial.print("Size of an empty string according to String length is: ");  
  Serial.print(b.length());
  Serial.println(); 

  b = "1";
  Serial.print("Size of a one char string according to String length is: ");  
  Serial.print(b.length());
  Serial.println(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
}
