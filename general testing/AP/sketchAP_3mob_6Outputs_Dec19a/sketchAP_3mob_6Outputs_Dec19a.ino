//#include <ESP8266WiFi.h> //already included in MessageAndOperation.h
#include "MessageAndOperation.h"
//#include <WiFiClient.h> 
// https://arduino.stackexchange.com/questions/37758/esp8266-tcp-connection-wificlient-issue/37759
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html


//https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-examples.rst
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html

//later when you want to make it a station as well, don't connect as a station to another wifi AP unless you hear it, like do a scan before...

const String moduleIdStr = "server1:"; 
char includedPinIndexAsChar[6] = {'0', '1', '2', '5', '6', '7'}; //please put them here in the same order as in the app. It's nicer but not necessary.
                                    //'0' means D0. '1' means D1. And so on.

#define maxReadingBuffer 1024
char reading_buff[maxReadingBuffer];
int readCharNumber;

MessageAndOperation* messageAndOperation; //think about deleting it from dynamic memory after finishing its role........
boolean messageAndOperationWasDefined = false;
    
//In the mobile app, the IP configuration must be set right before trying to connect to the AP! It must have 

void setup()
{  
  Serial.begin(115200);
  Serial.println();

  NodeMCU::wifiSetup();
    
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  //pinMode(D3, INPUT); //this if GPIO0, use it only in INPUT mode. Just to be more safe.
  //pinMode(D4, INPUT); //this is GPIO2, use it only in INPUT mode. Just to be more safe.
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  //pinMode(D8, INPUT);//this is GPIO15, use it only in INPUT mode. Just to be more safe.
  //pinMode(D9, INPUT);//this is GPIO3 or the Rx pin
  //pinMode(D10, INPUT);//this is GPIO1 or the Tx pin

  //EEPROM stuff
  //should fetch the EEPROM to set the pins according to most recent known state (e.g. before last restart).
  NodeMCU::determineEEPROMSize();
  NodeMCU::decodeEEPROM(); //this determines the value of moduleIdStr which must be like "server1:" and the state of the pins.

}

//Answering the client here can be made inside the same loop because the module has all the needed data.
//But in case this module 1 needed to get info from another module, then module 1 will answer the client in the next loop. Say having 3 clients it could answer them in turn,
// each in a loop. SEQUENCING is the key.
//This "in-turn" mechanism can be made even if the answer is quick.
///And being polite, it can notify the client which had the bad luck that due to contention and lack of resources it will need to drop the socket.

void loop()
{
  int connectedStationsNumber = WiFi.softAPgetStationNum();
  Serial.printf("Stations connected = %d\n", connectedStationsNumber);
  boolean possibleNewConnectionMade = false;
  if (connectedStationsNumber > 0){
    for(int i = 0; i < MAX_SRV_CLIENTS; i++){
      if (!serverClients[i])
      {
        Serial.println("checking for a connection (client is null)...");  
        serverClients[i] = server.available(); 
        possibleNewConnectionMade = true;
      } else {
        if (!serverClients[i].connected()) { //Is it reliable in case client was actually not connected but serverClients[i].connected() is giving a true value...
                                             //Answer: somehow yes, because after some time of idleness the server disconnects alone.
          Serial.println("checking for a connection (client is not null but not connected)...");
          delete &(serverClients[i]);
          serverClients[i] = server.available(); //only for creating new socket. Not for data.
          possibleNewConnectionMade = true;
          //check what happens to the client if the server intentionally closed the socket here. will .isClosed() there returns true??        
        }
      }
    }
        
    if (possibleNewConnectionMade){ //only entered once the new connection to any client is established, and an update will be sent to all connected clients.
      if (serverClients[0]){
        if (serverClients[0].connected()){
          Serial.println("Looks like it's really connected.");
          //assuming messageAndOperation is not defined of was deleted
          messageAndOperation = new MessageAndOperation(&serverClients[0]);//reading_buff and readCharNumber are dummy here.
          messageAndOperationWasDefined = true;
          messageAndOperation->sendReport();
        }
      }    
    }
    
    NodeMCU::yieldAndDelay(50);
    if (serverClients[0]){//don't know if this causes trouble............
      if (serverClients[0].connected()){
        if (serverClients[0].available()){
          //send an ACK to mob1          
          readCharNumber = serverClients[0].readBytesUntil('\0', reading_buff, maxReadingBuffer); //I'm expecting to read 7 or 8 characters then the '\0'          
          NodeMCU::yieldAndDelay();         
          if (!messageAndOperationWasDefined)
            messageAndOperation = new MessageAndOperation(&serverClients[0], reading_buff, readCharNumber);
          messageAndOperation->sendAck();                   
          if (messageAndOperation->analyze()){
            if (!messageAndOperation->isJustReport()){
              NodeMCU::yieldAndDelay();
              messageAndOperation->updatePinsAndEEPROM();
            }
            messageAndOperation->sendReport();
          }                         
        } else {
          Serial.println("Nothing available from first client.");
        }    
      } else {
        Serial.println("Client was once connected but not now.\n");      
      }
    } else {
      Serial.println("Client was never connected.\n");      
    }             
  } else {
    //do nothing.
  }
  if (messageAndOperationWasDefined)
    delete (messageAndOperation);
    
  NodeMCU::yieldAndDelay(1000);
}



/*for reference only
      if (serverClients[0]){//it mmay be deleted somewhere! Make sure in case you delete the object that it is deleted only once.
        serverClients[0].stop(); //this disconnects the client and sets serverClients[0] to null but has no effect on the mobile. The mobile still thinks itself connected.
      }
  for reference only
  // get the IP of a connected station: https://github.com/esp8266/Arduino/issues/2100 but I won't use it now.
  for reference only
    uint8_t macAddr[6];
    WiFi.softAPmacAddress(macAddr);
    Serial.printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    //or you may use 
    Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  for reference only
  
*/



