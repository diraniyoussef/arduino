#include "LocalServer.h"

class InformLocal { //If you want to disable this you may do it in the main sketch file.  
private:    
  ObeyingIPs obeying_IPs;
/*PLEASE NOTE THAT 
  * THE VALUES OF EACH ITEM OF panel_IP IS MADE OF LENGTH 15 ON PURPOSE.
  * Actually, the value of each item of panel_IP is dummy here, since it will be overriden by the user. (Actually it 
  * can be set according to a convention with the user, which is to be incremented from this panel's IP but I chose to take the IPs from 
  * user.)
  */
//  char panel_IP[ Max_Inform_Local_Panels_Number ][ Max_IP_Size_As_Buff ]; //this didn't work, so I went with the following.
/*
  char* panel_IP[ Max_Inform_Local_Panels_Number ] = { "192.168.123.212" };
  const char* panel_id[ Max_Inform_Local_Panels_Number ] = { "5:\0" }; //Never dummy
  int counter_to_await_report_receipt[ Max_Inform_Local_Panels_Number ] = { 0 }; //-1 means not awaiting and 0 means to send right away  
  AsynchroClient asynchro_client[ Max_Inform_Local_Panels_Number ];
  const AsynchroClient* asynchro_client_pointer_array[ Max_Inform_Local_Panels_Number ] = { &asynchro_client[0] };
*/
  char* panel_IP[ Max_Inform_Local_Panels_Number ] = { "192.168.123.212", "192.168.123.213" };
  const char* panel_id[ Max_Inform_Local_Panels_Number ] = { "9:\0", "10:\0" }; //Not dummy. These are the serious values.
  int counter_to_await_report_receipt[ Max_Inform_Local_Panels_Number ] = { 0 , 0 }; //-1 means not awaiting and 0 means to send right away
  AsynchroClient asynchro_client[ Max_Inform_Local_Panels_Number ];  
  const AsynchroClient* asynchro_client_pointer_array[ Max_Inform_Local_Panels_Number ] = { &asynchro_client[0], &asynchro_client[1] };
  
  long time_to_await_for_report_receipt = 8000; //PLEASE let this smaller than max_counter_to_destroy_old_socket
  InformEntity inform_local_entity;
  
  static void sendReport( AsyncClient* newest_async_client, const char* panel_id_i ) {
    NodeMCU::yieldAndDelay(); //I think this delay may be useful...
    int last_length = strlen( (const char*) owner_id_buff ) + strlen( (const char*) mod_id_buff ) + strlen( (const char*) panel_id_i );
    char* totalMessage_buff = new char[ last_length + 3 * PCF::absolute_max_pins_number + 1 ]; //the + 1 is for the '\0'
    strcpy( totalMessage_buff, (const char*) owner_id_buff );
    strcat( totalMessage_buff, (const char*) mod_id_buff );
    strcat( totalMessage_buff, (const char*) panel_id_i );
    
    last_length = RemoteServerMessageOp::addPinsToReport( totalMessage_buff, last_length ); //nothing is special about RemoteServerMessageOp, it's just the addPinsToReport
    totalMessage_buff[ last_length ] = '\0';

    RemoteServerMessageOp::sendMessageToAsync( newest_async_client, totalMessage_buff );    
    delete[] totalMessage_buff;
  }
  
public:  
  void setup( ) {
    //first let's set the panel_IP from EEPROM    
    obeying_IPs.other_IPs_gotten = false;
    //now getting from EEPROM the obeying IPs
    obeying_IPs.getObeyingIPsFromEEPROM( (char**) panel_IP );
    
    if( !obeying_IPs.other_IPs_gotten ) {  
      toggle_connect_failure_notifier_pin = true;
      return;
    }
    
    for( int i = 0 ; i < obeying_IPs.panels_number ; i++ ) {
      asynchro_client[i].setAsynchroClient( (char*) panel_IP[i] , 120 , 121 ); //usually all local clients listen on these two ports as being servers.
    }
    inform_local_entity.setup( obeying_IPs.panels_number, panel_id , counter_to_await_report_receipt , time_to_await_for_report_receipt , 
                                asynchro_client_pointer_array , sendReport );
  }

  void stopOperations() {
    obeying_IPs.other_IPs_gotten = false;
    obeying_IPs.panels_number = 0;   
    for( int i = 0 ; i < obeying_IPs.panels_number ; i++ ) {
      asynchro_client[i].stopOperations();
    }
    inform_local_entity.stopOperations();
  }

  void process() {
    if( !obeying_IPs.other_IPs_gotten ) {
      toggle_connect_failure_notifier_pin = true;      
      return;
    }
    Serial.println("Processing local entities.");
    for( int i = 0 ; i < obeying_IPs.panels_number ; i++ ) {
      asynchro_client[i].preProcess();
      
      //check incoming messages if any
      for( int incoming_message1_index = 0 ; incoming_message1_index < AsynchroClient::max_incoming_messages_number ; incoming_message1_index++ ) {    
        if( asynchro_client[i].message_in_processing_from_client_1[ incoming_message1_index ] ) {
          Serial.printf("Checking incoming local message from async 1 of asynchro index %d\n", i);
          Serial.printf("So processing message of port 1 of buffer index %d\n", incoming_message1_index);
          inform_local_entity.checkIncomingMessageAndFixCounter( (char*) asynchro_client[i].buff_1[ incoming_message1_index ], 
                                                                  asynchro_client[i].len_buff_1[ incoming_message1_index ] );
                                                                  /* Not much important note,
                                                        * but anyway: inside this method "checkIncomingMessageAndFixCounter", we have 
                                                        * "getInformedEntityId". This latter works right but makes sort of a redundant task inside when it 
                                                        * comes to knowing the index of the particular mobile (sender) within the "mob_i_Id_buff".
                                                         */         
          asynchro_client[i].message_in_processing_from_client_1[ incoming_message1_index ] = false;
        }
      }

      for( int incoming_message2_index = 0 ; incoming_message2_index < AsynchroClient::max_incoming_messages_number ; incoming_message2_index++ ) {    
        if( asynchro_client[i].message_in_processing_from_client_2[ incoming_message2_index ] ) { 
          Serial.printf("Checking incoming local message from async 2 of asynchro index %d\n", i);
          Serial.printf("So processing message of port 2 of buffer index %d\n", incoming_message2_index);
          inform_local_entity.checkIncomingMessageAndFixCounter( (char*) asynchro_client[i].buff_2[ incoming_message2_index ], 
                                                                  asynchro_client[i].len_buff_2[ incoming_message2_index ] );                                                         
          asynchro_client[i].message_in_processing_from_client_2[ incoming_message2_index ] = false;
        }
      }
    }      

    //check In pins and send reports accordingly
    inform_local_entity.informAllIfInPinChanged();    
    //Say it was connected then disconnected then connected again, the following gives attention to the non-connected entities.
    inform_local_entity.adjustCounterIfDisconnected();
    //handle counters
    inform_local_entity.resendReport_and_adjustCounter();
             
    for( int i = 0 ; i < obeying_IPs.panels_number ; i++ ) {
      asynchro_client[i].postProcess();
    }
  }

};
