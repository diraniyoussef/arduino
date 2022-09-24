#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleLogin();
void handleNotFound();

// Replace with your network credentials
const char *ssid = "TP-LINK_2B5E";
const char *password = "87654321";

void setup(){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  Serial.println("Before Connecting");
  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());               // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("refrigerator")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/applywifi", HTTP_POST, handleApplyWifi); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}

void loop(){
  MDNS.update();
  server.handleClient();                     // Listen for HTTP requests from clients
}

void handleRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", 
    "<head>"
		  "<meta name='viewport' content='width=device-width, initial-scale=1'>"
		  "<title>AP mode- Home Load Balancer</title>"
		  "<link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABmJLR0QA/wD/AP+gvaeTAAABL0lEQVQ4jZXTMUpDQRQF0JN8UTAKEdE6BCKIihJwARaCFha2rkJXYKWF9omFREF7g4JmAboUu2zARi3+ixm+icELw7w/c++8d/+84W/MxvgX1lGP+AXPEa9gtUieKnzP4Qp9HOMdGWq4wDJ28DEq83zMd9hCC01so41N3Ba4P1jDa4gfo5IiqnjCfXDX0s06upF5ID7ETYyDxOJmcOsKqEXZA/EpSijjHPux1wruDyro4To8i6ylhJOhE3EzuD1UyrH4GaQsEaUHwFdyWBaaXxbaER/gLCFfYnecBfIm6cp/UDXW9qLsTiJewAYe0GDYSBmWcIJFHIXHXpKkKr/mvryhUrtgJua7qKTYSBuGjTRdFA8wJ2+Sbvi8lt9ILdbekkRjMe4xNYx4TJMw8Tl/A1fLMmj4BEFEAAAAAElFTkSuQmCC'>"
      "<style> html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
					".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; float:none;}"
					".button2 {background-color: #77878A;}"
					"label {clear: both; width: 10ch;}"
					"input {width: 15ch;}"
					"div {margin-bottom: 10px;}"
					".entryblock {width: 25ch; height: 7ch; margin:auto;}"
					".entry {float: right;}"
		  "</style>"
    "</head>"
    "<body>"
		  "<section>"
			  "<h1>"
				  "This is 'Home Load Balancer' settings."
			  "</h1>"
		  "</section>"
      "<section>"
			"<p>This module will connect to your router."
        "<br>"
				"Please fill the router's configuration :"
			"</p>"
			"<form action='/applywifi' method='POST'>"
				"<div class='entryblock'>"
					"<div class='entry'>"
						"<label>router SSID*</label>"
						"<input type='text' name='ssid' maxlength='30' placeholder='TP-LINK_2B5E'>"
					"</div>"
					"<div class='entry'>"
						"<label>password   </label>"
						"<input type='password' name='password' maxlength='30' placeholder='87654321'>"
					"</div>"
				"</div>"
				"<input class='button' type='submit' value='Apply'>"
			"</form>"
		"</section>"
    "</body>"
  );
}

void handleApplyWifi() {                         // If a POST request is made to URI /login
  //the following if makes sure user has entered something
  if( ! server.hasArg("ssid") || ! server.hasArg("password") 
      || server.arg("ssid") == NULL || server.arg("password") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }

  const char* ssid = server.arg("ssid").c_str();
  const char* password = server.arg("password").c_str();
  Serial.print("ssid : ");
  Serial.println(ssid);
  Serial.print("password : ");
  Serial.println(password);
  server.send(200, "text/html", 
    "<head>"
		  "<meta name='viewport' content='width=device-width, initial-scale=1'>"
		  "<title>AP mode- Home Load Balancer</title>"
		  "<link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABmJLR0QA/wD/AP+gvaeTAAABL0lEQVQ4jZXTMUpDQRQF0JN8UTAKEdE6BCKIihJwARaCFha2rkJXYKWF9omFREF7g4JmAboUu2zARi3+ixm+icELw7w/c++8d/+84W/MxvgX1lGP+AXPEa9gtUieKnzP4Qp9HOMdGWq4wDJ28DEq83zMd9hCC01so41N3Ba4P1jDa4gfo5IiqnjCfXDX0s06upF5ID7ETYyDxOJmcOsKqEXZA/EpSijjHPux1wruDyro4To8i6ylhJOhE3EzuD1UyrH4GaQsEaUHwFdyWBaaXxbaER/gLCFfYnecBfIm6cp/UDXW9qLsTiJewAYe0GDYSBmWcIJFHIXHXpKkKr/mvryhUrtgJua7qKTYSBuGjTRdFA8wJ2+Sbvi8lt9ILdbekkRjMe4xNYx4TJMw8Tl/A1fLMmj4BEFEAAAAAElFTkSuQmCC'>"
      "<style> html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
        "div {text-align: left;}"
      "</style>"
    "</head>"
    "<body>"
      "<section>"
        "<h1>Home Load Balancer</h1>"
      "</section>"
      "<p>Attempting to connect to router in station mode...</p>"
      "<div>"
        "<p>If connection was successful, the LED should turn off to indicate success."
        "<br>"
        "Otherwise please enter AP mode again by holding down again the button for 5 seconds"
        " and re-enter the WiFi credentials.</p>"
      "</div>"
    "</body>"
  );
  /*
  if(server.arg("ssid") == "John Doe" && server.arg("password") == "password123") { // If both the username and the password are correct
    server.send(200, "text/html", "<h1>Welcome, " + server.arg("username") + "!</h1><p>Login successful</p>");
  } else {                                                                              // Username and password don't match
    server.send(401, "text/plain", "401: Unauthorized");
  }
  */
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

