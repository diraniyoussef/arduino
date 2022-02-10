#include "_Generic.h"

extern const String moduleIdStr; //this is very unintuitive to be correct (since in the sketch its definition is placed after the include to this header file) but luckily it is.
extern  char includedPinIndexAsChar[4];

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
  Request(RequestType*);
  boolean isJustReport();
  RequestType* getRequestType();
};

class MessageAndOperation {
  private: 
  boolean onConnectionStart = false;
  char* reading_buff; 
  int readCharNumber;
  WiFiClient* aClient;

  Request* theRequest;
  
  char* outMessageHeader_buff;
  int lastColonOfIncomingMessage; //on each received message the module sends two messages: Ack and a reply after an action on the module's pins.
  int getFirstCharOccurence(char charToCheck, int searchUpToIndex) {
    int i;
    for (i = 0 ; i < searchUpToIndex - 1 ; i++) {
      if (reading_buff[i] == charToCheck) {          
        return (i);
      }
    }
    return(-1);
  }
        
  public: 
  
  MessageAndOperation(WiFiClient* aClient_arg) { //this constructor is called at the beginning of a new connection. BTW in a new connection we only use the sendReport method
    //In case this constructor was used, you must not use the method sendAck() because it asks for the id of the app e.g. mob1: which we can't get because we don't consider
    // the incoming message.
    aClient = aClient_arg;                
    onConnectionStart = true; 
  }
  MessageAndOperation(WiFiClient* aClient_arg, char* reading_buff_arg, int readCharNumber_arg){
    aClient = aClient_arg;
    reading_buff = reading_buff_arg;
    readCharNumber = readCharNumber_arg;
  }

/*  void sendAckWithoutId(){
    Serial.println("now starting the block of sendAckWithoutId");
    String usefulResponse = "ACK";
    
    //setting the final array according to size. 
    //Our response will be someting like:         server1:ACK      to a message like     mob1:R?     but this method   sendAckWithoutId  is used when we're not going to read the incoming message.
    //We just want to send the report.
    
    int totalMessageLength = moduleIdStr.length() + usefulResponse.length() + 1 ; //+1
    char* totalMessage_buff = new char [totalMessageLength]; //(lastColonOfIncomingMessage + 1) is to get the length of "mob1:"
    
    //assigning the final array
    char* moduleId_buff = new char[moduleIdStr.length()];
    moduleIdStr.toCharArray(moduleId_buff, moduleIdStr.length() + 1); //probably the +1 is necessary because of the null char at the end?

    char* usefulResponse_buff = new char[usefulResponse.length()];
    usefulResponse.toCharArray(usefulResponse_buff, usefulResponse.length() + 1);

    //setting the final array
    memcpy(totalMessage_buff, moduleId_buff, moduleIdStr.length());//that's for server1: //it should be  * sizeof(char) but it's ok.    
        
      //interrupting the original task to assign outgoing header message buffer for future use
    outMessageHeader_buff = new char[moduleIdStr.length() + 1]; //clearly the length of useful chars is moduleIdStr.length() + lastColonOfIncomingMessage + 1. 
    outMessageHeader_buff[moduleIdStr.length()] = '\0'; //I need this null char...
    memcpy( outMessageHeader_buff, totalMessage_buff, moduleIdStr.length() ); //the memcpy is correct 100%
    Serial.println("now comes the out message buffer");
    Serial.println(outMessageHeader_buff);   

    //now getting back to our original task
    memcpy(totalMessage_buff + moduleIdStr.length(), usefulResponse_buff, usefulResponse.length());        

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
    Serial.println("now ending the block of sendAckWithoutId");
  }
  */
  
  void sendAck(){//in addition to sending an ack, this method assigns values to outMessageHeader_buff and lastColonOfIncomingMessage
    
    Serial.println("now starting the block of sendAckWithId");
    String usefulResponse = "ACK";
    //Assuming the gotten message is like mob1:..... so to get the id of the sender we need to get the position of the colon ":"
    lastColonOfIncomingMessage = getFirstCharOccurence(':', readCharNumber);////useful characters in reading_buff are from 0 to readCharNumber - 1
    
    //setting the final array according to size. 
    //Our response will be someting like:         server1:mob1:ACK 
    int totalMessageLength = moduleIdStr.length() + (lastColonOfIncomingMessage + 1) + usefulResponse.length() + 1 ; //+1
    char* totalMessage_buff = new char [totalMessageLength]; //(lastColonOfIncomingMessage + 1) is to get the length of "mob1:"
                                                             // the first + 1 at the end is to add '\n'
                                                             // and the second +1 is to add '\0'
    //assigning the final array
    char* moduleId_buff = new char[moduleIdStr.length()];
    moduleIdStr.toCharArray(moduleId_buff, moduleIdStr.length() + 1); //probably the +1 is necessary because of the null char at the end?
    
    char* usefulResponse_buff = new char[usefulResponse.length()];
    usefulResponse.toCharArray(usefulResponse_buff, usefulResponse.length() + 1);
    
    //setting the final array
    memcpy(totalMessage_buff, moduleId_buff, moduleIdStr.length());//that's for server1: //it should be  * sizeof(char) but it's ok.
    memcpy(totalMessage_buff + moduleIdStr.length(), reading_buff, lastColonOfIncomingMessage + 1);//that's for mob1:
         
      //interrupting the original task to assign outgoing header message buffer for future use
    outMessageHeader_buff = new char[moduleIdStr.length() + lastColonOfIncomingMessage + 2]; //clearly the length of useful chars is moduleIdStr.length() + lastColonOfIncomingMessage + 1. 
    outMessageHeader_buff[moduleIdStr.length() + lastColonOfIncomingMessage + 1] = '\0'; //I need this null char...
    memcpy(outMessageHeader_buff, totalMessage_buff, moduleIdStr.length() + lastColonOfIncomingMessage + 1); //the memcpy is correct 100%
    Serial.println("now comes the out message buffer");
    Serial.println(outMessageHeader_buff);   
    
      //now getting back to our original task
    memcpy(totalMessage_buff + moduleIdStr.length() + lastColonOfIncomingMessage + 1, usefulResponse_buff, usefulResponse.length());
    
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
    Serial.println("now ending the block of sendAckWithId");
  }
  
  boolean analyze(){ //this method returns true if the received message can be understood    
    Serial.printf("the length of the read chars is %d\n", readCharNumber);
    if (readCharNumber - 1 >= lastColonOfIncomingMessage + 2){ //the +2 is for the 2 chars R?
      if (reading_buff[lastColonOfIncomingMessage + 1] == 'R' && reading_buff[lastColonOfIncomingMessage + 2] == '?') {//a report request for pins status
        theRequest = new Request();
        return (true);
      } else {
        //typically it's like mob1:O1T?
        if (readCharNumber - 1 >= lastColonOfIncomingMessage + 4){
          if (reading_buff[lastColonOfIncomingMessage + 1] == 'O'){
            if ( General::arrayIncludeElement(includedPinIndexAsChar, sizeof(includedPinIndexAsChar), reading_buff[lastColonOfIncomingMessage + 2]) ) { //to make sure the incoming message indicates the pin clearly //not so needed but good
              //unfortunately String(includedPinIndexAsChar).length() is 7 instead of 6 because of the added '\0' char when the buff is converted to String.
              if (reading_buff[lastColonOfIncomingMessage + 3] == 'T' || reading_buff[lastColonOfIncomingMessage + 3] == 'F'){                            
                boolean actionType;
                if (reading_buff[lastColonOfIncomingMessage + 3] == 'T'){
                  actionType = true;
                } else {
                  actionType = false;
                }
                RequestType* theRequestType = new RequestType(reading_buff[lastColonOfIncomingMessage + 2], actionType);
                theRequest = new Request(theRequestType);                
                return(true);
              } else {
                Serial.println("Useful received message doesn't state whether to turn the pin on or off.");
              }
            } else {
              Serial.println("Useful received message doesn't state the pin number.");
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
    if (theRequest){//the protection is not necessary if the method is correctly used. //don't know if this causes trouble................
      return(theRequest->isJustReport());
    }
    return(true);
  }
  
  void updatePinsAndEEPROM() {
    digitalWrite( NodeMCU::getRealPin( General::getIntFromHexSymbol( theRequest->getRequestType()->getPin() ) ), theRequest->getRequestType()->getAction()); 
    //I also need writing to EEPROM....................    
    NodeMCU::encodeEEPROM(theRequest->getRequestType()->getPin(), theRequest->getRequestType()->getAction());         
  }

  void sendReport(){
    NodeMCU::yieldAndDelay(50); //I think this delay may be useful...
    //I need to concatenate something like O1TO2FO3TO4FO5FO6FO7TO8T to outMessageHeader_buff and send it to client.
    int i = 1;   
    String usefulOutMessage;
    if (!onConnectionStart){
      usefulOutMessage = String(outMessageHeader_buff);    //it's like server1:mob1: or server1: when the communication has just been initiated
    } else {
      usefulOutMessage = moduleIdStr; //in this case it should hold "server1:" without determining any particular mobile.
    }
    
    delay(1);//since I will read pins. It may not be a need but it's good anyway.
    String stateOfPinString;
    //Serial.printf("It should be: %d\n", String(includedPinIndexAsChar).length() - 2);
    for (i = 0; i < sizeof(includedPinIndexAsChar); i++){ //please note that the real length of includedPinIndexAsChar is String(includedPinIndexAsChar).length() - 1
      int pinAsInt = General::getIntFromHexSymbol( includedPinIndexAsChar[i] );
      if ( digitalRead( NodeMCU::getRealPin( pinAsInt ) ) ){ //NodeMCU::getRealPin(i) is the same as saying Di as of D1 or D2 or ...
        stateOfPinString = "T";                       
      } else {
        stateOfPinString = "F";
      }
      usefulOutMessage = usefulOutMessage + "O" + String( includedPinIndexAsChar[i] ) + stateOfPinString;      
    }
    usefulOutMessage = usefulOutMessage + "7";//any character here I will override it few lines later
      
    //getting the buff out of the message
    char* usefulOutMessage_buff = new char[usefulOutMessage.length()]; //try it without +1   //wrong to add them: lastColonOfIncomingMessage + 1 + 
    usefulOutMessage.toCharArray(usefulOutMessage_buff, usefulOutMessage.length() + 1);//the +1 here is like an exception..... no +1 I guess.............
    usefulOutMessage_buff[usefulOutMessage.length() - 1] = '\0'; //this interworks with the +1  //wrong to add them: lastColonOfIncomingMessage + 1 + 
    //sending the message
    if (aClient) {
      if (aClient->connected()){
        Serial.print("Now sending a report: ");
        Serial.println(usefulOutMessage);
        aClient->write((const uint8_t*)usefulOutMessage_buff, usefulOutMessage.length());
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

boolean Request::isJustReport(){
  return (justReport);
}

RequestType* Request::getRequestType(){  
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



