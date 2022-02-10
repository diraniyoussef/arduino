#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
  //char charBuffer = new char[10];
  char* charBuffer = "Hi there\0";
  //charBuffer[0] = 'H';
  
  print1( charBuffer ); //it's 8 which is correct.

  char c[20];
  c[0] = 'H';
  c[1] = 'i';
  c[2] = '\0';
  Serial.printf( "Size of buffer c is smaller than 20 and it is: %d\n", strlen(c) );
}

void print1( char* c ){
  Serial.printf( "Size of buffer is: %d\n", strlen(c) );
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
}
