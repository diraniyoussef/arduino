#include  <SoftwareSerial.h>

SoftwareSerial mySerial(D8, D7); // RX, TX

void setup() {
  Serial.begin(19200); //Скорость порта для связи Arduino с компьютером
  
  // SIM800 PWK pin is conected to D0
  pinMode(D0, OUTPUT);
  
  // switching on SIM800 by powering down PWK pin for a second
  digitalWrite(D0, LOW);
  delay(1000);
  digitalWrite(D0, HIGH);
  
  mySerial.begin(19200);
  mySerial.println("AT");
  
  mySerial.begin(19200);
  delay(5000);
  
  if(mySerial.available())
    Serial.write(mySerial.read());
  
  // setting some parameters
  mySerial.write("AT+CMGF=1\n\r"); // SMS output text mode

  if(mySerial.available())
    Serial.write(mySerial.read());

  mySerial.write("AT+CNMI=1,2,2,1,0\n\r");­ // Show SMS text in output
  
  if(mySerial.available())
    Serial.write(mySerial.read());

  mySerial.write("at+clip=1\n\r"); // Calling line presentation in output
}

void loop() {
  // code to play with AT commands from the terminal
  if (mySerial.available())  
    Serial.write(mySerial.read());

  if (Serial.available())  
    mySerial.write(Serial.read());

}


  
