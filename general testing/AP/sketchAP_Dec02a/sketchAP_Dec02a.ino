#include <ESP8266WiFi.h>
//#include <WiFiClient.h> 
// https://arduino.stackexchange.com/questions/37758/esp8266-tcp-connection-wificlient-issue/37759
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html


//https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-examples.rst
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html

//later when you want to make it a station as well, don't connect as a station to another wifi AP unless you hear it, like do a scan before...

#define MAX_SRV_CLIENTS 3 //related to socket only. Don't know if better to use const keyword.
IPAddress local_IP(192,168,4,201);
IPAddress gateway(192,168,4,200); //let the gateway be the same as the local_IP, but maybe it works if 192.168.4.251 was the same gateway for all modules.
IPAddress subnet(255,255,255,0);
boolean Allright = false;
WiFiServer server(3551); //this is the server socket I guess
WiFiClient serverClients[MAX_SRV_CLIENTS];      
 
//In the mobile app, the IP configuration must be set right before trying to connect to the AP! It must have 

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (WiFi.softAPConfig(local_IP, gateway, subnet)){
    Serial.print("Setting soft-AP ... ");  
    boolean result = WiFi.softAP("ESPsoftAP_01", "AP01_123456");
    if(result == true)
    {
      Serial.println("Ready");                                
      Serial.print("Soft-AP IP address = ");
      Serial.println(WiFi.softAPIP());
      //Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());      
      //WiFi.softAPdisconnect(wifioff);
      /*
      uint8_t macAddr[6];
      WiFi.softAPmacAddress(macAddr);
      Serial.printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
      //or you may use 
      Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
      */
      
      server.begin();
      //I want a server socket
      server.setNoDelay(true);//I think everything you send is directly sent without the need to wait for the buffer to be full
      //Serial.print("Ready! Use 'telnet ");
      Allright = true; 

      pinMode(D0, OUTPUT);
      pinMode(D1, OUTPUT);
      pinMode(D2, OUTPUT);
      pinMode(D3, OUTPUT);
      pinMode(D4, OUTPUT);
      pinMode(D5, OUTPUT);
      pinMode(D6, OUTPUT);
      pinMode(D7, OUTPUT);

      
    }
    else
    {
      Serial.println("Failed!");
    }  
  } else {
    
  }
  

  

  
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
  if (Allright){
    serverClients[0] = server.available();
    //here the module will accept up to specific number of connections determined by the sold version...
    if (serverClients[0]){
      Serial.print("some data is available fromt the mobile client.\n");      
      while(serverClients[0].available())
        Serial.write(serverClients[0].read());//only reads one byte?
      /*
       * by the way, client.stop() sends a disconnect message to the client, and the client should accordingly disconnect, so 
       * if you test client.connected() on the client side it should be false.
       */
    }
  }
}
/*
//char* PacketMessage = "Help! Help!";
WiFiClient client; //https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClient.cpp
char* server_IP = "192.168.1.20";

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  Serial.println("\nStarting...\n");

  //WiFi.mode(WIFI_STA);
//  Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");
  WiFi.disconnect();
  //WiFi.begin("Lumia 930 3419", "123yesno");  
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());            

  client.setLocalPortStart(34660);
  const int server_port = 34550;
  while (!client.connect(server_IP, server_port)) //if client.connect was false we have to retry I guess forever.
  {
    Serial.println("Connection failed. Retrying...");
    delay(500);
  }
  Serial.println("Connection should have worked by now :)\n");
  Serial.printf("Now sending something to server\n");
  //Sending ...
  client.print("register emergency");
   unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      //Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
}  
  
}

void loop()
{
    
}
*/
