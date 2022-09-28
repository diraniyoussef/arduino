#include "serverresponse.h"
#include "stringoper.h"

class MyServer:public ServerResponse
{
private:
	using ServerResponse::server;

  const char* logo_settings = "'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABmJLR0QA/wD/AP+gvaeTAAABL0lEQVQ4jZXTMUpDQRQF0JN8UTAKEdE6BCKIihJwARaCFha2rkJXYKWF9omFREF7g4JmAboUu2zARi3+ixm+icELw7w/c++8d/+84W/MxvgX1lGP+AXPEa9gtUieKnzP4Qp9HOMdGWq4wDJ28DEq83zMd9hCC01so41N3Ba4P1jDa4gfo5IiqnjCfXDX0s06upF5ID7ETYyDxOJmcOsKqEXZA/EpSijjHPux1wruDyro4To8i6ylhJOhE3EzuD1UyrH4GaQsEaUHwFdyWBaaXxbaER/gLCFfYnecBfIm6cp/UDXW9qLsTiJewAYe0GDYSBmWcIJFHIXHXpKkKr/mvryhUrtgJua7qKTYSBuGjTRdFA8wJ2+Sbvi8lt9ILdbekkRjMe4xNYx4TJMw8Tl/A1fLMmj4BEFEAAAAAElFTkSuQmCC'";
  //const char* logo_settings = "'data:,'";

  void handleRoot() 
  {                          // When URI / is requested, send a web page with a button to toggle the LED
  Serial.println("handleRoot is called");
    ServerResponse::server->send(200, "text/html", concat(concat(
      "<head>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<title>AP mode- Home Load Balancer</title>"
        "<link rel='icon' href=", logo_settings), ">"
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
      )
    );
  } //don't worry about freeing the memory resulting from concat. It's atomatic when we get out of scope I guess.

  void handleApplyWifi()
  {                         // If a POST request is made to URI /login
  Serial.println("handleApplyWifi is called");
    //the following if makes sure user has entered something
    if( ! ServerResponse::server->hasArg("ssid") || ! ServerResponse::server->hasArg("password") 
        || ServerResponse::server->arg("ssid") == NULL || ServerResponse::server->arg("password") == NULL) { // If the POST request doesn't have username and password data
      ServerResponse::server->send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
      return;
    }

    const char* ssid = ServerResponse::server->arg("ssid").c_str();
    const char* password = ServerResponse::server->arg("password").c_str();
    Serial.print("ssid : ");
    Serial.println(ssid);
    Serial.print("password : ");
    Serial.println(password);
    ServerResponse::server->send(200, "text/html", concat(concat(
      "<head>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<title>AP mode- Home Load Balancer</title>"
        "<link rel='icon' href=", logo_settings), ">"
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
      )
    );
    /*
    if(server.arg("ssid") == "John Doe" && server.arg("password") == "password123") { // If both the username and the password are correct
      server.send(200, "text/html", "<h1>Welcome, " + server.arg("username") + "!</h1><p>Login successful</p>");
    } else {                                                                              // Username and password don't match
      server.send(401, "text/plain", "401: Unauthorized");
    }
    */
  }

  void handleNotFound()
  {
    Serial.println("handleNotFound is called");
    ServerResponse::server->send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
  }

  void setAdditionalHandlers()
  {
    ServerResponse::server->on("/applywifi", HTTP_POST, [this]() { this->handleApplyWifi(); }); // Call the 'handleApplyWifi' function when a POST request is made to URI "/applywifi"
    Serial.println("handleApplyWifi is set");
  }

};