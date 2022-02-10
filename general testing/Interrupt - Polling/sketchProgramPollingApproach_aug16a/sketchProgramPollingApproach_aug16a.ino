#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incoming_packet[255];
int server_IPAddr[4];
int server_port = 62003;
const byte alarm_pin = 14;
const byte cancel_pin = 13;
boolean cancel_help_while_sending_help_packet = false;
boolean help_packet_sent = false; //this condition is like "silence"  
byte time_difference_cancel_and_help = 10;  //watch it!! you're using "byte" with some other variable types!!

//setup for the minimum time interval for a new interrupt to be considered.
unsigned long last_button_time = 0;
unsigned long button_time = 0;
unsigned long disabled_help_request_time_interval = 10 * 60 * 1000; //10 minutes. A typical minimal value is probably 250 or 1000, but a 1 minutes value is ok for the police application
unsigned long ack_waiting_time_interval = 5 * 60 * 1000; //5 minutes. 
unsigned long long_int_max = 4294967295;
boolean not_first_time = false;



void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  pinMode(alarm_pin, INPUT);
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

void helpRequest() {
  button_time = millis();
  Serial.printf("helpRequest function entered, time in ms is: %u\n", button_time);
  //The following if statement is just to prevent reentering the help request in the transient case of voltage change, also not to call the police within a short amount of time.
  if (button_time - last_button_time > disabled_help_request_time_interval || (button_time < last_button_time && (button_time + long_int_max - last_button_time > disabled_help_request_time_interval)) || !not_first_time)//after about 50 days, millis() will overflow
    //By the way, this condition "button_time - last_button_time > disabled_help_request_time_interval" is ok even if an overflow of about 50 days passed between button_time and last_button_time
  {
    Serial.printf("help request to be considered valid\n", millis());
    last_button_time = button_time;
    contactServer("help", 1);
  }
  not_first_time = true;
}

void contactServer(char* message, byte type) { //it's about sending and receiving a request and an ack in the same function
  boolean server_ack = false;
  unsigned long temp_val;
  temp_val = add2UnsignedLong( last_button_time, ack_waiting_time_interval );
  do {
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
    while (i < 3600000000) //some time, but it's less than 2 seconds most probably
    {
      if (type == 1) { //type 1 message is a help request.
        //We now check if there is a cancel request while we are waiting for the server ack. This might take an indefinite amount of time.
        if (i % 900000000 == 0) { //check digitalRead each 1 ms or more for stability. (By the way the esp8266 speedis 80MHz as the default speed)
          if ( !digitalRead(cancel_pin) ) { //since cancel_pin is wired to a pull-up resistor, when it's LOW a cancel request should start
            cancel_help_while_sending_help_packet = true;
            //help_packet_sent = false; //later...
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
        if (checkPacketSource()) {
          int len = Udp.read(incoming_packet, 255);
          if (len > 0)
          {
            incoming_packet[len] = 0; //is this like a null character?
          }
          //now must check the content of the incoming packet
          if (type == 1 && strcmp( incoming_packet, "help ack" ) == 0 ) {
            Serial.printf("%s received\n", incoming_packet);
            server_ack = true;
            break;
          }
          if (type == 2 && strcmp( incoming_packet, "cancel ack" ) == 0 ) {
            Serial.printf("%s received\n", incoming_packet);
            server_ack = true;
            break;
          }
        }
      }
    }
    if (type == 1 && cancel_help_while_sending_help_packet) {
      break;
    }
    
    // neglect the request if no ack is received
    unsigned long current_time;
    current_time = millis();
    if(current_time > temp_val){ //temp_val is 5 minutes past last_button_time
      help_packet_sent = false; //ok, whether it was type 1 or 2
      break;
    }
  } while (!server_ack); //resend the help request e.g. each 2 seconds if it hasn't received any ack from server
}

String int_array_to_string(int int_array[], int size_of_array) {
  String return_string = "";
  int temp = 0;
  for (temp = 0; temp < size_of_array - 1; temp++) {
    return_string += String(int_array[temp]) + ".";
  }
  return_string += String(int_array[temp]);
  Serial.printf(return_string.c_str());
  return return_string;
}

boolean checkPacketSource() {
  Serial.printf("Received an UDP packet from %s, port %d\n", Udp.remoteIP().toString().c_str(), Udp.remotePort());
  if ( strcmp( Udp.remoteIP().toString().c_str(), int_array_to_string(server_IPAddr, 4).c_str() ) == 0 && Udp.remotePort() == server_port) {
    return true;
  } else {
    return false;
  }
  Serial.printf("Received an UDP packet from %s, port %d\n", Udp.remoteIP().toString().c_str(), Udp.remotePort());
}

unsigned long add2UnsignedLong(unsigned long val1, unsigned long val2) {
  unsigned long val3;
  if (val1 < val2){ //this isn't necessary but ok.
    val3 = val1;
    val1 = val2;
    val2 = val3;
  }
  val3 = val1;
  while(val3 < valMax && val2 > 0){
    val3 = val3 + 1;
    val2 = val2 - 1;  
    Serial.printf("\n new values are %lu and %lu", val3, val2);     
  }
  if ( val3 == valMax)
    val3 = val2;
  return val3;
}


void loop() {
    
  if ( !digitalRead(alarm_pin) ) {//alarm is triggered since it is normally connected to a pullup resistor
    helpRequest();
  }

  //keep updating last_button_time to avoid some problems in the first condition of the very lone "if" statement in helpRequest function
  // when button_time passes last_button_time by 50 days.  
  // The procedure is: 30 minutes past last_button_time, last_button_time will be shifted 10 minutes forward
  unsigned long current_time;
  unsigned long temp_val;
  current_time = millis();
  temp_val = add2UnsignedLong( last_button_time, disabled_help_request_time_interval * 3 );
  if ( current_time > temp_val ) {    
    last_button_time = add2UnsignedLong( last_button_time, disabled_help_request_time_interval );
  }
  
  //after 10 minutes make help make help_packet_sent = false; 
  if (help_packet_sent){
    temp_val = add2UnsignedLong( last_button_time, disabled_help_request_time_interval);
    if ( current_time > temp_val )    
    {    
      help_packet_sent = false;
    }
  }
  
  if (cancel_help_while_sending_help_packet) {
    help_packet_sent = false; 
    Serial.printf("Now in loop and processing cancel\n");
    cancel_help_while_sending_help_packet = false;
    //now must send a packet to server asking him to cancel
    contactServer("cancel", 2); //a cancel request is a type 2 message.
  } else {
    if ( help_packet_sent ) {                 
      if ( current_time < temp_val ) {
        //time_difference_cancel_and_help is a supposed dedicated time to make a cancel request without the possibility of congesting with a help request that might (with a very low
        // probablity) to happen when the interrupt becomes enabled again. It's like giving a priority for the cancel over the help request.
        if ( !digitalRead(cancel_pin) ) { //I guess 1 second while cancel_pin is down is enough - 
                                          // I believe I'm reading the value for micro or nano second? Anyway, this can be enhanced if check say for 5 times within half a second;
                                          // if all of the 5 tests were positive then the read digital value holds! But not sure how much that is necessary?!
          //IT IS FORBIDDEN TO SEND CANCEL REQUEST UNLESS IT IS LINKED TO A HELP REQUEST.........
          //Based on the fact that during 10 minutes no second help request can be sent,
          // assuming that last interrupt (of course which didn't cancel its help by itself since cancel_help_while_sending_help_packet is false) took 3 minutes at maximum, so 7 minutes
          // remain at maximum for the difference between the start of that interrupt and right now. If the difference is less than 7 minutes then the cancel requested is considered valid.
          help_packet_sent = false;
          contactServer("cancel", 2); //a cancel request is a type 2 message.
        }
      }
    }
  }
  delay(1); //for stability of digitalRead
}
//you should use LEDs to notify the end user about the system's state (working fine, no connection, awaiting response from server, ...)...........
// Also the reset button should be apparent and probably the circuit should be able to be flashed on site...

