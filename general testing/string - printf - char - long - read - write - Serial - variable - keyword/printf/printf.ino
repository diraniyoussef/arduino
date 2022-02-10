#include <ESP8266WiFi.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();  
  int i = 5;
  String s = "yes!";
  //std::basic_string<char> s = "yes!";
  //std::string s = "yes!!";
  Serial.printf("The number is: %d and the string is: %s\n", i, s.c_str());
  Serial.println("it really worked!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
