#include <ESP8266WiFi.h>
#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Starting...");
  WiFi.disconnect();
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected to WiFi\n");

  EEPROM.begin(30); //maximum bytes to cover
  char b[30];
  /*
  EEPROM.get(0, b);
  Serial.printf("Read value is:");
  Serial.println(b); //It succeded
  */
  
  //EEPROM.put could be better than EEPROM.write but anyway.
  //EEPROM.get could be better than EEPROM.read

/*  char* readit = eeGetString(0);  
    Serial.printf("%s\n", readit);
*/
  for( int i = 0; i < 20 ; i++ ) {
    b[i] = (char) EEPROM.read(i);  
  }
  Serial.printf( "%s", b );
  
}


void loop() {
  // put your main code here, to run repeatedly:

}

