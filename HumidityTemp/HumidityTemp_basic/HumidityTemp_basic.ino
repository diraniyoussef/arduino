#include <ESP8266WiFi.h>
/*
 * This is the Arduino code for  DHT11 module to read temprature and humidity
 * This code can display temprature in:
 * C is used to get Celsius
 * F is used to get fahrenheit
 * K is used for Kelvin
 * Watch the video https://youtu.be/FjZBt6eU9b4
 *  * 
 * Written by Ahmad Nejrabi for Robojax Video
 * Date: Jan 04, 2018, in Ajax, Ontario, Canada
 * Permission granted to share this code given that this
 * note is kept with the code.
 * Disclaimer: this code is "AS IS" and for educational purpose only.
 * 
 */

// header file from GetHub: https://github.com/adidax/dht11
#include <dht11.h>

dht11 DHT11; // create object of DHT11
#define 
 A0 // set the pin to connect to DHT11

void setup() {
    Serial.begin(115200);// setting up serial monitor

}

void loop() {
  DHT11.read(dhtpin);// initialize the reading
  //code for Robojax.com video
  int humidity = DHT11.humidity;// get humidity

   Serial.print(getTemp('C'));
   Serial.print("C ");
   Serial.print(getTemp('F'));
   Serial.print("F ");
   Serial.print(getTemp('K'));
   Serial.print("K ");
   Serial.print(" humidity:");
   Serial.print (humidity);
   Serial.println("% ");
   Serial.println();
 delay(500);
}


/*
 * getTemp(char type)
 * type character of upper case 
 * C is used to get Celsius
 * F is used to get fahrenheit
 * K is used for Kelvin
 */
float getTemp(char type) {
    float temp = (float)DHT11.temperature;//get temp
  if(type =='F')
  {
    return temp * 1.8 + 32;// convert to fahrenheit
  }else if(type =='K')
  {
    return temp + 274.15;// convert to Kelvin
  }else{
   return temp; 
  }
  
}
