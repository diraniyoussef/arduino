#include  <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

SoftwareSerial SIMSerial(D8, D7); // RX, TX
WiFiClient client;
const byte reset_trigger_pin = D6;

void setup() {
  SIMSerial.begin(9600);
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());            

  pinMode(reset_trigger_pin, INPUT);
  
  
  // SIM800 Reset pin is conected to D2
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);

  Serial.println("hello\n");
  // switching on SIM800 by powering down Reset pin for 50 milliseconds
/*  
  SIMSerial.write("AT");
  if(SIMSerial.available()){
    Serial.write(SIMSerial.read());
    Serial.println("The OK received from SIM800 and should have showed up on screen\n");
  }
  digitalWrite(D2, HIGH);
  Serial.println("D2 is high for the first time");
  
  //Serial.println("hello after waking up the SIM800\n");
  //Serial.begin(9600); //is it really necessary
  //SIMSerial.write("AT");
  
  //Serial.begin(9600); //is it really necessary
  //delay(1000); //is it necessary?
  
  //Serial.println("hello after receiving the ok from SIM800\n");
  /*later...
   * 
  // setting some parameters
  Serial.write("AT+CMGF=1\n\r"); // SMS output text mode
  delay(1000); //I made it...
  if(Serial.available())
    Serial1.write(Serial.read());

  Serial.write("AT+CNMI=1,2,2,1,0\n\r");­ // Show SMS text in output
  delay(1000); //I made it...
  if(SIMSerial.available())
    Serial.write(SIMSerial.read());

  SIMSerial.write("at+clip=1\n\r"); // Calling line presentation in output
  
  */
}

void loop() {
  // code to play with AT commands from the terminal
//  if (SIMSerial.available()) {
    SIMSerial.write("AT\n\r");
    Serial.write(SIMSerial.read());
    Serial.println("something I received from SIM800 and should have shown up on screen\n");
//  }
    
  if (Serial.available())  {
//    digitalWrite(D2, LOW);
//    delay(100);
    SIMSerial.write(Serial.read());
//    digitalWrite(D2, HIGH);
    Serial.println("something I've just sent now\n");
  }

  if (digitalRead(reset_trigger_pin)){
    Serial.print("D2 is now high\n");
    digitalWrite(D2, LOW);
    delay(110);
    digitalWrite(D2, HIGH);    
  }
}



