#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  char* buf = "Hi there\0";
  char buf1[30];
  
  strcpy( buf1, buf );
  strcat( buf1, ":" );
  
  Serial.printf("Size of buffer is: %d\n", strlen(buf1) );

  Serial.printf("Buffer is: ");
  for( int i=0; i < strlen(buf1) ; i++)  {
    Serial.printf("%c", buf1[i] );
  }


  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
}
