#include "wiFiClient.h"

AllClients* clients = new AllClients(); //I want to explicitly delete this pointer when I want to restart the NodeMCU. It will never be deleted anywhere else.
const String moduleIdStr = "Zaher_home_03884818:panel1:";
char includedPinIndexAsChar[5] = {'3', '4', '5', '6', '9'}; //please put them here in the same order as in the app. It's nicer but not necessary.
                                    //'0' means D0. '1' means D1. 'a' means 10. And so on.

//char includedPinIndexAsChar[4] = {'3', '4', '6', '5'}; //for the sake of debugging using the serial monitor you must free '9'
#define maxReadingBuffer 1024
    
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
  pinMode(D9, OUTPUT);  

  pinMode(D10, OUTPUT);  
    
  NodeMCU::wifiSetup(); //if not connected to WiFi, NodeMCU will restart.
  //EEPROM stuff
  //should fetch the EEPROM to set the pins according to most recent known state (e.g. before last restart).
  NodeMCU::lowerAllOutPins();//this is not to let any pin floating.
  NodeMCU::yieldAndDelay(1);
  NodeMCU::determineEEPROMSize();
  NodeMCU::decodeEEPROM(); //this determines the value of moduleIdStr which must be like "server1:" and the state of the pins.  
}

void loop()
{
  if( !NodeMCU::isConnectedToWiFi() ) {    
    delete clients; 
    NodeMCU::restartNodeMCU();
  }
  
  NodeMCU::checkManualUpdate();

  boolean clientIsNull = false;  
  Serial.println("for loop.");

  for(int server_i = 0; server_i <= SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; server_i++) {
    for(int client_i = 0; client_i < MAX_SERVED_CLIENTS; client_i++) {
      Serial.printf("client_i = %d and server_i = %d", client_i, server_i);    
      clientIsNull = false;

      //This "if" block checks if the client is to be nulled, and increments the client's freeing-resource counter.
      if( !clients->client[client_i][server_i]->theClient ) {//it is theClient that is nulled, precedence operator is safe here.
        clientIsNull = true;        
      } else {
        if ( !clients->client[client_i][server_i]->theClient.connected() ) {
          //not sure if wise to do ...->theClient.stop() //I think it's not useful at all.
          clientIsNull = true;           
        } else {
          clients->client[client_i][server_i]->counterToAssignANewClient++;
          if ( clients->client[client_i][server_i]->counterToAssignANewClient == clients->client[client_i][server_i]->maxLoopCounterToAssignANewClient ) {
            //send a report anyway then close. This may be useful. This is logical before intentionally closing a socket.
            sendReport(client_i, server_i);//we already checked that it's connected...
            clients->client[client_i][server_i]->theClient.stop();//hopefully that when destroying it from the server, 
                                                                  // the client would be notified in his Android code
            clientIsNull = true;
          }
        }        
      }

      if ( clientIsNull ) {
        clients->client[client_i][server_i]->counterToAssignANewClient = 0;        
        
        //Now manage order and id
        if ( clients->order[ client_i ][ General::getOtherIndex( server_i ) ] != clients->order[ client_i ][ server_i ] ) {
          if ( clients->id[ client_i ][ General::getOtherIndex( server_i ) ] != clients->id[ client_i ][ server_i ] ) { //correct String comparison in Arduino            
              //this condition has to be true by the way, because if the brothers had the same order then they must have the same id          
            if ( clients->order[ client_i ][ server_i ] != 0 ) {//Now reorder all higher order clients...
              clients->reorderHigherClients( client_i, server_i );
            }
          } else {
            Serial.println("Weird case: brother client has the same id but different order.");
          }
        } else {
          Serial.println("client has a twin brother");
        }
        clients->order[client_i][server_i] = 0;
        clients->id[client_i][server_i] = "";

        //get a new client if possible and send a report if it really has gotten a client.
        clients->client[client_i][server_i]->theClient = server[server_i].available();     
        if ( clients->client[client_i][server_i]->theClient ) {
          if ( clients->client[client_i][server_i]->theClient.connected() ) {
            sendReport(client_i, server_i);
          }
        }   
      }
    }//end for
  }//end for
  
  NodeMCU::yieldAndDelay(50);
  //Now listen to clients and process the ones that sent something   
  /*
   * Big question first:
   * Is it good to do an order in the first place?
   * Answer: is I want to serve them all, no! If in each loop I want to serve just one then yes. But serving them all is better!
   * So I'm not sure if I want to continue!
   */
  /*
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
*/      
  NodeMCU::yieldAndDelay(750); //This is VERY IMPORTANT not only for the NodeMCU but also not to let the module cause flooding in messages to the requester (client).
                                // Flooding reports to the requester may cause the requester to execute the new report before the old one (in case of multi-threading)!
//NodeMCU::yieldAndDelay(2000);//for debugging

}

void sendReport(int i, int j) {
  //assuming messageAndOperation is not defined or was deleted
  clients->client[i][j]->messageAndOperation = new MessageAndOperation(&clients->client[i][j]->theClient);//the asterisk can work without parentheses according to operator precedence, so it's fine.
  //reading_buff and readCharNumber are dummy here because the intent is just to send a report
  // Also you must not use the method sendAck!
  Serial.println("Sending only report to allow a slot for a new mobile.");
  clients->client[i][j]->messageAndOperation->sendReport();
  Serial.println("now deleting messageAndOperation");
  delete clients->client[i][j]->messageAndOperation;
}

void manipulatingClientCounter(int i, int j) {
  if (clients->client[i][j]->theClient.connected()) { //if the mob disconects from its side, will serverClients[i].theClient remain not null?
    clients->client[i][j]->counterToAssignANewClient++;
  } else {
    if (clients->client[i][j]->counterToAssignANewClient > 0) { //We enter here when the mob e.g. closes the connection.
      clients->client[i][j]->counterToAssignANewClient = 0;
      Serial.println("WEIRD case. Not connected but had its counter > 0");
    }
  }
}

