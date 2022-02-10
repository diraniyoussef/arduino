#include "ConnectionSetup.h"
#include "ESPAsyncTCP.h"
extern boolean is_PCF_connected;
const int MAX_READING_BUFFER = 60;  

//extern char out_pin[5];
extern char out_pin[2]; //comment for debugging

extern PCF PCF1;
//by the way another module belonging to the same owner can have its own set of mobiles.
const int mob_Number = 3;
const char* mob_i_Id_buff[ mob_Number ] = { "mob_S4:\0", "mob2:\0", "mob_Marwa_S7E:\0"}; //e.g.(mob_Id_buff + 2)[4] will the 'M' ???

const char* owner_id_buff = "Youssef_70853721:\0";
const char* mod_id_buff = "3:\0";

class Request{
public:
  char pin = '\0'; //this will be the guide if the incoming message was only R? or O3T? e.g.
  boolean action;  
  void getPinAndAction(char pin, boolean action) {
    this->pin = pin;
    this->action = action; 
  }
  boolean isJustReport() {
    return( pin == '\0' );
  }
};

class Operation {
public:  
  void getIdOfClient( char* mob_id_arg, char* reading_buff ) {
    //Resetting stuff
    mob_id_arg[0] = '\0'; //this is usually set before calling this method
    firstColon = 0;
    //Done resetting
    
    this->reading_buff = reading_buff;
    char owner_id_of_message[ MAX_READING_BUFFER ];
    owner_id_of_message[0] = '\0';
    getOwnerId( owner_id_of_message, reading_buff ); //reading_buff is input and owner_id_of_message is output
    if ( owner_id_of_message[0] == '\0' || strcmp( owner_id_buff, (const char*) owner_id_of_message ) != 0 ) {
      return;
    }

    char temporary_mob_Id_of_message [ MAX_READING_BUFFER ];
    temporary_mob_Id_of_message[0] = '\0';
    getMobIdOfMessage( temporary_mob_Id_of_message, reading_buff ); 
    if ( temporary_mob_Id_of_message[0] == '\0' ) {
      return;     
    }
    boolean mob_Id_of_message_is_valid = false;   
    for ( int i = 0; i < mob_Number; i++ ) {
      if( strcmp( mob_i_Id_buff[ i ], (const char*) temporary_mob_Id_of_message ) == 0 ) {
        mob_Id_of_message_is_valid = true;
        break;
      }
    }
    if( !mob_Id_of_message_is_valid ) {
      return;
    }
  
    char mod_Id_of_message[ MAX_READING_BUFFER ];
    mod_Id_of_message[0] = '\0';
    getModIdOfMessage( mod_Id_of_message, reading_buff );
    
    if ( mod_Id_of_message[0] == '\0' || strcmp( mod_id_buff, (const char*) mod_Id_of_message ) != 0 ) { //e.g. if it didn't find the second ':'
      return;   
    }
    strcpy( mob_id_arg, temporary_mob_Id_of_message );
    mob_id_arg[ strlen( (const char*) temporary_mob_Id_of_message ) ] = '\0';/*the char we are replacing now corresponds to the size of the useful word and 
      not to MAX_READING_BUFFER, because an additional '\0' is added in getMobIdOfMessage(...) where temporary_mob_Id_of_message has been defined.
    */
    //Serial.printf( "mob id seems to be: %s\n", mob_id_arg.c_str() );
  }  

private:
  int firstColon;
  char* reading_buff;  
  
  void getOwnerId( char* owner_id_buff, char* message_buff ) { //owner_id must be '\0' terminated    
    firstColon = getFirstCharOccurenceStartingFromIndex( 0, ':', message_buff );
    if ( firstColon > 0 ) { 
      strncpy( owner_id_buff, (const char*)message_buff, firstColon + 1 );       
      owner_id_buff[ firstColon + 1 ] = '\0';
    }
  }
  
  int getFirstCharOccurenceStartingFromIndex( int index, char charToCheck, char* s_buff ) {
    if ( s_buff[0] == '\0' ) {
      return (-1);
    }
    for (int i = index ; i < strlen( (const char*) s_buff ) ; i++) { //strlen won't be more than MAX_READING_BUFFER, the size of s_buff, by declaration
      if ( s_buff[i] == charToCheck ) {
        return (i);
      }
    }
    return(-1);
  }
  
  void getMobIdOfMessage( char* mob_id_buff, char* s_buff ) { //return value would be like "mob_S4:"
    if ( s_buff[0] == '\0') {
      return;
    }
    int secondColon = getFirstCharOccurenceStartingFromIndex( firstColon + 1, ':', s_buff );
    if( secondColon == -1 )
      return;
    if( secondColon > firstColon + 1 ) {
      strncpy( mob_id_buff, (const char*)(s_buff + firstColon + 1), secondColon - firstColon );
      mob_id_buff[ secondColon - firstColon ] = '\0';
    }
    firstColon = secondColon;
  }

  void getModIdOfMessage( char* mod_id_buff, char* s_buff ) {
    getMobIdOfMessage( mod_id_buff, s_buff );
  }

};

class MessageAndOperation {
private:   
  char reading_buff[ MAX_READING_BUFFER ];
  int readCharNumber;
  Operation oper;
  boolean is_intelligible_message_context = false; //just to know if theRequest has been initialized.  
  
  WiFiClient* aClient;    

public:        
  char mob_id_buff[ MAX_READING_BUFFER ];
  Request the_request; 
  
  MessageAndOperation( WiFiClient* aClient ) {
    this->aClient = aClient;
  }
  
  boolean getMessage() {    
    if( aClient->available() ) { //This will always be true, nevertheless it's good to protect it
      readCharNumber = aClient->readBytesUntil('\0', reading_buff, MAX_READING_BUFFER); //BTW the '\0' is not counted in the value of readCharNumber - tested
      reading_buff[ MAX_READING_BUFFER - 1 ] = '\0';
      //Serial.printf("Read char number of the incoming message is %d\n", readCharNumber);
      NodeMCU::yieldAndDelay();
      return true;               
    } else {
      return false;
    }
  }

  static int getLastCharOccurenceOfMessage( char charToCheck, char* reading_buff, int searchUpToIndex ) {
    for ( int i = searchUpToIndex - 2 ; i >= 0 ; i-- ) { 
      if (reading_buff[i] == charToCheck) {          
        return (i);
      }
    }
    return(-1);
  }

  void getIdOfClient() {    
    mob_id_buff[0] = '\0';
    //Serial.println("Trying to get the id of local incoming message");
    oper.getIdOfClient( mob_id_buff, reading_buff );
  }
  
  void sendAck() {
    char* useful_message_buff = "ACK\0";
    int total_message_length = strlen( (const char*) owner_id_buff ) + strlen( (const char*) mod_id_buff ) + 
                                strlen( (const char*) mob_id_buff) + strlen( (const char*) useful_message_buff ) + 1; //the + 1 is to add a \0 to it.
    char* totalMessage_buff = new char[ total_message_length ];
    strcpy( totalMessage_buff, (const char*) owner_id_buff );
    strcat( totalMessage_buff, (const char*) mod_id_buff );
    strcat( totalMessage_buff, (const char*) mob_id_buff );
    strcat( totalMessage_buff, (const char*) useful_message_buff );
        
    totalMessage_buff[ total_message_length - 1 ] = '\0';
    
    if (aClient)
      if (aClient->connected())
        aClient->write( (const uint8_t*)totalMessage_buff, total_message_length );

    delete[] totalMessage_buff;
  }
  
  boolean analyze(){ //this method returns true if the received message can be understood    
    is_intelligible_message_context = false;    
    int lastColonOfIncomingMessage = getLastCharOccurenceOfMessage( ':', reading_buff, readCharNumber);////useful characters in reading_buff are from 0 to readCharNumber - 1
    if( readCharNumber - 1 >= lastColonOfIncomingMessage + 2 ) { //the +2 is for the 2 chars R?
        //Because of using lastColonOfIncomingMessage then we must call sendAck() before analyze()
      if (reading_buff[lastColonOfIncomingMessage + 1] == 'R' && reading_buff[lastColonOfIncomingMessage + 2] == '?') { //a report request for pins status        
        is_intelligible_message_context = true;        
        the_request.pin = '\0'; //this will be the guide if the incoming message was only R? or O3T? e.g.
        return (true);
      } else {
        //typically it's like mob1:O1T?
        if (readCharNumber - 1 >= lastColonOfIncomingMessage + 4){
          if (reading_buff[lastColonOfIncomingMessage + 1] == 'O'){
            //now making sure the incoming message indicates the pin clearly
            if ( General::arrayIncludeElement( out_pin , sizeof(out_pin) , reading_buff[lastColonOfIncomingMessage + 2] ) 
              || General::arrayIncludeElement( PCF1.out_pin_symbol , PCF1.out_pins_number , reading_buff[ lastColonOfIncomingMessage + 2 ] ) 
            ) {
              if ( reading_buff[lastColonOfIncomingMessage + 3] == 'T' || reading_buff[lastColonOfIncomingMessage + 3] == 'F' ) {
                boolean actionType;
                if ( reading_buff[ lastColonOfIncomingMessage + 3 ] == 'T' ) {
                  actionType = true;
                } else {
                  actionType = false;
                }
                the_request.getPinAndAction( reading_buff[lastColonOfIncomingMessage + 2], actionType );
                is_intelligible_message_context = true;        
                return(true);
              } else {
                //Serial.println("Useful received local message doesn't state whether to turn the pin on or off.");
              }
            } else {
              //Serial.println("Useful received local message doesn't state the pin number.");
            }            
          } else {
            //Serial.println("Useful received local message doesn't start with R or O.");
          }
        } else {
          //Serial.println("Useful received local message is less than 3 chars and is not \"R?\"");
        }
      }
    } else {
      //something is wrong in the received message. But I won't feedback the user.
      //Serial.println("Useful received local message is less than 2 chars.");
    }
    return (false);
  }
  
  boolean isJustReport() {
    if( is_intelligible_message_context ) { //the protection is not necessary if the method is correctly used.
      return( the_request.isJustReport() );
    }
    return( true );
  }
  
  void updatePinAndEEPROM() { //it is bad to enter this method if the pcf was not connected.
    char symbol = the_request.pin;
    boolean state_bool = the_request.action;
    if( General::arrayIncludeElement( out_pin , sizeof(out_pin) , symbol ) ) {
      digitalWrite( NodeMCU::getRealPinFromD( General::getIntFromHexChar( symbol ) ), state_bool );

      NodeMCU::encodeEEPROM( symbol , state_bool );
    } else if( ! PCF1.updatePinAndEEPROM( symbol , state_bool ) ) {  //the return value of updatePinAndEEPROM method of PCF is just to know if the pin belongs to that particular PCF or not.
      /*
      if( ! PCF2->updatePinAndEEPROM( symbol, state_bool ) ) {
        ...
      } 
      */         
    }
  }

  void sendReport() { //Inside this method we take care of the case if the pcf was not connected.
    NodeMCU::yieldAndDelay(50); //I think this delay may be useful...
    int pins_in_report = sizeof(out_pin);
    int last_length = strlen( (const char*) owner_id_buff ) + strlen( (const char*) mod_id_buff ) + strlen( (const char*) mob_id_buff );
    char* totalMessage_buff = new char[ last_length + 3 * PCF::absolute_max_pins_number + 1 ];
    strcpy( totalMessage_buff, (const char*) owner_id_buff );
    strcat( totalMessage_buff, (const char*) mod_id_buff );
    strcat( totalMessage_buff, (const char*) mob_id_buff );       
    
    for( int i = 0 ; i < pins_in_report ; i++ ) { 
      totalMessage_buff[ last_length ] = 'O';
      char stateOfPin;
      int pinAsInt = General::getIntFromHexChar( out_pin[i] );
      if( digitalRead( NodeMCU::getRealPinFromD( pinAsInt ) ) ) { //NodeMCU::getRealPinFromD(i) is the same as saying Di as of D1 or D2 or ...
        stateOfPin = 'T';                       
      } else {
        stateOfPin = 'F';
      }
      totalMessage_buff[ last_length + 1 ] = out_pin[i];
      totalMessage_buff[ last_length + 2 ] = stateOfPin;
      last_length = last_length + 3;      
    }
    
    if(! PCF1.addPinsToReport( totalMessage_buff , &last_length ) ) {//This also tests whether the PCF is connected.
      //Serial.println("message not sent since the PCF is disconnected");
      //don't send the message.
      return;
    }
       
    totalMessage_buff[ last_length ] = '\0';
    //last_length++; //is it necessary?

    if (aClient) {
      if (aClient->connected()) {
        aClient->write( (const uint8_t*)totalMessage_buff, last_length );       
        //Serial.printf( "Sent message was like %s\n", String(usefulOutMessage_buff).c_str() );
      } else {
        //Serial.println("aClient is not connected.");
      }
    } else {
      //Serial.println("aClient is null.");
    }
    
    delete[] totalMessage_buff;
  }
};

class IntermediateMAndOp {
private:   
  AsyncClient* async_client; //never delete this
  
  char* reading_buff;
  int readCharNumber = 0;
  Operation oper;
  boolean is_intelligible_message_context = false; //just to know if theRequest has been initialized.    
  
public:
  Request the_request; //this is not null only if analyze() method is called.    
  char mob_id_buff[ MAX_READING_BUFFER ];
  
  IntermediateMAndOp( AsyncClient* async_client ) {
    this->async_client = async_client;    
  }

  boolean getMessage( char* message_buff, int readCharNumber ) {    
    if ( readCharNumber > 0 ) {
      reading_buff = message_buff;
      reading_buff[ MAX_READING_BUFFER - 1 ] = '\0';
      this->readCharNumber = readCharNumber;
      return true;
    } else {
      return false;
    }
  }

  void getIdOfClient() {    
    mob_id_buff[0] = '\0';
    oper.getIdOfClient( mob_id_buff, reading_buff );
  }

  void sendPredefinedMessage( char* useful_message_buff, const char* mob_id_buff ) {
    int total_message_length = strlen( (const char*) owner_id_buff ) + strlen( (const char*) mod_id_buff ) + 
                                strlen( (const char*) mob_id_buff ) + strlen( (const char*) useful_message_buff ) + 1; //the + 1 is to add a \0 to it.
    char* totalMessage_buff = new char[ total_message_length ];
    strcpy( totalMessage_buff, (const char*) owner_id_buff );
    strcat( totalMessage_buff, (const char*) mod_id_buff );
    strcat( totalMessage_buff, (const char*) mob_id_buff );
    strcat( totalMessage_buff, (const char*) useful_message_buff );
        
    totalMessage_buff[ total_message_length - 1 ] = '\0';

    if( async_client )
      if( async_client->connected() )
        async_client->write( (const char*) totalMessage_buff );
  }
  
  void sendHiIntermediate() {     
    sendPredefinedMessage( "0z\0", mob_i_Id_buff[0] );    
  }

  void sendAck(){    
    sendPredefinedMessage( "ACKz\0", (const char*) mob_id_buff );    
  }

  boolean analyze(){ 
    is_intelligible_message_context = false;    
    int lastColonOfIncomingMessage = MessageAndOperation::getLastCharOccurenceOfMessage( ':', reading_buff, readCharNumber);
    if (readCharNumber - 1 >= lastColonOfIncomingMessage + 2) { 
      if ( reading_buff[ lastColonOfIncomingMessage + 1 ] == 'R' && reading_buff[ lastColonOfIncomingMessage + 2 ] == '?' ) {
        is_intelligible_message_context = true;        
        the_request.pin = '\0'; 
        return (true);
      } else {
        if (readCharNumber - 1 >= lastColonOfIncomingMessage + 4){
          if( reading_buff[ lastColonOfIncomingMessage + 1 ] == 'O' ) {
            if ( General::arrayIncludeElement( out_pin , sizeof(out_pin) , reading_buff[lastColonOfIncomingMessage + 2] ) 
              || General::arrayIncludeElement( PCF1.out_pin_symbol , PCF1.out_pins_number , reading_buff[lastColonOfIncomingMessage + 2] ) 
            ) {
              if ( reading_buff[lastColonOfIncomingMessage + 3] == 'T' || reading_buff[lastColonOfIncomingMessage + 3] == 'F' ) {
                boolean actionType;
                if (reading_buff[lastColonOfIncomingMessage + 3] == 'T') {
                  actionType = true;
                } else {
                  actionType = false;
                }
                //Serial.printf("In Analyze, the pin is %c and the action is ...", reading_buff[lastColonOfIncomingMessage + 2] );
                the_request.getPinAndAction( reading_buff[ lastColonOfIncomingMessage + 2 ], actionType );
                is_intelligible_message_context = true;
                return(true);
              } else {
                //Serial.println("Useful received message doesn't state whether to turn the pin on or off.");
              }
            } else {
              //Serial.println("Useful received message doesn't state the pin number.");
            }
          } else {
            //Serial.println("Useful received message doesn't start with R or O.");
          }
        } else {
          //Serial.println("Useful received message is less than 3 chars and is not \"R?\"");
        }
      }
    } else {
      //Serial.println("Useful received message is less than 2 chars.");
    }
    return (false);
  }

  boolean isJustReport() {
    if( is_intelligible_message_context ){ //the protection is not necessary if the method is correctly used. I.e. we always enter here.
      return( the_request.isJustReport() );
    }
    return( true );
  }

  void updatePinAndEEPROM() { //it is bad to enter this method if the pcf was not connected.
    char symbol = the_request.pin;
    boolean state_bool = the_request.action;
    if( General::arrayIncludeElement( out_pin , sizeof(out_pin) , symbol ) ) {
      digitalWrite( NodeMCU::getRealPinFromD( General::getIntFromHexChar( symbol ) ), state_bool );
      NodeMCU::encodeEEPROM( symbol , state_bool );
    } else if( ! PCF1.updatePinAndEEPROM( symbol, state_bool ) ) {  //the return value of updatePinAndEEPROM method of PCF is just to know if the pin belongs to that particular PCF or not.
      /*
      if( ! PCF2->updatePinAndEEPROM( symbol, state_bool ) ) {
        ...
      } 
      */         
    }
  }
  
  void sendReport( AsyncClient* newest_async_client ) {
    int pins_in_report = sizeof( out_pin );
    int last_length = strlen( (const char*) owner_id_buff ) + strlen( (const char*) mod_id_buff ) + strlen( (const char*) mob_id_buff );
    char* totalMessage_buff = new char[ last_length + 3 * PCF::absolute_max_pins_number + 2 ]; //the + 2 is the 'z' and the '\0'
    strcpy( totalMessage_buff, (const char*) owner_id_buff );
    strcat( totalMessage_buff, (const char*) mod_id_buff );
    strcat( totalMessage_buff, (const char*) mob_id_buff );       
    
    for( int i = 0 ; i < pins_in_report ; i++ ) {
      totalMessage_buff[ last_length ] = 'O';
      char stateOfPin;
      int pinAsInt = General::getIntFromHexChar( out_pin[i] );
      if( digitalRead( NodeMCU::getRealPinFromD( pinAsInt ) ) ) { //NodeMCU::getRealPinFromD(i) is the same as saying Di as of D1 or D2 or ...
        stateOfPin = 'T';                       
      } else {
        stateOfPin = 'F';
      }
      totalMessage_buff[ last_length + 1 ] = out_pin[i];
      totalMessage_buff[ last_length + 2 ] = stateOfPin;
      last_length = last_length + 3;      
    }

    if(! PCF1.addPinsToReport( totalMessage_buff , &last_length ) ) {//This also tests whether the PCF is connected.
      //Serial.println("message not sent since the PCF is disconnected");
      //don't send the message.
      return;
    }
    
    totalMessage_buff[ last_length ] = 'z';
    totalMessage_buff[ last_length + 1] = '\0';

    if( newest_async_client )
      if( newest_async_client->connected() )
        newest_async_client->write( (const char*) totalMessage_buff );

    delete[] totalMessage_buff;
  }
  
};


