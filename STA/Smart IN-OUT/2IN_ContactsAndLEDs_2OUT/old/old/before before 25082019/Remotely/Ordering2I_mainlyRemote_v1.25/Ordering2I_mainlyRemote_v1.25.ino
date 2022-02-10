#include "InformLocal.h"

const int delay_per_loop = 750;
//D1 and D2 are naturally SCL and SDA
//D7 is reserved as connectFailureNotifierPin

int PCF::absolute_max_pins_number = Out_Pins_Number;
//************************************************This block is for PCF
/*
const uint8_t address_PCF = 0x38;
const byte in_pins_number_PCF1 = 1;
const byte in_pin_PCF1[ in_pins_number_PCF1 ] = {100}; //the order between the two arrays in_pin_PCF1 and out_pin_PCF1 matters.
                                                 // '100' is just a fake input. For the PCF it must not be between  0 and 7
const byte out_pins_number_PCF1 = 8; //I feel it's better to dedicate the whole PCF pins to be outputs or inputs, unless otherwise successfully tested.
const byte out_pin_PCF1[ out_pins_number_PCF1 ] = {0, 1, 2, 3, 4, 5, 6, 7};  
const char out_pin_symbol_PCF1[ out_pins_number_PCF1 ] = {'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'}; //the order between the two arrays out_pin_PCF1 and out_pin_symbol_PCF1 matters.

PCF PCF1( address_PCF , in_pins_number_PCF1 , in_pin_PCF1 , out_pins_number_PCF1 , out_pin_PCF1 , out_pin_symbol_PCF1 ); //needed to be instantiated before setup(). //comment for PCF exclusion
*/
//************************************************End of block.
Remote remote; //this is about both server (receiving requests) and client (informing other entities) functionalities.
//InformLocal inform_local;
LocalServer local_server;

void setup()
{  
  Serial.begin(115200);  
  //Serial.println();  

  NodeMCU::setPins();    
  WIFI::connectionSetup(); 
  NodeMCU::yieldAndDelay(50); //I added it just in case...      
  WIFI::serverSetup();
  
  //EEPROM stuff
  NodeMCU::lowerAllOutPins(); //this is not to let any pin floating; because the idea is to make the NodeMCU pin HIGH in order to make it into effect.
  //NodeMCU::yieldAndDelay(1); 
  //PCF1.setEEPROM_PinRange(); //comment for PCF exclusion
  NodeMCU::beginEEPROM(); //This should be after all the PCFs have used setEEPROM_PinRange() so that PCF::absolute_max_pins_number has its correct value.
  /*The EEPROM is something like "4T5F8T9TbTcFdFeF".*/
  NodeMCU::decodeEEPROM(); //this should be before the decodeEEPROM() of the PCFs. It should be the first.  
  NodeMCU::yieldAndDelay(500); //delaying like 2 seconds before setting pins of PCF8574A, assuming the PCD8574A is powered from the same power supply as the Node MCU's. 
                                /*We might need more delay time if the PCF was powered from another source.*/    
  //PCF1.decodeEEPROM();  //comment for PCF exclusion
  NodeMCU::getInPinsState();  

  remote.setup(); 
//  inform_local.setup();
}

void loop()
{
  if( ! NodeMCU::isConnectedToWiFi() ) {
    NodeMCU::restartNodeMCU();
  }  
  
  toggle_connect_failure_notifier_pin = false; /*I have to set it at the beginning of each loop. This variable may be changed in 
                                                * class AsynchroClient and class InformEntity
                                                 */
  
//  PCF1.pcf8574A.getByte(); //this is to evaluate the method "isDataRead()" //comment for PCF exclusion
//  if(! PCF1.pcf8574A.isDataRead() ) { //comment for PCF exclusion
    //Serial.println("PCF is not properly connected to Node MCU");
    /*BTW later, make a special blinking for the connectFailureNotifierPin in case of an internal damage causing the PCF to be disconnected.*/
//  } else { //comment for PCF exclusion
//    NodeMCU::checkManualUpdate();    
//    PCF1.checkManualUpdate();    //comment for PCF exclusion
    
    NodeMCU::checkIfInPinsChanged(); //this has to be before inform_local and inform_remote processing
    
    //informing registered local servers
    //inform_local.process(); //this mantains connections to every local server (preprocessing and postprocessing) and sends a report when an IN pin changes
    
    //run as remote server for incoming requests from remote clients (currently mobiles). Also it runs the InformRemote functionality.
    remote.process();
    
    //run as local server for incoming requests from local clients (currently mobiles)
//    local_server.process();

    if( toggle_connect_failure_notifier_pin ) {
      NodeMCU::toggleConnectFailureNotifierPin();
    } else {
      NodeMCU::setConnectFailureNotifierPinLow(); //Comment for debugging
    }    
    
  //} //comment for PCF exclusion
  
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



