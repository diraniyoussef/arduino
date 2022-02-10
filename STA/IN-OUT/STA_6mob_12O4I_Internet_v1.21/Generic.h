#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "ESP_PCF8574.h"

//extern char in_pin[5];
extern char in_pin[4]; //comment for debugging 
//extern char out_pin[5];
extern char out_pin[2]; //comment for debugging  

#define MAX_SRV_CLIENTS 3 //number of mobiles.

class General{
public:    
  static int getIntFromHexChar(char aChar) {
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

class PCF {
private:  
  byte in_pins_number;
  byte* in_pin; //Do NOT call delete on this pointer because it points to something on the stack.
    
  int pins_index_min;
  int pins_index_max;

  void toggle_pin_and_encode_EEPROM( int out_pin_index ) {
    boolean bit_status = pcf8574A.getBit( out_pin[ out_pin_index ] );
    if(! pcf8574A.isDataRead() ) { //not necessary since checked somewhere else and this method is not called if condition were not satisfied
      return;
    }
    if( bit_status ) {      
      pcf8574A.setBit( out_pin[ out_pin_index ] , false );
      encodeEEPROM( out_pin_symbol[ out_pin_index ] , 'F' );
    } else {
      pcf8574A.setBit( out_pin[ out_pin_index ] , true );
      encodeEEPROM( out_pin_symbol[ out_pin_index ] , 'T' );
    }
  }

public:  
  byte out_pins_number;
  byte* out_pin; //Do NOT call delete on this pointer because it points to something on the stack.
  char* out_pin_symbol; //Do NOT call delete on this pointer because it points to something on the stack.  
  static int absolute_max_pins_number;
  ESP_PCF8574 pcf8574A;
    
  PCF ( uint8_t address_arg , byte in_pins_number_arg , byte* in_pin_arg , byte out_pins_number_arg , byte* out_pin_arg , char* out_pin_symbol_arg ) {    
    in_pins_number = in_pins_number_arg;    
    in_pin = in_pin_arg;
    out_pins_number = out_pins_number_arg;
    out_pin = out_pin_arg;
    out_pin_symbol = out_pin_symbol_arg;
    pcf8574A.begin( address_arg ); //whenever PCF8574A starts all its 8 pins will be high
    pcf8574A.setByte(0);//to lower its 8 pins.
  }

  byte symbolToPin( char out_pin_symbol_arg ) { //the in is a D... and the out is also a D...
    for ( int i = 0 ; i < out_pins_number ; i++ ) {
      if ( out_pin_symbol[i] == out_pin_symbol_arg ) {
        return out_pin[i];
      }
    }
    return 255;//shouldn't get here.
  }  

  void setEEPROM_PinRange() {
    pins_index_min = absolute_max_pins_number;    
    absolute_max_pins_number = absolute_max_pins_number + out_pins_number;
    pins_index_max = absolute_max_pins_number;
  }
  
  void decodeEEPROM() {                                          
    char pinChar; //e.g. 'b', 'c', ...
    char pinState; //'T' or 'F'
    boolean boolPinState;
    for ( int i = pins_index_min ; i < pins_index_max ; i++ ) {
      pinChar = (char) EEPROM.read(2*i);      
      pinState = (char) EEPROM.read(2*i + 1); 
      if(pinState == 'T') {
        boolPinState = true;
      } else {
        boolPinState = false;
      }
      Serial.printf("Reading %c%c from EEPROM during encoding\n", pinChar, pinState);
      if ( ! General::arrayIncludeElement( out_pin_symbol , out_pins_number , pinChar ) || !( pinState == 'F' || pinState == 'T' ) ) {
        //Whenever what I read in the EEEPROM is not valid, I make sure to free up to the REST of the EEPROM and then leave.
        Serial.printf("During EEPROM decoding, inconsistent reading from EEPROM happened so I will write '-' into EEPROM from place %d to the end\n", 2*i);
        for (int j = 2 * i ; j < 2 * pins_index_max ; j++) {
          EEPROM.write(j, '-'); //I free the EEPROM by putting a dumb char      
        }
        EEPROM.commit();
        break; 
      } else { //valid search.
        Serial.println("Got a valid search in decoding EEPROM");       
        pcf8574A.setBit ( symbolToPin( pinChar ) , boolPinState );  //I can test whether the PCF is connected or not, but since it's a best effort job then it's ok.
        //We know that pinchar is found for the first time at index 2*i, but is it repeated for any reason at another 2*j where j > i ? 
        // If it's repeated I will have to free it in 2*j place.        
        for (int j = 0 ; j < absolute_max_pins_number ; j++) {
          if ( j == i) {
            continue;
          }          
          if ( EEPROM.read(2*j) == pinChar ) {
            Serial.printf("During EEPROM decoding, found a duplicate of char %c at place %d\n", pinChar, 2*j);
            EEPROM.write( 2*j , '-');
            EEPROM.write( 2*j + 1 , '-');
            EEPROM.commit();
          }
        }
      }
    }          
  }

  boolean checkManualUpdate() {
    for( int i = 0 ; i < in_pins_number ; i++ ) {
      if ( ! pcf8574A.getBit( in_pin[i] ) ) { //this means that only when the pin is grounded then it's considered as as activated input.
        if(! pcf8574A.isDataRead() ) {
          return false;
        }
        //toggle corresponding out pin
        toggle_pin_and_encode_EEPROM( i );
      }
    }
    return true;
  }

  boolean updatePinAndEEPROM( char symbol, boolean bool_state ) {    
    if( General::arrayIncludeElement( out_pin_symbol , out_pins_number , symbol ) ) { 
      char state;
      if( bool_state ) {
        state = 'T';
      } else {
        state = 'F';
      }
      if( pcf8574A.setBit( symbolToPin( symbol ), bool_state )  ) { //not sure if this is necessary since we check in arrayIncludeElement beforehand
        encodeEEPROM( symbol, state );
      }
      return true;
    } else {
      return false;
    }
  }
  
  void encodeEEPROM( char out_pin_char, char state ) {
    Serial.printf("Now encoding %c%c into EEPROM\n", out_pin_char, state );
    int i; //i represents the pin index in EEPROM
    for( i = pins_index_min ; i < pins_index_max ; i++ ) {
      if ( EEPROM.read(2*i) == out_pin_char ) {
        Serial.printf("Found the pin in EEPROM during encoding... At position: %d\n", i);
        EEPROM.write(2*i + 1, state);
        EEPROM.commit();
        //once you update the EEPROM, your job is over.
        return;
      }        
    }
    if ( i == pins_index_max ) {//usually you won't reach here unless it's the first time.      
      Serial.printf("Pin %c was not found during EEPROM encoding.\n", out_pin_char);      
      for ( i = 2 * pins_index_min ; i < 2 * pins_index_max ; i++ ) {
        if( EEPROM.read(i) == '-' ) {
          Serial.printf("Allocating the pin for the first time in EEPROM during encoding... At the position of the first '-' which is position: %d\n", i);
          EEPROM.write( i , out_pin_char );
          EEPROM.write( i + 1 , state );
          EEPROM.commit();
          return;
        }
      }
    }
  }

  boolean addPinsToReport( char* message , int* last_length_ptr ) {
    //adding something like ObTOcTOdFOeT to *message in order to send a report.
    char char_symbol;
    boolean state;
    char state_of_pin;
    
    for( int i = 0 ; i < out_pins_number ; i++ ) { //please note that the real length of out_pin_symbol is String(out_pin_symbol).length() - 1      
      boolean state = pcf8574A.getBit( out_pin[i] );             
      if(! pcf8574A.isDataRead() ) {
        return false;
      }      
      if( state ) { //NodeMCU::getRealPinFromD(i) is the same as saying Di as of D1 or D2 or ...
        state_of_pin = 'T';
      } else {
        state_of_pin = 'F';
      }
      message[ *last_length_ptr ] = 'O';
      message[ *last_length_ptr + 1 ] = out_pin_symbol[i];
      message[ *last_length_ptr + 2 ] = state_of_pin;
      
      *last_length_ptr = *last_length_ptr + 3;  
    }
    
    return true;
  }
  
/*
  boolean getStateFromEEPROM_AndRefresh( byte i ) { //"i" is the index of an output pin of the PCF8574A as listed in the EEPROM
    /*The phylosophy behind this method:
    *Since any mechanical shake given to the PCF makes all the output pins (I guess it's all the pins) go low and since what fixes this is to set any output pin to any value
    * (actually it is also fixed if an input pin had also caused the output to be set) then I will set one output pin to its value in the EEPROM.
    * By the way if the PCF8574A was not connected that's not a problem here.
    */
/*    char pinChar; //e.g. 'b', 'c', ...
    char pinState; //'T' or 'F'
    boolean bool_pin_state;
    pinChar = (char) EEPROM.read( 2 * ( pins_index_min + i ) );
    pinState = (char) EEPROM.read( 2 * ( pins_index_min + i ) + 1 );
    
    if( pinChar == '-' ) {
      bool_pin_state = pcf8574A.getBit( out_pin[ 0 ] );
      pcf8574A.setBit( out_pin[ 0 ] , bool_pin_state ); //This is the refresh part.
      return bool_pin_state; //most probably the value would be "true", all of them are set from the beginning.
    }
    
    if( pinState == 'T') {
      bool_pin_state = true;
    } else {
      bool_pin_state = false;
    }
    pcf8574A.setBit( symbolToPin( pinChar ) , bool_pin_state ); //This is the refresh part.
    return bool_pin_state;
  }
  */

};

class NodeMCU{  
private:
  const static byte connectFailureNotifierPin = 7;  //may be even 10 

  static void setConnectFailureNotifierPinHigh() {
    digitalWrite( getRealPinFromD( connectFailureNotifierPin ), HIGH );
  }
  
  static void toggle_D_pin_State(int D_pin) {
    if (digitalRead( getRealPinFromD( D_pin ) ) ){
      digitalWrite( getRealPinFromD( D_pin ), LOW );
    } else {
      digitalWrite( getRealPinFromD( D_pin ), HIGH );
    }
  }
  
  static void toggle_D_pin_State_AndWriteEEPROM(char symbol) {
    int real_pin = getRealPinFromD( General::getIntFromHexChar( symbol ) );
    if( digitalRead( real_pin ) ){
      digitalWrite( real_pin, LOW );
      encodeEEPROM( symbol, false);
    } else {
      digitalWrite( real_pin, HIGH );
      encodeEEPROM( symbol, true);
    }
  }  
    
public:
  static void setPins() {
    pinMode(D0, INPUT);  
    pinMode(D3, INPUT);
    pinMode(D4, INPUT);    
    pinMode(D5, INPUT);
    //pinMode(D10, INPUT);  //Comment for debugging    
    
    pinMode(D6, OUTPUT);  
    pinMode(D8, OUTPUT);  
    //pinMode(D9, OUTPUT);  
    //pinMode(D10, OUTPUT);   
    //pinMode(D9, OUTPUT);  //Comment for debugging 
  
    pinMode( getRealPinFromD( connectFailureNotifierPin ) , OUTPUT );   
  }

  static void setConnectFailureNotifierPinLow() {
    digitalWrite( getRealPinFromD( connectFailureNotifierPin ), LOW );
  }    

  static int beginEEPROM() {
    Serial.println("beginning the EEPROM");
    EEPROM.begin(2 * PCF::absolute_max_pins_number ); //e.g. 10T1F2T5T6F decoded like this: first byte is moduleId interpreted as a "byte" type, then 0T where 0 is char interpreted
                                                      // instead of a byte because pin 10 may one day be used which would be 'a'    
  }
  
  static void lowerAllOutPins() {    
    for (int i = 0; i < sizeof( out_pin ); i++ ) {
      Serial.println("digitalWrite");
      digitalWrite( getRealPinFromD( General::getIntFromHexChar( out_pin[i] ) ), LOW);            
    }
  }

  static void toggleConnectFailureNotifierPin() {
    toggle_D_pin_State(connectFailureNotifierPin);
  }
  
  static boolean isConnectedToWiFi() { 
    /*The caller of this method usually directly restart the NodeMCU if this method returned false.
     * In this sense we may say that this method blocks the execution in case it was not connected.
     */
    int i = 0;    
    boolean notifier_pin_set_to_high = false;
    while( WiFi.status() != WL_CONNECTED && i < 16 ) { //waiting for 10 seconds.
      i++;      
      if (i > 5) {
        setConnectFailureNotifierPinHigh(); //Comment for debugging
        notifier_pin_set_to_high = true;
      }
      delay(750);
      Serial.print(".");
    }
    if( notifier_pin_set_to_high ) { //needed
      setConnectFailureNotifierPinLow(); //Comment for debugging
    }

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
   
  static void decodeEEPROM(){    //I believe everything in the EEPROM is of type "byte". https://www.arduino.cc/en/Reference/EEPROMWrite        
    //determining the state of various pins
    char pinChar; //e.g. '0', '1', ...
    char pinState; //'T' or 'F'
    boolean boolPinState;
    for (int i = 0; i < sizeof(out_pin); i++) {
      pinChar = (char) EEPROM.read(2*i);      
      pinState = (char) EEPROM.read(2*i + 1); 
      if(pinState == 'T') {
        boolPinState = true;
      } else {
        boolPinState = false;
      }
      if ( !General::arrayIncludeElement( out_pin, sizeof(out_pin), pinChar ) || !( pinState == 'F' || pinState == 'T' ) ) {
        //Whenever what I read in the EEEPROM is not valid, I make sure to free up to the REST of the EEPROM and then leave.
        Serial.printf("During EEPROM decoding, inconsistent reading from EEPROM happened so I will write '-' into EEPROM from place %d to the end\n", 2*i);
        for (int j = 2 * i; j < 2 * sizeof(out_pin); j++) {
          EEPROM.write(j, '-'); //I free the EEPROM by putting a dumb char      
        }
        EEPROM.commit();
        break; 
      } else { //valid search.
        digitalWrite( getRealPinFromD( General::getIntFromHexChar( pinChar ) ), boolPinState );
        //We know that pinchar is found for the first time at index 2*i, but is it repeated for any reason at another 2*j where j > i ? 
        // If it's repeated I will have to free it in 2*j place.        
        for (int j = i + 1 ; j < PCF::absolute_max_pins_number; j++) {
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
    for (i = 0; i < sizeof(out_pin); i++) {
      if (EEPROM.read(2*i) == pin) {
        Serial.printf("Found the pin in EEPROM during encoding... At position: %d\n", i);
        EEPROM.write(2*i + 1, state);
        EEPROM.commit();
        //once you update the EEPROM, your job is over.
        return;
      }        
    }
    if (i == sizeof(out_pin)) {//usually you won't reach here unless it's the first time.      
      //i = General::getIndexOfArrayElement( out_pin, sizeof(out_pin), pin );      
      Serial.printf("Pin %c was not found during EEPROM encoding.\n", pin);      
      for (i = 0; i < 2 * sizeof(out_pin); i++) {
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
  
  static int getRealPinFromD(int D_pin){
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
  static void yieldAndDelay( int delayTime ){
    yield();
    delay(delayTime);
  }
  static void yieldAndDelay(){
    yield();
    delay(33);
  }
  
  static void checkManualUpdate(){
    for ( int i = 0 ; i < sizeof(in_pin) ; i++ ) {
      if( digitalRead( getRealPinFromD( General::getIntFromHexChar( in_pin[i] ) ) ) ) {
        toggle_D_pin_State_AndWriteEEPROM( out_pin[i] );
      }        
    }    
  }
    
};


