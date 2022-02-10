#include "wiFiClient.h"

//D1 and D2 are SCL and SDA
//D10 is reserved as connectFailureNotifierPin
/*
const int OUT_PINS_NUMBER = 4;
byte in_pin[4] = { 0 , 5 , 6 , 7 }; //order is in coherence with the out pins array(s).
char out_pin_symbol[ OUT_PINS_NUMBER ] = {'3', '4', '8', '9'}; //please put them here in the same order as in the app. It's nicer but not necessary.
//NOTE that these chars must  not be 'T' or 'F' because these two chars are used in the EEPROM.
*/
const int OUT_PINS_NUMBER = 3;
byte in_pin[OUT_PINS_NUMBER] = { 0 , 5 , 6 }; 
char out_pin_symbol[OUT_PINS_NUMBER] = {'3' , '4' , '8'}; //for the sake of debugging using the serial monitor you must free '9'

int PCF::absolute_max_pins_number = OUT_PINS_NUMBER;
//************************************************This block is for PCF
uint8_t address_PCF = 0x38;
const byte in_pins_number_PCF1 = 1;
byte in_pin_PCF1[ in_pins_number_PCF1 ] = {0}; //it's not really an input pin, it's dummy. I just left it in order not to make any change to the PCF class.
const byte out_pins_number_PCF1 = 8;
byte out_pin_PCF1[ out_pins_number_PCF1 ] = {0, 1, 2, 3, 4, 5, 6, 7}; 
char out_pin_symbol_PCF1[ out_pins_number_PCF1 ] = {'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'}; //the order between the two arrays out_pin_PCF1 and out_pin_symbol_PCF1 matters.
//************************************************End of block.

PCF* PCF1 = new PCF( address_PCF , in_pins_number_PCF1 , in_pin_PCF1 , out_pins_number_PCF1 , out_pin_PCF1 , out_pin_symbol_PCF1 ); //needed to be instantiated before setup().
Clients* clients;

void setup()
{  
  Serial.begin(115200);
  Serial.println();  
  
  NodeMCU::setPins();    
  NodeMCU::wifiSetup( PCF1 ); //if not connected to WiFi, NodeMCU will restart. PCF1 handed as an argument is just to delete it there if decided to restart.
  //EEPROM stuff
  //should fetch the EEPROM to set the pins according to most recent known state (e.g. before last restart).
  Serial.println("before lowering all out pins");
  NodeMCU::lowerAllOutPins();//this is not to let any pin floating; because the idea is to make the NodeMCU pin HIGH in order to make it into effect.
  NodeMCU::yieldAndDelay(1); 
  PCF1->setEEPROM_PinRange();
  NodeMCU::beginEEPROM(); //this should be after all the PCFs have used setEEPROM_PinRange() so that PCF::absolute_max_pins_number has its correct value.
                          //The EEPROM is something like "4T5F8T9TbTcFdFeF".
  NodeMCU::decodeEEPROM(); //this should be before the decodeEEPROM() of the PCFs. It should be the first.  
  NodeMCU::yieldAndDelay(2000); //delaying like 2 seconds before setting pins of PCF8574A, assuming the PCD8574A is powered from the same power supply as the Node MCU's. 
                                /*We might need more delay time if the PCF was powered from another source.*/    
  PCF1->decodeEEPROM();

  clients = new Clients(); //Please don't change the locaction of this statement...
                            //clients must not be defined (instantiated) right at the declaration because this (instantiation) statement is preceded with 
                            // NodeMCU::wifiSetup(); which may restart the NodeMCU and I can't in NodeMCU class delete "clients" because "Clients" class 
                            // is not recognized in the _Generic module.
  Serial.println("end of setup()");
}

//If for a reason the mobile app closed a socket, will it get a report from the NodeMCU? NO. If the NodeMCU wanted to close a socket, it will send a report before 
// closing it.
//If a client is now connected, will the NodeMCU send a report? Yes, BUT according to the normal operation and not automatically, I mean the mobile app will either
// ask for a report or control an output and accordingly the NodeMCU will send a report.
void loop()
{
  if( ! NodeMCU::isConnectedToWiFi() ) {        
    Serial.println("deleting and restarting in loop.");
    delete clients;    
    delete PCF1;
    NodeMCU::restartNodeMCU();
  }
  
  NodeMCU::checkManualUpdate();
  boolean is_PCF_connected = true;
/*//Don't want to use PCF1->checkManualUpdate() since it is involved with input pins.
  if(! PCF1->checkManualUpdate() ) { 
    //Serial.println("Disabling change through mobiles because the PCF is not properly connected to Node MCU");
    is_PCF_connected = false;
  }
  */
  PCF1->pcf8574A.getBit( out_pin_PCF1[0] ); 
  if(! PCF1->pcf8574A.isDataRead() ) {
    is_PCF_connected = false;
  }  
  boolean clientToBeCreated = false;
  wiFiClient* theClientObj;
  WiFiClient* theClient;
  
  for(int server_i = 0; server_i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; server_i++) {
    for( int client_i = 0; client_i < MAX_SRV_CLIENTS; client_i++ ) {
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
            
            if ( processClient && is_PCF_connected ) { //As for is_PCF_connected, the method sendReport() is actually protected from inside, so is_PCF_connected actually provides
                                                        // protection to updatePinAndEEPROM()
              Serial.println("this socket is to be processed");
              theClientObj->messageAndOperation->sendAck();
              if( theClientObj->messageAndOperation->analyze() ) {
                if (!theClientObj->messageAndOperation->isJustReport()) {
                  NodeMCU::yieldAndDelay();
                  theClientObj->messageAndOperation->updatePinAndEEPROM();
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
      
  for( int i = 0 ; i < 18 ; i++ ) { // 18 x 40 = 720 which is about 750 ms
    NodeMCU::yieldAndDelay(40); //If I want some more accurracy I can delay 5 ms instead of 40 ms.
    PCF1->getStateFromEEPROM_AndRefresh(0); //I don't care about getting the state from the EEPROM, but refresh is what I care about.
    /*This is really needed because I don't know another way to tell if it's really connected or not.
    * For a critical application, shutting down a pin for 40 ms because the PCF8574A was like shaked is something BAD.
    * Please note tha of the EEPROM were made of inputs (thus 8 input pins) then we may not need the getStateFromEEPROM_AndRefresh(...) method, plus its implementation 
    * would need t if all the pinsmodification.
    */
  }
//  NodeMCU::yieldAndDelay(750); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
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

