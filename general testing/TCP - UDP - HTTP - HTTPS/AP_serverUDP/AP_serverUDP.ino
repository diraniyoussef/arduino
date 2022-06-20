//-- Libraries Included --------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

//------------------------------------------------------------------------------------
  // Define I/O Pins
  #define     LED0        2         // WIFI Module LED

//------------------------------------------------------------------------------------
  // Authentication Variables
  char*       ssid;              // SERVER WIFI NAME
  char*       password;          // SERVER PASSWORD

//------------------------------------------------------------------------------------
  #define     MAXSC     6           // MAXIMUM NUMBER OF CLIENTS
    
  IPAddress APlocal_IP(192, 168, 4, 1);
  IPAddress APgateway(192, 168, 4, 1);
  IPAddress APsubnet(255, 255, 255, 0);

  unsigned int UDPPort = 2390;      // local port to listen on

  WiFiUDP Udp;
//------------------------------------------------------------------------------------
  // Some Variables
  char packetBuffer[255]; //buffer to hold incoming packet
  char result[10];


void setup() {
  Serial.begin(115200);
  Serial.println();

  // Setting Up A Wifi Access Point
  SetWifi();
}

void loop() {
  //Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  //delay(3000);
  HandleClients();
  
}



void HandleClients(){
  unsigned long tNow;
  int packetSize = Udp.parsePacket(); //somewhat non-blocking call. It might have 10 ms of delay even if not received anything.
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    
    Serial.print("From ");
    Serial.print(Udp.remoteIP());
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    
    tNow=millis();
    dtostrf(tNow, 8, 0, result); //convert a float to a c-string char array
    
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(result);
    Udp.endPacket();
  }
}

void SetWifi(){
  // Stop active previous WIFI
  //WiFi.disconnect(); //not needed probably

  // Setting the WiFi mode
  /*
  WiFi.mode(WIFI_AP_STA);
  Serial.println("WIFI Mode : AccessPoint Station");
  */
  //I don't get it why he used AccessPoint Station as WiFi mode. AccessPoint is enough !
  WiFi.mode(WIFI_AP);
  Serial.println("WIFI Mode : AccessPoint");
  
  char* ssid = "ESPsoftAP_01";
  char* pass = "AP01_123456"; //can be empty string 
  if (WiFi.softAPConfig(local_IP, gateway, subnet)){
    Serial.print("Setting soft-AP ... ");  
    boolean result = WiFi.softAP( ssid, pass ); //WiFi.softAP(ssid, password, 1 ,0, MAXSC); // WiFi.softAP(ssid, password, channel, hidden, max_connection)
    if(result == true)
    {
      Serial.println("WIFI < " + String(ssid) + " > ... Started");
      Serial.println("Ready"); //as a server, it's probably better to let the client close the connection after that he is satisfied.
      Serial.print("Soft-AP IP address = ");
      Serial.println(WiFi.softAPIP());
      //WiFi.softAPdisconnect(wifioff);
      
      server.begin();
      server.setNoDelay(true); //I think everything you send is directly sent without the need to wait for the buffer to be full
    }
    else
    {
      Serial.println("Failed! Couldn't establish an AP");
    }  
  } else {
    Serial.println("Failed! Couldn't configure an AP");
  }  

  // Starting UDP Server
  Udp.begin(UDPPort);
  
  Serial.println("Server Started");
}
