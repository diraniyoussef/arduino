#include <ESP8266WiFi.h>
#include <EEPROM.h>
//#define EEPROM_size_minus_size_of_int 96

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
  
//  int k = sizeof(stringOne);
  EEPROM.begin(100);
  EEPROM.put(0, sizeof(int)); //4 is the number of characters to be put in EEPROM
  char a[96]; //96 is 100 minus sizeof(int)
  //value of a is the name the esp gets from server which might be e.g. "protection 0001"
  a[0] = 'h';
  a[1] = 'e';
  a[2] = 'y';
  a[3] = '\0';
  EEPROM.put( sizeof(int), a ); 
  EEPROM.commit();
  
//  delay(1000);
  Serial.println("before get");
  int num_bytes_in_EEPROM;
  EEPROM.get(0, num_bytes_in_EEPROM);
  //char *b = (char*)malloc(num_bytes_in_EEPROM); //dynamic pointer to hold value from EEPROM seems to be not working
  //char* b = new char[ num_bytes_in_EEPROM ];    // same here!
  //the solution:
  //char b[EEPROM_size_minus_size_of_int];
  char b[96];

  /*if (b == NULL)
  {
    Serial.println("malloc failed");
  }
  else
  {
    Serial.println("malloc succeeded");
//    free(b);
  }*/
  
  EEPROM.get( sizeof(int), b );
  Serial.printf("Read value is: ");
  Serial.println(b);
  char *bb = (char*)malloc(num_bytes_in_EEPROM);
  int i = 0;
  for (i = 0; i < num_bytes_in_EEPROM; i++){
     *(bb + i) = b[i];
  }
  Serial.println(bb);
  free(bb);
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

