#include <ESP8266WiFi.h>
extern "C" {
#include <user_interface.h>
}

//uint8_t mac[6];                     // Use this line if you haven't used uint8_t mac[6] {...}; and wifi_set_macaddr(...); lines. It's either this or those.


void setup()
{
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  Serial.println();
  
  WiFi.mode(WIFI_STA);
  
//ATTENTION ATTENTION    //use these 2 lines if you haven't used the uint8_t mac[6]; line. It's either these or that.
  uint8_t mac[6] {0x2C, 0x3A, 0xE8, 0x40, 0x31, 0xBA}; 
  wifi_set_macaddr(STATION_IF, mac); 
//ATTENTION ATTENTION 
  
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls");  
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());                     
  
  WiFi.macAddress(mac); 
  Serial.print("MAC: ");
  Serial.print(mac[0],HEX); 
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[5],HEX);
  Serial.println();

}
void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  //Serial.println("Alright!");
}
