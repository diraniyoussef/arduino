#include "Main.h"
#include <WiFiManager.h>
#include <SimpleTimer.h>

const int delay_per_loop = 1; // 1 ms

//AP variables
WiFiManager wm;
const char *AP_SSID = "LoadBalancer_RouterSet";
bool first_time_STA_connected = true; //this is a setup to go out of AP to normal operation

void setupMainVars()
{
  // setting up main operations variables. This is instead of the setup(). It is for the true purpose of the NodeMCU, not the AP stuff. It is when it's properly connected as STA to a router.
  
}

void setup()
{
  /*
  try to connect to wifi here
  */
  // WiFi.mode(WIFI_STA);
  WiFi.begin(); // Calling it will enable station mode and connect to the last used access point based on configuration saved in flash memory. https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/station-class.html

  Serial.begin(115200);

  wm.setBreakAfterConfig(true); // after configuration, in case it's unable to connect to router, it shuts down the configuration portal and goes back to station mode

  Setup::initializeTimers();
  NodeMCU::setPins();
  // NodeMCU::beginEEPROM(); //makes a reset because of wifimanager autoconnect I guess
}

void loop()
{
  /*
  - check if button has been continuously pressed for 5 seconds then enter AP mode.
  - check Wifi connection. If it's not connected then blink and loop.
  - do your normal task here.
  */
  delay(delay_per_loop);

  Setup::check_APmode_pin(&wm, AP_SSID);

  if (!Setup::notifyIfNotConnected())
  {
    first_time_STA_connected = true;
    return; // return if not connected to router
  }

  if (first_time_STA_connected)
  {
    first_time_STA_connected = false;
    setupMainVars();
  }

  // do the usual task now
  
}
