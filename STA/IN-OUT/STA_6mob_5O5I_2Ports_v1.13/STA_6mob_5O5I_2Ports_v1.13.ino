#include "wiFiClient.h"

char includedPinIndexAsChar[5] = {'3', '4', '5', '6', '9'}; //please put them here in the same order as in the app. It's nicer but not necessary.
//NOTE that these chars must  not be 'T' or 'F' because these two chars are used in the EEPROM.

//char includedPinIndexAsChar[4] = {'3', '4', '5', '6'}; //for the sake of debugging using the serial monitor you must free '9'

Clients* clients;

void setup()
{  
  Serial.begin(115200);
  Serial.println();  
  
  pinMode(D0, INPUT);  
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D7, INPUT);
  pinMode(D8, INPUT);
  
  pinMode(D3, OUTPUT);  
  pinMode(D4, OUTPUT);  
  pinMode(D5, OUTPUT);  
  pinMode(D6, OUTPUT);
  pinMode(D9, OUTPUT);  

  pinMode(D10, OUTPUT);  
    
  NodeMCU::wifiSetup(); //if not connected to WiFi, NodeMCU will restart.
  //EEPROM stuff
  //should fetch the EEPROM to set the pins according to most recent known state (e.g. before last restart).
  NodeMCU::lowerAllOutPins();//this is not to let any pin floating.
  NodeMCU::yieldAndDelay(1); 
  NodeMCU::beginEEPROM(); 
  NodeMCU::decodeEEPROM(); //this determines the state of the pins.  

  clients = new Clients(); //Please don't changle the locaction of this statement...
                            //clients must not be defined (instantiated) right at the declaration because this (instantiation) statement is preceded with 
                            // NodeMCU::wifiSetup(); which may restart the NodeMCU and I can't in NodeMCU class delete "clients" because "Clients" class 
                            // is not recognized in the _Generic module.
}

//If for a reason the mobile app closed a socket, will it get a report from the NodeMCU? NO. If the NodeMCU wanted to close a socket, it will send a report before 
// closing it.
//If a client is now connected, will the NodeMCU send a report? Yes, BUT according to the normal operation and not automatically, I mean the mobile app will either
// ask for a report or control an output and accordingly the NodeMCU will send a report.
void loop()
{
  if( !NodeMCU::isConnectedToWiFi() ) {        
    Serial.println("deleting and restarting in loop.");
    delete clients;    
    NodeMCU::restartNodeMCU();
  }
  
  NodeMCU::checkManualUpdate();
  
  boolean clientToBeCreated = false;
  wiFiClient* theClientObj;
  WiFiClient* theClient;
  
  for(int server_i = 0; server_i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; server_i++) {
    for(int client_i = 0; client_i < MAX_SRV_CLIENTS; client_i++) {
      theClientObj = &clients->client_[client_i][server_i]; //for reading simplicity. Operator precedence is respected.
      theClient = theClientObj->theClient;
      Serial.printf("client_i = %d and server_i = %d\n", client_i, server_i);    
      clientToBeCreated = false;

      //This "if" block checks if the client is to be nulled, and increments the client's freeing-resource-counter.
      if( !(*theClient) ) { //theClient is already instantiated when clients was.
        clientToBeCreated = true;        
      } else {
        if ( !theClient->connected() ) {
          //not sure if wise to do ...->theClient.stop() //I think it's not useful at all.
          clientToBeCreated = true;           
        } else {
          //Serial.println("It's connected again");
          theClientObj->counterToAssignANewClient++;
          if ( theClientObj->counterToAssignANewClient == theClientObj->maxLoopCounterToAssignANewClient ) { //hopefully we won't be here since 
                  //the mobile app will intentionally destroy the socket (after it had created a new one) before maxLoopCounterToAssignANewClient is reached.
            //Now send a report anyway then close. This may be useful since it is logical before intentionally closing a socket.
            sendReport(theClientObj);//we already checked that it's connected...
            theClient->stop();//hopefully that when destroying it from the server, the client would be notified in his Android code
            clientToBeCreated = true;
          }
        }        
      }

      if ( clientToBeCreated ) {
        theClientObj->counterToAssignANewClient = 0;                       
        theClientObj->id = "";
        //get a new client if possible
        Serial.println("trying to link to a new socket, since this socket is considered old.");
        *theClient = server[server_i].available();               
      }
    } //end for
  } //end for
  
  NodeMCU::yieldAndDelay(50);
  //Now listen to clients and process the ones that sent something     
  for(int server_i = 0; server_i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; server_i++) {
    for(int client_i = 0; client_i < MAX_SRV_CLIENTS; client_i++) {
      theClientObj = &clients->client_[client_i][server_i]; //for reading simplicity.
      theClient = theClientObj->theClient;
      Serial.printf("client_i = %d and server_i = %d\n", client_i, server_i);    
      
      if (*theClient){
        if (theClient->connected()) {
          if (theClient->available()) {
            //process received message           
            theClientObj->messageAndOperation = new MessageAndOperation( theClient, true ); //message is gotten in this statement.
            boolean processClient = false;
            if ( theClientObj->id == "" ) { //was never processed before
              String id = theClientObj->messageAndOperation->getIdOfClient();
              Serial.printf("id was: %s\n", id.c_str() );
              boolean client_to_be_deleted = false;
              if ( id == "" ) { //this happens if the method setIdOfClient found that the mobile does not belong to the owner's family
                client_to_be_deleted = true;
              } else { 
/*                if ( clients->idPreExist( server_i, id ) ){ //it should not exist unless the user somehow installed the app on another mobile.
                                                            // This is not effective as a security measure since the mobile app will enhance the connection 
                                                            //  by going to the other server (the other port) so the 2 mobiles (with the same id) will function
                                                            // simultaneously on 2 servers.
                  client_to_be_deleted = true;
                } else { 
                  */
                  //accepted as a client
                  theClientObj->id = id;
                  processClient = true;                             
//                }
              }
              if ( client_to_be_deleted ) {
                Serial.println("this socket's message was not suitable to come from a member of the family");
                theClient->stop();
                theClientObj->counterToAssignANewClient = 0;
              }
            } else {
              processClient = true;
            }
            
            if ( processClient ) {
              Serial.println("this socket is to be processed");
              theClientObj->messageAndOperation->sendAck();
              if (theClientObj->messageAndOperation->analyze()) {
                if (!theClientObj->messageAndOperation->isJustReport()) {
                  NodeMCU::yieldAndDelay();
                  theClientObj->messageAndOperation->updatePinsAndEEPROM();
                }
                theClientObj->messageAndOperation->sendReport();
              }
            }

            NodeMCU::yieldAndDelay(5);
            delete (theClientObj->messageAndOperation);            
          } else {
            //Serial.println("Nothing available from first client.");
          }          
        } else {
          //Serial.println("Client was once connected but not now.\n");      
        }
      } else {
        //Serial.println("Client was never connected.\n");      
      }
    }
  }    
      
  NodeMCU::yieldAndDelay(750); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
                                // Flooding reports to the requester may cause the requester to execute the new report before the old one (in case of multi-threading)!
//NodeMCU::yieldAndDelay(2000);//for debugging
}

void sendReport(wiFiClient* theClientObj){
  //assuming messageAndOperation is not defined or was deleted
  theClientObj->messageAndOperation = new MessageAndOperation(theClientObj->theClient);//reading_buff and readCharNumber are dummy here 
                                                                                        //because the intent is just to send a report
                                                                                        // Also you must not use the method sendAck!
  Serial.println("Sending only report to allow a slot for a new mobile.");
  theClientObj->messageAndOperation->sendReport();
  Serial.println("now deleting messageAndOperation");
  delete theClientObj->messageAndOperation;
}


