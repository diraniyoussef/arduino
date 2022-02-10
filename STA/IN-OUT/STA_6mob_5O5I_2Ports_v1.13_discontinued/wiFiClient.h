#include "MessageAndOperation.h"

class wiFiClient{
public:
  ~wiFiClient() {
    if (messageAndOperation)//not needed but ok. It's not needed because when the call to delete the wiFiClient instance happens 
                            // (at the begining of each loop) they would have been deleted before.
      delete messageAndOperation;
  }
  
  WiFiClient theClient;
  
  int counterToAssignANewClient = 0;
  static const int maxLoopCounterToAssignANewClient = 250;//it's 250 times for a delay of 750 ms. The mobile app needs 150 out of the 250 to renew its socket.
  
  MessageAndOperation* messageAndOperation;
  boolean messageAndOperationWasDefined = false;  
};

class Couple {
public:
  int i = -1;
  int j = -1;
};


class AllClients {
public:
//  wiFiClient*** client = new wiFiClient*[MAX_SERVED_CLIENTS][SERVERS_NUMBER_TO_MAINTAIN_CONNECTION]; //I thought of a pointer to make it easy to reorder them in the array
  wiFiClient* client[MAX_SERVED_CLIENTS][SERVERS_NUMBER_TO_MAINTAIN_CONNECTION];//these "client" pointers are on the stack.
  int order[MAX_SERVED_CLIENTS][SERVERS_NUMBER_TO_MAINTAIN_CONNECTION];
  String id[MAX_SERVED_CLIENTS][SERVERS_NUMBER_TO_MAINTAIN_CONNECTION];
  //This ordering is useful in manipulating the received messages. And it will be set after
  // manipulating, at the beginning of a socket connection, and at its closure.
  //Note: it's ok for the order[i][j] to have the same value as its twin order[i][the other j]. But order[i][j] should
  // NOT have the same value another order[ANOTHER i][any j]
  // Note: When order is not 0 then there must be connected client. And if it's 0 then no connected client.

  AllClients() {
    for (int i = 0; i < MAX_SERVED_CLIENTS; i++)
      for (int j = 0; j < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; j++)
        client[i][j] = new wiFiClient();
  }

  wiFiClient aClient;
  
  int getMaxOrder() {
    int maxVal = 0;
    for (int i = 0; i < MAX_SERVED_CLIENTS; i++) {
      for (int j = 0; j < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; j++) {
        maxVal = max(maxVal, order[i][j]);
      }
    }
    return (maxVal);
  }
  
  int incrementOrderOfAllClients() { //a client whose order is 0 won't increment.
    //incrementing means lowering the priority (usually in order to give the highest prio "1" to the newly created client).
    for (int i = 0; i < MAX_SERVED_CLIENTS; i++) {
      for (int j = 0; j < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; j++) {
        if (order[i][j] != 0)
          order[i][j]++;
      }
    }
  }
  
  void orderNewClient(int i, int j) {
    if (order[i][General::getOtherIndex(j)] != 0) { //General::getOtherIndex works for just 0 and 1 no more.
      order[i][j] = order[i][General::getOtherIndex(j)];
    } else {
      incrementOrderOfAllClients();
      order[i][j] = 1;
    }
  }

  void reorderHigherClients(int k, int l) {
    //The index of all clients (or sockets) who have a greater index than order[k][l] will be decremented by 1.
    for (int i = 0; i < MAX_SERVED_CLIENTS; i++) {
      for (int j = 0; j < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; j++) {
        if ( order[i][j] != 0 && order[i][j] > order[k][l] )
          order[i][j]--;
      }
    }
    //It's up to the caller now what to do with order[k][l], e.g. to get rid of it or move it to the lowest priority (higher order).
  }

  Couple getFirstIndexFoundFromOrderValue(int orderVal) {
    Couple c;
    for (int i = 0; i < MAX_SERVED_CLIENTS; i++) {
      for (int j = 0; j < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; j++) {
        if (order[i][j] != 0 && order[i][j] == orderVal)
          c.i = i;
        c.j = j;
        return (c); //according to the usage  of this method, the code will always enter here.
      }
    }
    return (c);
  }

  ~AllClients() {
    for (int i = 0; i < MAX_SERVED_CLIENTS; i++)
      for (int j = 0; j < SERVERS_NUMBER_TO_MAINTAIN_CONNECTION; j++)
        if (client[i][j])
          delete client[i][j];
/*    if (client)//I don't think this is valid since "client" array is on the stack.
      delete client;*/
  }

};
