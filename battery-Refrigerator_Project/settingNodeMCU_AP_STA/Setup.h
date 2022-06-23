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
const char trailor = 127; //should'd been public inside the class. 

class AP_Op {
private:  
  byte buffer_info[ Max_AP_Buffer_Size ];
  /*The following are 6 dedicated varables used to connect to wifi's router*/
  byte determinant; //either 1, 2, 5, or 6. If it was the trailor then no network configuration is considered in EEPROM.
  char SSID_buff[ SSID_Buff_Size ];  
  char password_buff[ Password_Buff_Size ];
  byte local_IP_bytes[ Max_IP_Buff_Size ];
  byte gateway_bytes[ Max_IP_Buff_Size ];
  byte subnet_bytes[ Max_IP_Buff_Size ];
  byte MAC_bytes[ 6 ]; /*these are fed to wifi_set_macaddr. The value will be set from the MAC chars in the buffer.*/
  
  WiFiClient AP_client;

	byte max_counter_to_start_APmode;
  byte counter_to_start_APmode;

  void resetEEPROM() {
    //This will close all opened sockets because all operations 
    Serial.println("Resetting the EEPROM");
    EEPROM.write(Start_AP_Index_In_EEPROM, trailor);
    EEPROM.commit();
  }

	boolean fromEEPROM_connectToRouterOrNotify() {
    /*Here we fetch the network configuration from EEPROM if existing, then we assign them to the corresponding private variables
     * through a special method checkBuffAndAssignConfigVariables that reads well the determinant (the first byte).
     * Next we connect.
     */
    determinant = (byte) EEPROM.read( Start_AP_Index_In_EEPROM );
    Serial.printf("fromEEPROM_connectToRouterOrNotify()    just after reading first char from EEPROM which is %d\n", determinant);
    
    

    //Now we're ready to connect
    connectToRouterOrNotify();
    return true;
  }

  boolean checkBuffAndAssignConfigVariables() {
    /*This analyzes buffer_info assuming that it has all the needed stuff ALREADY.      
     * This method is used either for the buffer gotten from the EEPROM or the one gotten from the user.      
     */
    // checking the validity of the determinant
    determinant = buffer_info[0];



    return true;
  }

  boolean getIncomingMessage() {
    Serial.printf("In AP mode, getIncomingMessage()\n");
    int connectedStationsNumber = WiFi.softAPgetStationNum();
    Serial.printf("In AP mode, connectedStationsNumber is %d\n", connectedStationsNumber);
    if (connectedStationsNumber > 0){
      //if( AP_client ) {//this should never be tested here because when it is not connected it returns false, so 
      //Serial.printf("In AP mode, AP_client is assumed as not null\n");
      if( AP_client.connected() ) {
        Serial.printf("In AP mode, AP_client is connected\n");
        if( AP_client.available() ) {
          Serial.printf("In AP mode, an AP_client is available\n");
          int readCharNumber = AP_client.readBytesUntil('\0', (char*) buffer_info, Max_AP_Buffer_Size); //BTW the '\0' is not counted in the value of readCharNumber - tested
          if( readCharNumber != 0 ) {
            Serial.printf("In AP mode, readCharNumber is not 0, so buffer_info should be gotten by now\n");
            if( readCharNumber < Max_AP_Buffer_Size ) {
              buffer_info[ readCharNumber ] = '\0';
              Serial.printf("In AP mode, getIncomingMessage()   Read buff is %s\n", buffer_info);
            }
            Serial.printf("In AP mode, getIncomingMessage()   stopping AP_client\n");
            AP_client.stop();
            return true;
          } 
          Serial.printf("In AP mode, getIncomingMessage()   weird case! Anyway, stopping AP_client\n");
          AP_client.stop();          
        }
      } else {
        AP_client = AP_server.available();
      }                 
      //}
    }
    return false;    
  }

  void updateFirstAndLastIndices( int* first_index, int* last_index ) { //there has to be max_search_interval argument to be passed
    *first_index = (*last_index) + 1; /*the index of the first useful char*/
    *last_index = *first_index;
    while( buffer_info[ *last_index ] != trailor && (*last_index) < Max_AP_Buffer_Size ) { /*we're sure we'll find a trailor*/
      //Serial.printf("updateFirstAndLastIndices   buffer_info char is : %c\n", buffer_info[ *last_index ] );
      (*last_index)++;
    }
  }

  boolean checkIP_HeaderThenCopy( int first_index, int last_index, byte* destination_buff ) {
    /*last_index - 1 is the index of the last useful char (so last_index is the index of the following trailor)
    * and first_index is the index of the first useful char, which is the header*/
    /*Max_IP_Buff_Size is 4 of course*/
    /*let's analyze the header*/
    byte header = buffer_info[ first_index ];
    for( int i = Max_IP_Buff_Size - 1 ; i >= 0 ; i-- ) {
      //Serial.printf("checkIP_HeaderThenCopy   header is %d\n", header ); 
      if( header & 1 != 0 ) {
        destination_buff[ i ] = 0;
      } else {
        last_index--;
        if( last_index == first_index ) {
          //this means that the IP bytes are less than the indicated number by the header       
          Serial.println("checkIP_HeaderThenCopy   position 1");   
          return false;
        }        
        destination_buff[ i ] = buffer_info[ last_index ];
        //Serial.printf("checkIP_HeaderThenCopy   'i' is %d and destination_buff[ i ] is %d\n", i , destination_buff[ i ] ); 
      }
      header = header >> 1; //this right shifts the header and places a 0 as the new MSbit.      
    }
    if( last_index != first_index + 1 ) {
      //this means that the IP bytes are more than the indicated number by the header          
      Serial.println("checkIP_HeaderThenCopy   position 2");   
      return false;
    }
    /*There is really no need for the following statements but I made it in order to make the code more exact (not loose)*/
    boolean all_bytes_are_non0 = true;
    for( int i = 0 ; i < Max_IP_Buff_Size ; i++ ) {
      if( destination_buff[ i ] == 0 ) {
        //Serial.println("checkIP_HeaderThenCopy   position 3");   
        all_bytes_are_non0 = false;
        break;
      }
    }
    if( header == 1 ) { //this means that it was 16 in the beginning, but after shifting 4 bits, it's now 1
      if( !all_bytes_are_non0 ) {
        Serial.println("checkIP_HeaderThenCopy   position 4");   
        return false;
      }
    } else if( header == 0 ) {//all of the 4 bytes must be different than 0 
      if( all_bytes_are_non0 ) {
        Serial.println("checkIP_HeaderThenCopy   position 5");   
        return false;
      }
    } else {
      Serial.println("checkIP_HeaderThenCopy   position 6");   
      return false; //no other value is allowed.
    }
    Serial.printf("checkIP_HeaderThenCopy   the IP bytes are : %d.%d.%d.%d\n", 
              destination_buff[0], destination_buff[1], destination_buff[2], destination_buff[3] );
    return true;
  }

  boolean checkMAC_BuffSizeThenCopy( int first_index, int last_index ) {
    int length_to_copy = last_index - first_index; /*last_index - 1 is the index of the last useful char 
    * and first_index - 1 is the length of the char which precedes the first useful char*/
    if( length_to_copy != MAC_Buff_Size ) {
      Serial.println("checkMAC_BuffSizeThenCopy()    size of MAC is not right");
      return false;
    }
    int byte_from_2_hex_chars;
    for( int i = 0 ; i < MAC_Buff_Size ; i++ ) {
      byte_from_2_hex_chars = General::getByteFromTwoHexChars( buffer_info[ first_index + i ] , buffer_info[ first_index + i + 1] ); /*buffer_info[ first_index + i ] is the higher nibble and buffer_info[ first_index + i + 1] is the lower nibble*/      
      if( byte_from_2_hex_chars == -1 ) {
        Serial.printf("checkMAC_BuffSizeThenCopy()    the %dth or the %dth char of the MAC chunk is not acceptable, they were %c and %c\n", i, i + 1,
                        buffer_info[ first_index + i ], buffer_info[ first_index + i + 1 ] );
        return false;
      }      
      MAC_bytes[ i / 2 ] = byte_from_2_hex_chars;
      i++;
    }
    return true;    
  }

  boolean acceptChar( byte buff_index, byte trailor_counter ) {
    return( ( buff_index < Max_AP_Buffer_Size ) && ( trailor_counter < determinant ) && ( buffer_info[ buff_index] != '\0' ) ); 
    /*there won't be a char gotten from the user
    * which is '\0' since this char would had truncated the incoming buffer (message)*/
  }

  void stopAll_AP_Operations() {
    if( AP_client ) {
      if( AP_client.connected() ) {
        AP_client.stop();
      }
    }    
    delay(50);
    AP_server.stop();
    delay(100);
    WiFi.softAPdisconnect (true);
  }

  void writeNetworkConfigVariablesToEEPROM() {
    /* The buffer is assumed to be well set, so we only have to write it to EEPROM.
     * I account on my specific format to know how many chars to write. I didn't account on something like ending with a null char.
     */
    byte i = 0;
    byte trailor_counter = 0;
    //Serial.println("writeNetworkConfigVariablesToEEPROM     about to write in EEPROM");
    while( acceptChar( i , trailor_counter ) ) { /*we start by writing the determinant, then the SSID, etc...*/
      EEPROM.write( Start_AP_Index_In_EEPROM + i , buffer_info[ i ] );      
      //Serial.printf("writing.");
      if( buffer_info[ i ] == trailor ) {
        trailor_counter++;
      }
      i++;
    }    
    EEPROM.commit();
    //Serial.println("");
    //Serial.println("done writing.");
  }

public:
  boolean must_APmode_be_activated = false; 
  boolean is_APmode_button_pressed = false;

  void check_APmode_pin() {/*this method is not called in AP mode*/
    if( NodeMCU::getInPinStateAsConsidered( AP_setup_pin ) ) { /*user is asking to enter AP mode, e.g. he's pressing a special button for that*/
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

	void launch() { //this method is called everytime the NodeMCU starts
    max_counter_to_start_APmode = floor( After_Press_Start_APmode_Timer * 1000 / delay_per_loop );
    counter_to_start_APmode = max_counter_to_start_APmode;
    /*Now the process of trying to connect to router or entering AP mode, etc.*/
    check_APmode_pin(); 
    if( !is_APmode_button_pressed ) { /*This check is necessary here; the user wants to change network configuration right after NodeMCU startup without falling into a restart (i.e. not being able to change network config)*/
      /*Aside note : in a weird case, it is possible that this block will be bypassed by a user pressing and then the user cancels his intention to set network configurations, so what happens then?
       * It won't be able to connect to router since the connection had never yet began (regardless whether the network configuration is good or bad) 
       * and so it will restart.
       */ 
      if( !fromEEPROM_connectToRouterOrNotify() ) { /*This metod returns true in case of successful connection to router. It returns false in case the info in the network configuration in the EEPROM was not valid in format*/
        /*If the info in the EEPROM are correct in format but the NodeMCU isn't able to connect then NodeMCU will restart.*/        
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

	void connectToRouterOrNotify() { //this is ("only" - I believe) called on success of reading the buffer containing the network configuration.
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
    //Serial.println("Restarting as not being connected..."); 
    //NodeMCU::restartNodeMCU();
  }

  void firstTimeAP_Setup() {
    //setting first-time-things to AP mode
    Serial.println("firstTimeAP_Setup()");      
    resetEEPROM();
    NodeMCU::setNotifierPinLow();
    //DONE setting first-time-things to AP mode
    runAsAP();     
  }

  boolean checkIncomingMessageAndReturnToNormalOperation() {
    //getting the message (and putting it inside buffer_info char buffer
    if( !getIncomingMessage() ) {
      return false;
    }
    
    if( !checkBuffAndAssignConfigVariables() ) {
      return false;
    }
    /*when we reach here, then we've got successful info from user so we want to return all back to normal*/
    stopAll_AP_Operations(); /*like closing opened socket with user*/
    writeNetworkConfigVariablesToEEPROM(); /*same for this method*/
    connectToRouterOrNotify(); /*this method is only called when the user network configuration variables are all set (thus has been accepted)*/    
    //now it is connected to router, so setting first-time-things back to normal operation
    NodeMCU::setNotifierPinHigh();
    counter_to_start_APmode = max_counter_to_start_APmode;
    is_APmode_button_pressed = false;
    must_APmode_be_activated = false;
    return true;    
  }  


};