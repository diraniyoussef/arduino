#include <ESP8266WiFi.h>
#include <EEPROM.h>

extern const int max_EEPROM;
const byte AP_setup_pin = 0; //the built-in flash button on the NodeMCU


class NodeMCU {
private:
	const static byte connect_failure_notifier_pin = 2;
	static void toggle_D_pin_State(int pin) {
    if (digitalRead( pin ) ){
      digitalWrite( pin, LOW );
    } else {
      digitalWrite( pin, HIGH );
    }
  }

public:
	static void setPins() {
		pinMode(AP_setup_pin, INPUT_PULLUP); //the built-in flash button on the NodeMCU
		pinMode(connect_failure_notifier_pin, OUTPUT); //the built-in flash button on the NodeMCU
	}

	static void toggleConnectFailureNotifierPin() {
    toggle_D_pin_State(connect_failure_notifier_pin);
  }

	static boolean getInPinStateAsConsidered( int pin ) {//I mean by "As Considered" that I may choose to consider the LOW as On state for the info. This is in accordance with PCB design
    return ( !digitalRead( pin ) );
  }

	static void setConnectFailureNotifierPinHigh() {
    setOutPinStateAsOpposite( connect_failure_notifier_pin, true );
    //digitalWrite( getRealPinFromD( connect_failure_notifier_pin ), HIGH );
  }

  static void setConnectFailureNotifierPinLow() {
    setOutPinStateAsOpposite( connect_failure_notifier_pin, false );
    //digitalWrite( getRealPinFromD( connect_failure_notifier_pin ), LOW );
  }

	static void setOutPinStateAsOpposite( int pin , boolean state_bool ) {/*always as opposite*/
    digitalWrite( pin, !state_bool );
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

	static boolean isConnectedToWiFi( boolean really_try_to_connect ) { //station mode
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

class General{
public:    
  static int getIntFromHexChar(char aChar) {
    switch (aChar) {
      case '0':
        return (0);
        break;
      case '1':
        return (1);
        break;
      case '2':
        return (2);
        break;
      case '3':
        return (3);
        break;
      case '4':
        return (4);
        break;
      case '5':
        return (5);
        break;
      case '6':
        return (6);
        break;
      case '7':
        return (7);
        break;
      case '8':
        return (8);
        break;
      case '9':
        return (9);
        break;
      case 'a':
        return (10);
        break;
      case 'A':
        return (10);
        break;
      case 'b':
        return (11);
        break;
      case 'B':
        return (11);
        break;
      case 'c':
        return (12);
        break;
      case 'C':
        return (12);
        break;
      case 'd':
        return (13);
        break;
      case 'D':
        return (13);
        break;
      case 'e':
        return (14);
        break;
      case 'E':
        return (14);
        break;        
      case 'f':
        return (15);
        break;
      case 'F':
        return (15);
        break;        
      default:        
        //must not enter here
        return(-1);    
        break;
    }
  }
    
  template <typename T>
  static int arrayIncludeElement( const T array[], int arrayLength, T element ) {
    for (int i = 0; i < arrayLength; i++) {
      if (array[i] == element) {
        return i;
      }
    }
    return -1;
  }

  static int getByteFromTwoHexChars(char higher_nibble_HexChar, char lower_nibble_HexChar ) {
    int higher_nibble = getIntFromHexChar( higher_nibble_HexChar );
    if( higher_nibble == -1 ) {
      return -1;
    }
    int lower_nibble = getIntFromHexChar( lower_nibble_HexChar );
    if( lower_nibble == -1 ) {
      return -1;
    }
    return( higher_nibble * 16 + lower_nibble );
  }

};
