#include <ESP8266WiFi.h>
#include <EEPROM.h>
//extern char includedPinIndexAsChar[4]; //debugging
extern char includedPinIndexAsChar[6];

WiFiServer server1(3551); //this is the server socket I guess
WiFiServer server2(3552);

#define MAX_SRV_CLIENTS 6
    
class General{
  public:    
  static int getIntFromHexSymbol(char aChar) {
    switch (aChar) {
      case '0':
        return (0);
        break;
      case '1':
        return (1);
        break;
      case '2':
        return (2);
        break;
      case '3':
        return (3);
        break;
      case '4':
        return (4);
        break;
      case '5':
        return (5);
        break;
      case '6':
        return (6);
        break;
      case '7':
        return (7);
        break;
      case '8':
        return (8);
        break;
      case '9':
        return (9);
        break;
      case 'a':
        return (10);
        break;
      case 'b':
        return (11);
        break;
      case 'c':
        return (12);
        break;
      case 'd':
        return (13);
        break;
      case 'e':
        return (14);
        break;
      case 'f':
        return (15);
        break;
      default:        
        //must not enter here
        return(-1);    
        break;
    }
  }
    
  template <typename T>
  static boolean arrayIncludeElement(T array[], int arrayLength, T element) {
  for (int i = 0; i < arrayLength; i++) {
    if (array[i] == element) {
        return true;
      }
    }
    return false;
  }

  template <typename T>
  static int getIndexOfArrayElement(T array[], int arrayLength, T element) {
    int i;
    for (i = 0; i < arrayLength; i++) {
      if (array[i] == element) {
        return i;
      }
    }
    return i; //which is the same as return arrayLength
  }
  
};

class NodeMCU{
  public:
  static void lowerAllOutPins() {    
    for (int i = 0; i < sizeof( includedPinIndexAsChar ); i++ ) {
      Serial.println("digitalWrite");
      digitalWrite( getRealPin( General::getIntFromHexSymbol( includedPinIndexAsChar[i] ) ), LOW);            
    }
  }
  
  static void wifiSetup(){

    boolean allRight = true;
    //IPAddress local_IP(192, 168, 1, 210);
    IPAddress local_IP(192, 168, 1, 211);    //This will change for each NodeMCU
    IPAddress gateway(192, 168, 1, 1);       //This is gotten from the user local network configuration
    IPAddress subnet(255, 255, 255, 0);      //This is gotten from the user local network configuration
    if ( WiFi.config(local_IP, gateway, subnet) ) {//this allocates a static IP.   //WiFi.config(ip, gateway, subnet);
      if ( WiFi.mode(WIFI_STA) ) {   //please follow this order: WiFi.config, then WiFi.mode(WIFI_STA) then WiFi.begin. https://github.com/esp8266/Arduino/issues/2371
        WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); //Nothing would happen if we executed this twice - tested.
        //WiFi.begin("WiFi-Repeater_plus", "onlyforworkpls"); //Any one of the 3 WiFis are fine...
        //WiFi.begin("WiFi-Repeater", "onlyforworkpls");
        //WiFi.begin("Electrotel_Dirani_plus", "onlyforworkpls");
        
        int i = 0;
        Serial.println("Attempting to connect to WiFi");
        while (WiFi.status() != WL_CONNECTED && i < 20) { //waiting for 10 seconds.
          i++;
          delay(500);
          Serial.print(".");
        }
        if (WiFi.status()!= WL_CONNECTED) {
          Serial.println("WiFi.status is not connected.");
          allRight = false;
        }
      } else {
        Serial.println("Couldn't make WiFi.mode a STA mode.");
        allRight = false;
      }
    } else {
      Serial.println("WiFi.config returned false.");
      allRight = false;
    }
    if (!allRight) {
      //0 - 1 - 1 for booting. And 0 - 0 - 1 for flashing. It's 15 - 0 - 2 sequence.
      delay (10);
      digitalWrite(D8, LOW);  //pin 15
      digitalWrite(D3, HIGH); //pin 0
      digitalWrite(D4, HIGH); //pin 2

      ESP.restart();//It actually works.
    }
    
    Serial.println("Ready"); //it was hanging here.                                   
    delay (50);//I added it just in case...      

    Serial.println("working before server1.");
    server1.begin();
    Serial.println("working after server1.");
    delay(1000);    
    yield();    
    server2.begin();
    Serial.println("working after server2.");
    //I want a server socket
    server1.setNoDelay(true);//I think everything you send is directly sent without the need to wait for the buffer to be full
    server2.setNoDelay(true);
    Serial.println("working after setNoDelay for both servers.");
  }

  static int determineEEPROMSize(){
    Serial.println("beginning the EEPROM");
    EEPROM.begin(2 * sizeof(includedPinIndexAsChar)); //e.g. 10T1F2T5T6F decoded like this: first byte is mobileId interpreted as a "byte" type, then 0T where 0 is char interpreted
                                                      // instead of a byte because pin 10 may one day be used which would be 'a'    
  }

  static void decodeEEPROM(){    //I believe everything in the EEPROM is of type "byte". https://www.arduino.cc/en/Reference/EEPROMWrite
    //determining the state of various pins
    char pinChar; //e.g. '0', '1', ...
    char pinState; //'T' or 'F'
    boolean boolPinState;
    for (int i = 0; i < sizeof(includedPinIndexAsChar); i++){
//      Serial.println("Clearly the problem is here");
      pinChar = (char) EEPROM.read(2*i);      
//      Serial.println("I may get to this line 1");
      pinState = (char) EEPROM.read(2*i + 1); 
      if(pinState == 'T') {
        boolPinState = true;
      } else {
        boolPinState = false;
      }
//      Serial.println("I may get to this line 2");
      if ( !General::arrayIncludeElement(includedPinIndexAsChar, sizeof(includedPinIndexAsChar), pinChar) || !(pinState == 'F' || pinState == 'T') ) {
    //    Serial.println("I may get to this line 4");
        break; //on any sign of non-validity I stop the process of updating the pins states.
      }
      //Serial.println("I may get to this line 3");

      digitalWrite( getRealPin( General::getIntFromHexSymbol( pinChar ) ), boolPinState);            
    }    
  }

  static void encodeEEPROM(char pin, boolean action) {
    char state;
    if (action) {
      state = 'T';
    } else {
      state = 'F';
    }
    int i; //i represents the pinIndexInEEPROM
    for (i = 0; i < sizeof(includedPinIndexAsChar); i++) {
      if (EEPROM.read(2*i) == pin){        
        Serial.print("found the pin in EEPROM during encoding... At position: ");
        Serial.println(i);        
        EEPROM.write(2*i + 1, state);
        EEPROM.commit();
        //once you update the EEPROM, your job is over.
        return;
      }        
    }
    if (i == sizeof(includedPinIndexAsChar)) {//usually you won't reach here unless it's the first time.
      Serial.print("allocating the pin in EEPROM during encoding... At position: ");
      i = General::getIndexOfArrayElement( includedPinIndexAsChar, sizeof(includedPinIndexAsChar), pin );
      Serial.println(i);
      EEPROM.write(2*i, pin);
      EEPROM.write(2*i + 1, state);
      EEPROM.commit();
    }   
  }
  
  static int getRealPin(int D_pin){
     switch (D_pin) {
      case 0:
        return (16);
        break;
      case 1:
        return (5);
        break;
      case 2:
        return (4);
        break;
      case 3:
        return (0);
        break;
      case 4:
        return (2);
        break;
      case 5:
        return (14);
        break;
      case 6:
        return (12);
        break;
      case 7:
        return (13);
        break;
      case 8:
        return (15);
        break;
      case 9:
        return (3);
        break;
      case 10:
        return (1);
        break;
      default:
        return (16);//this should never be used.
        // statements
        break;
     }
  }

  //this is recommended. Check this issue https://github.com/esp8266/Arduino/issues/1017
  static void yieldAndDelay(int delayTime){
    yield();
    delay(delayTime);
  }
  static void yieldAndDelay(){
    yield();
    delay(33);
  }

  static int inToOutPin(int D_pin){ //the in is a D... and the out is also a D...
     switch (D_pin) {
      case 0:
        return (3);
        break;
      case 1:
        return (4);
        break;
      case 2:
        return (6);
        break;
      case 7:
        return (9);
        break;
      case 8:
        return (10);
        break;
      default:
        return (16);//this should never be used.
        // statements
        break;
     }
  }

  
  static void checkManualUpdate(){
    if (digitalRead(D0)){ //if user presses on D3 (in the panel) then toggle the state of the corresponding output pin.
      if (digitalRead( getRealPin( inToOutPin(0) ) ) ){
        digitalWrite( getRealPin( inToOutPin(0) ), LOW );
        encodeEEPROM( getRealPin( inToOutPin(0) ), false);
      } else {
        digitalWrite( getRealPin( inToOutPin(0) ), HIGH );
        encodeEEPROM( getRealPin( inToOutPin(0) ), true);
      }
    }
    if (digitalRead(D1)){ 
      if (digitalRead( getRealPin( inToOutPin(1) ) ) ){
        digitalWrite( getRealPin( inToOutPin(1) ), LOW );
        encodeEEPROM( getRealPin( inToOutPin(1) ), false);
      } else {
        digitalWrite( getRealPin( inToOutPin(1) ), HIGH );
        encodeEEPROM( getRealPin( inToOutPin(1) ), true);
      }
    }
    if (digitalRead(D2)){ 
      if (digitalRead( getRealPin( inToOutPin(2) ) ) ){
        digitalWrite( getRealPin( inToOutPin(2) ), LOW );
        encodeEEPROM( getRealPin( inToOutPin(2) ), false);
      } else {
        digitalWrite( getRealPin( inToOutPin(2) ), HIGH );
        encodeEEPROM( getRealPin( inToOutPin(2) ), true);
      }
    }
    if (digitalRead(D7)){ 
      if (digitalRead( getRealPin( inToOutPin(7) ) ) ){
        digitalWrite( getRealPin( inToOutPin(7) ), LOW );
        encodeEEPROM( getRealPin( inToOutPin(7) ), false);
      } else {
        digitalWrite( getRealPin( inToOutPin(7) ), HIGH );
        encodeEEPROM( getRealPin( inToOutPin(7) ), true);
      }
    }
    if (digitalRead(D8)){ 
      if (digitalRead( getRealPin( inToOutPin(8) ) ) ){
        digitalWrite( getRealPin( inToOutPin(8) ), LOW );
        encodeEEPROM( getRealPin( inToOutPin(8) ), false);
      } else {
        digitalWrite( getRealPin( inToOutPin(8) ), HIGH );
        encodeEEPROM( getRealPin( inToOutPin(8) ), true);
      }
    }
  }  
};

