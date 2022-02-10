//#include <SPI.h>
//#include <WiFi.h>
#include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  Serial.println();

  WiFi.begin("Electrotel_Dirani", "onlyforworkpls");  //

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());                     //
}

void loop() 
{
  
}

