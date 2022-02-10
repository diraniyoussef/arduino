#include <ESP8266WiFi.h>
class class1{ 
};
void setup() {
  Serial.begin(115200);
  Serial.println();

  class1* c = new class1();
  if (c){
    delete (c);
    Serial.println("c is deleted");
  }
  if (c != 0){
    Serial.println("c is still fine after being deleted!!!"); //it enters here unfortunately. Deleting an instance does not make it null.
    //therefor careful attention must be taken to make sure that it is deleted only once after being instantiated somewhere.
    
  } else {
    Serial.println("c is null after being deleted...");
  }

  

  class1* c1;
  if(!c1){
    Serial.println("c1 is null as expected...");
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
