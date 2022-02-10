#include <ESP8266WiFi.h>
#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Starting...");
  WiFi.disconnect();
  
//  EEPROM.begin(100);//15 bytes to be allocated
//  eeWriteString(0, "scutectukj");
  //EEPROM.put could be better than EEPROM.write but anyway.

//  char* readit = eeGetString(0);
  //EEPROM.get could be better than EEPROM.read
//  Serial.printf("%s\n", readit);
}

char* eeGetString(int pos) {
  char* val;
  byte* p = (byte*) val;
  int i = 0;
  //int str_length = strlen(str);
  for (i = 0; i < 15; i++) {
    Serial.printf("This character is read from memory: %c\n", (char) EEPROM.read(pos + i));
    *(p + i) = EEPROM.read(pos + i);
  }  
  return val;
}


void eeWriteString(int pos, char* str) {
  byte* p = (byte*) str;
  int i = 0;
  int str_length = strlen(str);
  for (i = 0; i < str_length; i++) {
    //Serial.printf("Before writing to memory, this character is: %c\n", (char) EEPROM.read(pos + i));
    EEPROM.write(pos + i, *(p + i));
    //Serial.printf("This character is read from memory: %c\n", (char) EEPROM.read(pos + i));
  }
  EEPROM.write(pos + 14, '\0');  
  EEPROM.commit();
}


void loop() {
  // put your main code here, to run repeatedly:

}

void eeWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    EEPROM.commit();
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}

