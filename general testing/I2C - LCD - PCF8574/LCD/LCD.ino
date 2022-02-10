
#include <Wire.h>

#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x3F, 16, 2);//0x27

void setup() {

Serial1.begin(115200);

//Use predefined PINS consts

Wire.begin(D2, D1);

lcd.begin();

lcd.home();
//lcd.backlight(); //turn on the backlight

lcd.print("Hello, NodeMCU");

}

void loop() { 
  // do nothing here 
}


