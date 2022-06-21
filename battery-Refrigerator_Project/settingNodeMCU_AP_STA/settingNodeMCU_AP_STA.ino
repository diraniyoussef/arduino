#include "Main.h"

const int delay_per_loop = 750;
boolean main_oper_vars_need_to_be_reset = true; /*Necessary. This won't be taken into effect unless NodeMCU connects to router*/
AP_Op AP_op;
boolean first_time_switchToAPMode_lock = false;

void setupMainVars() {
  //setting up main operations variables

}

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
        toggle_connect_failure_notifier_pin = false; /*I have to set it at the beginning of each loop. This variable may be changed in Main*/        
				
				//doing all the work you want here below



					
      
				if( toggle_connect_failure_notifier_pin ) {
					NodeMCU::toggleConnectFailureNotifierPin();
				} else {
					NodeMCU::setConnectFailureNotifierPinLow(); //Comment for debugging
				}    
      }
    }
  }
  delay( delay_per_loop ); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
                                // Flooding reports to the requester may cause the requester to execute the new report before the old one (in case of multi-threading)!

}
