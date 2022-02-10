#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
// https://arduino.stackexchange.com/questions/37758/esp8266-tcp-connection-wificlient-issue/37759
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html

//char* PacketMessage = "Help! Help!";
WiFiClient client; //https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClient.cpp
char* server_IP = "192.168.1.20";

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  Serial.println("\nStarting...\n");

  //WiFi.mode(WIFI_STA);
//  Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");
  WiFi.disconnect();
  //WiFi.begin("Lumia 930 3419", "123yesno");  
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());            

  client.setLocalPortStart(34660);
  const int server_port = 34550;
  while (!client.connect(server_IP, server_port)) //if client.connect was false we have to retry I guess forever.
  {
    Serial.println("Connection failed. Retrying...");
    delay(500);
  }
  Serial.println("Connection should have worked by now :)\n");
  Serial.printf("Now sending something to server\n");
  //Sending ...
  client.print("register emergency");
  
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      //Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
}

void loop()
{
    
}

