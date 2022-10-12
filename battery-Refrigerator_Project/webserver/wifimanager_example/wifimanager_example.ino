#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
WiFiManager wm;

void setup()
{
	WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
	/*
		BTW if Wifi was already connected to a router then here it will directly connect again even without any explicit credentials. I.e. you don't need to call WiFi.begin(SSID, password);
	*/

	// put your setup code here, to run once:
	Serial.begin(115200);

	// reset settings - wipe credentials for testing
	//wm.resetSettings();

	//wm.setConfigPortalBlocking(false);
	delay(3000);
	Serial.print("After delay");
	// automatically connect using saved credentials if they exist
	// If connection fails it starts an access point with the specified name
	if (wm.autoConnect("BLUE1"))
	{
		Serial.println("connected...great :)");
	}
	else
	{
		Serial.println("Configportal running");
	}

	Serial.print("WiFi.SSID() is ");
	Serial.println(WiFi.SSID()); //this contains the SSID
	Serial.print("WiFi.psk() is ");
	Serial.println(WiFi.psk()); //this contains password
}

void loop()
{
	//wm.process();
	// put your main code here, to run repeatedly:
	delay(3000);
	Serial.println("in loop");
}