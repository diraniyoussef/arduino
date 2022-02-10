#include <SoftwareSerial.h>

SoftwareSerial SMS(9, 10);// rx(9) and tx(10)
                          // rx(9-arduino) connect to tx(gsm module)
                          // tx(10-arduino) connect to rx(gsm module)
int x=0;
int y=0;

void setup()
{
  SMS.begin(9600);       // baud rate for gsm module  
  Serial.begin(9600);    // baud rate for serial monitor 
  delay(100);
  pinMode(12, INPUT);
  pinMode(2, INPUT); 
}
void loop()
{
  x=digitalRead(12);
  y=digitalRead(2);
  if (x==HIGH)
  {
    sendmessage();
    //recievemessage(); // to receive message enable receivemessage function 
                         // and "//" the sendmessage function
  }
  if(y==HIGH){
    sendmessage1();
  }
  if (SMS.available()>0){// verify any bits which income through pins 9 and 10
    Serial.write(SMS.read());// write income data on serial monitor
  }
}
 void sendmessage()
{
  SMS.println("AT+CMGF=1"); //AT command for text mode of gsm module 
  delay(1000);  
  SMS.println("AT+CMGS=\"0758802779\"\r"); //  x is your mobile number
  delay(1000);
  SMS.println("sms send by arduino when button 1 is pressed");// the text you want to send //it's like help! Emergency! this is ..... calling...
  delay(100);
  SMS.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
void sendmessage1()
{
  SMS.println("AT+CMGF=1"); //AT command for text mode of gsm module 
  delay(1000);  
 SMS.println("AT+CMGS=\"0758802779\"\r"); //  x is your mobile number
  delay(1000);
  SMS.println("sms send by arduino when button 2 is pressed");// the text you want to send
  delay(100);
  SMS.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
 void recievemessage()
{
  SMS.println("AT+CNMI=2,2,0,0,0"); // AT command to receive SMS
  delay(1000);
}
