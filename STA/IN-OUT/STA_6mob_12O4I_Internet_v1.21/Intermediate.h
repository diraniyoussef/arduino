#include "wiFiClient.h"
#include "ESPAsyncTCP.h"

const int port1 = 3558; //3554; //3558;
const int port2 = 3559; //3555; //3559;

extern const int MAX_READING_BUFFER;
extern boolean is_PCF_connected;

AsyncClient async_client_1;
AsyncClient async_client_2;

IntermediateMAndOp messageAndOperation_1( &async_client_1 );
IntermediateMAndOp messageAndOperation_2( &async_client_2 );

char buff_1[ MAX_READING_BUFFER ];
int len_buff_1;
char buff_2[ MAX_READING_BUFFER ];
int len_buff_2;

//defining volatile locks
volatile boolean message_received1 = false; //I made this a lock for both clients
volatile boolean message_received2 = false;
volatile boolean message_in_processing_from_client_1 = false; //I made this a lock for async_client_1
volatile boolean message_in_processing_from_client_2 = false; //I made this a lock for async_client_2
volatile boolean just_connected_1 = false;
volatile boolean just_connected_2 = false;

class IntermediateOperation {
  //example given by  me-no-dev in https://github.com/me-no-dev/ESPAsyncTCP/issues/18
  //https://github.com/me-no-dev/ESPAsyncTCP/blob/master/src/ESPAsyncTCP.cpp
  //https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#watchdog //this may help to resolve the crash
  //In case the crash continued, check this https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
private:
  const char* IP = "91.240.81.106";  
  //const char* IP = "192.168.1.21";  
  
  int most_recent_client_connect_attempt_index = 2; //will be either 1 or 2. Maybe this is better be a union but ok.
  
  //defining counters
  int counter_to_manipulate_sockets = 0; 
  int max_counter_to_manipulate_sockets; 
  /*IMPORTANT********** 
   * It's better to make a dedicated counter_to_manipulate_sockets for client_1 and for client_2. This can prevent a weird problem
   * of a client that diconnects directly after just conneting, in effect this renews counter_to_manipulate_sockets for the same old still connected client.
   * This problem is not occuring much! Only at the beginning.
   */
  int max_counter_to_destroy_old_socket; 
  int counter_to_destroy_old_socket; 
  int max_counter_to_retry_socket_connection; 
  int counter_to_retry_socket_connection = 0; 
  int max_counter_to_force_close_socket; 
  int counter_to_force_close_socket = 0; //during execution this may reach negative values.

  boolean set_to_low_ConnectFailureNotifierPin_if_connected = false;

  void createSocket1() {
    counter_to_retry_socket_connection = max_counter_to_retry_socket_connection;
    counter_to_force_close_socket = max_counter_to_force_close_socket;
    most_recent_client_connect_attempt_index = 1;    
    async_client_1.connect( IP, port1 );    
    Serial.printf("Now connecting to intermediate on port %d\n", port1); 
  }

  void createSocket2() {         
    counter_to_retry_socket_connection = max_counter_to_retry_socket_connection;
    counter_to_force_close_socket = max_counter_to_force_close_socket;
    most_recent_client_connect_attempt_index = 2;
    async_client_2.connect( IP, port2 );
    Serial.printf("Now connecting to intermediate on port %d\n", port2); 
  }

  void setEvents1() {
    async_client_1.onConnect([](void * arg, AsyncClient * client) {
      Serial.printf("onConnect triggered for port %d. Success.\n", port1);
      //async_client_1.onError(NULL, NULL);    //not sure if it's good to uncomment it
      just_connected_1 = true;             
    }, NULL);            
  
    async_client_1.onError([](void * arg, AsyncClient * client, int error) { //may activate if an error happened even when connect(..., ...) runs.
      Serial.printf("Error number is %d for port %d\n", error, port1);
      async_client_1.stop();
    }, NULL );

    async_client_1.onDisconnect([](void * arg, AsyncClient * client) {      
      Serial.printf("out of Disconnect event for port %d\n\n\n", port1);
    }, NULL );

    async_client_1.onData([]( void * arg, AsyncClient * c, void * data, size_t len ) { //I would rather avoid setting data to anything
      /*IMPORTANT********** Since onData stupidely hangs sometimes and not receive anything then a mechanism to expect an incoming message right after the 
       * sendHiToIntermediate like within 4 seconds of waiting is necessary in order to switch socket if onData isn't responding. 
       * Will implement this in the next day maybe. It will be like testAndFixConnection
       */
      
      Serial.printf("\ninside onData for client_1\n");
      if( !message_received1 && !message_in_processing_from_client_1 ) {
        message_received1 = true;            
        if( is_PCF_connected ) {
          message_in_processing_from_client_1 = true;                              
          char* c = (char *) data; //needed maybe
          strncpy( buff_1, (const char*) c, len ); //the const cast is probably very needed
          buff_1[ len ] = '\0'; //maybe not necessary          
          len_buff_1 = len;                         
        }
        message_received1 = false;
      }        
    }, NULL); 
      
  }

  void setEvents2() {
    
    async_client_2.onConnect([](void * arg, AsyncClient * client) {            
      Serial.printf("onConnect triggered for port %d. Success.\n", port2);
      just_connected_2 = true;
    }, NULL);           
  
    async_client_2.onError([](void * arg, AsyncClient * client, int error){ 
      Serial.printf("Error number is %d for port %d\n", error, port2);
      async_client_2.stop();
    }, NULL );

    async_client_2.onDisconnect([](void * arg, AsyncClient * client) {      
      Serial.printf("out of Disconnect event for port %d\n\n\n", port2);
    }, NULL );    

    async_client_2.onData([]( void * arg, AsyncClient * c, void * data, size_t len ) { 
      Serial.printf("\ninside onData for client_2\n");
      if( !message_received2 && !message_in_processing_from_client_2 ) { 
        message_received2 = true;
        if( is_PCF_connected ) {
          message_in_processing_from_client_2 = true;
          char* c = (char*) data;
          strncpy( buff_2, (const char*) c, len ); 
          buff_2[ len ] = '\0';   
   
          len_buff_2 = len;
        }
        message_received2 = false;                   
      }        
    }, NULL);
  
  }

  void preConnectivity() { //it's about updating pin and EEPROM flaged in the AsyncClient onData
    
    if( just_connected_1 || just_connected_2 ) {
      if( set_to_low_ConnectFailureNotifierPin_if_connected ) {
        set_to_low_ConnectFailureNotifierPin_if_connected = false;
        NodeMCU::setConnectFailureNotifierPinLow(); //Comment for debugging
      }
      
      //resetting counters
      counter_to_manipulate_sockets = max_counter_to_manipulate_sockets;
      counter_to_destroy_old_socket = max_counter_to_destroy_old_socket;
      counter_to_retry_socket_connection = 0;  
      Serial.println("onConnect - counters set in preConnectivity");
    }
    if( just_connected_1 ) {
      messageAndOperation_1.sendHiIntermediate(); //the idea is that the intermediate server knows the id of this NodeMCU and registers a dedicated Printer there.
      just_connected_1 = false;
    }
    if( just_connected_2 ) {
      messageAndOperation_2.sendHiIntermediate(); 
      just_connected_2 = false;
    }
    
    if( message_in_processing_from_client_1 ) {
      if( messageAndOperation_1.getMessage( buff_1, len_buff_1 ) ) {
        messageAndOperation_1.getIdOfClient();
        if( messageAndOperation_1.mob_id_buff[0] != '\0' ) {//this is for authentication
          if( messageAndOperation_1.analyze() ) { //this is to check if message context is intelligible
            if( !messageAndOperation_1.isJustReport() ) {
              messageAndOperation_1.updatePinAndEEPROM();
            }
            NodeMCU::yieldAndDelay();
            if( async_client_1.connected() && async_client_2.connected() && most_recent_client_connect_attempt_index == 2 ) {
              messageAndOperation_1.sendReport( &async_client_2 );
            } else {
              messageAndOperation_1.sendReport( &async_client_1 );
            }
          }            
        }        
      }
      message_in_processing_from_client_1 = false;
    }
    
    if( message_in_processing_from_client_2 ) {
      if( messageAndOperation_2.getMessage( buff_2, len_buff_2 ) ) {
        messageAndOperation_2.getIdOfClient();
        if( messageAndOperation_2.mob_id_buff[0] != '\0' ) {//this is for authentication
          if( messageAndOperation_2.analyze() ) { //this is to check if message context is intelligible
            if( !messageAndOperation_2.isJustReport() ) {
              messageAndOperation_2.updatePinAndEEPROM();
            }
            NodeMCU::yieldAndDelay(); //take a breath between two successive writings. It may prevent an exception of some kind.
            if( async_client_1.connected() && async_client_2.connected() && most_recent_client_connect_attempt_index == 1 ) {
              messageAndOperation_2.sendReport( &async_client_1 );
            } else {
              messageAndOperation_2.sendReport( &async_client_2 );
            }
          }            
        }
      }
      message_in_processing_from_client_2 = false;
    }
  }
  
public:
  IntermediateOperation() {
    //following 2 counters are in effect once we're connected
    max_counter_to_manipulate_sockets = floor( 2 * 60 * 1000 / delay_per_loop );    
    counter_to_manipulate_sockets = 0;
    max_counter_to_destroy_old_socket = floor( 10000 / delay_per_loop ); //that is after having a different new socket assigned 
    counter_to_destroy_old_socket = max_counter_to_destroy_old_socket;
    //next counter is in effect once we're not connected
    max_counter_to_retry_socket_connection = floor( 20000 / delay_per_loop ); //logically this must be > timeout to create a socket
    counter_to_retry_socket_connection = 0;    
    max_counter_to_force_close_socket = floor( max_counter_to_retry_socket_connection / 2 ); //it's half the time of the retry_socket_connection counter
    //counter_to_force_close_socket = no need to set it
    
    setEvents1();
    setEvents2();
  }
  
  ~IntermediateOperation() {
    //delete[] buff_1;
    //delete[] buff_2;
  }

  void checkSocket() {        
    /*Mechanism: 
     * client_2 is firstly connected at the beginning
     * if client_1 is already connected and it's time to make a new socket with client_2 and we will keep trying with client_2 until it connects 
     */
    //Serial.printf( "%d %d %d %d %d\n", message_received, just_connected_1, just_connected_2, message_in_processing_from_client_1, 
    //                                                                                          message_in_processing_from_client_2 );
    preConnectivity();
    if( !async_client_1.connected() || async_client_1.disconnected() ) { //AFAIB disconnected() is included in the 'or'ed !connected() so we may drop the '|| disconnected()'
      if( !async_client_2.connected() || async_client_2.disconnected() ) {
        //Serial.println("toggeling notifier pin");
        NodeMCU::toggleConnectFailureNotifierPin();
        set_to_low_ConnectFailureNotifierPin_if_connected = true;
        if( counter_to_retry_socket_connection > 0 ) {
          counter_to_retry_socket_connection--; //a socket is/was connecting (it may succeed or fail)
          counter_to_force_close_socket--;
          if( counter_to_force_close_socket == 0 ) {
            if( most_recent_client_connect_attempt_index == 2 ) {
              if( async_client_2.connecting() || async_client_2.disconnecting() ) {
                Serial.println("both clients are not connected. client 2 is weird.");
                async_client_2.stop();
              }
            } else {
              if( async_client_1.connecting() || async_client_1.disconnecting() ) { 
                Serial.println("both clients are not connected. client 1 is weird.");
                async_client_1.stop();
              }
            }
          }
        } else {    
          Serial.println("both clients are not connected. So connecting.");
          if( most_recent_client_connect_attempt_index == 1 ) { //it's alternating now between client 1 and 2. So we try with the 2 ports not just with port 3555.
            createSocket2();
          } else {
            createSocket1();
          }
        }
      } else {
        if( counter_to_manipulate_sockets > 0 ) {
          counter_to_manipulate_sockets--;                
        } else { //counter_to_manipulate_sockets is now 0 and it will remain 0 until a new socket is created successfully and onConnect is triggered
          //it is totally not wise to destroy the current_socket because it's the only one left
          if( counter_to_retry_socket_connection > 0 ) {
            counter_to_retry_socket_connection--; //a socket is/was connecting (it may succeed or fail)            
            counter_to_force_close_socket--;
            if( counter_to_force_close_socket == 0 ) {
              if( async_client_1.connecting() || async_client_1.disconnecting() ) { 
                Serial.println("client 2 connected. client 1 not connected. client 1 being weird.");
                async_client_1.stop();
              }
            }
          } else {             
            createSocket1();     
          }
        }
      }   
    } else { //The following commentary is for clarification
      /*client_1 is connected*/
      if( !async_client_2.connected() || async_client_2.disconnected() ) { //AFAIB disconnected() is included in the 'or'ed !connected() so we may drop the '|| disconnected()'
        /* Is it time to try to connect client_2? */
        if( counter_to_manipulate_sockets > 0 ) {
          counter_to_manipulate_sockets--;                
        } else { 
          /* yes it is the time. Have we tried to connect client_2 before? */          
          if( counter_to_retry_socket_connection > 0 ) {
            counter_to_retry_socket_connection--; //a socket is/was connecting (it may succeed or fail)
            counter_to_force_close_socket--;
            if( counter_to_force_close_socket == 0 ) {
              if( async_client_2.connecting() || async_client_2.disconnecting() ) { 
                Serial.println("client 1 connected. client 2 not connected. client 2 being weird.");
                async_client_2.stop();
              }
            }
          } else {             
            createSocket2();     
          }          
        }         
      } else {
        /* Both clients are connected, so we need to know which one to ask to terminate after counter_to_destroy_old_socket has timed out */          
        if( counter_to_destroy_old_socket > 0 ) {
          counter_to_destroy_old_socket--;
        } else {
          if( most_recent_client_connect_attempt_index == 1 ) {
            if( !message_received2 ) {              
              if( !async_client_2.disconnecting() ) { //to ensure we enter only once
                Serial.println("2 clients connected, stopping client_2");
                async_client_2.stop(); 
              }
            }
          } else {
            if( !message_received1 ) {              
              if( !async_client_1.disconnecting() ) {
                Serial.println("2 clients connected, stopping client_1");  
                async_client_1.stop();   
              }           
            }
          }
        }        
      }      
    }   
  }
  
};

