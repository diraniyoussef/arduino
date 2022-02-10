/* I'm advertisiing the youtuber whom I like.
 * I only got a few lines from his code. Anyway here is his attribute :
 * 
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


#include "AsynchroClient.h"
#include <dht11.h>

class TempHumValue {
public:
float temperature;
int humidity;
};

class TempHum {
private:
dht11 DHT11;
const int dhtpin = A0;


public:
static const int Temperature_Float_Size = 4; /*reflecting the readable digits with the decimal point. 
  *tens-units-decimal_point-tenths so in all 4.*/
static const int Temperature_Decimals = 1; /*that is tenths so it is 1*/  
static const int Humidity_Int_Size = 3; /*from 0 (1 readable digit) to 100 (3 readable digits).*/

TempHumValue readValues() {
  TempHumValue tempHumValue;
  DHT11.read(dhtpin);
  tempHumValue.humidity = DHT11.humidity; //returning the relative humidity which must be between 0 and 100
  tempHumValue.temperature = (float)DHT11.temperature; //returning in degree Celcius. Max value for DHT 11 module is 60 degrees.
  return tempHumValue;
}

};
