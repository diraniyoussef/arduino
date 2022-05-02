/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

const char* ssid     = "TP-LINK_2B5E";
const char* password = "22221111";

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  //byte MAC_bytes[ 6 ];
  byte local_IP_bytes[ 4 ] = {192, 168, 0, 235};;
  byte gateway_bytes[ 4 ] = {192, 168, 0, 1};
  byte subnet_bytes[ 4 ] = {255, 255, 255, 0};;
  
  IPAddress local_IP( local_IP_bytes[0], local_IP_bytes[1], local_IP_bytes[2], local_IP_bytes[3] );  
  IPAddress gateway( gateway_bytes[0], gateway_bytes[1], gateway_bytes[2], gateway_bytes[3] );   
  IPAddress subnet( subnet_bytes[0], subnet_bytes[1], subnet_bytes[2], subnet_bytes[3] );      
  
//  wifi_set_macaddr(STATION_IF, MAC_bytes)

// Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA static IP Failed to configure");
  }
  
//  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("inside loop");
  delay(3000);  
}
