#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incoming_packet[255];
int server_IPAddr[4];
int server_port = 62003;
const byte alarm_pin = 14;
const byte cancel_pin = 13;
volatile boolean cancel_help = false;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  pinMode(alarm_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(alarm_pin), contactServer, FALLING);
  sei();
  pinMode(cancel_pin, INPUT);
  
  server_IPAddr[0] = 192;
  server_IPAddr[1] = 168;
  server_IPAddr[2] = 1;
  server_IPAddr[3] = 20;
  
  Serial.println("Contacting WiFi...");  
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

void contactServer(){
  boolean server_ack = false;
  do{
    // sending help request
    Udp.begin(localUdpPort);
    //PACKET SENT TO THE IP ADDRESS AND PORT OF -------------------------------THE SERVER-------------------------------------------  
    //later to make a fully secured communication 
    Udp.beginPacket( IPAddress(server_IPAddr[0], server_IPAddr[1], server_IPAddr[2], server_IPAddr[3]), server_port ); //213,204,75,44
    Udp.write("help");
    Udp.endPacket();  
    
    unsigned long i = 0;
    
    while (i<1600000) //2 seconds
    {
      if (i%80000 == 0) {//check digitalRead each 1 ms or more for stability. (By the way the esp8266 speedis 80MHz as the default speed)
        if(digitalRead(cancel_pin)) { //since cancel_pin is wired to a pull-down resistor, when it's HIGH all operations must stop for 10 minutes
          cancel_help = true;
          break;
        }
      }
      incoming_packet[0] = 0; //is this like a null character?
      i = i + 1;
      int packetSize = Udp.parsePacket(); //Control doesn't hold here; it passes quickly over it.
      if (packetSize)
      {
        //must check, in a separate function, that the received packet is from server...
        if (checkPacketSource()){
          server_ack = true;  
          break;
          int len = Udp.read(incoming_packet, 255);
          if (len > 0)
          {
            incoming_packet[len] = 0; //is this like a null character?
          }
          //now must check the content of the incoming packet
          Serial.printf("UDP packet content: %s\n", incoming_packet);
        }
      }
    }
    if (cancel_help){
      break;
    }
  } while(!server_ack);//resend the help request e.g. each 2 seconds if it hasn't received any ack from server
    
/*
    //now should receive a packet from server, ideally telling that the policeman is aware about the alarm
    i = 0;
    server_ack = false;
    do {
      incoming_packet[0] = 0; //is this like a null character?
      i = i + 1;
      int packetSize = Udp.parsePacket(); //Control doesn't hold here; it passes quickly over it.
      if (packetSize)
      {
        //must check, in a separate function, that the received packet is from server...
        if (checkPacketSource()){
          server_ack = true;
        }
      }
    } while (i <= 1600000 * 7) //15 seconds is not bad
    
    //
*/
  
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

boolean checkPacketSource() {
  if ( strcmp( Udp.remoteIP().toString().c_str(), int_array_to_string(server_IPAddr,4).c_str() ) == 0 && Udp.remotePort() == server_port){
    return true;
  } else {
    return false;
  }
  Serial.printf("Received an UDP packet from %s, port %d\n", Udp.remoteIP().toString().c_str(), Udp.remotePort());          
}

void loop()
{
  if (cancel_help || digitalRead(cancel_pin)){
    cancel_help = false;
    //now must send a packet to server asking him to cancel 
    Udp.beginPacket( IPAddress(server_IPAddr[0], server_IPAddr[1], server_IPAddr[2], server_IPAddr[3]), server_port ); //213,204,75,44
    Udp.write("cancel");
    Udp.endPacket();  
  } 
  delay(1);//for stability of digitalRead 
}
