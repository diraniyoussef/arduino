#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

WiFiServer server(11359);
WiFiClient client1;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  Serial.println();
  
  //WiFi.hostname("module_NodeMCU");//This WiFi.hostname("") didn't work with me when mob app was "socket client to server test" 
                                    // and it looks like being unreliable according to https://github.com/esp8266/Arduino/issues/1597
                                    // Currently, I'm accounting on static IP.
                                    //If you insist on something like hostname check this https://stackoverflow.com/questions/20020604/java-getting-a-servers-hostname-and-or-ip-address-from-client
//  IPAddress ip(192, 168, 1, 210); //This IP is the same IP as the statically assigned IP for the NodeMCU in the router
                                  //you may run the sketch "MAC_address" for the purpose.
//  WiFi.config(ip);//this allocates a static IP.
/*
  IPAddress local_IP(192, 168, 1, 210);    
  IPAddress gateway(192, 168, 1, 1);    
  IPAddress subnet(255, 255, 255, 0);        
*/
  //WiFi.config(local_IP, gateway, subnet);//this allocates a static IP.   //WiFi.config(ip, gateway, subnet);
  //if ( WiFi.config(local_IP, gateway, subnet) ) {
    if (WiFi.mode(WIFI_STA)) {
          
      WiFi.begin("Electrotel_Dirani", "onlyforworkpls");    
      delay(20);
    //  WiFi.begin("Electrotel_Dirani", "onlyforworkpls");    
    //  WiFi.hostname("module_NodeMCU01");
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
    Serial.println(WiFi.localIP());      

    if( MDNS.begin("youssef") ) {
      Serial.println("MDNS responder started");
    }
                   
    server.begin();
    //I want a server socket
    server.setNoDelay(true);//I think everything you send is directly sent without the need to wait for the buffer to be full

    Serial.println("After server.begin");
    
    //In order to see the name of the NodeMCU in the DHCP client list of the TP-Link router I am trying the following
    //If I have assigned a static IP to it then I won't see the name in the list. I can ping the NodeMCU though.
  //  WiFiClient client;
    //char* fake_server_IP = "255.255.255.255";
   // const int fake_server_port = 34550;
//    client.connect(fake_server_IP, fake_server_port);
  /*
  } else {
    Serial.println("WiFi.config didn't work!");
  }
  */
}

void loop() 
{

  MDNS.update();
  
  client1 = server.available();
  if (client1)
    if(client1.connected())
      Serial.println("socket connected.");

  //The following block is to repeat something every 10 seconds.
  static int i  = 0;
  i++;
  Serial.println(i);  
  if (i == 10){
    i = 0;    
    //ESP.restart();
  }

  delay(2000);

}
