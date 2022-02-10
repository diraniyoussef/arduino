#include <ESP8266WiFi.h>
const String module_Id_Str = "Zaher_03884818:";

  int getFirstCharOccurenceOfModuleId(char charToCheck) {
    char* moduleId_buff = new char[ module_Id_Str.length() ];
    module_Id_Str.toCharArray(moduleId_buff, module_Id_Str.length() + 1); //there has to be + 1 to reach the last char (I guess because toCharArray automatically assigns the last char to the null char
    Serial.printf("First char is %c\n", moduleId_buff[0] );
    Serial.printf("Last char is %c \n", moduleId_buff[ module_Id_Str.length() - 1 ] );
      
    for (int i = 0 ; i < module_Id_Str.length() ; i++) {      
      if (moduleId_buff[i] == charToCheck) {          
        return (i);
        Serial.println ("A valid index has been returned");
      }
    }
    return(-1);
  }

  String getOwnerId() {
    int firstColonOfModuleId = getFirstCharOccurenceOfModuleId( ':' );
    String s = "";
    if ( firstColonOfModuleId > 0 ) {
      s = module_Id_Str.substring( 0, firstColonOfModuleId ); //..substring( 0, 1 ); will return the char at index 0 only.

    } else {
    }
    return (s);
  }
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();  
  Serial.println( getOwnerId() );
  Serial.println("It works");
}

void loop() {
  // put your main code here, to run repeatedly:

}
