#include "MessageAndOperation.h"

class wiFiClient{
  public:
  WiFiClient theClient;
    
  int counterToAssignANewClient = 0;
  static const int maxLoopCounterToAssignANewClient = 180; //e.g. every 180 loops (each loop is delayed by 1 second) the client if connected will be deleted - no it's on the stack not the heap
  
  MessageAndOperation* messageAndOperation;
  boolean messageAndOperationWasDefined = false;  
};


