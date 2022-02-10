#include <Dns.h>  //this thing won't compile until you make changes probably to the Ethernet.cpp found in 
//C:\Users\Dirani\Documents\Arduino\libraries\Ethernet\src in eclipse
#include <ESP8266WiFi.h>

extern "C" {
#include <user_interface.h> 
}



  static boolean isConnectedToWiFi() { 
    /*The caller of this method usually directly restart the NodeMCU if this method returned false.
     * In this sense we may say that this method blocks the execution in case it was not connected.
     */
    int i = 0;    
    boolean notifier_pin_set_to_high = false;
    while (WiFi.status() != WL_CONNECTED && i < 16) { //waiting for 10 seconds.
      i++;      
      if (i > 5) {
//        setConnectFailureNotifierPinHigh(); //Comment for debugging
        notifier_pin_set_to_high = true;
      }
      delay(750);
      Serial.print(".");
    }
    if( notifier_pin_set_to_high ) { //needed
//      setConnectFailureNotifierPinLow(); //Comment for debugging
    }

    if (WiFi.status()!= WL_CONNECTED) {
      Serial.println("WiFi.status is not connected.");
      return(false);
    } else {
      return(true);
    }
  }


static void connectionSetup() {        
    //the following 2 lines are to set the mac address if the original mac address was considered invalid for static IP allocation by the router.
    uint8_t mac[6] {0x2C, 0x3A, 0xE8, 0x40,0x31, 0xB9}; //Actually, this is the meant MAC 2C:3A:E8:40:31:BA. 
    //It is the second nibble that matters being 2, 6, A, or E. But is it the second nibble from last or beginning?? 
    // It looks like from last  B2:31:40:E8:3A:2C or B6-... or BA-... or BE-... 
    wifi_set_macaddr(STATION_IF, mac); 
    
    boolean allRight = true;
    IPAddress local_IP( 192, 168, 1, 218 );  //This will change for each NodeMCU
    IPAddress gateway(192, 168, 1, 1);       //This is gotten from the user local network configuration
    IPAddress subnet(255, 255, 255, 0);      //This is gotten from the user local network configuration
//    IPAddress local_IP( 192, 168, 4, 5 );  //This will change for each NodeMCU
//    IPAddress gateway(192, 168, 4, 1);       //This is gotten from the user local network configuration
//    IPAddress subnet(255, 255, 255, 0);      //This is gotten from the user local network configuration
//    if ( WiFi.config(local_IP, gateway, subnet) ) {//this allocates a static IP.  //comment for debugging in case this NodeMCU will use the DHCP of the local network router
      if ( WiFi.mode(WIFI_STA) ) {   //please follow this order: WiFi.config, then WiFi.mode(WIFI_STA) then WiFi.begin. https://github.com/esp8266/Arduino/issues/2371                
        WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); //Nothing would happen if we executed this twice - tested.
        //WiFi.begin("WiFi-Repeater_plus", "onlyforworkpls"); //Any one of the 3 WiFis are fine...
        //WiFi.begin("WiFi-Repeater", "onlyforworkpls");
        //WiFi.begin("Electrotel_Dirani_plus", "onlyforworkpls");
        //WiFi.begin("Repeater-GF", "12345678");
        //WiFi.begin("S7_AP", "12345678");
        //WiFi.begin("sub_electrotel", "onlyforworkpls");
        //WiFi.begin("sub_electrotel-Rep", "onlyforworkpls");
        //After changing the WiFi.begin input check whether to comment "if ( WiFi.config" or not.
        
        if( !isConnectedToWiFi() ) { //checks if connected
          Serial.println("Wasn't connected!");
          allRight = false;
        }
      } else {
        Serial.println("Couldn't make WiFi.mode a STA mode.");
        allRight = false;
      }
/*    
    } else {  //comment for debugging this 'else' block in case this NodeMCU will use the DHCP of the local network router
      Serial.println("WiFi.config returned false."); 
      allRight = false; 
    } 
*/    
   
    
    Serial.println("Ready"); //it was hanging here.                                              
  }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  
  //Serial.println(); 
  connectionSetup();

  DNSClient dnClient;  
  const IPAddress DNS_IP( 8, 8, 8, 8 );
  dnClient.begin( DNS_IP );

 IPAddress IPtofind;
  //if(dnClient.getHostByName("google.com", IPtofind) == 1) {
  if(dnClient.getHostByName("google.com", IPtofind) > 0) {
//    Serial.print(F("ntp = "));
    Serial.println(IPtofind);
  }
  else Serial.print(F("dns lookup failed"));
}

void loop() {
  // put your main code here, to run repeatedly:

}
