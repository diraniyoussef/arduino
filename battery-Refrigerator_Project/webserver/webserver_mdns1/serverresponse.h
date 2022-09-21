#include "stringoper.h"

class ServerResponse
{
private:
	// Current time
	unsigned long currentTime = millis();
	// Previous time
	unsigned long previousTime = 0;
	// Define timeout time in milliseconds (example: 2000ms = 2s)
	const long timeoutTime = 2000;

public:
	WiFiServer *server = new WiFiServer(80);
	Buff header; // Variable to store the HTTP request
	WiFiClient client;

	void begin(char* domain_name)
	{
		if (!MDNS.begin(domain_name))
		{ // Start the mDNS responder for refrigerator.local
			Serial.println("Error setting up MDNS responder!");
		}
		Serial.println("mDNS responder started");

		server->begin();
	}

	~ServerResponse()
	{
		delete (server);
	}

	void loop()
	{
		MDNS.update();
		client = server->available(); // Listen for incoming clients

		if (client)
		{																 // If a new client connects,
			Serial.println("New Client."); // print a message out in the serial port
			// String currentLine = "";                // make a String to hold incoming data from the client
			bool two_backslash_n = true; // this is a fit succinct alternative to currentLine
			currentTime = millis();
			previousTime = currentTime;
			while (client.connected() && currentTime - previousTime <= timeoutTime)
			{ // loop while the client's connected
				currentTime = millis();
				//delayMicroseconds(50);
				if (client.available())
				{													// if there's bytes to read from the client
					char c = client.read(); // read a byte, then
					// Serial.println("a char is available");
					Serial.write(c); // print it out the serial monitor
					header.add(c);
					if (c == '\n')
					{ // if the byte is a newline character
						// if the current line is blank, you got two newline characters in a row.
						// that's the end of the client HTTP request, so send a response:
						Serial.println("char is \\n");
						// if (currentLine.length() == 0) {
						if (two_backslash_n)
						{
							Serial.println("responding, acting and showing");
							respondOk();
							action();
							displayPage();
							// The HTTP response ends with another blank line
							client.println();		
							// Break out of the while loop
							break;
						}
						else
						{ // if you got a newline, then clear currentLine
							Serial.println("making currentLine empty");
							// currentLine = "";
							two_backslash_n = true; // actually this is the first \n, not the second.
						}
					}
					else if (c != '\r')
					{ // if you got anything else but a carriage return character,
						Serial.println("adding char to currentLine");
						// currentLine += c;      // add it to the end of the currentLine
						two_backslash_n = false;
					}
					else
					{
						Serial.println("char is \\r");
					}
				}
			}
			// Clear the header variable
			header.reset();
			// Close the connection
			client.stop();
			Serial.println("Client disconnected.");
			// Serial.println("");
		}
	}

	void respondOk()
	{
		Serial.println("responding ok");
		// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
		// and a content-type so the client knows what's coming, then a blank line:
		client.println("HTTP/1.1 200 OK");
		client.println("Content-type:text/html");
		client.println("Connection: close");
		client.println();
	}

	virtual void action() = 0; //this enforces ServerResponse to be an abstract class

	virtual void displayPage() {
		// Display the HTML web page
		client.println("<!DOCTYPE html><html>");
		// Web Page Heading
		client.println("<body><h1>ESP8266 Web Server</h1>");

		client.println("</body></html>");
	}
};