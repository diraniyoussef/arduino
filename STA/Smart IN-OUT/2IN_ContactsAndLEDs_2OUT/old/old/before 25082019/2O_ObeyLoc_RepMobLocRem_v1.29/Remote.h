#include "AsynchroClient.h"

AsynchroClient remote_connection; //intermediate relay server

const char* owner_id_buff = "Electrotel_Store:\0";
const char* mod_id_buff = "11:\0";

class RemoteServerMessageOp {
private:   
  static constexpr char* dummy_buff = "dummy:\0";
  static const int Reflected_Panels_Number = 3; //Don't forget to add "dummy" in the intermediate server.
  const char* reflected_i_Id_buff[ Reflected_Panels_Number ] = { dummy_buff, "S4:\0", "TrekStor_Tab:\0" }; /*By convention, it's just 1 reflected panel that our panel is linked to, 
                                                                             *so it's either a local one or a reflected one.
                                                                             */ 
                                                                             /*As for dummy, it's necessary because we need to verify it in the 
                                                                             * incoming message signature.
                                                                             */
                                                                             /*Please note that it is not professional to have only 1 array for the mobs and mods
                                                                              * but it is working for now.
                                                                              */
  //const char* owner_id_buff = "Youssef_70853721:\0";
  //const char* mod_id_buff = "5:\0";
  
  char* read_buff; //BTW, this is never changed in its own set of operations. Only changed when we have a new message in a new loop.
  int read_char_number = 0;
  Operation oper;

  static void sendPredefinedMessage( AsyncClient* async_client, char* useful_message_buff, const char* reflected_id_buff ) {
    int total_message_length = strlen( (const char*) owner_id_buff ) + strlen( (const char*) mod_id_buff ) + 
                                strlen( (const char*) reflected_id_buff ) + strlen( (const char*) useful_message_buff ) + 2; //the + 1 is to add a \0 to it.
    Serial.printf("Size of message is %d\n", total_message_length);
    char* totalMessage_buff = new char[ total_message_length ];
    
    strcpy( totalMessage_buff, (const char*) owner_id_buff );
    strcat( totalMessage_buff, (const char*) mod_id_buff );
    strcat( totalMessage_buff, (const char*) reflected_id_buff );
    strcat( totalMessage_buff, (const char*) useful_message_buff );
        
    totalMessage_buff[ total_message_length - 2 ] = '\\';
    totalMessage_buff[ total_message_length - 1 ] = '\0';
    
    sendMessageToAsync( async_client, totalMessage_buff );
    
    delete[] totalMessage_buff;
  }
  
public:
  Request the_request[ Reflector_Pins_Number ]; //this is not null only if analyze() method is called.    
  char reflected_id_buff[ Max_Reading_Buffer ];  
  char report_part_message_buff[ Max_Reading_Buffer ];
  
  void stopOperations() {
    if( read_buff )
      read_buff[0] = '\0';
    read_char_number = 0;
    //is_intelligible_message_context = false;
    for( int i = 0 ; i < Reflector_Pins_Number ; i++ )  {
      the_request[i].stopOperations(); /*not necessary since it'll be overwritten, but ok*/
    }    
    reflected_id_buff[0] = '\0';    
  }
  
  static int getReportPartFromThisNodeMCU( char* useful_message_buff ) {
    int pins_to_report_about = sizeof( reflector_pin ); //we could had used Reflector_Pins_Number instead
    int char_index = 0;
    for( int i = 0 ; i < pins_to_report_about ; i++ ) {     
      useful_message_buff[ char_index ] = 'O';
      char stateOfPin;
      int pinAsInt = General::getIntFromHexChar( reflector_pin[i] );
      if( NodeMCU::getInPinStateAsConsidered( pinAsInt ) ) { //NodeMCU::getRealPinFromD(i) is the same as saying Di as of D1 or D2 or ...
        stateOfPin = 'T';
      } else {
        stateOfPin = 'F';
      }
      useful_message_buff[ char_index + 1 ] = reflector_pin[i];
      useful_message_buff[ char_index + 2 ] = stateOfPin;
      char_index = char_index + 3;      
    }
  }
  //I find the following method better than getLastCharOccurenceOfMessage(...) in case many messages are being buffered and received by async_client
  static int getThirdOccurrenceOfCharInUsefulMessage( char charToCheck, char* read_buff, int max_buff_index ) {
    int index = 0;
    int i;
    for( int j = 0 ; j < 3 ; j++ ) {      
      for ( i = index ; i < max_buff_index ; i++ ) { 
        if( read_buff[i] == charToCheck ) {
          index = i + 1;
          break;
        }
      }      
    }
    if( i == max_buff_index ) { //this protection is not really needed since we know the ":" will occur at least 3 times.
      return -1;
    } else {
      return i;
    }
  }
  
  void setup() {
    oper.setOperInfo( owner_id_buff , Reflected_Panels_Number , reflected_i_Id_buff , mod_id_buff );
  }
  
  boolean getMessage( char* message_buff, int read_char_number ) {    
    Serial.printf( "incoming message is %s\n" , message_buff );
    if ( read_char_number > 0 ) {
      read_buff = message_buff;
      read_buff[ Max_Reading_Buffer - 1 ] = '\0';
      this->read_char_number = read_char_number;
      return true;
    } else {
      return false;
    }
  }

  void getRequesterId() {    
    reflected_id_buff[ 0 ] = '\0';
    oper.getIdOfClient( reflected_id_buff , read_buff );
  }
  
  static void sendHiIntermediate( AsyncClient* async_client ) {
    sendPredefinedMessage( async_client , "HI\0" , dummy_buff );    
  }

  void sendReport( AsyncClient* async_client ) {    
    byte old_size = strlen( report_part_message_buff ); /* We already have a '\0'
    * We never send a report unless analyze() has returned true, thus report_part_message_buff has a useful value.
     */
    //report_part_message_buff[ old_size + 1 ] = '\0';
    //report_part_message_buff[ old_size ] = '\\';
    
    sendPredefinedMessage( async_client , report_part_message_buff , (const char*) reflected_id_buff );    
  }

  static boolean analyze( Request* the_request , char* read_buff , int read_char_number , char* useful_message_buff ) {
    boolean dummy_is_intelligible_message_received_ptr;
    return analyze( the_request , read_buff , read_char_number , useful_message_buff, &dummy_is_intelligible_message_received_ptr );
  }

  static boolean analyze( Request* the_request , char* read_buff , int read_char_number , char* useful_message_buff, boolean* is_intelligible_message_received_ptr ) {
    boolean is_intelligible_report = false; //it'll be true when the received useful message is fine to all expected pins.    
    the_request[0].pin = '\0'; /*this will be the guide to differentiate whether the incoming message was only R? or O3T? or O3TO4F? for example.
                                * It's better to reset them all but the first inctance is still fine though as long as wee rely on the return value value of 
                                * analyze()
                                 */
    int last_colon_of_incoming_message = getThirdOccurrenceOfCharInUsefulMessage( ':' , read_buff , read_char_number ); //useful characters in read_buff are from 0 to read_char_number - 1    
    //Serial.printf("Message is %s and last colon is at index %d\n", read_buff, last_colon_of_incoming_message);
    //Serial.printf("read_char_number is %d\n", read_char_number);
    //Serial.printf("Supposed index must be %d\n", last_colon_of_incoming_message + 3 * Reflector_Pins_Number + 1 );
    //there was a code here in case the incoming message wasn't a report (like being a report request or a pin change request )
    
    if( read_char_number - 1 >= last_colon_of_incoming_message + 2 ) { 
      if ( read_buff[ last_colon_of_incoming_message + 1 ] == 'R' && read_buff[ last_colon_of_incoming_message + 2 ] == '?' ) {
        /*Note that we use this same method for either receiving a report request, receiving an ordering report, or receiving an ack.
         * Actually, this is not professional (i.e. we must have like 3 methods), but it works for now.
         */        
        getReportPartFromThisNodeMCU( useful_message_buff );
        useful_message_buff[ 3 * Reflector_Pins_Number ] = '\0';
        return (true);      
      } else {    
        if( read_char_number - 1 >= last_colon_of_incoming_message + 3 * Reflector_Pins_Number ) { /*Consider them as indices and you'll be convinced.
                                                                                                 * Typically the content is like O3TO4F?
                                                                                                 */
          int test_index = last_colon_of_incoming_message;
          for( int i = 0 ; i < Reflector_Pins_Number ; i++ ) {
            if( read_buff[ test_index + 1 ] == 'O') {
              is_intelligible_report = true;
              //Serial.printf("Analyzing, 'O' is fine\n");
              //now making sure the incoming message indicates the pin clearly
              if ( General::arrayIncludeElement( (char*) reflector_pin , Reflector_Pins_Number , read_buff[ test_index + 2 ] ) != -1
                //|| General::arrayIncludeElement( PCF1.out_pin_symbol , PCF1.out_pins_number , read_buff[ last_colon_of_incoming_message + 2 ] ) //comment for PCF exclusion
                //PLEASE BEWARE THAT I MADE A CHANGE ABOUT THE RETURN VALUE OF arrayIncludeElement
              ) {
                //Serial.printf("Analyzing, Pin %c is fine\n", read_buff[ test_index + 2 ] );
                if ( read_buff[ test_index + 3 ] == 'T' || read_buff[ test_index + 3 ] == 'F' ) {
                  boolean actionType;
                  Serial.printf("Analyzing, Action type is fine and it is: %c\n", read_buff[ test_index + 3 ] );
                  if ( read_buff[ test_index + 3 ] == 'T' ) {
                    actionType = true;
                  } else {
                    actionType = false;
                  }
                  the_request[ i ].getPinAndAction( read_buff[ test_index + 2 ], actionType );
                  test_index = test_index + 3;
                  
                } else {
                  //Serial.println("Useful received local message doesn't state whether to turn the pin on or off.");
                  is_intelligible_report = false;
                  break; //have to break once it's false
                }
              } else {
                is_intelligible_report = false;
                break; //have to break once it's false
                //Serial.println("Useful received local message doesn't state the pin number.");
              }            
            } else {
              is_intelligible_report = false;
              break; //have to break once it's false
            }
          }
        } else {
          //Serial.println("Useful received local message is less than the usual report's size");
        }
      }
    } else {
      //Serial.println("Useful received local message is less than a report request's size");
    }     
    if( is_intelligible_report ) { //it is a report that we have received in this incoming message
      strncpy( useful_message_buff , &read_buff[ last_colon_of_incoming_message + 1 ] , 3 * Reflector_Pins_Number ); //we are using the same useful part of the incoming message to put it in the useful part of the report to send
      useful_message_buff[ 3 * Reflector_Pins_Number ] = '\0';
      //*is_intelligible_message_received_ptr = true; //although this was not an ack message but at the end it was received from the intermediate so it is well connected. But if I relied only on the ack it will still be fine though.
                                                      // Well, since I want an indication only about the newly created connection so most probably I won't receive anything but the ack (on the connection I care about I mean)
    } else {
      the_request[0].pin = '\0'; //added on 09/07/2019 - it's not necessary anyway
      if( read_char_number - 1 >= last_colon_of_incoming_message + 3 ) { //Please note that *is_intelligible_message_received_ptr is already set to false elsewhere
        if( read_buff[ last_colon_of_incoming_message + 1 ] == 'A' && read_buff[ last_colon_of_incoming_message + 2 ] == 'C' &&
            read_buff[ last_colon_of_incoming_message + 3 ] == 'K' ) {
              *is_intelligible_message_received_ptr = true;
        }
      }
    }
    //Serial.printf("was an intelligible report? %d\n", is_intelligible_report);
    return (is_intelligible_report);
  }

  boolean analyze( boolean* is_intelligible_message_received_ptr ) {
    return analyze( the_request , read_buff , read_char_number , report_part_message_buff , is_intelligible_message_received_ptr );
  }

  //there was the isJustReport method here
  /*in this particular panel where we only accept reports, no need for isJustReport*/

  static void updatePinAndEEPROM( Request* the_request ) { //it is bad to enter this method if the pcf was not connected.
    if( the_request[0].pin == '\0' ) {
      return;
    }
    for( int i = 0 ; i < Reflector_Pins_Number ; i++ ) {
      char symbol = the_request[i].pin;
      boolean state_bool = the_request[i].action;
      if( General::arrayIncludeElement( (char*) reflector_pin , Reflector_Pins_Number , symbol ) != -1 ) { //not needed I guess
        //digitalWrite( NodeMCU::getRealPinFromD( General::getIntFromHexChar( symbol ) ), state_bool );
        NodeMCU::setOutPinStateAsConsidered( General::getIntFromHexChar( symbol ) , state_bool );
        NodeMCU::encodeEEPROM( symbol , state_bool );
      }
    }
    //there was a code here related to PCF
  }
 
  //here was a method to add pins to report code (from NodeMCU and PCF)
  
  static void sendMessageToAsync( AsyncClient* async_client , char* message ) {
    Serial.printf("Sent message through remote async is %s\n", message);
    if( async_client )
      //if( *async_client )
      if( async_client->connected() )
        async_client->write( (const char*) message );
  }

  void processMessage( char* message_buff, int read_char_number, boolean* is_intelligible_message_received_ptr ) { //we have a message when we enter here
    if( getMessage( message_buff, read_char_number ) ) {//should normally be always true
      /*If we wanted to analyze many messages in the whole received buffer, then here is the place to make a loop, 
       * and we need to add to m&op a char pointer that will be modified in getMessage and used in getRequesterId.
       */
      getRequesterId();
      if( reflected_id_buff[0] != '\0' ) {//this is for authentication
        if( analyze( is_intelligible_message_received_ptr ) ) { //this is to check if message context is intelligible
          //Please note that analyze returns true in one of 2 cases, an incoming report request, or an incoming order (thus simply an incoming report)
          
          //there was an isJustReport check here
          updatePinAndEEPROM( the_request ); //this won't update anything if the incoming message was a report request

          //Now we want to send back the report
          /*if( async_client_1.connected() && async_client_2.connected() && remote_connection.most_recent_client_connect_attempt_index == 2 ) {*/
          if( remote_connection.async_client_2.connected() ) {
            sendReport( &remote_connection.async_client_2 ); //sendReport is not static because we need the reflected_id_buff and it's already in remote_server_message_op1
          }
          if( remote_connection.async_client_1.connected() ) {
            sendReport( &remote_connection.async_client_1 );
          }
        }            
      }       
    }
  }

//there was a code to send report here.
};

class Remote {
private:  
  int counter_to_ack_hi;
  int max_counter_to_ack_hi;
  boolean is_intelligible_message_received_1, is_intelligible_message_received_2;
  char last_just_connected;
  char temporary_just_connected;

  RemoteServerMessageOp remote_server_message_op; //no need for remote_server_message_op1 and remote_server_message_op2.
    
  void preProcess() { //this is called from loop()
    remote_connection.preProcess( &temporary_just_connected );
    Serial.printf("value of temporary_just_connected is %c\n", temporary_just_connected);
    
    if( temporary_just_connected == '1' || temporary_just_connected == '2' ) {
      last_just_connected = temporary_just_connected;
      counter_to_ack_hi = max_counter_to_ack_hi;
      Serial.println("Awaiting ack after sending Hi to intermediate");
    }
    
    if( temporary_just_connected == '1' ) { /*send Hi to intermediate. 
                                                * BTW we could have put this in AsynchroClient
                                                * (and frankly this may have been better) but for pure technical reasons we didn't.
                                                * Even if technically possible, we wouldn't do that is that AsynchroClient class is also used 
                                                * in InformLocal class and sendHiToServer and this is not needed there.
                                                */
      RemoteServerMessageOp::sendHiIntermediate( &remote_connection.async_client_1 );      
    } else if( temporary_just_connected == '2' ) { //also send Hi to intermediate.
      RemoteServerMessageOp::sendHiIntermediate( &remote_connection.async_client_2 );
    } 
  }

  void postProcess() {
    remote_connection.postProcess();
  }

  void preSetAckHi() {
    is_intelligible_message_received_1 = false;
    is_intelligible_message_received_2 = false;
  }

  void processAckHi() {
    if( counter_to_ack_hi > 0 ) {      
      if( ( last_just_connected == '1' && is_intelligible_message_received_1 ) || ( last_just_connected == '2' && is_intelligible_message_received_2 ) ) {
        counter_to_ack_hi = -1;
        Serial.println("Ack of HI is received");
      } else {        
        counter_to_ack_hi--;
        Serial.println("Awaiting for ack of HI");
      }
    }    
    if( counter_to_ack_hi == 0 ) {
      remote_connection.rushSocketSwitch();
      counter_to_ack_hi = max_counter_to_ack_hi; //we keep trying to establish a connection.
      Serial.println("Rushing socket connection since ack of HI is not received");
    }
    
    if( counter_to_ack_hi != -1 ) {
      toggle_connect_failure_notifier_pin = true;    
    }
    
  }  
  
public:  
  void setup() {
    //remote_connection.setAsynchroClient( "91.240.81.106" , 3552 , 3553 ); //It should had been 3558, 3559 but because the intermediate relay code is not mature it's ok for now.
    remote_connection.setAsynchroClient( "dirani.jvmhost.net" , 11360 , 11359 );
    remote_server_message_op.setup();    
    max_counter_to_ack_hi = floor( 8000 / delay_per_loop );
    counter_to_ack_hi = -1; //don't ask to rush right away
  }

  void stopOperations() {
    remote_connection.stopOperations();
    remote_server_message_op.stopOperations();
    counter_to_ack_hi = -1;
    preSetAckHi();
    last_just_connected = '\0'; /*not necessary*/
    temporary_just_connected = '\0'; /*not necessary*/
  }

  void process() {
    preProcess(); /*This will handle the preprocessing of the asynchro_client to the intermediate server. 
                    * It is obligatory to sendHiToIntermediate and this is implemented (and only needed) in RemoteServerMessageOp. 
                    * It is not needed in InformRemote because the purpose of sending "Hi" is to let the intermediate server 
                    * relay an incoming message to this NodeMCU.
                    */
    preSetAckHi();
    
    for( int incoming_message1_index = 0 ; incoming_message1_index < AsynchroClient::max_incoming_messages_number ; incoming_message1_index++ ) {    
      if( remote_connection.message_in_processing_from_client_1[ incoming_message1_index ] ) { 
        Serial.printf("So processing remote message of port 1 of buffer index %d\n", incoming_message1_index);
        remote_server_message_op.processMessage( (char*) remote_connection.buff_1[ incoming_message1_index ], 
                                                                remote_connection.len_buff_1[ incoming_message1_index ], &is_intelligible_message_received_1 );                                                                
        remote_connection.message_in_processing_from_client_1[ incoming_message1_index ] = false;
      }
    }

    for( int incoming_message2_index = 0 ; incoming_message2_index < AsynchroClient::max_incoming_messages_number ; incoming_message2_index++ ) {    
      if( remote_connection.message_in_processing_from_client_2[ incoming_message2_index ] ) { 
        Serial.printf("So processing remote message of port 2 of buffer index %d\n", incoming_message2_index);
        remote_server_message_op.processMessage( (char*) remote_connection.buff_2[ incoming_message2_index ], 
                                                                remote_connection.len_buff_2[ incoming_message2_index ], &is_intelligible_message_received_2 );                                                                
        remote_connection.message_in_processing_from_client_2[ incoming_message2_index ] = false;
      }
    }

    processAckHi();
    
    postProcess();
  }

};
