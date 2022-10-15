#include "Generic.h"
#include <WiFiManager.h>
#include <SimpleTimer.h>

class Setup
{
private:
  inline static SimpleTimer timer_togglePinNoWifi; // had to be inline because it's not static and defined somewhere inside the class
  inline static int timerId_togglePinNoWifi;
  static const int timeout_wifiLoss_notificationToggle = 750; // ms

public:
  boolean must_APmode_be_activated = false;
  boolean is_APmode_button_pressed = false;

  static void initializeTimers()
  {
    timerId_togglePinNoWifi = timer_togglePinNoWifi.setInterval(timeout_wifiLoss_notificationToggle, NodeMCU::toggleNotifierPin);
  }

  static void check_APmode_pin(WiFiManager *wm, const char *AP_SSID)
  {
    if (NodeMCU::getAP_InPinState())
    {
      Serial.println("getAP_InPinState returned true");
      wm->resetSettings();
      wm->startConfigPortal(AP_SSID); // either startConfigPortal or wm->autoConnect(AP_SSID);
    }
  }

  static bool notifyIfNotConnected()
  { // notifying happens by toggling blinking
    if (WiFi.status() != WL_CONNECTED)
    {
      if (!timer_togglePinNoWifi.isEnabled(timerId_togglePinNoWifi))
      {
        Serial.println("timer not enabled, so enabling and running it");
        timer_togglePinNoWifi.enable(timerId_togglePinNoWifi);
      }
      timer_togglePinNoWifi.run();
      return false;
    }
    else
    {
      if (timer_togglePinNoWifi.isEnabled(timerId_togglePinNoWifi))
      {
        Serial.println("timer enabled, so disabling it");
        timer_togglePinNoWifi.disable(timerId_togglePinNoWifi);
        NodeMCU::setNotifierPinHigh();
      }
      return true;
    }
    // Serial.println("Restarting as not being connected...");
    // NodeMCU::restartNodeMCU();
  }
};