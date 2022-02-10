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
  const String  Devicename = "Device_1";

//------------------------------------------------------------------------------------
  // WIFI Module Role & Port    
  IPAddress APlocal_IP(192, 168, 4, 1);
  //IPAddress APlocal_IP(192, 168, 1, 1);
  //IPAddress    apIP(10, 10, 10, 1);
  IPAddress APgateway(192, 168, 4, 1);
  IPAddress APsubnet(255, 255, 255, 0);

  unsigned int UDPPort = 3552; //2390;      // local port to send and listen to  

  WiFiUDP Udp;

//====================================================================================

void setup() {
  // Setting The Serial Port ----------------------------------------------
  Serial.begin(115200);
  Serial.println();

  // Setting Up A Wifi Access Point
  SetWifi();  
}

//====================================================================================

void loop() {
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);

  sendDataToServer();
  receiveDataFromServer();
}

//====================================================================================
void receiveDataToServer() {
  char packetBuffer[255];       // buffer for incoming data
  int packetSize = Udp.parsePacket(); //usually non-blocking   
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.print("Contents:");
    Serial.println(packetBuffer);
    break;                                                    // exit the while-loop
  }
  if((millis()-tNow)>1000){                                   // if more then 1 second no reply -> exit
    Serial.println("timeout");
    break;                                                    // exit
  }
}

void sendDataToServer() {
  unsigned long tNow;
    
  tNow=millis();                                                // get the current runtime
  dtostrf(tNow, 8, 0, value);                                  // translate it to a char array.
  char value[16];              // Buffer big enough for 7-character float

  IPAddress server_IP(192, 168, 4, 1);
  Udp.beginPacket(server_IP, UDPPort);                         // the IP Adress must be known 
  // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());         // this can be used, to answer to a peer, if data was received first
  Udp.write(value);
  Udp.endPacket();                                              // this will automatically send the data
}

//====================================================================================

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
    if( WiFi.softAP( ssid, pass ) ) //WiFi.softAP(ssid, password, 1 ,0, MAXSC); // WiFi.softAP(ssid, password, channel, hidden, max_connection)
    {
      Serial.println("WIFI < " + String(ssid) + " > ... Started");
      Serial.println("Ready"); //as a server, it's probably better to let the client close the connection after that he is satisfied.
      Serial.print("Soft-AP IP address = ");
      Serial.println(WiFi.softAPIP());
      //WiFi.softAPdisconnect(wifioff);

      // Starting UDP Server
      Udp.begin(UDPPort);
      Serial.println("Server Started"); 
    }
    else
    {
      Serial.println("Failed! Couldn't establish an AP");
    }  
  } else {
    Serial.println("Failed! Couldn't configure an AP");
  }  
}
