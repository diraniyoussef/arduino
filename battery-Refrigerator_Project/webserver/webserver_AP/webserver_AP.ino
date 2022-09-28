//https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "myserver.h"

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

MyServer myserver;

// Replace with your network credentials
const char *ssid = "TP-LINK_2B5E";
const char *password = "87654321";

void setup(){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  Serial.println("Before Connecting");
  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());               // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer

  myserver.begin("refrigerator");                            // Actually start the server

}

void loop(){
  myserver.loop();  
}
