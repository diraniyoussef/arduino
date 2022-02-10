#include "MessageAndOperation.h"
extern const int delay_per_loop;

class wiFiClient {
private:
  const int max_counter_to_assign_a_new_client = 3.5 * 60 * 1000; 
public:
  int max_loop_counter_to_assign_a_new_client;
  WiFiClient theClient;
  MessageAndOperation* messageAndOperation;

  wiFiClient() {
    max_loop_counter_to_assign_a_new_client = floor( max_counter_to_assign_a_new_client / delay_per_loop );//it's like 250 times for a delay of 750 ms. 
    messageAndOperation = new MessageAndOperation( &theClient );
  }
  
  int counterToAssignANewClient = 0;

  ~wiFiClient() {
    delete messageAndOperation;
  }
  
};


