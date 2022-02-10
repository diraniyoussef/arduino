#include "Intermediate.h"

const int delay_per_loop = 750;
//D1 and D2 are naturally SCL and SDA
//D7 is reserved as connectFailureNotifierPin

/*
//PLEASE any amendment here must be followed by an amendment in NodeMCU::setPins()
const int OUT_PINS_NUMBER = 5;
char in_pin[OUT_PINS_NUMBER]  = {'0', '1', '2', '8', 'a'}; 
char out_pin[OUT_PINS_NUMBER] = {'3', '4', '5', '6', '9'}; //'9'
*/

//Comment for debugging
//PLEASE any amendment here must be followed by an amendment in NodeMCU::setPins()
const int IN_PINS_NUMBER = 4;
const int OUT_PINS_NUMBER = 2;
char in_pin[ IN_PINS_NUMBER ] = { '0', '3', '4', '5'}; 
//char out_pin[OUT_PINS_NUMBER] = { '6', '8', '9', 'a' }; //'8' is better off as output since I set it to low before restarting and user may press on it while restarting which will physically set it to HIGH thus damaging it
char out_pin[OUT_PINS_NUMBER] = { '6', '8'}; //'8' is better off as output since I set it to low before restarting and user may press on it while restarting which will physically set it to HIGH thus damaging it

int PCF::absolute_max_pins_number = OUT_PINS_NUMBER;
boolean is_PCF_connected = true;
//************************************************This block is for PCF
uint8_t address_PCF = 0x38;
const byte in_pins_number_PCF1 = 1;
byte in_pin_PCF1[ in_pins_number_PCF1 ] = {100}; /*the order between the two arrays in_pin_PCF1 and out_pin_PCF1 matters.
                                                  '100' is just a fake input. For the PCF it must not be between  0 and 7
*/
const byte out_pins_number_PCF1 = 8; //I feel it's better to dedicate the whole PCF pins to be outputs or inputs, unless otherwise successfully tested.
byte out_pin_PCF1[ out_pins_number_PCF1 ] = {0, 1, 2, 3, 4, 5, 6, 7};  
char out_pin_symbol_PCF1[ out_pins_number_PCF1 ] = {'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'}; //the order between the two arrays out_pin_PCF1 and out_pin_symbol_PCF1 matters.
//************************************************End of block.

PCF PCF1( address_PCF , in_pins_number_PCF1 , in_pin_PCF1 , out_pins_number_PCF1 , out_pin_PCF1 , out_pin_symbol_PCF1 ); //needed to be instantiated before setup().
wiFiClient client_[ MAX_SRV_CLIENTS ][ SERVERS_NUMBER_TO_MAINTAIN_CONNECTION ];
IntermediateOperation intermediate_obj;

void setup()
{  
  Serial.begin(115200);  
  //Serial.println();  

  NodeMCU::setPins();    
  WIFI::connectionSetup(); 
  NodeMCU::yieldAndDelay(50);//I added it just in case...      
  WIFI::serverSetup();
  
  //EEPROM stuff
  NodeMCU::lowerAllOutPins(); //this is not to let any pin floating; because the idea is to make the NodeMCU pin HIGH in order to make it into effect.
  NodeMCU::yieldAndDelay(1); 
  PCF1.setEEPROM_PinRange();
  NodeMCU::beginEEPROM(); //This should be after all the PCFs have used setEEPROM_PinRange() so that PCF::absolute_max_pins_number has its correct value.
  /*The EEPROM is something like "4T5F8T9TbTcFdFeF".*/
  NodeMCU::decodeEEPROM(); //this should be before the decodeEEPROM() of the PCFs. It should be the first.  
  NodeMCU::yieldAndDelay(2000); //delaying like 2 seconds before setting pins of PCF8574A, assuming the PCD8574A is powered from the same power supply as the Node MCU's. 
                                /*We might need more delay time if the PCF was powered from another source.*/    
  PCF1.decodeEEPROM();  
}

void loop()
{
  if( ! NodeMCU::isConnectedToWiFi() ) {        
    NodeMCU::restartNodeMCU();
  }  
  
  is_PCF_connected = true;
  PCF1.pcf8574A.getByte(); //this is to evaluate the method "isDataRead()"
  if(! PCF1.pcf8574A.isDataRead() ) {
    //Serial.println("PCF is not properly connected to Node MCU");
    is_PCF_connected = false;
    /*BTW later, make a special blinking for the connectFailureNotifierPin in case of an internal damage causing the PCF to be disconnected.*/
  } else {
//    NodeMCU::checkManualUpdate();    
//    PCF1.checkManualUpdate();    
    intermediate_obj.checkSocket(); //keep it after evaluating is_PCF_connected
    
    boolean clientToBeCreated = false;
    wiFiClient* theClientObj;
    WiFiClient* theClient;
    
    for(int server_i = 0; server_i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; server_i++) {
      for( int client_i = 0; client_i < MAX_SRV_CLIENTS; client_i++ ) {
        theClientObj = &client_[client_i][server_i]; //for reading simplicity. Operator precedence is respected.
        theClient = &theClientObj->theClient;
        //Serial.printf("client_i = %d and server_i = %d\n", client_i, server_i);
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
            if ( theClientObj->counterToAssignANewClient == theClientObj->max_loop_counter_to_assign_a_new_client ) { 
              if ( theClientObj->messageAndOperation->mob_id_buff[0] != '\0' ) { //was never processed before              
                theClientObj->messageAndOperation->sendReport();
              }
              theClient->stop();//hopefully that when destroying it from the server, the client would be notified in his Android code
              clientToBeCreated = true;
            }
          }        
        }
  
        if ( clientToBeCreated ) {
          //resetting stuff related to messageAndOperation field is up to that class itself.
          theClientObj->counterToAssignANewClient = 0; 
          theClientObj->messageAndOperation->mob_id_buff[0] = '\0';
          //trying to get a new client.
          *theClient = server[server_i].available();               
        }
      } //end for
    } //end for
    
    NodeMCU::yieldAndDelay(50);
    //Now listen to clients and process the ones that sent something     
    for(int server_i = 0; server_i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; server_i++) {
      for(int client_i = 0; client_i < MAX_SRV_CLIENTS; client_i++) {
        theClientObj = &client_[client_i][server_i]; //for reading simplicity.
        theClient = &theClientObj->theClient;
        //Serial.printf("client_i = %d and server_i = %d\n", client_i, server_i);    
        
        if( *theClient ) {
          if( theClient->connected() ) {
            if( theClientObj->messageAndOperation->getMessage() ) {
              //process received message           
              boolean processClient = false;
              if ( theClientObj->messageAndOperation->mob_id_buff[0] == '\0' ) { //was never processed before
                theClientObj->messageAndOperation->getIdOfClient( ); //this also checks if the received message is valid in terms of signature
                
                boolean client_to_be_deleted = false;
                if ( theClientObj->messageAndOperation->mob_id_buff[0] == '\0' ) { //this happens if the method setIdOfClient found that the mobile does not belong to the owner's family                
                  //Serial.println("local client to be deleted after mob id not being recognized");
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
                    processClient = true;                             
  //                }
                }
                if ( client_to_be_deleted ) {
                  //Serial.println("this local socket's message was not suitable to come from a member of the family");
                  theClient->stop();
                  theClientObj->counterToAssignANewClient = 0;                
                }
              } else {
                processClient = true;
              }
              
              if ( processClient && is_PCF_connected ) { //As for is_PCF_connected, the method sendReport() is actually protected from inside, so is_PCF_connected actually provides
                                                          // protection to updatePinAndEEPROM()
                //Serial.println("this local socket is to be processed");
                NodeMCU::yieldAndDelay();
                theClientObj->messageAndOperation->sendAck();                 
                if( theClientObj->messageAndOperation->analyze() ) {
                  if (!theClientObj->messageAndOperation->isJustReport()) {
                    NodeMCU::yieldAndDelay(); //take a breath between two successive writings
                    theClientObj->messageAndOperation->updatePinAndEEPROM();
                  }
                  NodeMCU::yieldAndDelay();
                  theClientObj->messageAndOperation->sendReport();
                }
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
  

    /* //UNCOMMENT THIS IN CASE OF PCF
  int pcf_check_interval = 5; //each 5 ms refresh pcf.  
  for( int i = 0 ; i < floor( delay_per_loop / pcf_check_interval ) ; i++ ) { // 18 loops x 40 ms = 720 ms which is about 750 ms
    NodeMCU::yieldAndDelay( pcf_check_interval ); //40 ms (instead of 5 ms) is probably noticeable
    
    PCF1->getStateFromEEPROM_AndRefresh(0); //I don't care about getting the state from the EEPROM, but refresh is what I care about.
    */

    /*This is really needed because I don't know another way to tell if it's really connected or not.
    * For a critical application, shutting down a pin for 40 ms because the PCF8574A was like shaked is something BAD.
    * Please note the of the EEPROM were made of inputs (thus 8 input pins) then we may not need the getStateFromEEPROM_AndRefresh(...) method, plus its implementation 
    * would need t if all the pinsmodification.
    */
  
  NodeMCU::yieldAndDelay( delay_per_loop ); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
                                // Flooding reports to the requester may cause the requester to execute the new report before the old one (in case of multi-threading)!
}



