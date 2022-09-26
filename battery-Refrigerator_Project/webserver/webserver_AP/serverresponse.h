#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

class ServerResponse
{
	
private:	
	virtual void handleRoot() = 0 ;              // function prototypes for HTTP handlers
	//void handleApplyWifi();
	virtual void handleNotFound() = 0;



public:
	ESP8266WebServer* server = new ESP8266WebServer(80);
	void begin(char* domain_name);
	void loop();
	
	char* logo_settings;

	void begin(char* domain_name)
	{
		if (!MDNS.begin(domain_name))
		{ // Start the mDNS responder for refrigerator.local
			Serial.println("Error setting up MDNS responder!");
		}
		Serial.println("mDNS responder started");


		server->on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"  	
  	server->onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
		
		
	}

	~ServerResponse()
	{
		delete (server);
	}

	void loop()
	{
	  MDNS.update();
  	server->handleClient();                     // Listen for HTTP requests from clients
	
	}

};