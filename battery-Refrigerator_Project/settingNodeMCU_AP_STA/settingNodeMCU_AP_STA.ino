#include "Setup.h"

const int delay_per_loop = 750;
boolean main_oper_vars_need_to_be_reset = true; /*Necessary. This won't be taken into effect unless NodeMCU connects to router*/
AP_Op AP_op;

void stopMainOperationsAndDisconnect() {/*this involves stopping and resetting the needed things to normal in case we returned back to normal operation*/
	//local_server.stopOperations();
	if( WiFi.status()== WL_CONNECTED )
		WiFi.disconnect(true); /*normally the last among them*/
	Serial.println("Normal WIFI is now disconnected");
}

void setup() {
	Serial.begin(115200);  
	NodeMCU::setPins();
	NodeMCU::beginEEPROM();
	AP_op.launch();
}

void loop() {
  if( AP_op.must_APmode_be_activated ) {
    /* must set connect_failure_notifier_pin to HIGH continuously, must setup panel to AP mode (and make it a server), then must wait for incoming client connection (initiated by the mobile app) to receive from it the SSID, password, (and optionally static IP and MAC address).
     * If nothing is received, it will keep entering here and waiting for something from the user.
     * Eventually, it will write the gotten info onto the EEPROM. And will test the connection to the router while in the AP_STA mode.
     */
    if( !first_time_switchToAPMode_lock ) {
      first_time_switchToAPMode_lock = true;
      stopMainOperationsAndDisconnect(); /*because of this method I made the lock here instead of inside Setup.h*/
      AP_op.firstTimeAP_Setup(); 
    }
    if( AP_op.checkIncomingMessageAndReturnToNormalOperation() ) { /*After this gets a (valid) message, it then, from inside,
      * connects to the router (given by the user).
      * It returns true if all was successful.
      * It is possible after getting the network configuration that the NodeMCU restarts
      * (from inside the getIncomingMessage() method) if it couldn't connect.
      */
      main_oper_vars_need_to_be_reset = true;
      first_time_switchToAPMode_lock = false;
    }
  } else {
    AP_op.check_APmode_pin();
    if( !AP_op.must_APmode_be_activated ) {/*if AP_op.must_APmode_be_activated has just been set to true
      * then all normal operations are stopped, e.g. all sockets are closed.
      */
      if( !NodeMCU::isConnectedToWiFi( !AP_op.is_APmode_button_pressed ) ) { /*Note that isConnectedToWiFi method actually tries to connect if not yet 
        * connected. I decided that : if APmode button is pressed and it's not yet connected, then I don't really want it to seriously try to connect*/ 
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

          NodeMCU::checkIfInPinsChanged(); //this has to be before inform_local and inform_remote processing
          
          //informing registered local servers
          inform_local.process(); //this mantains connections to every local server (preprocessing and postprocessing) and sends a report when an IN pin changes
          
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
  delay( delay_per_loop ); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
                                // Flooding reports to the requester may cause the requester to execute the new report before the old one (in case of multi-threading)!

}
