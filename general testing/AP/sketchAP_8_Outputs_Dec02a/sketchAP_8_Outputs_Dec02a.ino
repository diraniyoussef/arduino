#include <ESP8266WiFi.h>
//#include <WiFiClient.h> 
// https://arduino.stackexchange.com/questions/37758/esp8266-tcp-connection-wificlient-issue/37759
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html


//https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-examples.rst
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html

//later when you want to make it a station as well, don't connect as a station to another wifi AP unless you hear it, like do a scan before...

#define maxReadingBuffer 256
#define MAX_SRV_CLIENTS 3 //related to socket only. Don't know if better to use const keyword.
IPAddress local_IP(192,168,4,201);
IPAddress gateway(192,168,4,200); //let the gateway be the same as the local_IP, but maybe it works if 192.168.4.251 was the same gateway for all modules.
IPAddress subnet(255,255,255,0);
boolean Allright = false;
WiFiServer server(3551); //this is the server socket I guess
WiFiClient serverClients[MAX_SRV_CLIENTS];      
char readingBuffer[maxReadingBuffer];
    
//In the mobile app, the IP configuration must be set right before trying to connect to the AP! It must have 

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (WiFi.softAPConfig(local_IP, gateway, subnet)) 
  {
    Serial.print("Setting soft-AP ... ");    
    if(WiFi.softAP("ESPsoftAP_01", "AP01_123456")) 
    {
      Serial.println("Ready");                                
      Serial.printf("Soft-AP IP address = ");
      Serial.print(WiFi.softAPIP());
      //WiFi.softAPdisconnect(wifioff);
      /*
      uint8_t macAddr[6];
      WiFi.softAPmacAddress(macAddr);
      Serial.printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
      //or you may use 
      Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
      */
      delay (50);//I added it just in case...      
      server.begin();
      //I want a server socket
      server.setNoDelay(true);//I think everything you send is directly sent without the need to wait for the buffer to be full
      
      Allright = true;
    }
  }

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, INPUT);
  pinMode(D7, OUTPUT);
  
  if (!Allright){
    delay(6000); //In case I wanted to flash or something. Although it may not be needed.
    digitalWrite(D8, LOW);  //pin 15
    digitalWrite(D3, HIGH); //pin 0
    digitalWrite(D4, HIGH); //pin 2
    //I though setting those pins could help in rebooting, anyway...
    ESP.restart();
  } 

  if (readingBuffer[maxReadingBuffer] == '\0') {
    Serial.print("last character is the null character");
  }
}

void loop()
{
  int connectedStationsNumber = WiFi.softAPgetStationNum();
  Serial.printf("Stations connected = %d\n", connectedStationsNumber);
  // get the IP of a connected station: https://github.com/esp8266/Arduino/issues/2100
  //but I won't use it now.
  
  if (connectedStationsNumber > 0){
    if (!serverClients[0])
    {
      Serial.print("checking for a connection...");
      serverClients[0] = server.available(); 
    } else {
      if (!serverClients[0].connected()) {        
        Serial.print("checking for a connection...");
        serverClients[0] = server.available(); //does this return true on socket connection without sending anything?? Yes.
      }
    }

      
    
    
    int readCharNumber;
    if (serverClients[0]){
      if (serverClients[0].connected()){
        if (serverClients[0].available()){
          //int i;
          //for (i = 0; i++, i < readCharNumber)
            //readingBuffer[i] = '-'; //any dumm character would do.
          readCharNumber = serverClients[0].readBytesUntil('\0', readingBuffer, maxReadingBuffer); //I'm expecting to read 7 or 8 characters then the '\0'          
          //useful characters are from 0 to readCharNumber - 1
          Serial.println(readingBuffer);
        } else {
          Serial.println("Nothing available from first client.");
        }    
      } else {
        Serial.println("Client was once connected but not now.\n");      
      }
    } else {
      Serial.println("Client was never connected.\n");      
    }         
/*
    if (digitalRead(D6)){
      char a = serverClients[0].read();
      String b = String (a);
      Serial.print(b);//only reads one byte?
      //serverClients[0].print("one byte is read in server\n");
    }
   */

/*//          working code...
          ch[i] = serverClients[0].read();
          i = i + 1;
          while (serverClients[0].available()){
            i = i + 1;
            ch[i] = serverClients[0].read();
          }
          */
          
   /*working I guess: https://github.com/esp8266/Arduino/issues/3228
      WiFiClient stream;
      size_t len=stream.available();
      unsigned char buf[len];
      stream.readBytes(buf, len);
      stream.write(buf, len);
    */

    //          serverClients[0].setTimeout(150); //https://www.arduino.cc/en/Reference/StreamSetTimeout I think it's the time within which if serverClients[0].read() is initiated another time
                                            //it can still read again. As if there's a temporary buffer that is dedicated to read within. But the countdown starts from which time?
                                            //starting from the time available() turns true maybe?
                                            //Interesting... Perhaps available() will return false after the timeout passed. Or would it be true but the read value not reliable.

                                            
    //Client.connected(); //this works if the client closes properly the socket. And I think it works if the server maybe closes for some reason the connection.
   
    
    /*
    serverClients[1] = server.available(); //does this return true on socket connection without sending anything??
    if (serverClients[1]){
      Serial.print("server.available returned the second client..\n");      
      if (serverClients[1].available()){
        Serial.println("second client info from client.available: ");
        Serial.write(serverClients[1].read());//only reads one byte?
      } else {
        Serial.print("Nothing sent from second client..\n");      
      }
    } else {
      Serial.print("No second client..\n");      
    }
    */
  } else {
    
  }
  delay(1000);
}
    /*
       * by the way, client.stop() sends a disconnect message to the client, and the client should accordingly disconnect, so 
       * if you test client.connected() on the client side it should be false.
       */
  
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


By the way, check WiFiClientEvents example for very interesting disconnetion event.
*/
