#include <ESP8266WiFi.h>
extern "C" {
#include <user_interface.h>
}

  IPAddress local_IP( 192, 168, 1, 210 );  //This will change for each NodeMCU
  IPAddress gateway(192, 168, 1, 1);       //This is gotten from the user local network configuration
  IPAddress subnet(255, 255, 255, 0);      //This is gotten from the user local network configuration
   
//uint8_t mac[6];                     // Use this line if you haven't used uint8_t mac[6] {...}; and wifi_set_macaddr(...); lines. It's either this or those.


void setup()
{ 
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  Serial.println();

//ATTENTION ATTENTION
  uint8_t mac[6] {0x2C, 0x3A, 0xE8, 0x40, 0x31, 0xBA}; //BA:31:40:E8:3A:2C    use these 2 lines if you haven't used the byte mac[6]; line. It's either these or that.
  wifi_set_macaddr(STATION_IF, mac);
//ATTENTION ATTENTION enter the MAC in reverse order per bytes (but each byte doesn't change) 

  //The order of these 3 lines is important. Please don't put any additional line in between
  WiFi.config(local_IP, gateway, subnet, gateway); //this allocates a static IP.  
  WiFi.mode(WIFI_STA);
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
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);

}
void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  Serial.println("Alright!");
}
