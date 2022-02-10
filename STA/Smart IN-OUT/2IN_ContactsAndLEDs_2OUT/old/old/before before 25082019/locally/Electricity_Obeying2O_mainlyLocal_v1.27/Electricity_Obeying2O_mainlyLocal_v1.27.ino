#include "LocalServer.h"

const int delay_per_loop = 750;
const int max_EEPROM = Start_AP_Index_In_EEPROM + Max_AP_Buffer_Size;
//D1 and D2 are naturally SCL and SDA
//D8 is reserved as connectFailureNotifierPin

int PCF::absolute_max_pins_number = Reflector_Pins_Number;

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
boolean main_oper_vars_need_to_be_reset = true;
//Remote remote; //this is about both server (receiving requests) and client (informing other entities) functionalities.
LocalServer local_server; //also a reflector, i.e. when received a report our panel replies with a report too.

AP_Op AP_op;
boolean first_time_switchToAPMode_lock = false;

void setupMainVars() {
  local_server.serverSetup(); //to receive report requests from mobile.
  //remote.setup(); 
}
void stopMainOperationsAndDisconnect() {
  local_server.stopOperations();
  //remote.stopOperations();
  if( WiFi.status()== WL_CONNECTED ) {
    WiFi.disconnect(true); /*normally the last among them*/
  }
  Serial.println("Normal WIFI is now disconnected");
  NodeMCU::yieldAndDelay();
}

void setup()
{  
  Serial.begin(115200);  
  //Serial.println();  

  NodeMCU::setPins();    
  NodeMCU::yieldAndDelay(50); //I added it just in case...      
    
  NodeMCU::lowerAllOutPins(); //this is not to let any pin floating; because the idea is to make the NodeMCU pin HIGH in order to make it into effect.
  NodeMCU::yieldAndDelay(1); 
  
  //EEPROM stuff
  //PCF1.setEEPROM_PinRange(); //comment for PCF exclusion
  NodeMCU::beginEEPROM(); //This should be after all the PCFs have used setEEPROM_PinRange() so that PCF::absolute_max_pins_number has its correct value.
  /*The EEPROM is something like "4T5F8T9TbTcFdFeF".*/
  NodeMCU::decodePinsFromEEPROM(); //this should be before the decodeEEPROM() of the PCFs. It should be the first.  
  NodeMCU::yieldAndDelay(); //delaying like 2 seconds before setting pins of PCF8574A, assuming the PCD8574A is powered from the same power supply as the Node MCU's. 
                                /*We might need more delay time if the PCF was powered from another source.*/    
  //PCF1.decodeEEPROM();  //comment for PCF exclusion
  AP_op.launch(); 
}

void loop()
{
  if( AP_op.must_APmode_be_activated ) {
    if( !first_time_switchToAPMode_lock ) {
      first_time_switchToAPMode_lock = true;
      stopMainOperationsAndDisconnect(); /*because of this method I made the lock here instead of inside Setup.h*/
      AP_op.firstTimeAP_Setup(); 
    }
    if( AP_op.checkIncomingMessageAndReturnToNormalOperation() ) {
      main_oper_vars_need_to_be_reset = true;
      first_time_switchToAPMode_lock = false;
    }
  } else {
    AP_op.check_APmode_pin();
    if( !AP_op.must_APmode_be_activated ) {
      if( !NodeMCU::isConnectedToWiFi( !AP_op.is_APmode_button_pressed ) ) {     
        if( !AP_op.is_APmode_button_pressed ) {
          stopMainOperationsAndDisconnect();
          NodeMCU::restartNodeMCU();
        }
      } else {
        if( main_oper_vars_need_to_be_reset ) { /*This is true after any first-time successful connection to router. This will ocur at the beginning
                                                 * and at the exit of the AP mode when it happens*/
          main_oper_vars_need_to_be_reset = false;
          setupMainVars();
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
           
    //run as remote server for incoming requests from remote clients (currently mobiles). Also it runs the InformRemote functionality.
        //remote.process();
    
    //run as local server for incoming requests from local clients (currently mobiles)
        local_server.process();

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
      }
    }
  }
  NodeMCU::yieldAndDelay( delay_per_loop ); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
                                // Flooding reports to the requester may cause the requester to execute the new report before the old one (in case of multi-threading)!
}
