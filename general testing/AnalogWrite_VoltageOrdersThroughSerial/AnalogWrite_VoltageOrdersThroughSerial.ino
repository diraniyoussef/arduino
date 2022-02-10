#include <ESP8266WiFi.h>
//The idea is to deliver a value from 0 to 3.3V
//I write the desired voltage to the Serial and it accepts up to THREE DIGITS after the decimal point!

String readString;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  pinMode(D0, OUTPUT);
  Serial.println();
  Serial.println("Please enter voltage value.");
}

void loop() {

  // put your main code here, to run repeatedly:
  delay(2000);
  readString = "";
     
  while( Serial.available() ) {        
//    Serial.println("Reading a byte");
    char c = Serial.read(); //important to convert I guess from byte to char
    readString += c;
    delay(3);  //delay to allow buffer to fill
  }
  
  if (readString.length() > 0) {
    Serial.print("Read value as is was: ");
    Serial.println(readString);    
    float voltage  = readString.toFloat();
    Serial.print("Read value as float was: ");
    Serial.println(voltage);
    int mapped_value = floor(voltage * 1023 / 3.3);
    Serial.print("Read value after floor Int was: ");
    Serial.println(mapped_value);    
    if (mapped_value > 1023)
      mapped_value = 1023;
    if (mapped_value < 0)
      mapped_value = 0;
    Serial.print("Considered value on 1023 scale was: ");
    Serial.println(mapped_value);
    Serial.print("Considered value on 0.9V scale is: ");
    Serial.println(voltage * 0.9 / 3.3);

    analogWrite(D0, mapped_value);
    Serial.println("Please enter voltage value.");
    Serial.println();
  }

}




