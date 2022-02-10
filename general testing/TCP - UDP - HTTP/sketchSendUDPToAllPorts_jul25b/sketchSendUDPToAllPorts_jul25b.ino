#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 161;
//char incomingPacket[255];
char PacketMessage[] = "Help! Help!";
//char  replyPacket[] = "Ok, I got your message!";

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  Serial.println("\nStarting...\n");
  
  WiFi.disconnect();
  WiFi.begin("Lumia 930 3419", "123yesno");  
  //WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());            //

  Udp.begin(localUdpPort);
  // send back a reply, to the IP address and port we got the packet from
  //Udp.beginPacket( IPAddress(192,168,1,107), 62001 ); //213,204,75,44
  Udp.write(PacketMessage);
  unsigned long i;
  for (i = 161; i <= 6000; i++){
    Udp.beginPacket( IPAddress(213,204,75,44), i ); 
    Udp.endPacket();       
    delay(100);
    Serial.printf("\nsent to IP address 213.204.75.44 port %u\n", i);
  }
  Serial.printf("all packets are sent\n");  
  
}

void loop()
{
  /*
int packetSize = Udp.parsePacket();
if (packetSize)
{
  Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
  int len = Udp.read(incomingPacket, 255);
  if (len > 0)
  {
    incomingPacket[len] = 0;
  }
  Serial.printf("UDP packet contents: %s\n", incomingPacket);
  
  // (...)
  
}
   */
/*
// send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(, 4211);
    Udp.write(replyPacket);
    Udp.endPacket();  
  */
}

