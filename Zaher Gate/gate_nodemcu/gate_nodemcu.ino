#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
  
void setup() {
  // Setting The Serial Port ----------------------------------------------
  Serial.begin(115200);
  Serial.println();

  // Setting Up A Wifi Access Point
  SetWifi();  
}

void loop() {
  int connectedStations = WiFi.softAPgetStationNum();
  Serial.printf("Stations connected = %d\n", connectedStations);
  const int out_pin = 0; //to control the LED we use pin 2 which turns on when set LOW and turns off when HIGH
  pinMode(out_pin, OUTPUT);
  if( connectedStations > 0 ){
    //opening the gate
    digitalWrite(out_pin, LOW);
  } else {
    //resetting the gate pin
    digitalWrite(out_pin, HIGH);
  }  
  delay(1000);
}

void SetWifi(){
  // Stop active previous WIFI
  WiFi.disconnect(); //not needed probably

  // Setting the WiFi mode
  WiFi.mode(WIFI_AP);
  Serial.println("WIFI Mode : AccessPoint");
  
  char* ssid = "zaher_house_gate";
  char* pass = "ydrs99sn"; //can be empty string 
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  //IPAddress gateway(0, 0, 0, 0); //unfortunateluy it is useful to let the mobile automatically connect to this module (even if the WiFi credentials are saved)
  IPAddress subnet(255, 255, 255, 0);

  if (WiFi.softAPConfig(local_IP, gateway, subnet)){
    Serial.print("Setting soft-AP ... ");      
    if( WiFi.softAP( ssid, pass ) ) //WiFi.softAP(ssid, password, 1 ,0, MAXSC); // WiFi.softAP(ssid, password, channel, hidden, max_connection)
    {
      Serial.println("WIFI < " + String(ssid) + " > ... Started");
      Serial.println("Ready"); //as a server, it's probably better to let the client close the connection after that he is satisfied.
      Serial.print("Soft-AP IP address = ");
      Serial.println(WiFi.softAPIP());
      //WiFi.softAPdisconnect(wifioff);
    }
    else
    {
      Serial.println("Failed! Couldn't establish an AP");
    }  
  } else {
    Serial.println("Failed! Couldn't configure an AP");
  }  
}
