#include "Remote.h"

class LocalMessageOp {
private:   
  //by the way another module belonging to the same owner can have its own set of mobiles.
  //const char* owner_id_buff = "Youssef_70853721:\0";
  //const char* mod_id_buff = "5:\0";
  static const int Reflected_Panels_Number = 3;   //only 1 panel is the informer (reflected)
  const char* reflected_i_Id_buff[ Reflected_Panels_Number ] = { "S4:\0", "TrekStor_Tab:\0", "S7_Edge:\0"  }; //e.g.(reflected_id_buff + 2)[4] will the 'M' ???  Local_Server_Number
  /*Note that these are not panels here for this panel's usage. We should have something like a dedicated array for the mobiles and for the ordering panels,
   * especially that you expect to have a specific incoming message content from each. But it's ok for now.
   */  
  char read_buff[ Max_Reading_Buffer ];
  int read_char_number;
  char report_part_message_buff[ Max_Reading_Buffer ]; //this is the report part, not the signature.
  Operation oper; 
  WiFiClient* aClient;    

  void sendReport() {
    NodeMCU::yieldAndDelay(); //I think this delay may be useful...
    int total_message_length = strlen( (const char*) owner_id_buff ) + strlen( (const char*) mod_id_buff ) + 
                                strlen( (const char*) reflected_id_buff) + strlen( (const char*) report_part_message_buff ) + 1; //the + 1 is to add a \0 to it.
    char* totalMessage_buff = new char[ total_message_length ];
    strcpy( totalMessage_buff, (const char*) owner_id_buff );
    strcat( totalMessage_buff, (const char*) mod_id_buff );
    strcat( totalMessage_buff, (const char*) reflected_id_buff );
    strcat( totalMessage_buff, (const char*) report_part_message_buff );

    totalMessage_buff[ total_message_length - 1 ] = '\0';
    Serial.printf("Local Server: sending report. Message is: %s\n", totalMessage_buff );
    
    if( *aClient ) {
      if( aClient->connected() ) {
        aClient->write( (const uint8_t*) totalMessage_buff , total_message_length );
        aClient->flush();
      }
    }
    delete[] totalMessage_buff;
  }

  //there was the isJustReport method here
  /*in this particular panel where we only accept reports, no need for isJustReport*/

public:
  char reflected_id_buff[ Max_Reading_Buffer ]; 
  Request the_request[ Reflector_Pins_Number ];   

  void setWiFiClient( WiFiClient* aClient ) {
    this->aClient = aClient;
    oper.setOperInfo( owner_id_buff, Reflected_Panels_Number, reflected_i_Id_buff, mod_id_buff );
  }
  
  boolean getMessage() {    
    if( aClient->available() ) { //This will always be true, nevertheless it's good to protect it
      read_char_number = aClient->readBytesUntil('\0', read_buff, Max_Reading_Buffer); //BTW the '\0' is not counted in the value of read_char_number - tested
      read_buff[ Max_Reading_Buffer - 1 ] = '\0';
      //Serial.printf("Read char number of the incoming message is %d\n", read_char_number);
      NodeMCU::yieldAndDelay();
      return true;               
    } else {
      return false;
    }
  }  

  void getIdOfClient() {    
    reflected_id_buff[0] = '\0';
    //Serial.println("Trying to get the id of local incoming message");
    oper.getIdOfClient( reflected_id_buff, read_buff );
  }  

  boolean analyze() { //this method returns true if the received message can be understood    
    return RemoteServerMessageOp::analyze( the_request , read_buff , read_char_number, report_part_message_buff );    
  }
  
  void processMessage() {    
    if( analyze() ) {
      //There was a check here about isJustReport()        
      NodeMCU::yieldAndDelay();      
      RemoteServerMessageOp::updatePinAndEEPROM( the_request ); //it is possible that no pin will be updated if the incoming message was in the first place simply a report request. This method updatePinAndEEPROM takes care of this
      sendReport();
    } else {
      Serial.println("After analysis the result was bad");
    }
  }
  
};

class LocalIncomingClient {
private:
  const int max_counter_to_assign_a_new_client = 3.5 * 60 * 1000;
  
public:
  int max_loop_counter_to_assign_a_new_client;
  WiFiClient theClient;
  LocalMessageOp m_and_op;
  //boolean just_connected;

  LocalIncomingClient() {
    max_loop_counter_to_assign_a_new_client = floor( max_counter_to_assign_a_new_client / delay_per_loop );//it's like 250 times for a delay of 750 ms. 
    m_and_op.setWiFiClient( &theClient );
  }

  int counter_to_assign_a_new_client = 0;
  
  void reset() {
    counter_to_assign_a_new_client = 0; 
    m_and_op.reflected_id_buff[0] = '\0';
  }
};

class LocalServer {
private:
  static const int Max_Concurrent_Clients = 4;
  static const int Local_Server_Number = 2; //This will remain 2 in my convention. Never changed
  WiFiServer server[Local_Server_Number] = {
    WiFiServer(3552),
    WiFiServer(3553)
  };
  LocalIncomingClient client_[ Max_Concurrent_Clients ][ Local_Server_Number ];  

  void stopAllClientsOnPort( int port_i ) {
    LocalIncomingClient* theClientObj;
    WiFiClient* theClient;
    for( int client_i = 0 ; client_i < Max_Concurrent_Clients ; client_i++ ) {
      theClientObj = &client_[client_i][port_i]; //for reading simplicity.
      theClient = &theClientObj->theClient;
      theClientObj->reset();
      if( *theClient ) {
        if( theClient->connected() ) {
          client_[client_i][port_i].theClient.flush();
          NodeMCU::yieldAndDelay();
          client_[client_i][port_i].theClient.stop();        
        }
      }
      //client_[client_i][port_i].just_connected = false;
    }
    NodeMCU::yieldAndDelay(50);
  }

  void restartServer( int port_i ) {
    server[ port_i ].stop(); 
    NodeMCU::yieldAndDelay(250); 
    server[ port_i ].begin();    
    server[ port_i ].setNoDelay(true);
    NodeMCU::yieldAndDelay(50);  
  }
  
public:
  void serverSetup() {          
    for ( int i = 0; i < Local_Server_Number; i++ ) {
      server[i].begin();    
      server[i].setNoDelay(true); //I think everything you send is directly sent without the need to wait for the buffer to be full
      NodeMCU::yieldAndDelay(1000);    
    }
  }
  
  void process() {    
    boolean clientToBeCreated = false;
    LocalIncomingClient* theClientObj;
    WiFiClient* theClient;

    for( int port_i = 0 ; port_i < Local_Server_Number ; port_i++ ) {
      for( int client_i = 0 ; client_i < Max_Concurrent_Clients ; client_i++ ) {
        theClientObj = &client_[client_i][port_i]; //for reading simplicity. Operator precedence is respected.
        
        theClient = &theClientObj->theClient;
        //Serial.printf("client_i %d on port_i %d\n", client_i, port_i);
        clientToBeCreated = false;
  
        //This "if" block checks if the client is to be nulled, and increments the client's freeing-resource-counter.
        if( !(*theClient) ) { //theClient is already instantiated when clients was.           
          clientToBeCreated = true;
          Serial.printf( "client_i %d on port_i %d was never connected.\n" , client_i , port_i );
          /*
          if( theClientObj->just_connected ) { //weird behavior
            NodeMCU::yieldAndDelay();
            stopAllClientsOnPort( port_i );            
            Serial.printf("Now restarting the server (listener) on port %d after done stopping all non-null clients on this port\n", port_i );                           
            restartServer( port_i ); //this is our intention...             
          }
          */
        } else {
          //theClientObj->just_connected = false;
          if ( !theClient->connected() ) { //Not sure if we can pass into this at all because when it's disconnected from the client *theClient evaluates to false. 
            //Not sure if wise to do ...->theClient.stop() //I think it's not useful at all.            
            clientToBeCreated = true;           
            Serial.printf("client_i %d on port_i %d was once connected but not now.\n", client_i, port_i); //We never enter here I believe
          } else {
            Serial.printf("client_i %d on port_i %d is now connected.\n", client_i, port_i);
            //Serial.println("It's connected again");
            theClientObj->counter_to_assign_a_new_client++;
            if ( theClientObj->counter_to_assign_a_new_client == theClientObj->max_loop_counter_to_assign_a_new_client ) { 
              //There used to be here a piece of code to send a report just before a socket closes due to socket lifetime ending. Here we don't need it.
              Serial.printf("client_i %d on port_i %d will be stopped to free resources.\n", client_i, port_i);
              theClient->stop();//hopefully that when destroying it from the server, the client would be notified in his Android code
              clientToBeCreated = true;
            }
          }
        }
  
        if ( clientToBeCreated ) {
          //resetting stuff related to m_and_op field is up to that class itself.
          theClientObj->reset();  
          //It is ok not to reset m_and_op because all its member variables can be overriden just fine.
          
          //Now trying to get a new client.
          *theClient = server[ port_i ].available(); //please note that this actually sets *theClient to return false (or maybe null) when tested later in an "if" e.g.          
          if( *theClient ) {
            if( theClient->connected() ) {
              //theClientObj->just_connected = true;
            }
          }
        }
      } //end for
    } //end for
    
    NodeMCU::yieldAndDelay(50);
    //Now listen to clients and process the ones that sent something     
    for(int port_i = 0; port_i < Local_Server_Number; port_i++) {
      for(int client_i = 0; client_i < Max_Concurrent_Clients; client_i++) {
        theClientObj = &client_[client_i][port_i]; //for reading simplicity.
        theClient = &theClientObj->theClient;
        //Serial.printf("client_i %d on port_i %d\n", client_i, port_i);    
        
        if( *theClient ) {
          if( theClient->connected() ) {
            Serial.printf("client_i %d on port_i %d is always connected.\n", client_i, port_i);
            if( theClientObj->m_and_op.getMessage() ) { //if true then an actual message is received now.
              Serial.printf("A message is received from client_i %d on port_i %d\n.", client_i, port_i);
              //process received message           
              boolean processClient = false;
              /*
               * BTW, according to the following code block of authentication through the message signature, the authentication process
               * is only made through the first message and not through every message. This is fine for now though.
               */
              if ( theClientObj->m_and_op.reflected_id_buff[0] == '\0' ) { //was never processed before or was processed then deleted
                theClientObj->m_and_op.getIdOfClient( ); //this also checks if the received message is valid in terms of signature
                Serial.println("Attempt to get Id of client is done by now.");
                boolean client_to_be_deleted = false;
                if ( theClientObj->m_and_op.reflected_id_buff[0] == '\0' ) { //this happens if the method setIdOfClient found that the mobile does not belong to the owner's family                
                  //Serial.println("local client to be deleted after mob id not being recognized");
                  Serial.printf("id failure for message so client will be deleted/freed.\n");
                  client_to_be_deleted = true;
                } else { 
  /*                if ( clients->idPreExist( port_i, id ) ){ //it should not exist unless the user somehow installed the app on another mobile.
                                                              // This is not effective as a security measure since the mobile app will enhance the connection 
                                                              //  by going to the other server (the other port) so the 2 mobiles (with the same id) will function
                                                              // simultaneously on 2 servers.
                    client_to_be_deleted = true;
                  } else { 
  */
                    //accepted as a client
                    Serial.println("Id of client is fine so processing message.");                
                    processClient = true;                             
  //                }
                }
                if ( client_to_be_deleted ) {
                  Serial.println("Client was better to be deleted!");
                  
                  theClient->stop();
                  theClientObj->reset();              
                }
              } else {
                processClient = true;
                Serial.println("Processing message.");  
              }
              
              if ( processClient ) {
                //Serial.println("this local socket is to be processed");
                theClientObj->m_and_op.processMessage();
              }
              NodeMCU::yieldAndDelay(5);
            } else {
              //Serial.println("Nothing available from local client.");
            }          
          } else {
            //Serial.println("Client was once connected but not now.\n");      
          }
        } else {
          //Serial.println("Client was never connected.\n");      
        }
      }
    }
  }

  void stopOperations() {    
    for( int port_i = 0 ; port_i < Local_Server_Number ; port_i++ ) {
      stopAllClientsOnPort( port_i );
      server[ port_i ].stop(); /*tested without problem if we enter this without beginning the server beforehand. Although it won't happen anyway.*/
      NodeMCU::yieldAndDelay(200);
    }
  }

  
};
