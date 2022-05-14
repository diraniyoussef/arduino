#include <ESP8266WiFi.h>

WiFiClient client2;
WiFiServer server(3553);

void setup() {
  Serial.begin(115200); // prepares the Serial class for use
  pinMode(LED_BUILTIN, OUTPUT); // to access the board and prepare it


  // Configuration
  IPAddress local_IP(192,168,0,46);
  IPAddress gateway(192,168,0,1);
  IPAddress subnet(255,255,255,0);
  if (WiFi.config(local_IP, gateway, subnet)) {
    Serial.print("\n successfully assigned static ip");
  }
  
  // Connection to WiFi router
  const char* ssid = "TP-LINK_2B5E";
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
 
  // Server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(1);
  //char c  = Serial.read(); //declares c as a char that reads 1 byte at a time
  
  client2 = server.available();
  if(client2) {
    if(client2.connected()) {
      Serial.println("A client is connected");
      delay(20);
      while( client2.available() ) {
        char b = client2.read();
        Serial.printf("b is %c", b);
      }
      /*
       //bad block
      char b = client2.read();
      if( b == -1 ) {
        Serial.println("Nothing received\n");
      } else {
        Serial.println("Received this\n");
        do {
          Serial.print("this is b ");
          Serial.printf("%c", b);
          b = client2.read();
        } while( b != -1 );
      }
      */
    }
  }
}
