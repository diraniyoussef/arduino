#include <ESP8266WiFi.h>
#include <EEPROM.h>

extern const int max_EEPROM;



class NodeMCU {
private:
public:
	static void setPins() {
		pinMode(0, INPUT_PULLUP);

	}

	static int beginEEPROM() {
		Serial.printf( "beginning the EEPROM up to %d\n", max_EEPROM );
		//EEPROM.begin( 2 * PCF::absolute_max_pins_number ); //e.g. 10T1F2T5T6F decoded like this: first byte is moduleId interpreted as a "byte" type, then 0T where 0 is char interpreted
		// instead of a byte because pin 10 may one day be used which would be 'a'    
		//Since I introduced the ability to set SSID, password, static IP and MAC by the user, I had to extend the EEPROM range to a larger memory    
		EEPROM.begin( max_EEPROM );
	}

	static void restartNodeMCU() {
    //setBasicPinsBeforeRestart();
    ESP.reset();//It actually works.
  }

	static boolean isConnectedToWiFi( boolean really_try_to_connect ) {
    /* The caller of this method usually directly restart the NodeMCU if this method returned false.
     * In this sense we may say that this method blocks the execution in case it was not connected.
     */
    int i = 0;    
    boolean notifier_pin_set_to_high = false;
    while( WiFi.status() != WL_CONNECTED && i < 5 ) { //waiting for 3 seconds. 
      i++;      
      if (i >= 3) {
        setConnectFailureNotifierPinHigh(); //Comment for debugging
        main_oper_vars_need_to_be_reset = true;
        notifier_pin_set_to_high = true;
      }
      if( really_try_to_connect ) {
        delay(750); //PLEASE DON'T WAIT LESS THAN THIS. LONG WAITING TIME TO CONNECT IS NECESSARY. 
      }
      Serial.print(".");
    }
    if( notifier_pin_set_to_high ) { //needed
      setConnectFailureNotifierPinLow(); //Comment for debugging
    }

    if (WiFi.status()!= WL_CONNECTED) {
      Serial.println("WiFi.status is not connected.");
      return(false);
    } else {
      return(true);
    }
  }
};