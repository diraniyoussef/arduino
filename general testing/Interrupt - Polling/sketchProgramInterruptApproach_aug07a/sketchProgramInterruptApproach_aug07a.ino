#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incoming_packet[255];
int server_IPAddr[4];
int server_port = 62003;
const byte alarm_pin = 14;
const byte cancel_pin = 13;
volatile boolean cancel_help_after_about_sending_packet = false;
volatile boolean help_packet_sent = false;
byte time_difference_cancel_and_help = 10;  

//setup for the minimum time interval for a new interrupt to be considered.
volatile unsigned long last_button_time = 0; 
unsigned long button_time = 0; 
unsigned long disabled_interrupt_time_interval = 10 * 60 * 1000; //10 minutes. A typical minimal value is probably 250 or 1000, but a 1 minutes value is ok for the police application                                                                
unsigned long long_int_max = 4294967295;
volatile boolean not_first_time = 0;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  pinMode(alarm_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(alarm_pin), helpRequest, FALLING);
  sei();
  pinMode(cancel_pin, INPUT);
  
  server_IPAddr[0] = 192;
  server_IPAddr[1] = 168;
  server_IPAddr[2] = 1;
  server_IPAddr[3] = 20;
  
  WiFi.disconnect();
  //WiFi.begin("Lumia 930 3419", "123yesno");  
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected to WiFi network with IP address: ");
  Serial.println(WiFi.localIP());              
}

void helpRequest(){
  button_time = millis();
  Serial.printf("Interrupt entered, time in ms is: %u\n", button_time);
  //The following if statement is just to prevent reentering the interrupt functionality in the transient case of voltage change, also not to call the police within a short amount of time.
  if (button_time - last_button_time > disabled_interrupt_time_interval || (button_time < last_button_time && (button_time + long_int_max - last_button_time > disabled_interrupt_time_interval)) || not_first_time == 0)//after about 50 days, millis() will overflow
  //By the way, this condition "button_time - last_button_time > disabled_interrupt_time_interval" is ok even if an overflow of about 50 days passed between button_time and last_button_time
  {
    Serial.printf("interrupt considered\n", millis());
    contactServer("help", 1);    
    last_button_time = button_time;
  }
  not_first_time = 1;
}

void contactServer(char* message, byte type){ //it's about sending and receiving a request and an ack in the same function
  boolean server_ack = false;
  do{
    // sending request
    Udp.begin(localUdpPort);
    //PACKET SENT TO THE IP ADDRESS AND PORT OF -------------------------------THE SERVER-------------------------------------------  
    //later to make a fully secured communication 
    Udp.beginPacket( IPAddress(server_IPAddr[0], server_IPAddr[1], server_IPAddr[2], server_IPAddr[3]), server_port ); //213,204,75,44
    Udp.write(message);
     //packet sending date
    Udp.endPacket();  
    help_packet_sent = true; //this means that after contactServer() function ends and so helpRequest(), and while control is in the loop, a cancel request may be considered
    Serial.printf("packet is sent.\n");

    //Now hold on to receive a packet from server
    unsigned long i = 0;    
    while (i<3600000000) //some time, but it's less than 2 seconds most probably
    {
      if (type == 1){ //type 1 message is a help request.
        if (i%8000000 == 0){//check digitalRead each 1 ms or more for stability. (By the way the esp8266 speedis 80MHz as the default speed)
          if( !digitalRead(cancel_pin) ){ //since cancel_pin is wired to a pull-up resistor, when it's LOW a cancel request should start
            cancel_help_after_about_sending_packet = true;
            help_packet_sent = false;
            Serial.printf("A cancel is triggered just after about sending the help packet\n");
            break;
          }
        }
      }
      incoming_packet[0] = 0; //is this like a null character?
      i = i + 1;
      //in case this is type 2 and somewhere the execution was interrupted before Udp.parsePacket() to send a help request, the cancel 
      int packetSize = Udp.parsePacket(); //Control doesn't hold here; it passes quickly over it.
      if (packetSize)
      {
        //must check, in a separate function, that the received packet is from server...
        Serial.printf("Received a packet.\n");
        if (checkPacketSource()){          
          int len = Udp.read(incoming_packet, 255);
          if (len > 0)
          {
            incoming_packet[len] = 0; //is this like a null character?
          }
          //now must check the content of the incoming packet          
          if (type == 1 && strcmp( incoming_packet, "help ack" ) == 0 ){
            Serial.printf("%s received\n", incoming_packet);
            server_ack = true;  
            break;
          }
          if (type == 2 && strcmp( incoming_packet, "cancel ack" ) == 0 ){
            Serial.printf("%s received\n", incoming_packet);
            server_ack = true;  
            break;
          }          
        }
      }
    }
    if (type == 1 && cancel_help_after_about_sending_packet){
      break;
    }
  } while(!server_ack);//resend the help request e.g. each 2 seconds if it hasn't received any ack from server  
}

String int_array_to_string(int int_array[], int size_of_array) {
  String returnstring = "";
  int temp = 0;
  for (temp = 0; temp < size_of_array - 1; temp++){    
    returnstring += String(int_array[temp]) + ".";
  }
  returnstring += String(int_array[temp]);
  Serial.printf(returnstring.c_str());
  return returnstring;
}

boolean checkPacketSource(){
  Serial.printf("Received an UDP packet from %s, port %d\n", Udp.remoteIP().toString().c_str(), Udp.remotePort());          
  if ( strcmp( Udp.remoteIP().toString().c_str(), int_array_to_string(server_IPAddr,4).c_str() ) == 0 && Udp.remotePort() == server_port){
    return true;
  } else {
    return false;
  }
  Serial.printf("Received an UDP packet from %s, port %d\n", Udp.remoteIP().toString().c_str(), Udp.remotePort());          
}
        

void loop()
{
  if (cancel_help_after_about_sending_packet){
    //help_packet_sent = false; //already made somewhere before this where it's necessary...
    Serial.printf("Now in loop and processing cancel\n");
    cancel_help_after_about_sending_packet = false;
    //now must send a packet to server asking him to cancel 
    contactServer("cancel", 2); //a cancel request is a type 2 message.
  } else {  
    if ( help_packet_sent ){            
      
      //cancel the interrupt pin ?in condition to make the impulse valid for some time like 1 second?
      //re-enable the interrupt pin
      unsigned long current_time = millis(); 
      if (current_time - last_button_time > disabled_interrupt_time_interval - time_difference_cancel_and_help || (current_time < last_button_time && (current_time + long_int_max - last_button_time > disabled_interrupt_time_interval - time_difference_cancel_and_help)))
      //time_difference_cancel_and_help is a supposed dedicated time to make a cancel request without the possibility of congesting with a help request that might (with a very low 
      // probablity) to happen when the interrupt becomes enabled again. It's like giving a priority for the cancel over the help request.
      {
        help_packet_sent = false;
      } else {        
        if( !digitalRead(cancel_pin) ) { //I guess 1 second while cancel_pin is down is enough
          //IT IS FORBIDDEN TO SEND CANCEL REQUEST UNLESS IT IS LINKED TO A HELP REQUEST.........
          //Based on the fact that during 10 minutes no second help request can be sent,
          // assuming that last interrupt (of course which didn't cancel its help by itself since cancel_help_after_about_sending_packet is false) took 3 minutes at maximum, so 7 minutes 
          // remain at maximum for the difference between the start of that interrupt and right now. If the difference is less than 7 minutes then the cancel requested is considered valid.
          help_packet_sent = false;
          contactServer("cancel", 2); //a cancel request is a type 2 message.
        }
        delay(1); //for stability of digitalRead 
      }
    }    
  } 
}
//you should use LEDs to notify the end user about the system's state (working fine, no connection, awaiting response from server, ...)...........


