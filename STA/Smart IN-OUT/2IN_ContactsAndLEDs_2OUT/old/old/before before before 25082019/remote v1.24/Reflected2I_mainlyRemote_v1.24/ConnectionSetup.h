#include "Generic.h"

#define SERVERS_NUMBER_TO_MAINTAIN_CONNECTION  2

//This extern "C" is to set the MAC address
extern "C" {
#include <user_interface.h> 
}

WiFiServer server[SERVERS_NUMBER_TO_MAINTAIN_CONNECTION] = {
  WiFiServer(3552), //120 - 3552
  WiFiServer(3553)  //121 - 3553
};
  
class WIFI {

public:
  static void connectionSetup() {        
    //the following 2 lines are to set the mac address if the original mac address was considered invalid for static IP allocation by the router.
    uint8_t mac[6] {0x2C, 0x3A, 0xE8, 0x40,0x31, 0xB2}; //Actually, this is the meant MAC 2C:3A:E8:40:31:BA. ?? I guess no...
    /*It is the second nibble that matters being 2, 6, A, or E (really?). But is it the second nibble from last or beginning?? 
     * It looks like from last  B2:31:40:E8:3A:2C or B6-... or BA-... or BE-... 
     */ 
    wifi_set_macaddr(STATION_IF, mac); 
    
    boolean allRight = true;
    IPAddress local_IP( 192, 168, 1, 211 );  //This will change for each NodeMCU
    IPAddress gateway(192, 168, 1, 1);       //This is gotten from the user local network configuration
    IPAddress subnet(255, 255, 255, 0);      //This is gotten from the user local network configuration
//    IPAddress local_IP( 192, 168, 4, 5 );    //This will change for each NodeMCU
//    IPAddress gateway( 192, 168, 4, 1 );       //This is gotten from the user local network configuration
//    IPAddress subnet( 255, 255, 255, 0 );      //This is gotten from the user local network configuration
//    if ( WiFi.config(local_IP, gateway, subnet) ) {//this allocates a static IP.  //comment for debugging in case this NodeMCU will use the DHCP of the local network router
      if ( WiFi.mode(WIFI_STA) ) {   //please follow this order: WiFi.config, then WiFi.mode(WIFI_STA) then WiFi.begin. https://github.com/esp8266/Arduino/issues/2371                
        //WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); //Nothing would happen if we executed this twice - tested.
        //WiFi.begin("WiFi-Repeater_plus", "onlyforworkpls"); //Any one of the 3 WiFis are fine...
        //WiFi.begin("WiFi-Repeater", "onlyforworkpls");
        //WiFi.begin("Electrotel_Dirani_plus", "onlyforworkpls");
        //WiFi.begin("Repeater-GF", "12345678");
        WiFi.begin("S7_AP", "12345678");
        //WiFi.begin("sub_electrotel", "onlyforworkpls");
        //WiFi.begin("sub_electrotel-Rep", "onlyforworkpls");
        //After changing the WiFi.begin input check whether to comment "if ( WiFi.config" or not.
        
        if( !NodeMCU::isConnectedToWiFi() ) { //checks if connected
          Serial.println("Wasn't connected!");
          allRight = false;
        }
      } else {
        Serial.println("Couldn't make WiFi.mode an STA mode.");
        allRight = false;
      }
    /*
    } else {  //comment for debugging this 'else' block in case this NodeMCU will use the DHCP of the local network router
      Serial.println("WiFi.config returned false."); 
      allRight = false; 
    }
    */
    if ( !allRight ) { //I believe begin is the real possible reason for not being allRight.
      NodeMCU::yieldAndDelay(10);      
      NodeMCU::restartNodeMCU();
    }
    
    Serial.println("Ready"); 
  }

  static void serverSetup() {          
    for ( int i = 0; i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; i++ ) {       
      server[i].begin();    
      server[i].setNoDelay(true); //I think everything you send is directly sent without the need to wait for the buffer to be full
      NodeMCU::yieldAndDelay(1000);    
    }
  }
  
};



