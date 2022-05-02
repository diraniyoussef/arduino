#include <ESP8266WiFi.h>
//#include <WiFiClient.h>

const char* ssid = "refri_panel";
const char* password = "123456";

WiFiServer server(11357);

void serverSetup() {
  server.begin();
  server.setNoDelay(true); //I think everything you send is directly sent without the need to wait for the buffer to be full
}

boolean is_battery_low;

boolean isBatteryLow() {
  char c  = Serial.read();
  if (c == 't') {
    Serial.println("battery is low");
    return true;    
  } else if(c == 'f') {
    Serial.println("battery is high");
    return false;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  
  IPAddress thisPanelIP(172,17,15,31);
  IPAddress gateway(172,18,19,200); //let the gateway be the same as the local_IP, but maybe it works if 192.168.4.251 was the same gateway for all modules.
  IPAddress subnet(255,240,0,0);
  WiFi.config(thisPanelIP, gateway, subnet);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  serverSetup();
  
  is_battery_low = isBatteryLow();
}

void sendBatteryState( WiFiClient* client) {
  if( is_battery_low ) {
    client->println("l");
  } else {
    client->println("h");
  }
}

void checkIfCalled() { 
  /*check if the other panel has just started/restarted and thus is 
  * connecting to this panel and is asking for information concerning the battery
  **/
  
  WiFiClient *a_client;
  *a_client = server.available(); 
  if( *a_client ) {
    if( a_client->connected() ) {
      //send info about the battery
      sendBatteryState(a_client);
      delay(20);
      a_client->flush();
    }
    delay(20);
    a_client->stop();
  }
}

boolean sendBatteryStateToServer() {
  const IPAddress serverIP(172,17,15,30);/*this follows the list of private IP addresses from 172.16.0.0 to 172.31.255.255
            * where subnet fixes the first 12 bits (10101100 for the first byte, and 0001xxxx for the second byte), 
            * so subnet has to be 255.240.0.0 according to https://en.wikipedia.org/wiki/Private_network */
  const int port = 11357;
  
  WiFiClient client;
  if (!client.connect(serverIP, port)) {
    Serial.println("connection failed");    
    return false;
  }
  // This will send a string to the server
  Serial.println("sending data to server");
  if( client.connected() ) {
    sendBatteryState(&client);    
    delay(20);
    client.flush();
    delay(20);
    client.stop();
    return true;
  }
  return false;
}

boolean failed_to_send_to_server_last_time = true; //this is to make this panel send battery state as a client to the refri_panel's server

void loop() {
  delay(50);

  //if battery state is changed send it to other panel's server
  if( failed_to_send_to_server_last_time || isBatteryLow() != is_battery_low ) {
    Serial.println("battery state is inversed");
    is_battery_low = isBatteryLow();    
    if( !sendBatteryStateToServer() ) {
      failed_to_send_to_server_last_time = true;
      return;
    } else {
      failed_to_send_to_server_last_time = false;
    }
  }

  //this server listening
  checkIfCalled();
}
