#include "serverresponse.h"

extern const int output4;
extern String output4State;
extern const int output5;
extern String output5State;

class MyServer:public ServerResponse
{
	void action() {
		// turns the GPIOs on and off
		if (header.has("GET /5/on") >= 0)
		{
			Serial.println("GPIO 5 on");
			output5State = "on";
			digitalWrite(output5, HIGH);
		}
		else if (header.has("GET /5/off") >= 0)
		{
			Serial.println("GPIO 5 off");
			output5State = "off";
			digitalWrite(output5, LOW);
		}
		else if (header.has("GET /4/on") >= 0)
		{
			Serial.println("GPIO 4 on");
			output4State = "on";
			digitalWrite(output4, HIGH);
		}
		else if (header.has("GET /4/off") >= 0)
		{
			Serial.println("GPIO 4 off");
			output4State = "off";
			digitalWrite(output4, LOW);
		}
	}

	void displayPage() {
		// Display the HTML web page
		client.println("<!DOCTYPE html><html>");
		client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
		client.println("<link rel=\"icon\" href=\"data:,\">");
		// CSS to style the on/off buttons
		// Feel free to change the background-color and font-size attributes to fit your preferences
		client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
		client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
		client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
		client.println(".button2 {background-color: #77878A;}</style></head>");

		// Web Page Heading
		client.println("<body><h1>ESP8266 Web Server</h1>");

		// Display current state, and ON/OFF buttons for GPIO 5
		client.println("<p>GPIO 5 - State " + output5State + "</p>");
		// If the output5State is off, it displays the ON button
		if (output5State == "off")
		{
			client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
		}
		else
		{
			client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
		}

		// Display current state, and ON/OFF buttons for GPIO 4
		client.println("<p>GPIO 4 - State " + output4State + "</p>");
		// If the output4State is off, it displays the ON button
		if (output4State == "off")
		{
			client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
		}
		else
		{
			client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
		}
		client.println("</body></html>");

		// The HTTP response ends with another blank line
		client.println();		
	}
};