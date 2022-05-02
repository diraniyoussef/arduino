/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

const char* ssid     = "TP-LINK_2B5E";
const char* password = "22221111";

WiFiServer server1(3553);

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  //byte MAC_bytes[ 6 ];
  byte local_IP_bytes[ 4 ] = {192, 168, 0, 46};
  byte gateway_bytes[ 4 ] = {192, 168, 0, 1};
  byte subnet_bytes[ 4 ] = {255, 255, 255, 0};
  
  IPAddress local_IP( local_IP_bytes[0], local_IP_bytes[1], local_IP_bytes[2], local_IP_bytes[3] );  
  IPAddress gateway( gateway_bytes[0], gateway_bytes[1], gateway_bytes[2], gateway_bytes[3] );   
  IPAddress subnet( subnet_bytes[0], subnet_bytes[1], subnet_bytes[2], subnet_bytes[3] );      
  
//  wifi_set_macaddr(STATION_IF, MAC_bytes)

// Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA static IP Failed to configure");
  }
  
//  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server1.begin();
}

boolean interactWithIncomingRequest(WiFiClient client2) {
  
  if( client2 ) {
    if( client2.connected() ) {
      //client2.write( (const uint8_t*) totalMessage_buff , total_message_length );
      char read_buff[ 50 ];
      read_buff[0] = '\0';
      char pre_message[] = "I got your message : ";      
      strcat(read_buff, pre_message);
      read_buff[sizeof(pre_message)] = '\0';

      Serial.println(read_buff);
      int read_char_number = 0;
      char received_message[30];
      
      while( read_char_number <= 0 )
        //read_char_number = client2.readBytesUntil('\n', read_buff + sizeof(pre_message), 30); //BTW the '\0' is not counted in the value of read_char_number - tested        
        read_char_number = client2.readBytesUntil('\n', received_message, 30);
      Serial.println(read_char_number);
      received_message[read_char_number] = '\0';
      strcat(read_buff, received_message);
      Serial.println(read_buff);
      
      //read_buff[ 50 - 1 ] = '\0';
      client2.println( read_buff );
      
      delay(5);      
      client2.flush();

      delay(10);
      client2.stop();
      return true;
    }
  }
  return false;
}

void loop() {
  //Serial.print("inside loop");
  delay(1);
  interactWithIncomingRequest( server1.available() );
}
