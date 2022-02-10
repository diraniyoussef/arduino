// ESP8266 boards installed via the boards manager but GIT is here - https://github.com/esp8266/Arduino

#include <ESP8266WiFi.h>
// WiFi library from https://github.com/esp8266/Arduino but install via the libraries manager
int red = 15;
int green = 12;
int blue = 13;
int LDRPin = A0;
const char* ssid = "---";
const char* password = "---";
const char* host1 = "twigglabs.com";
const int httpPort1 = 80;
String url = "/IoT/files/---/data.php?hash=---&data=";
String url2 = "/IoT/files/---/bool.txt";
boolean onOff = 0;
void setup() {
   Serial.begin(115200);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  connectWifi();

}

void loop() {
  ledCall("blue", 150);
  int LDRReading = 0;
  LDRReading = analogRead(LDRPin);
  delay(60000);
   ReadFromURL(host1, httpPort1, url2);
   delay(5000);
  SendToURL(host1, httpPort1, url, "david", LDRReading);


}
void ReadFromURL(const char* host, int httpPort, String url) {

  //Serial.print("connecting to ");
  //Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    // Serial.println("connection failed");
    ledCall("red", 150);
    delay(2000);
    return;
  }

  // We now create a URI for the request


  //Serial.print("Requesting URL: ");
  //Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
     // Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    
    
      String tempMsg = client.readStringUntil('\n');
      if(tempMsg.length() == 1){ //ignores the headers
      if(tempMsg == "0"){
      onOff = false;
      //Serial.println(onOff);
      }
      if(tempMsg == "1"){
      onOff = true;
      //Serial.println(onOff);
      }
      }
  }
  
  //Serial.println();
 //Serial.println("closing connection");

}
void SendToURL(const char* host, int httpPort, String url, String id, int data){

  //Serial.print("connecting to ");
  //Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  if (!client.connect(host, httpPort)) {
   // Serial.println("connection failed");
     ledCall("red", 150);
     delay(60000);
    return;
  }
  
  // We now create a URI for the request
  
  url += data;
  
  //Serial.print("Requesting URL: ");
  //Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      ledCall("red", 150);
      //Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    ledCall("green", 150);
    String line = client.readStringUntil('\r');
    
   // Serial.print(line);
  }
  
 // Serial.println();
 // Serial.println("closing connection");
}

void ledCall(String colour, int strength){
  analogWrite(red, 0);
  analogWrite(green, 0);
  analogWrite(blue, 0);
  if(onOff == true){
  if(colour == "red"){
   analogWrite(red, strength); 
  }

  if(colour == "green"){
   analogWrite(green, strength);  
  }

  if(colour == "blue"){
   analogWrite(blue, strength); 
  }
  }
}
void connectWifi() {
  WiFi.begin(ssid, password);
  // put your setup code here, to run once:
  while (WiFi.status() != WL_CONNECTED) {
    ledCall("red", 150);
    delay(500);
   // Serial.print(".");
  }
  //Serial.println("");
 // Serial.print("Connected to ");
 // Serial.println(ssid);
 // Serial.print("IP address: ");
 // Serial.println(WiFi.localIP());

}

