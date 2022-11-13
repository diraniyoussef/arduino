#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

class ServerResponse
{

private:	
	virtual void handleRoot() = 0 ;              // function prototypes for HTTP handlers
	virtual void handleNotFound() = 0;
	virtual void setAdditionalHandlers() = 0; //if there are no additional handlers the derived class can simply make it an empty function

public:
	ESP8266WebServer* server = new ESP8266WebServer(80);

	~ServerResponse()
	{
		delete (server);
	}

	void begin(char* domain_name)
	{
		if (!MDNS.begin(domain_name))
		{ // Start the mDNS responder for refrigerator.local
			Serial.println("Error setting up MDNS responder!");
		}
		Serial.println("mDNS responder started");

		// Call the 'handleRoot' function when a client requests URI "/"
		server->on("/", HTTP_GET, [this]() { this->handleRoot(); });  /*[this]() { this->handleRoot(); } is because the "on" method takes std::function<void ()> but handleRoot is not a function, rather it's a method.*/
 		Serial.println("handleRoot is set");
		//setting additional handlers
		setAdditionalHandlers();

		// When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"  	
  	server->onNotFound([this]() { this->handleNotFound(); });           
		Serial.println("handleNotFound is set");
		
		server->begin();
	}

	void loop()
	{
	  MDNS.update();
  	server->handleClient();                     // Listen for HTTP requests from clients	
	}

};