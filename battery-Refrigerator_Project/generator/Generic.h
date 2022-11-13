#include <ESP8266WiFi.h>
//#include <EEPROM.h>

//extern const int max_EEPROM;

class NodeMCU
{
private:
  const static byte notifier_pin = 2; // the built-in LED on NodeMCU
  const static byte AP_setup_pin = 0; // the built-in flash button on NodeMCU
  static void toggle_pin_State(int pin)
  {
    if (digitalRead(pin))
    {
      digitalWrite(pin, LOW);
    }
    else
    {
      digitalWrite(pin, HIGH);
    }
  }

  static boolean getInPinStateAsConsidered(int pin)
  { // I mean by "As Considered" that I may choose to consider the LOW as On state for the info. This is in accordance with PCB design
    return (!digitalRead(pin));
  }

public:
  static void setPins()
  {
    pinMode(AP_setup_pin, INPUT_PULLUP); // the built-in flash button on the NodeMCU
    pinMode(notifier_pin, OUTPUT);       // the built-in flash button on the NodeMCU
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
  }

  static void toggleNotifierPin()
  {
    toggle_pin_State(notifier_pin);
  }

  static boolean getAP_InPinState()
  {
    return getInPinStateAsConsidered(AP_setup_pin);
  }

  static void setNotifierPinHigh()
  {
    setOutPinStateAsOpposite(notifier_pin, true);
  }

  static void setNotifierPinLow()
  {
    setOutPinStateAsOpposite(notifier_pin, false);
  }

  static void setOutPinStateAsOpposite(int pin, boolean state_bool)
  { // always as opposite
    digitalWrite(pin, !state_bool);
  }
  /*
    static int beginEEPROM()
    {
      Serial.printf("beginning the EEPROM up to %d\n", max_EEPROM);
      // EEPROM.begin( 2 * PCF::absolute_max_pins_number ); //e.g. 10T1F2T5T6F decoded like this: first byte is moduleId interpreted as a "byte" type, then 0T where 0 is char interpreted
      //  instead of a byte because pin 10 may one day be used which would be 'a'
      // Since I introduced the ability to set SSID, password, static IP and MAC by the user, I had to extend the EEPROM range to a larger memory
      EEPROM.begin(max_EEPROM);
    }

    static void restartNodeMCU()
    {
      // setBasicPinsBeforeRestart();
      ESP.reset(); // It actually works.
    }
  */
};
