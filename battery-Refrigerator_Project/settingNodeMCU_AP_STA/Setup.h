#include "Generic.h"

extern const int delay_per_loop;
const byte After_Press_Start_APmode_Timer = 5; /*in seconds*/

/*The following few variables should'd been private inside the class*/
const byte Start_AP_Index_In_EEPROM = 101; 
const byte SSID_Buff_Size = 32; 
const byte Password_Buff_Size = 64;
const byte Max_IP_Buff_Size = 4;
const byte MAC_Buff_Size = 12;
WiFiServer AP_server(3551); /*interesting why this cannot be a private member*/
/* In the EEPROM or in the received buffer from the user, the info are like one of these: 
 * 6 -> Electrotel_Dirani -> trailor -> onlyforworkpls -> trailor -> header of static IP -> static IP -> trailor -> 
 *    header of gateway IP -> gateway IP -> trailor -> header of subnet -> subnet -> trailor -> mac -> trailor
 * 5 -> Electrotel_Dirani -> trailor -> header of static IP -> static IP -> trailor -> 
 *    header of gateway IP -> gateway IP -> trailor -> header of subnet -> subnet -> trailor -> mac -> trailor
 * 2 -> Electrotel_Dirani -> trailor -> onlyforworkpls -> trailor
 * 1 -> Electrotel_Dirani -> trailor
 * At maximum the size of the whole buffer is 1 + 32 + 1 + 64 + 2 + 4 + 2 + 4 + 2 + 4 + 1 + 12 + 1 = 130 bytes.
 */
const byte Max_AP_Buffer_Size = 1 + SSID_Buff_Size + 1 + Password_Buff_Size + 2 + Max_IP_Buff_Size + 2 + Max_IP_Buff_Size + 2 + Max_IP_Buff_Size + 1 +
                              MAC_Buff_Size + 1; /*If more than a 255 (because in many places I used 'byte' and not 'int' to refer to the buffer) 
                                                  * which is the byte size then something must be changed in the code below!*/
                              /*should'd been public inside the class*/

const int max_EEPROM = Start_AP_Index_In_EEPROM + Max_AP_Buffer_Size;

class AP_Op {
private:  
  byte buffer_info[ Max_AP_Buffer_Size ];
  /*The following are 6 dedicated varables used to connect to wifi's router*/
  byte determinant; //either 1, 2, 5, or 6. If '\n' then no network configuration is considered in EEPROM.
  char SSID_buff[ SSID_Buff_Size ];  
  char password_buff[ Password_Buff_Size ];
  byte local_IP_bytes[ Max_IP_Buff_Size ];
  byte gateway_bytes[ Max_IP_Buff_Size ];
  byte subnet_bytes[ Max_IP_Buff_Size ];
  byte MAC_bytes[ 6 ]; /*these are fed to wifi_set_macaddr. The value will be set from the MAC chars in the buffer.*/
  
  WiFiClient AP_client;

	byte max_counter_to_start_APmode;
  byte counter_to_start_APmode;

	boolean connectToRouterFromEEPROM_OrRestart() {
    /*Here we fetch the network configuration from EEPROM if existing, then we assign them to the corresponding private variables
     * through a special method checkBuffAndAssignConfigVariables that reads well the determinant (the first byte).
     * Next we connect.
     */
    determinant = (byte) EEPROM.read( Start_AP_Index_In_EEPROM );
    Serial.printf("connectToRouterFromEEPROM_OrRestart()    just after reading first char from EEPROM which is %d\n", determinant);
    
    

    //Now we're ready to connect
    connectToRouterOrRestart();
    return true;
  }

public:
  boolean must_APmode_be_activated = false; 
  boolean is_APmode_button_pressed = false;

  void check_APmode_pin() {/*only accessed when not in AP mode*/
    if( !digitalRead(0) ) { /*user is asking to enter AP mode, e.g. he's pressing a special button for that*/
      is_APmode_button_pressed = true;
      counter_to_start_APmode--;
      if( counter_to_start_APmode == 0 ) {
        //counter_to_start_APmode = max_counter_to_start_APmode; //check note below
        //is_APmode_button_pressed = false;        //check note below
        /*It's not wrong to set counter_to_start_APmode and is_APmode_button_pressed here, 
         * but I prefer not to put it here because I may enter AP mode from another place
         * so I prefer to put it in a sort of final one place.
         */
        must_APmode_be_activated = true;/*this line is the whole point of this method*/
      }
    } else if( counter_to_start_APmode != max_counter_to_start_APmode ) {/*the long pressing has been cancelled.*/
      counter_to_start_APmode = max_counter_to_start_APmode; //necessary here
      must_APmode_be_activated = false;
      is_APmode_button_pressed = false;
    }    
  }  

	void launch() {
    max_counter_to_start_APmode = floor( After_Press_Start_APmode_Timer * 1000 / delay_per_loop );    
    counter_to_start_APmode = max_counter_to_start_APmode;
    /*Now the process of trying to connect to router or entering AP mode, etc.*/
    check_APmode_pin(); 
    if( !is_APmode_button_pressed ) { /*This check is necessary here; the user wants to change network configuration right after 
                                       * NodeMCU startup without falling into a restart (i.e. not being able to change network config)*/
      /*Aside note: in a weird case, it is possible that this block will be bypassed by a user pressing and then the user 
       * cancels his intention to set network configurations, so what happens then?
       * It won't be able to connect to router since the connection had never yet begun
       * (regardless whether the network configuration is good or bad) 
       * and so it will restart.
       */ 
      if( !connectToRouterFromEEPROM_OrRestart() ) { /*If the info in the EEPROM are correct in format but the NodeMCU isn't able to connect 
                                                      * then NodeMCU will restart.*/
        /*This metod returns true in case of successful connection to router*/
        /*This method returns false in case the info in the network configuration in the EEPROM was not valid in format*/
        must_APmode_be_activated = true; /*This is a strict lock to never do anything unless the network configuration are set with a good format*/
      }
    }
  }

	void runAsAP() {
    /*will restart if failed to run as AP, but normally should never fail*/
    IPAddress local_IP(172,17,15,30); /*this follows the list of private IP addresses from 172.16.0.0 to 172.31.255.255
            * where subnet fixes the first 12 bits (10101100 for the first byte, and 0001xxxx for the second byte), so subnet has to be
            * 255.240.0.0 according to https://en.wikipedia.org/wiki/Private_network */
    IPAddress gateway(172,18,19,200); //let the gateway be the same as the local_IP, but maybe it works if 192.168.4.251 was the same gateway for all modules.
    IPAddress subnet(255,240,0,0);
    if( WiFi.softAPConfig(local_IP, gateway, subnet) ) {
      Serial.println("runAsAP()   Setting soft-AP ... ");
      if( WiFi.softAP( "SetPanelNetConfig", "", 1, false, 1 ) ) {/*no password, channel is 1 (as the default value), SSID is not hidden, 
        * and the last 1 is the max_connection allowed to connect, which is what I really want*/
      //if( WiFi.softAP( "SetPanelNetConfig" ) ) {/*no password*/
        Serial.println("runAsAP()      Soft-AP SSID broadcast success...");
        delay (50);//I added it just in case...      
        AP_server.begin();
        //I want a server socket
        AP_server.setNoDelay(true);//I think everything you send is directly sent without the need to wait for the buffer to be full
        return;
      }
    }
    /*normally should not restart.*/
    Serial.println("runAsAP()     Couldn't run as AP probably!!! So restarting.");
    NodeMCU::restartNodeMCU();
  }

	void connectToRouterOrRestart() { //this is ("only" - I believe) called on success of reading the buffer containing the network configuration.
    /*The following 2 lines are to set the mac address if the original mac address was considered invalid for static IP allocation by the router.
     * uint8_t mac[6] {0x2C, 0x3A, 0xE8, 0x40,0x31, 0xB6}; //Actually, this is the meant MAC 2C:3A:E8:40:31:BA. ??
     * It is the second nibble that matters being 2, 6, A, or E (really?). But is it the second nibble from last or beginning?? 
     * It looks like from last  B2:31:40:E8:3A:2C or B6-... or BA-... or BE-... 
     */    
    /* 
    if( determinant == trailor ) {
      //do nothing as it will reastart at the end
    }
    */
    if ( WiFi.mode(WIFI_STA) ) {   //please follow this order: WiFi.config, then WiFi.mode(WIFI_STA) then WiFi.begin. https://github.com/esp8266/Arduino/issues/2371      
      if( determinant == 1 ) {
        WiFi.begin( SSID_buff );
      }
      if( determinant == 2 ) {
        WiFi.begin( SSID_buff, password_buff );
      }
      if( determinant == 5 || determinant == 6 ) {
        if( wifi_set_macaddr(STATION_IF, MAC_bytes) ) {
          IPAddress local_IP( local_IP_bytes[0], local_IP_bytes[1], local_IP_bytes[2], local_IP_bytes[3] );  //This will change for each NodeMCU
          IPAddress gateway( gateway_bytes[0], gateway_bytes[1], gateway_bytes[2], gateway_bytes[3] );       //This is gotten from the user local network configuration
          IPAddress subnet( subnet_bytes[0], subnet_bytes[1], subnet_bytes[2], subnet_bytes[3] );      //This is gotten from the user local network configuration
          if ( WiFi.config(local_IP, gateway, gateway, subnet) ) {//this allocates a static IP.  //comment for debugging in case this NodeMCU will use the DHCP of the local network router     
            if( determinant == 5 ) {
              WiFi.begin( SSID_buff );
            } else if( determinant == 6 ) {
              WiFi.begin( SSID_buff, password_buff ); //Nothing would happen if we executed this twice - tested.
            }
          } else {
            Serial.println("connectToRouterOrRestart() error when configuring the 3 IPs");          
          }          
        } else {//mac value wasn't set for some reason
          Serial.println("connectToRouterOrRestart() mac value wasn't set");          
        }
      }
    } else {
      Serial.println("Couldn't make WiFi.mode an STA mode.");  
    }
    if( NodeMCU::isConnectedToWiFi( true ) ) { //checks if connected. This actually waits some time before returning false.
      Serial.println("connected to Router..."); 
      return;
    }
    Serial.println("Restarting as not being connected..."); 
    NodeMCU::restartNodeMCU();
  }

};