//#include  <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

//SoftwareSerial SIMSerial(D8, D7); // RX, TX
WiFiClient client;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.setDebugOutput(true);

  WiFi.disconnect();
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); 
  Serial1.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial1.print(".");
  }
  Serial1.printf("\nConnected, IP address: ");
  Serial1.println(WiFi.localIP());            
  
  // SIM800 DTR pin is conected to D2
  pinMode(D2, OUTPUT);

  Serial1.println("hello\n");
  // switching on SIM800 by powering down DTR pin for 50 milliseconds
  digitalWrite(D2, HIGH);
  delay(5000);
  digitalWrite(D2, LOW);
  delay(100);
  Serial.write("AT");
  if(Serial.available()){
    Serial1.write(Serial.read());
    Serial1.println("The OK received from SIM800 and should have showed up on screen\n");
  }
  digitalWrite(D2, HIGH);
  Serial1.println("D2 is high again");
  
  //Serial.println("hello after waking up the SIM800\n");
  //Serial.begin(9600); //is it really necessary
  //SIMSerial.write("AT");
  
  //Serial.begin(9600); //is it really necessary

  
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

}



