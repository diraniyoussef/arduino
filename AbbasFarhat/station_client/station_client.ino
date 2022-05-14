#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200); // prepares the Serial class for use
  pinMode(LED_BUILTIN, OUTPUT); // to access the board and prepare it


  // Configuration
  IPAddress local_IP(192,168,0,235);
  IPAddress gateway(192,168,0,1);
  IPAddress subnet(255,255,255,0);
  if (WiFi.config(local_IP, gateway, subnet)) {
    Serial.print("\n successfully assigned static ip");
  }
  
  // Connection to WiFi router
  const char* ssid = "TP-LINK_2B5E";
  // const char password[] = {'2', '2', '2', '2', '1','1','1','1'};
  const char* password = "22221111";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nconnected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Mac Address");
  Serial.println(WiFi.macAddress());
 
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print("I'm in loop\n"); // prints "I'm in loop\n" into the serial monitor
  delay(20); 
  char c  = Serial.read(); // declares c as a char that reads 1 byte at a time
  if( c == 's' ) {
    // Client
    IPAddress serverIP(192,168,0,46);
    int port = 3553;
    WiFiClient client;
    if (!client.connect(serverIP, port)) {
      Serial.println("connection failed");
    } else {
      // This will send a char to the server
      Serial.println("sending data to server");
      if(client.connected()) {
        client.print("t");
        //client.write('t');
      } else {
        Serial.println("not connected !");
      }
    }
  }
}
