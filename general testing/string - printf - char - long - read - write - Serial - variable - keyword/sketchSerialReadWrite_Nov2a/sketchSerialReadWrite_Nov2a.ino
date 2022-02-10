//#include  <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
//SoftwareSerial SIMSerial(D8, D7); // RX, TX
WiFiClient client;

void setup() {
  //SIMSerial.begin(9600);
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  WiFi.disconnect();
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  //Serial.printf("\nConnected, IP address: ");
  //Serial.println(WiFi.localIP());            
  
  
}

void loop() {
  if(Serial.available()){
    Serial.write(Serial.read());
//    Serial.println("The OK received from SIM800 and should have shown up on screen\n");
  }  
}



