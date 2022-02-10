//#include  <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

//SoftwareSerial SIMSerial(D8, D7); // RX, TX
WiFiClient client;

void setup() {
  
  Serial1.begin(9600);
  Serial1.setDebugOutput(true);

  WiFi.disconnect();
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial1.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial1.print(".");
  }
  Serial1.printf("\nConnected, IP address: ");
  Serial1.println(WiFi.localIP());            
  
}

void loop() {
  // code to play with AT commands from the terminal
  Serial1.print("hi\n");
  delay(10000);
}



