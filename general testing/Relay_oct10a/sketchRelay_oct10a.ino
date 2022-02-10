#include <ESP8266WiFi.h>

const byte alarm_pin = D6;
const byte cancel_pin = 13;
const byte relay1_pin = D7;
const byte relay2_pin = 16;

//setup for the minimum time interval for a new interrupt to be considered.

void setup()
{
  Serial1.begin(9600);
  Serial1.setDebugOutput(true);
  pinMode(alarm_pin, INPUT);
  pinMode(cancel_pin, INPUT); 
  pinMode(relay1_pin, OUTPUT);
  digitalWrite(relay1_pin, LOW);
  Serial1.printf("\nRelay pin 14 is set to LOW\n");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  //WiFi.begin("Lumia 930 3419", "123yesno");
  WiFi.begin("Electrotel_Dirani", "onlyforworkpls");
  Serial1.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial1.print(".");
  }
  Serial1.printf("\nConnected to WiFi network with IP address: ");
  Serial1.println(WiFi.localIP());
}

void loop() {
  if (digitalRead(alarm_pin)){
    digitalWrite(relay1_pin, HIGH);
    Serial1.printf("\nRelay pin 14 is set to HIGH\n");
    delay(10000);
    digitalWrite(relay1_pin, LOW);
    Serial1.printf("\nRelay pin 14 is set to LOW again\n");
  }
}
//you should use LEDs to notify the end user about the system's state (working fine, no connection, awaiting response from server, ...)...........
// Also the reset button should be apparent and probably the circuit should be able to be flashed on site...

