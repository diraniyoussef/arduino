#include <ESP8266WiFi.h>
WiFiServer server1(3552); //the idea is to let server1 be disconnected so we test another panel if this disconnection will be reflected there. 
//Answer is YES. IT WORKS.
                          // In my case, the other panel will connect through an asyncClient
WiFiClient client1;

void setup() {
  Serial.begin(115200);  
  uint8_t mac[6] {0x2C, 0x3A, 0xE8, 0x40,0x31, 0xBC}; //Actually, this is the meant MAC 2C:3A:E8:40:31:BA. ?? I guess no...
  wifi_set_macaddr(STATION_IF, mac);
  IPAddress local_IP( 192, 168, 1, 212 );  //This will change for each NodeMCU
  IPAddress gateway(192, 168, 1, 1);       //This is gotten from the user local network configuration
  IPAddress subnet(255, 255, 255, 0);      //This is gotten from the user local network configuration
  if ( WiFi.config(local_IP, gateway, subnet) ) {//this allocates a static IP.  //comment for debugging in case this NodeMCU will use the DHCP of the local network router
    if ( WiFi.mode(WIFI_STA) ) {   //please follow this order: WiFi.config, then WiFi.mode(WIFI_STA) then WiFi.begin. https://github.com/esp8266/Arduino/issues/2371                
      WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); //Nothing would happen if we executed this twice - tested.
      Serial.println("wifi connected");
    }
  }
  server1.stop();
  delay(50);
  server1.stop();  
}

int i = 0;

void loop() {
  
  Serial.println("looping");
  server1.begin();
  if( !client1 ) {
    i = 0;
    Serial.println("trying to connect");
    client1 = server1.available();
  } else {
    if( client1.connected() ) {
      Serial.println("client1 is connected");
      i++;
    }
  }
    
  if( i == 45 ) {
    Serial.println("i is now 45");
    i = 0;
    if( client1 ) {
      if( client1.connected() ) {
        Serial.println("Client has been stopped");
        client1.stop();
      }
    }
  }

  delay(1000);
}
