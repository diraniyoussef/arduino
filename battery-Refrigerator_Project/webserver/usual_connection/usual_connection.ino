#include <ESP8266WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
WiFiManager wm;

const char *SSID = "TP-LINK_2B5E";
const char *password = "87654321";

void setup()
{
	Serial.begin(115200);
	delay(5000);
	Serial.println("after 5s delay");
	if (WiFi.mode(WIFI_STA))
	{
		/*
		WiFi.begin(SSID, password);
		Serial.print("connecting");
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(1000);
			Serial.print(".");
		}
		Serial.println("Now it's connected");
		*/

		WiFi.disconnect();
		if (WiFi.status() != WL_CONNECTED)
		{
			Serial.println("not connected");
		}
		else
		{
			Serial.println("still connected ?!");
		}
		
		Serial.println("Trying autoConnect");
		if (wm.autoConnect("BLUE1"))
		{
			Serial.println("connected...great :)");
		}
		else
		{
			Serial.println("Configportal running");
		}

		Serial.print("WiFi.SSID() is ");
		Serial.println(WiFi.SSID()); // this contains the SSID
		Serial.print("WiFi.psk() is ");
		Serial.println(WiFi.psk()); // this contains password
	}
}

void loop()
{
	Serial.println("in loop");
	delay(3000);
}