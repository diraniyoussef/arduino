#include "MessageAndOperation.h"

class wiFiClient{
  public:
  WiFiClient theClient;
    
  int counterToAssignANewClient = 0;
  static const int maxLoopCounterToAssignANewClient = 100; //e.g. every 180 loops (each loop is delayed by 1 second) or 540 loops (each loop is delayed by 300 milliseconds) the client if connected will be deleted - no it's on the stack not the heap
  //The idea of maxLoopCounterToAssignANewClient is not harmful to the client which is receiving reports since the report should have been already sent in a previous loop before I delete the client here.
    
  MessageAndOperation* messageAndOperation;
  boolean messageAndOperationWasDefined = false;  
};


