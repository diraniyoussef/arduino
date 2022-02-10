#include <ESP8266WiFi.h>
#include <EEPROM.h>

extern char includedPinIndexAsChar[5];
//extern char includedPinIndexAsChar[4]; //debugging

#define MAX_SRV_CLIENTS 6
#define SERVERS_NUMBER_TO_MAINTAIN_CONNECTION  2

WiFiServer server[SERVERS_NUMBER_TO_MAINTAIN_CONNECTION] = {
  WiFiServer(3551),
  WiFiServer(3552)
};

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
/*
  template <typename T>
  static int getIndexOfArrayElement(T array[], int arrayLength, T element) {
    int i;
    for (i = 0; i < arrayLength; i++) {
      if (array[i] == element) {
        return i;
      }
    }
    return i; //which is the same as return arrayLength
  } */ 
};

class NodeMCU{  
private:
  const static int connectFailureNotifierPin = 10;  
  static void setNotifierPinLow() {
    digitalWrite( getRealPin( connectFailureNotifierPin ), LOW );
  }  

  static void toggle_D_pin_State(int D_pin) {
    if (digitalRead( getRealPin( D_pin ) ) ){
      digitalWrite( getRealPin( D_pin ), LOW );
    } else {
      digitalWrite( getRealPin( D_pin ), HIGH );
    }
  }
  
  static void toggle_D_pin_State_AndWriteEEPROM(int D_pin) {
    if (digitalRead( getRealPin( D_pin ) ) ){
      digitalWrite( getRealPin( D_pin ), LOW );
      encodeEEPROM( getRealPin( D_pin ), false);
    } else {
      digitalWrite( getRealPin( D_pin ), HIGH );
      encodeEEPROM( getRealPin( D_pin ), true);
    }
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
        return (5);
        break;
      case 7:
        return (6);
        break;
      case 8:
        return (9);
        break;
      default:
        return (16);//this should never be used.
        // statements
        break;
     }
  }
    
public:
  static int beginEEPROM(){
    Serial.println("beginning the EEPROM");
    EEPROM.begin(2 * sizeof(includedPinIndexAsChar)); //e.g. 10T1F2T5T6F decoded like this: first byte is moduleId interpreted as a "byte" type, then 0T where 0 is char interpreted
                                                      // instead of a byte because pin 10 may one day be used which would be 'a'    
  }
  
  static void lowerAllOutPins() {    
    for (int i = 0; i < sizeof( includedPinIndexAsChar ); i++ ) {
      Serial.println("digitalWrite");
      digitalWrite( getRealPin( General::getIntFromHexSymbol( includedPinIndexAsChar[i] ) ), LOW);            
    }
  }
  
  static boolean isConnectedToWiFi() {
    int i = 0;    
    while (WiFi.status() != WL_CONNECTED && i < 16) { //waiting for 10 seconds.
      i++;      
      if (i > 5) {
        toggle_D_pin_State(connectFailureNotifierPin);
      }
      delay(750);
      Serial.print(".");
    }
    setNotifierPinLow(); //I have to set it to low because even if the NodeMCU were to connect to WiFi, it will still enter the while loop 
                          // while (WiFi.status() != WL_CONNECTED && i < 14)
                          // maybe for one time until it really connects.

    if (WiFi.status()!= WL_CONNECTED) {
      Serial.println("WiFi.status is not connected.");
      return(false);
    } else {
      return(true);
    }
  }
  
  static void restartNodeMCU() {
    //0 - 1 - 1 for booting. And 0 - 0 - 1 for flashing. It's 15 - 0 - 2 sequence.    
    digitalWrite(D8, LOW);  //pin 15
    digitalWrite(D3, HIGH); //pin 0
    digitalWrite(D4, HIGH); //pin 2
   
    ESP.restart();//It actually works.
  }
  
  static void wifiSetup(){
    boolean allRight = true;
    IPAddress local_IP(192, 168, 1, 210);
    //IPAddress local_IP(192, 168, 0, 201);    //This will change for each NodeMCU
    IPAddress gateway(192, 168, 1, 1);       //This is gotten from the user local network configuration
    IPAddress subnet(255, 255, 255, 0);      //This is gotten from the user local network configuration
    if ( WiFi.config(local_IP, gateway, subnet) ) {//this allocates a static IP.   //WiFi.config(ip, gateway, subnet);
      if ( WiFi.mode(WIFI_STA) ) {   //please follow this order: WiFi.config, then WiFi.mode(WIFI_STA) then WiFi.begin. https://github.com/esp8266/Arduino/issues/2371
        WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); //Nothing would happen if we executed this twice - tested.
        //WiFi.begin("WiFi-Repeater_plus", "onlyforworkpls"); //Any one of the 3 WiFis are fine...
        //WiFi.begin("WiFi-Repeater", "onlyforworkpls");
        //WiFi.begin("Electrotel_Dirani_plus", "onlyforworkpls");
        //WiFi.begin("Repeater-GF", "12345678");
        if( !isConnectedToWiFi() ) { //checks if connected
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
    if (!allRight) { //I believe begin is the real possible reason for not being allRight.
      yieldAndDelay(10);
      
      restartNodeMCU();
    }
    
    Serial.println("Ready"); //it was hanging here.                                   
    yieldAndDelay(50);//I added it just in case...      

    for (int i = 0; i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; i++) { 
      yieldAndDelay(1000);
      server[i].begin();    
      server[i].setNoDelay(true);//I think everything you send is directly sent without the need to wait for the buffer to be full
    }
    
    Serial.println("working after setNoDelay for both servers.");
  }  

  static void decodeEEPROM(){    //I believe everything in the EEPROM is of type "byte". https://www.arduino.cc/en/Reference/EEPROMWrite        
    //determining the state of various pins
    char pinChar; //e.g. '0', '1', ...
    char pinState; //'T' or 'F'
    boolean boolPinState;
    for (int i = 0; i < sizeof(includedPinIndexAsChar); i++) {
      pinChar = (char) EEPROM.read(2*i);      
      pinState = (char) EEPROM.read(2*i + 1); 
      if(pinState == 'T') {
        boolPinState = true;
      } else {
        boolPinState = false;
      }
      if ( !General::arrayIncludeElement( includedPinIndexAsChar, sizeof(includedPinIndexAsChar), pinChar ) || !( pinState == 'F' || pinState == 'T' ) ) {
        //Whenever what I read in the EEEPROM is not valid, I make sure to free up to the REST of the EEPROM and then leave.
        Serial.printf("During EEPROM decoding, inconsistent reading from EEPROM happened so I will write '-' into EEPROM from place %d to the end\n", 2*i);
        for (int j = 2 * i; j < 2 * sizeof(includedPinIndexAsChar); j++) {
          EEPROM.write(j, '-'); //I free the EEPROM by putting a dumb char      
        }
        EEPROM.commit();
        break; 
      } else { //valid search.
        digitalWrite( getRealPin( General::getIntFromHexSymbol( pinChar ) ), boolPinState );
        //We know that pinchar is found for the first time at index 2*i, but is it repeated for any reason at another 2*j where j > i ? 
        // If it's repeated I will have to free it in 2*j place.        
        for (int j = i + 1 ; j < sizeof(includedPinIndexAsChar); j++) {
          if ( EEPROM.read(2*j) == pinChar ) {
            Serial.printf("During EEPROM decoding, found a duplicate of char %c at place %d\n", pinChar, 2*j);
            EEPROM.write(2*j, '-');
            EEPROM.write(2*j + 1, '-');
            EEPROM.commit();
          }
        }
      }
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
      if (EEPROM.read(2*i) == pin) {
        Serial.printf("Found the pin in EEPROM during encoding... At position: %d\n", i);
        EEPROM.write(2*i + 1, state);
        EEPROM.commit();
        //once you update the EEPROM, your job is over.
        return;
      }        
    }
    if (i == sizeof(includedPinIndexAsChar)) {//usually you won't reach here unless it's the first time.      
      //i = General::getIndexOfArrayElement( includedPinIndexAsChar, sizeof(includedPinIndexAsChar), pin );      
      Serial.printf("Pin %c was not found during EEPROM encoding.\n", pin);      
      for (i = 0; i < 2 * sizeof(includedPinIndexAsChar); i++) {
        if (EEPROM.read(i) == '-') {
          Serial.printf("Allocating the pin for the first time in EEPROM during encoding... At the position of the first '-' which is position: %d\n", i);
          EEPROM.write(i, pin);
          EEPROM.write(i + 1, state);
          EEPROM.commit();
          return;
        }
      }
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
  
  static void checkManualUpdate(){
    if (digitalRead(D0)){ //if user presses on D3 (in the panel) then toggle the state of the corresponding output pin.
      toggle_D_pin_State_AndWriteEEPROM(inToOutPin(0));
    }
    if (digitalRead(D1)){ 
      toggle_D_pin_State_AndWriteEEPROM(inToOutPin(1));
    }
    if (digitalRead(D2)){ 
      toggle_D_pin_State_AndWriteEEPROM(inToOutPin(2));
    }
    if (digitalRead(D7)){ 
      toggle_D_pin_State_AndWriteEEPROM(inToOutPin(7));
    }
    if (digitalRead(D8)){ 
      toggle_D_pin_State_AndWriteEEPROM(inToOutPin(8));
    }
  }
    
};

