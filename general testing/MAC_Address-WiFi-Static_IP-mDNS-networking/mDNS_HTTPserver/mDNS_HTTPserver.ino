//This is working when I simply write in my browser "youssef/"
//unfortunately it's not working on my android devices' web browsers.
//There are still some issues with mDNS as for Android devices https://stackoverflow.com/questions/4656379/bonjour-implementation-on-android

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);
WiFiClient client1;

void sendTemp() {
  String page = "<!DOCTYPE html>\n";
  page +="    <meta http-equiv='refresh' content='";
  page += String(3);// how often temperature is read
  page +="'/>\n";  
  page +="<html>\n";
  page +="<body>\n"; 
  page +="<h1>Robojax.com DHT Code</h1>\n";    
  page +="<p style=\"font-size:50px;\">Temperature<br/>\n";  
  page +="<p style=\"color:red; font-size:50px;\">";
  page += "my temperature value";
  page +="</p>\n";  
  page +="</body>\n";  
  page +="</html>\n";  
 server.send(200,  "text/html",page);

}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  Serial.println();
  
  //WiFi.hostname("module_NodeMCU");//This WiFi.hostname("") didn't work with me when mob app was "socket client to server test" 
                                    // and it looks like being unreliable according to https://github.com/esp8266/Arduino/issues/1597
                                    // Currently, I'm accounting on static IP.
                                    //If you insist on something like hostname check this https://stackoverflow.com/questions/20020604/java-getting-a-servers-hostname-and-or-ip-address-from-client
    if (WiFi.mode(WIFI_STA)) {
          
      WiFi.begin("Electrotel_Dirani", "onlyforworkpls");    
      delay(20);
      Serial.print("Connecting");
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }
    } else {
      Serial.print("Couldn't make Wifi station...");
    }
    
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println( WiFi.localIP() );      

    if( MDNS.begin("youssef") ) {
      Serial.println("MDNS responder started");
    }
    /*
    server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
    */
    server.on("/", sendTemp);               
    server.begin();
    //I want a server socket

    Serial.println("After server.begin");
        
  /*
  } else {
    Serial.println("WiFi.config didn't work!");
  }
  */
}

void loop() 
{
  server.handleClient();
  MDNS.update();
  
  delay(300);

}
