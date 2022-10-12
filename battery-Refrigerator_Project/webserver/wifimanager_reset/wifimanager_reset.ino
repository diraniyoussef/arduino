#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
WiFiManager wm;

void setup()
{
	WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

	Serial.begin(115200);
	delay(5000);
	Serial.print("After 5s delay");

	Serial.print("resetting");
	wm.resetSettings();

	Serial.print("will it connect ?");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.print(".");
	}
	Serial.println("Now it's connected");
}

void loop()
{
	Serial.println("in loop");
	delay(3000);
}