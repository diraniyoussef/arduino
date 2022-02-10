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

  
//  EEPROM.begin(4);//15 bytes to be allocated
//  Serial.println("after begin");
  Serial.println("before put");
  char* a = "hey!";
  Serial.println("before declaring stringOne");
  String stringOne =  String(a);
  Serial.println("before putting stringOne variable into EEPROM"); 
  int k = sizeof(stringOne);
  //EEPROM.begin(k);
  EEPROM.put(0,stringOne);
  EEPROM.commit();
  delay(1000);
  Serial.println("before get");
  String stringTwo = String("iot");
  EEPROM.get(0, stringTwo);
  Serial.printf("Read value is:");
  Serial.println(stringTwo);
  
//  eeWriteString(0, "scutectukj");
  //EEPROM.put could be better than EEPROM.write but anyway.

//  char* readit = eeGetString(0);
  //EEPROM.get could be better than EEPROM.read
//  Serial.printf("%s\n", readit);
/**/
}


void loop() {
  // put your main code here, to run repeatedly:

}

