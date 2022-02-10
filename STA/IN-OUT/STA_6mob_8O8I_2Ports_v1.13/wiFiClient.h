#include "MessageAndOperation.h"

class wiFiClient{
public:
  WiFiClient* theClient = new WiFiClient();
  String id; //having a field on the stack is not a big deal before restarting I guess  
                                                                        // because I'm deleting the "clients" instance, so I think the stack would be managed.  
  ~wiFiClient() {
    /*
    if (messageAndOperation)//not needed but ok. It's not needed because when the call to delete the wiFiClient instance happens 
                            //  (at the begining of each loop) they would have been deleted before.
                            // I omitted it because I believe it's dangerous (until I find a solution to it) 
                            //  so I keep manual track of messageAndOperation and delete it before.
      delete messageAndOperation;
    */
    //if (theClient) {//I guess it's fine since theClient is really instanciated, nevertheless I omitted it because this is not how we do it!
      delete theClient;
    //}
  }
    
  int counterToAssignANewClient = 0;
  static const int maxLoopCounterToAssignANewClient = 250;//it's 250 times for a delay of 750 ms. The mobile app needs 150 out of the 250 to renew its socket.
  
  MessageAndOperation* messageAndOperation;
  boolean messageAndOperationWasDefined = false;  
};

class Clients {
public:  
  wiFiClient client_[ MAX_SRV_CLIENTS ][ SERVERS_NUMBER_TO_MAINTAIN_CONNECTION ]; //it's an array of 'wiFiClient's. Hopefully because I delete the "clients"
                                                                                  // instance before restarting ESP so this array would be cleaned?
/*  boolean idPreExist( int server_i, String id ) { //normally should return false
    boolean doesExist = false;
    //Serial.printf("id to compare with is: %s\n", id.c_str() );
    for ( int client_i = 0; client_i < MAX_SRV_CLIENTS; client_i++ ) {
      //Serial.printf("id of client %d is %s\n", client_i, client_[client_i][server_i].id.c_str() );
      if ( client_[client_i][server_i].id == id ) {
        doesExist = true;
        break;
      }
    }
    return (doesExist);
  }
*/
  
  ~Clients(){
    for (int i = MAX_SRV_CLIENTS - 1; i >= 0 ; i--) {
      for (int j = 0; i < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; j++) {
        Serial.println("deleting client_ array members");
        delete &client_[i][j];
      }
    }
  }
  
};




