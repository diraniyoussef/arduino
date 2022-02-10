// ESP8266 boards installed via the boards manager but GIT is here - https://github.com/esp8266/Arduino
//screen
#include <Wire.h>
#include "SSD1306.h"
// OLED library from https://github.com/squix78/esp8266-oled-ssd1306
#include "images.h"
SSD1306  display(0x3c, 4, 5);
//OLED connected to GPIO4 and GPIO5
//wifi
const char* ssid = "---";
const char* password = "---";
const char* host1 = "davidjwatts.com";
const int httpPort1 = 80;
String url = "/arduino/esp82663333.php";
#include <ESP8266WiFi.h>
// WiFi library from https://github.com/esp8266/Arduino but install via the libraries manager
//LED and LDR
int red = 15;
int green = 12;
int blue = 13;
int LDRPin = A0;
// variables
unsigned long previousMillis = 0;
const long interval = 15000;
String dateVal = "";
String timeVal = "";
String updateVal = "";
String conditionVal = "";
String temperatureVal = "";
boolean sleep = false;


void setup() {
  Serial.begin(115200);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  connectWifi();
  display.init();
  display.flipScreenVertically();

}

void printWeather() {
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, timeVal);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 25, dateVal);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 35, temperatureVal);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 45, conditionVal);
}

void connectWifi() {
  WiFi.begin(ssid, password);
  // put your setup code here, to run once:
  while (WiFi.status() != WL_CONNECTED) {
    ledCall("red", 150);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  sleep = false;

}

void drawWeather() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files to go into your images.h file
  String tempCondition = conditionVal;
  tempCondition.toLowerCase();
  int conditionLen = tempCondition.length();

  Serial.println(tempCondition);
  if (tempCondition.indexOf("sunny") > -1) {
    //Serial.println(tempCondition.indexOf("sunny"));
    display.drawXbm(78, 0, sunny_width, sunny_height, sunny);
  }
  if (tempCondition.indexOf("rain") > -1) {
    //Serial.println("found");
    display.drawXbm(78, 0, rain_width, rain_height, rain);
  }
  if (tempCondition.indexOf("cloud") > -1) {
    // Serial.println(tempCondition.indexOf("cloud"));
    display.drawXbm(78, 0, cloudy_width, cloudy_height, cloudy);
  }
  if (tempCondition.indexOf("clear") > -1) {
    //Serial.println("found");
    display.drawXbm(78, 0, clearw_width, clearw_height, clearw);
  }
  if (tempCondition.indexOf("interval") > -1) {
    //Serial.println("found");
    display.drawXbm(78, 0, cloudint_width, cloudint_height, cloudint);
  }

}

void ledClear() {
  analogWrite(red, 0);
  analogWrite(green, 0);
  analogWrite(blue, 0);
}

void ledCall(String colour, int strength) {
  analogWrite(red, 0);
  analogWrite(green, 0);
  analogWrite(blue, 0);
  if (colour == "red") {
    analogWrite(red, strength);
  }
  if (colour == "green") {
    analogWrite(green, strength);
  }
  if (colour == "blue") {
    analogWrite(blue, strength);
  }
  if (colour == "purple") {
    analogWrite(blue, strength);
    analogWrite(red, strength);
  }
  delay(100);
}

void ReadFromURL(const char* host, int httpPort, String url, String id, String weather) {

  //Serial.print("connecting to ");
  //Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    // Serial.println("connection failed");
    ledCall("red", 150);
    delay(2000);
    return;
  }

  // We now create a URI for the request

  url += "?id=";
  url += id;
  url += "&weather=";
  url += weather;

  //Serial.print("Requesting URL: ");
  //Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      ledCall("red", 150);
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    
    if (client.find("*")) {
      String tempMsg = client.readStringUntil('\n');
      dateVal = splitToVal(tempMsg, "+", "@");
      timeVal = splitToVal(tempMsg, "@", "|");
      timeVal.remove(5);
      updateVal = splitToVal(tempMsg, "|", "$");
      conditionVal = splitToVal(tempMsg, "$", "^");
      temperatureVal = splitToVal(tempMsg, "^", "(");
    }
  }
  
  Serial.println();
  Serial.println("closing connection");
  sleep = true;
  WiFi.forceSleepBegin();
  delay(500);
}

String splitToVal(String inputString, String delimiter, String endChar) {
  String tempString = "";
  int from;
  int to;
  for (int i = 0; i < inputString.length(); i++) {
    if (inputString.substring(i, i + 1) == delimiter) {
      from = i + 1;
    }
    if (inputString.substring(i, i + 1) == endChar) {
      to = i;
    }
  }
  tempString = inputString.substring(from, to);
  return tempString;
}

void loop() {
  if (sleep == true) {
    ledClear();
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      WiFi.forceSleepWake();
      delay(200);
      connectWifi();
    }
  }
  else {
    ledCall("green", 150);
    ReadFromURL(host1, httpPort1, url, "david", "2644668");
    // the david bit here is just a temporary id string and doesn't actually do anything, feel free to use your own name. If you want to use your own location search for it here http://www.bbc.co.uk/weather/ and copy the number from the resulting URL
    display.clear();
    drawWeather();
    printWeather();
    display.display();
  }
}
