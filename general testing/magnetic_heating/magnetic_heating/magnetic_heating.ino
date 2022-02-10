#include <ESP8266WiFi.h>

//int outPin = 8; //remember you must mention D8 not 8

unsigned int charge_delay = 3;
unsigned int discharge_delay = 3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();  
  pinMode( D8 , OUTPUT);   
  Serial.println("Starting");  
  //the following is PWM. Please check https://randomnerdtutorials.com/esp8266-pwm-arduino-ide/
  /*pin: PWM may be used on pins 0 to 16, so I'm fine with D8
  */
  analogWriteFreq(40000); /*equivalent to 25 microSeconds.
  * Default frequency is 1KHz. 
  * Here https://github.com/esp8266/Arduino/issues/1265#issuecomment-166279205 the commenter said that max frequency 
  * can reach up to 78.125KHz, and weird enough to me - he linked it to the duty cycle range !
  * He also said something about 80000000/range, but I won't be interested in this. 
  * Here however https://github.com/esp8266/Arduino/blob/master/doc/reference.rst the "Analog output" section, they made everything clear
  * It is 40KHz ...
  */
  analogWrite( D7, 511); /*330 is for 9 microSeconds ON time and 19 microSeconds off time, for a total of 28 microSeconds period.
  * You might wonder that the period is 25 not 28 microSeconds, which is true but Saleae Logic Analyzer had another opinion.
  * 511 for a duty cycle of 50%. max default duty cycle of 100% is 1023. This max 1023 number can be changed though using 
  * analogWriteRange(new_range);
  */
  analogWriteFreq(40000);
  
}

void loop() {
  /*
   * //unfortunately this is not PWM since it's unreliable; I checked it in Logic Analyzer
    digitalWrite( D8 , HIGH); // sets the pin on    
    delayMicroseconds(charge_delay);      // use delay() when needed to delay in the order of seconds.
    digitalWrite(D8, LOW);  // sets the pin off
    delayMicroseconds(discharge_delay);      // pauses for 50 microseconds    
    */
}
