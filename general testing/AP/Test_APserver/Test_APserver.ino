#include <ESP8266WiFi.h>

WiFiClient client1;  
//int counterToAssignANewClient = 0;
//static const int maxLoopCounterToAssignANewClient = 180; //e.g. every 180 loops (each loop is delayed by 1 second) the client if connected will be deleted - no it's on the stack not the heap
WiFiServer server(3551);

void setup() {
  Serial.begin(115200);
  Serial.println();
  IPAddress local_IP(192,168,4,201);
  IPAddress gateway(192,168,4,200); //let the gateway be the same as the local_IP, but maybe it works if 192.168.4.251 was the same gateway for all modules.
  IPAddress subnet(255,255,255,0);
//  WiFi.hostname("module_NodeMCU01");
  
  if (WiFi.softAPConfig(local_IP, gateway, subnet)){
    Serial.print("Setting soft-AP ... ");    
    if(WiFi.softAP("ESPsoftAP_01", "AP01_123456")){
      Serial.println("Ready");                                
      Serial.printf("Soft-AP IP address = ");
      //Serial.print(WiFi.softAPIP());
      //WiFi.softAPdisconnect(wifioff);

      delay (50);//I added it just in case...      
      server.begin();
      //I want a server socket
      server.setNoDelay(true);//I think everything you send is directly sent without the need to wait for the buffer to be full
      
    }
  }      
  //WiFiClient* client1 = new WiFiClient();
  //delete client1;
}

void loop() { //this code is useful to test something with the client, like to know when the socket or the connection are gone.
  static int i = 0;
  i++;
  if (i%7 == 0) { //once a client is connected, within 7 seconds it can disconnect and I can test anything else...
    i = 0;
    Serial.println("resetting client1");
    client1 = server.available();
  }     
  
  if (client1) {
    Serial.println(client1);//this will print "1"
    if(client1.connected()) {
      Serial.println("socket connected.");
    } else {
      Serial.println("socket NOT connected."); //when the socket disconnects from the client side, the server knows it.
    }
  } else {
    Serial.println("No socket.");
  }
  delay(1000);
}



