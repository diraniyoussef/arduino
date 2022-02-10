#include "Generic.h"

//extern  char out_pin_symbol[4];
extern  char out_pin_symbol[3]; //for debugging

extern PCF* PCF1;

class RequestType{
  private: 
  char pin;
  boolean action;
  public:
  RequestType(byte, boolean);
  char getPin();
  boolean getAction();
};

class Request{
  private:
  boolean justReport;
  RequestType* theRequestType;
  public:
  Request();
  ~Request();
  Request(RequestType*);
  boolean isJustReport();
  RequestType* getRequestType();
};

class MessageAndOperation {
private: 
  const String module_Id_Str = "Zaher_03884818:panel_home_under_stairs_bathroom:"; //this is very unintuitive to be correct (since in the sketch its definition is placed after the include to this header file) but luckily it is.
  static const int MAX_READING_BUFFER = 1024;  
  char* reading_buff;
  int readCharNumber;
  WiFiClient* aClient;

  boolean received_message = false; //just to know if reading_buff has been initialized.
  boolean haveSentAck = false; //just to know if outMessageHeader_buff has been initialized
  boolean haveAnalyzed = false; //just to know if theRequest has been initialized.

  Request* theRequest; //this is not null only if analyze() method is called.
  
  char* outMessageHeader_buff;
  int lastColonOfIncomingMessage; //on each received message the module sends two messages: Ack and a reply after an action on the module's pins.
  int getLastCharOccurenceOfMessage(char charToCheck, int searchUpToIndex) {
    for ( int i = searchUpToIndex - 2 ; i >= 0 ; i-- ) {//it's searchUpToIndex - 2 and not searchUpToIndex - 1 because
                                              // I don't want to read the null char sent by the mobile. But is it bad if it were '-1'? I think not.
      if (reading_buff[i] == charToCheck) {          
        return (i);
      }
    }
    return(-1);
  }

  int getFirstCharOccurenceStartingFromIndex( int index, char charToCheck, String s ) {
    if ( s.length() == 0 ) {
      return (-1);
    }
    char* buff = new char[ s.length() ]; //I have tested this section...
    s.toCharArray( buff, s.length() + 1 ); //there has to be + 1 to reach the last char (I guess because toCharArray automatically assigns the last char to the null char
    for (int i = index ; i < s.length() ; i++) {  
      if (s[i] == charToCheck) {          
        return (i);
      }
    }
    return(-1);
  }
  
  int firstColon;
  String getOwnerId( String s ) {
    firstColon = getFirstCharOccurenceStartingFromIndex( 0, ':', s );
    String s1 = "";
    if ( firstColon > 0 ) { //at least get one valid char from string s
      s1 = s.substring( 0, firstColon ); //..substring( 0, 1 ); will return the char at index 0 only.
    }
    return (s1);
  }

  const static int mob_Number = 2;
  const String mob_Id_Str[mob_Number] = { "mob1", "mob2"}; 
    
  String getMobIdOfMessage( String s ) {
    String s1 = "";
    if ( firstColon == -1) {
      return (s1);
    }
    int secondColon = getFirstCharOccurenceStartingFromIndex( firstColon + 1, ':', s );
    if ( secondColon > firstColon + 1 ) {
      s1 = s.substring( firstColon + 1, secondColon ); 
    }
    return (s1);
  }

public:   
  MessageAndOperation(WiFiClient* aClient_arg) { //this constructor is called at the beginning of a new connection. BTW in a new connection we only use the sendReport method
    //In case this constructor was used, you must not use the method sendAck() because it asks for the id of the app e.g. mob1: which we can't get because we don't consider
    // the incoming message.
    aClient = aClient_arg;
  }

  MessageAndOperation(WiFiClient* aClient_arg, boolean received_message_arg){ //received_message_arg is a dumb parameter whose sole perpose is to call this particular constructor and within this constructor we set received_message to true
    received_message = true;
    aClient = aClient_arg;
    if ( aClient->available() ) { //we've already known that it's true
      reading_buff = new char[MAX_READING_BUFFER];
      readCharNumber = aClient->readBytesUntil('\0', reading_buff, MAX_READING_BUFFER); //I'm expecting to read 7 or 8 characters then the '\0'
      reading_buff[ MAX_READING_BUFFER - 1 ] = '\0'; //is useful anyway...
      NodeMCU::yieldAndDelay();                   
    }
  }
  
  ~MessageAndOperation() {
    Serial.println("inside the destructor");
    //The method of   if(...) then delete (...) is fine here because I won't be deleting the (...) anywhere else.
    if ( received_message ) {
      Serial.println("deleting reading_buff");
      delete[] reading_buff;
      //delete aClient; //don't do that... the WiFiClient (which is a socket) must remain!
      Serial.println("deleting theRequest");
      if ( haveAnalyzed )
        delete theRequest;
      Serial.println("deleting outMessageHeader_buff");
      if ( haveSentAck )
      delete[] outMessageHeader_buff;
    }
    /*I believe something is wrong by just calling 
     * if (ptr)
     *   delete (ptr)
     * This is why I keep manual track of pointers.
     */
  }
    
  String getIdOfClient() {
    /*check is the mobile belongs to the owner's family. If not then assign nothing to *id_ptr.
     * 1) compare the client to the owner's id - they should match
     * 2) compare the client to the list of mob ids - it should match one
    */
    String owner_id = getOwnerId( module_Id_Str );//this should be safe because we set module_id_str here in this class.
    String message = String(reading_buff);
    String owner_id_of_message = getOwnerId( message ); //String(...) works since we know that reading_buff is null terminated.
    String mob_Id_of_message;
    if ( owner_id_of_message == "" || owner_id_of_message != owner_id ) {
      return ("");
    }

    mob_Id_of_message = getMobIdOfMessage( message );//MUST use getMobIdOfMessage right after getOwnerId( message ) and not right after getOwnerId( module_Id_Str ) in order for firstColon to be valid
    if ( mob_Id_of_message == "" ) //e.g. if it didn't find the second ':'
      return "";     
    boolean mob_Id_of_message_is_valid = false;
    for ( int i = 0; i < mob_Number; i++ ) {
      if ( mob_Id_Str[i] == mob_Id_of_message){
        mob_Id_of_message_is_valid = true;
        break;
      }          
    }
    if (!mob_Id_of_message_is_valid) {
      return ("");
    }       
    return( mob_Id_of_message );   
  }
  
  void sendAck(){//in addition to sending an ack, this method assigns values to outMessageHeader_buff and lastColonOfIncomingMessage
   
    Serial.println("now starting the block of sendAck");
    String usefulResponse = "ACK";
    //Assuming the gotten message is like mob1:..... so to get the id of the sender we need to get the position of the colon ":"
    lastColonOfIncomingMessage = getLastCharOccurenceOfMessage(':', readCharNumber);////useful characters in reading_buff are from 0 to readCharNumber - 1
    
    //setting the final array according to size. 
    //Our response will be someting like:         server1:mob1:ACK 
    int totalMessageLength = module_Id_Str.length() + (lastColonOfIncomingMessage + 1) + usefulResponse.length() + 1 ; //+1 for the null char I believe
    char* totalMessage_buff = new char [totalMessageLength]; //(lastColonOfIncomingMessage + 1) is to get the length of "mob1:"
                                                             // the first + 1 at the end is to add '\n'
                                                             // and the second +1 is to add '\0'
    //assigning the final array
    char* moduleId_buff = new char[module_Id_Str.length()];
    module_Id_Str.toCharArray(moduleId_buff, module_Id_Str.length() + 1); //probably the +1 is necessary because of the null char at the end?
    
    char* usefulResponse_buff = new char[usefulResponse.length()];
    usefulResponse.toCharArray(usefulResponse_buff, usefulResponse.length() + 1);
    
    //setting the final array
    memcpy( totalMessage_buff, moduleId_buff, module_Id_Str.length() );//that's for server1: //it should be  * sizeof(char) but it's ok.
    memcpy( totalMessage_buff + module_Id_Str.length(), reading_buff, lastColonOfIncomingMessage + 1 );//that's for mob1:
         
      //interrupting the original task to assign outgoing header message buffer for future use
    haveSentAck = true;
    outMessageHeader_buff = new char[module_Id_Str.length() + lastColonOfIncomingMessage + 2]; //clearly the length of useful chars is module_Id_Str.length() + lastColonOfIncomingMessage + 1. 
    outMessageHeader_buff[module_Id_Str.length() + lastColonOfIncomingMessage + 1] = '\0'; //I need this null char...
    memcpy(outMessageHeader_buff, totalMessage_buff, module_Id_Str.length() + lastColonOfIncomingMessage + 1); //the memcpy is correct 100%    
    
      //now getting back to our original task
    memcpy(totalMessage_buff + module_Id_Str.length() + lastColonOfIncomingMessage + 1, usefulResponse_buff, usefulResponse.length());
    Serial.printf( "now comes the out message buffer %s\n", String(outMessageHeader_buff).c_str() );    
    //totalMessage_buff[totalMessageLength - 2] = '\n';
    //totalMessage_buff[totalMessageLength - 1] = '\0';
    totalMessage_buff[totalMessageLength - 1] = '\0';
    
    //sending the message
    if (aClient)
      if (aClient->connected())
        aClient->write((const uint8_t*)totalMessage_buff, totalMessageLength); //I hope I can protect this...
    //aClient->flush(); //doesn't force sending
    
    //now delete unnecessary buffers
    delete[] totalMessage_buff;
    delete[] moduleId_buff;
    delete[] usefulResponse_buff;
    Serial.println("now ending the block of sendAck");
  }
  
  boolean analyze(){ //this method returns true if the received message can be understood    
    Serial.printf("the length of the read chars is %d\n", readCharNumber); //known when we used sendAck()
    if (readCharNumber - 1 >= lastColonOfIncomingMessage + 2){ //the +2 is for the 2 chars R?
      if (reading_buff[lastColonOfIncomingMessage + 1] == 'R' && reading_buff[lastColonOfIncomingMessage + 2] == '?') { //a report request for pins status        
        haveAnalyzed = true;
        theRequest = new Request();
        return (true);
      } else {
        //typically it's like mob1:O1T?
        if (readCharNumber - 1 >= lastColonOfIncomingMessage + 4){
          if (reading_buff[lastColonOfIncomingMessage + 1] == 'O'){
            //now making sure the incoming message indicates the pin clearly
            if ( General::arrayIncludeElement( out_pin_symbol , sizeof(out_pin_symbol) , reading_buff[lastColonOfIncomingMessage + 2] ) ||
                  General::arrayIncludeElement( PCF1->out_pin_symbol , PCF1->out_pins_number , reading_buff[lastColonOfIncomingMessage + 2] ) ) {
              //unfortunately String(out_pin_symbol).length() is 7 instead of 6 because of the added '\0' char when the buff is converted to String.
              if ( reading_buff[lastColonOfIncomingMessage + 3] == 'T' || reading_buff[lastColonOfIncomingMessage + 3] == 'F' ) {
                boolean actionType;
                if (reading_buff[lastColonOfIncomingMessage + 3] == 'T') {
                  actionType = true;
                } else {
                  actionType = false;
                }
                Serial.printf("In Analyze, the pin is %c and the action is ...\n", reading_buff[lastColonOfIncomingMessage + 2] );
                RequestType* theRequestType = new RequestType(reading_buff[lastColonOfIncomingMessage + 2], actionType);
                haveAnalyzed = true;
                theRequest = new Request(theRequestType);                
                return(true);
              } else {
                Serial.println("Useful received message doesn't state whether to turn the pin on or off.");
              }
            } else {
              Serial.printf("Useful received message doesn't state a known pin symbol. Claimed pin symbol was: %c%c%c\n", 
                              reading_buff[lastColonOfIncomingMessage + 1], reading_buff[lastColonOfIncomingMessage + 2], reading_buff[lastColonOfIncomingMessage + 3]);
            }            
          } else {
            Serial.println("Useful received message doesn't start with R or O.");
          }
        } else {
          Serial.println("Useful received message is less than 3 chars and is not \"R?\"");
        }
      }
    } else {
      //something is wrong in the received message. But I won't feedback the user.
      Serial.println("Useful received message is less than 2 chars.");
    }
    return (false);
  }
  
  boolean isJustReport() {
    if (haveAnalyzed){ //the protection is not necessary if the method is correctly used.
      return(theRequest->isJustReport());
    }
    return(true);
  }
  
  void updatePinAndEEPROM() { //it is bad to enter this method if the pcf was not connected.
    char symbol = theRequest->getRequestType()->getPin();
    boolean state_bool = theRequest->getRequestType()->getAction();
    if( General::arrayIncludeElement( out_pin_symbol , sizeof(out_pin_symbol) , symbol ) ) {
      digitalWrite( NodeMCU::getRealPin( General::getIntFromHexChar( symbol ) ), state_bool );
      //I also need writing to EEPROM....................    
      NodeMCU::encodeEEPROM( symbol , state_bool );
    } else if( ! PCF1->updatePinAndEEPROM( symbol, state_bool ) ) {  //the return value of updatePinAndEEPROM method of PCF is just to know if the pin belongs to that particular PCF or not.
      /*
      if( ! PCF2->updatePinAndEEPROM( symbol, state_bool ) ) {
        ...
      } 
      */     
    }
  }

  void sendReport(){ //it is bad to enter this method if the pcf was not connected.
    NodeMCU::yieldAndDelay(50); //I think this delay may be useful...
    //I need to concatenate something like O1TO2FO3TO4FO5FO6FO7TO8T to outMessageHeader_buff and send it to client.

    String usefulOutMessage;
    if (received_message){
      usefulOutMessage = String(outMessageHeader_buff);    //it's like server1:mob1: or server1: when the communication has just been initiated
    } else {
      usefulOutMessage = module_Id_Str; //in this case it should hold "server1:" without determining any particular mobile.
    }
    
    NodeMCU::yieldAndDelay(1);//since I will read pins. It may not be a need but it's good anyway.    
    //Serial.printf("It should be: %d\n", String(out_pin_symbol).length() - 2);
    for( int i = 0 ; i < sizeof(out_pin_symbol) ; i++ ) { //please note that the real length of out_pin_symbol is String(out_pin_symbol).length() - 1
      String stateOfPinString;
      int pinAsInt = General::getIntFromHexChar( out_pin_symbol[i] );
      if( digitalRead( NodeMCU::getRealPin( pinAsInt ) ) ) { //NodeMCU::getRealPin(i) is the same as saying Di as of D1 or D2 or ...
        stateOfPinString = "T";                       
      } else {
        stateOfPinString = "F";
      }
      usefulOutMessage = usefulOutMessage + "O" + String( out_pin_symbol[i] ) + stateOfPinString;      
    }
    if(! PCF1->addPinsToReport( &usefulOutMessage ) ) {//valid condition if the PCF is not connected.
      Serial.println("message not sent since the PCF is disconnected");
      //don't send the message.
      return;
    }    
    usefulOutMessage = usefulOutMessage + "x";//any character here I will override it few lines later
      
    //getting the buff out of the message
    char* usefulOutMessage_buff = new char[usefulOutMessage.length()]; //try it without +1   //wrong to add them: lastColonOfIncomingMessage + 1 + 
    usefulOutMessage.toCharArray(usefulOutMessage_buff, usefulOutMessage.length() + 1);//the +1 here is like an exception..... no +1 I guess.............
    usefulOutMessage_buff[usefulOutMessage.length() - 1] = '\0'; //this interworks with the +1  //wrong to add them: lastColonOfIncomingMessage + 1 + 
    //sending the message
    if (aClient) {
      if (aClient->connected()){
        aClient->write( (const uint8_t*)usefulOutMessage_buff, usefulOutMessage.length() );
        Serial.printf( "Sent message was like %s\n", String(usefulOutMessage_buff).c_str() );
      } else {
        Serial.println("aClient is not connected.");
      }
    } else {
      Serial.println("aClient is null.");
    }
//    aClient->flush();

    //now delete unnecessary buffers    
    delete[] usefulOutMessage_buff;
  }
    
};

Request::Request(){
  justReport = true;    
};

Request::Request(RequestType* theRequestType_arg){
  theRequestType = theRequestType_arg;
  justReport = false;
}

Request::~Request() {
  if (!justReport)
    delete theRequestType;
};

boolean Request::isJustReport(){
  return (justReport);
}

RequestType* Request::getRequestType() {  
  return(theRequestType);
}

RequestType::RequestType(byte pin_arg, boolean action_arg){
  pin = pin_arg;
  action = action_arg;
}
char RequestType::getPin(){
  return(pin);
}
boolean RequestType::getAction(){
  return(action);
}




