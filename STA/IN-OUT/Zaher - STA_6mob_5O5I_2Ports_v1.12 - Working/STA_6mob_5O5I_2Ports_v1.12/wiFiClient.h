#include "MessageAndOperation.h"

class wiFiClient{
  public:
  WiFiClient theClient;
    
  int counterToAssignANewClient = 0;
  static const int maxLoopCounterToAssignANewClient = 250;//it's 250 times for a delay of 750 ms. The mobile app needs 150 out of the 250 to renew its socket.
  //it's like 3 minutes for a value of 1000 //e.g. every 180 loops (each loop is delayed by 1 second) or 540 loops (each loop is delayed by 300 milliseconds) the client if connected will be deleted - no it's on the stack not the heap
  //The idea of maxLoopCounterToAssignANewClient is not harmful to the client which is receiving reports since the report should have been already sent in a previous loop before I delete the client here.
  //So this is the "socket lifetime"
  //static const int maxLoopCounterToAssignANewClient = 13;
  
  MessageAndOperation* messageAndOperation;
  boolean messageAndOperationWasDefined = false;  
};


