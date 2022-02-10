//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClient.cpp
//#include <ESP8266WiFi.h> //already included in MessageAndOperation.h
#include "wiFiClient.h"

//#include <WiFiClient.h> 
// https://arduino.stackexchange.com/questions/37758/esp8266-tcp-connection-wificlient-issue/37759
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html
// https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-examples.rst
// http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html

// later when you want to make it a station as well, don't connect as a station to another wifi AP unless you hear it, like do a scan before...
  
const String moduleIdStr = "server1:"; 
//char includedPinIndexAsChar[5] = {'3', '4', '5', '6', '9'}; //please put them here in the same order as in the app. It's nicer but not necessary.
                                    //'0' means D0. '1' means D1. 'a' means 10. And so on.

char includedPinIndexAsChar[4] = {'3', '4', '6', '5'}; //for the sake of debugging using the serial monitor you must free '9' and 'a'! especially 'a'.
WiFiClient dummyClient;
wiFiClient* serverClients  = new wiFiClient[ MAX_SRV_CLIENTS ];
#define maxReadingBuffer 1024
    
//In the mobile app, the IP configuration must be set right before trying to connect to the AP! It must have 

void setup()
{  
  Serial.begin(115200);
  Serial.println();  
    
  pinMode(D0, INPUT);  
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D7, INPUT);
  pinMode(D8, INPUT);
  
  pinMode(D3, OUTPUT);  
  pinMode(D4, OUTPUT);  
  pinMode(D5, OUTPUT);  
  pinMode(D6, OUTPUT);
//  pinMode(D9, OUTPUT);  

//  pinMode(D10, OUTPUT);  
    
  NodeMCU::wifiSetup(); //if not connected to WiFi, NodeMCU will restart.
  //EEPROM stuff
  //should fetch the EEPROM to set the pins according to most recent known state (e.g. before last restart).
  NodeMCU::lowerAllOutPins();//this is not to let any pin floating.
  NodeMCU::yieldAndDelay(1);
  NodeMCU::determineEEPROMSize();
  NodeMCU::decodeEEPROM(); //this determines the value of moduleIdStr which must be like "server1:" and the state of the pins.  

//BAD line...  Serial.println(ESP.getResetInfo()); //stack trace https://github.com/esp8266/Arduino/issues/1152
}

//Answering the client here can be made inside the same loop because the module has all the needed data.
//But in case this module 1 needed to get info from another module, then module 1 will answer the client in the next loop. Say having 3 clients it could answer them in turn,
// each in a loop. SEQUENCING is the key.
//This "in-turn" mechanism can be made even if the answer is quick.
// And being polite, it can notify the client which had the bad luck that due to contention and lack of resources it will need to drop the socket.

void loop()
{
  if( !NodeMCU::isConnectedToWiFi() ) {    
    NodeMCU::restartNodeMCU();
  }
  /* 
   *  The program is divided as follows: Some things may be optimized later.
   *  1) Each client once connected starts a counter called "counterToAssignANewClient" which counts how many loops it is going in, at the end of the loop() function there is a 1 second delay. After the maximum is attained, this client asks for a new connection.
   *  2) Normally, each client in each loop seeks for a new connection, they all may get the chance, and maybe none of them in this particular loop may.
   *  3) The server sends an ack and a report to any newly connected client in this loop.
   *  4) In-turn and loop by loop, ONE connected client processes a potential message that the server received.
  */
  
  NodeMCU::checkManualUpdate();
  //Serial.printf("Stations connected = %d\n", connectedStationsNumber);  
  boolean maxLoopCounterReached = false;
  boolean newly_created_socket = false;        
  
  Serial.println("for loop.");
  for(int server_index = 0; server_index <= 1; server_index++) {
    for(int i = 0; i < MAX_SRV_CLIENTS; i++) {
      newly_created_socket = false;
      maxLoopCounterReached = false;

      if (serverClients[i].theClient) { //once connected, even if disconnected afterwards, this will give true. Unfortunately.
        if (serverClients[i].theClient.connected()) { //if the mob disconects from its side, will serverClients[i].theClient remain not null?
          if (server_index == 0)//nothing special about server_index being 0, but I just want the increment to be done once per serverClient and not twice.
            serverClients[i].counterToAssignANewClient++; 
        } 
        else {
          if (serverClients[i].counterToAssignANewClient > 0)//We enter here when the mob e.g. closes the connection.
            serverClients[i].counterToAssignANewClient = 0;
            Serial.println("WEIRD case");
        }

        //Now, this is to avoid false connection indication.
        if ( serverClients[i].counterToAssignANewClient == wiFiClient::maxLoopCounterToAssignANewClient ) { 
          //The idea is to close the socket (connected or not) after some time (determined by the means of maxLoopCounterToAssignANewClient) has passed to its first connection.
          //That time is determined by thenumber of loops counted and the delay each loop has.
          //The goal is not to consume resources infinitely.
          maxLoopCounterReached = true; 
          serverClients[i].counterToAssignANewClient = 0;       
          //send a report anyway then close. This may be useful. This is logical before intentionally closing a socket.
          sendReport(i);//we already checked that it's connected...
          //now lose this socket and check if there is another pending connection to tie to this serverClients[i]
          Serial.println("trying to assign a new client (using server.available) to serverClients[");
          Serial.print(i);
          Serial.print("] of server ");
          Serial.print(server_index);
          Serial.println(" after resetting the counter loop...");
          serverClients[i].theClient.stop();//hopefully that when destroying it from the server, the client would be notified in his Android code
          NodeMCU::yieldAndDelay(3);
          //delete (serverClients[i].theClient);   //if theClient field was a pointer then this works.
          //serverClients[i].theClient = new WiFiClient();
          if (server_index == 0){
            serverClients[i].theClient = server1.available();
          } else {
            serverClients[i].theClient = server2.available();
          }
          if (serverClients[i].theClient)
            if (serverClients[i].theClient.connected()) {//this is a newly created socket
              sendReport(i);
              Serial.print("serverClients[");
              Serial.print(i);
              Serial.print("] of server ");
              Serial.print(server_index);
              Serial.println(" is connected after that this same client was connected long time ago.");
            }
          Serial.println("After trying to assign a new client (using server.available) after resetting the counter loop...");                 
        }
      } else {
        serverClients[i].counterToAssignANewClient = 0; //probably not needed.
      /*  Serial.print("serverClients[");
        Serial.print(i);
        Serial.print("] of server ");
        Serial.print(server_index);
        Serial.println(" is false, as of null.");*/
      }
      
      if (!maxLoopCounterReached){         //Important! In order not to lose a connection.
        if ( !serverClients[i].theClient ) //Unfortunately after being deleted theClient remains not null. But this can be fixed by directly assigning server.available to it, because
                                           // server.available returns false if unsuccessful and true (I think when boolean casted) in case a client was connected
        {
          //Serial.println("checking for a connection (on a client that is not connected or maybe null)...");  
          /*Serial.print("TRYING to assign a socket to serverClients[");
          Serial.print(i);
          Serial.print("] of server ");
          Serial.print(server_index);
          Serial.println("."); */
          if (server_index == 0){
            serverClients[i].theClient = server1.available();
          } else {
            serverClients[i].theClient = server2.available();
          }
          serverClients[i].counterToAssignANewClient = 0;
          
          if (serverClients[i].theClient)
            if (serverClients[i].theClient.connected()){//this is a newly created socket
              Serial.print("serverClients[");
              Serial.print(i);
              Serial.print("] of server ");
              Serial.print(server_index);
              Serial.println(" is connected after that this same client was null.");               
              sendReport(i);
            }          
        } else {
          if (!serverClients[i].theClient.connected()) { //Is it reliable in case client was actually not connected but serverClients[i].connected() is giving a true value...
                                               //Answer: somehow yes, because after some time of idleness the server disconnects alone.
            Serial.println("checking for a connection (client is not null but not connected)...");
            //delete (&(serverClients[i].theClient)); //this is wrong in case theClient was defined on the stack instead of the heap
            serverClients[i].theClient.stop();
            if (server_index == 0){
              serverClients[i].theClient = server1.available(); //only for creating new socket. Not for data... 
            } else {
              serverClients[i].theClient = server2.available();
            }
            serverClients[i].counterToAssignANewClient = 0;
            if (serverClients[i].theClient)
              if (serverClients[i].theClient.connected()){//this is a newly created socket                 
                sendReport(i);
                Serial.print("serverClients[");
                Serial.print(i);
                Serial.print("] of server ");
                Serial.print(server_index);
                Serial.println(" is connected after that this same client was disconnected.");
              }           
            //Check what happens to the client if the server intentionally closed the socket here. Will .isClosed() there return true??        
            // It may have crashed before, but not now.
          } else {
            Serial.print("serverClients[");
            Serial.print(i);
            Serial.print("] of server ");
            Serial.print(server_index);
            Serial.print(" is still connected, with counter ");
            Serial.println(serverClients[i].counterToAssignANewClient);
          }
        }
      }
      //this block is useful in case I was using testAndFixConnection (and testAndFixConnection didn't ask the NodeMCU for a report)
      //Only entered once the new connection to any client is established thus a new socket is  established, then an update will be sent to mob. 
      // The reason behind this is a bug that happens sometimes, when the module changes the state of a certain pin but not the app. As if I was accounting on the testAndFixConnection to initiate a new socket.
      // This is not a good behavior in case the user presses on a switch and an enhanced connection is made because that won't send the request, instead
      // it will only create a new socket thus the user will get the state of the pins without his intended change.
      // Another reason is when the user presses on the "Refresh" button then a communication is enhanced only by creating a new socket.
      // This is not needed when the mobile app resumes from sleeping e.g. because a silent socket connection would be made then.           
    }
  }
  dummyClient = server1.available();
  //Serial.println("just after dummyClient with server1.available");
  if (dummyClient)  
    if (dummyClient.connected()) {
      Serial.println("Dummy client wasted.");
      dummyClient.stop();
      //no delete
    }
  dummyClient = server2.available();
  //Serial.println("just after dummyClient with server2.available");
  if (dummyClient)  
    if (dummyClient.connected()) {
      Serial.println("Dummy client wasted.");
      dummyClient.stop();
      //no delete
    }

  
  NodeMCU::yieldAndDelay(50);
  //Now listen to clients and process the one that sent something   
  static int activeClientIndex = MAX_SRV_CLIENTS - 1;    
  int counterToExitLoop;
  counterToExitLoop = 1;
  while(counterToExitLoop <= MAX_SRV_CLIENTS)
  {
    counterToExitLoop++;
    
    if (activeClientIndex == MAX_SRV_CLIENTS - 1){
      activeClientIndex = 0;
    } else {
      activeClientIndex++;
    }
    
    if (serverClients[activeClientIndex].theClient){
      if (serverClients[activeClientIndex].theClient.connected()){
        if (serverClients[activeClientIndex].theClient.available()){            
          //send an ACK 
          char reading_buff[maxReadingBuffer];
          int readCharNumber = serverClients[activeClientIndex].theClient.readBytesUntil('\0', reading_buff, maxReadingBuffer); //I'm expecting to read 7 or 8 characters then the '\0'
/*            
          //for testing purpose...
          if (serverClients[activeClientIndex].theClient.available()){
            Serial.println("An extra message is available in the same connected client!!!"); 
            char reading_buff1[maxReadingBuffer];
            int readCharNumber1 = serverClients[activeClientIndex].theClient.readBytesUntil('\0', reading_buff, maxReadingBuffer);
            if (readCharNumber1 > 0){
              Serial.print("The next reading buffer is: "); 
              Serial.println(String(reading_buff1));
            }
          }
*/          
          NodeMCU::yieldAndDelay();         
          Serial.print("received message on client ");
          Serial.print(activeClientIndex);
          Serial.print(" is:");
          Serial.println(String(reading_buff));
          serverClients[activeClientIndex].messageAndOperation = new MessageAndOperation( &(serverClients[activeClientIndex].theClient), reading_buff, readCharNumber );
          serverClients[activeClientIndex].messageAndOperation->sendAck();
          if (serverClients[activeClientIndex].messageAndOperation->analyze()){
            if (!serverClients[activeClientIndex].messageAndOperation->isJustReport()){
              NodeMCU::yieldAndDelay();
              serverClients[activeClientIndex].messageAndOperation->updatePinsAndEEPROM();
            }
            serverClients[activeClientIndex].messageAndOperation->sendReport();
          }            
          delete (serverClients[activeClientIndex].messageAndOperation);
          Serial.println("after deleting in block 4");
          break;
        } else {
          //Serial.println("Nothing available from first client.");
        }          
      } else {
        //Serial.println("Client was once connected but not now.\n");      
      }
    } else {
      //Serial.println("Client was never connected.\n");      
    }
  }    
      
  NodeMCU::yieldAndDelay(750); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
                                // Flooding reports to the requester may cause the requester to execute the new report before the old one (in case of multi-threading)!
//NodeMCU::yieldAndDelay(2000);//for debugging
}

void sendReport(int i){
  //assuming messageAndOperation is not defined or was deleted
  serverClients[i].messageAndOperation = new MessageAndOperation(&(serverClients[i].theClient));//reading_buff and readCharNumber are dummy here because the intent is just to send a report
                                                                                                // Also you must not use the method sendAck!
  Serial.println("Sending only report");
  serverClients[i].messageAndOperation->sendReport();
  Serial.println("now deleting messageAndOperation");
  delete (serverClients[i].messageAndOperation);
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

