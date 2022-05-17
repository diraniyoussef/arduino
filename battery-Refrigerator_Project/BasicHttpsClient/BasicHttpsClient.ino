/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

//#include <Arduino.h>
//#include <memory>
//#include <unique_ptr.h>

#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>

// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date
//const uint8_t fingerprint[20] = {0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3};
//const uint8_t fingerprint[20] = {0x15, 0x77, 0xdc, 0x04, 0x7c, 0x00, 0xf8, 0x70, 0x09, 0x34, 0x24, 0xf4, 0xd3, 0xa1, 0x7a, 0x6c, 0x1e, 0xa3, 0xe0, 0x2a}; //this is gotten from StreamHttpsClient example (which is another file).

//ESP8266WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  /*
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  */

  //WiFi.mode(WIFI_STA);
  //WiFiMulti.addAP("TP-LINK_2B5E", "87654321");

  WiFi.begin("TP-LINK_2B5E", "87654321");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // wait for WiFi connection
  //if ((WiFiMulti.run() == WL_CONNECTED)) {

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  //BearSSL::WiFiClientSecure* client = new BearSSL::WiFiClientSecure();

  //client->setFingerprint(fingerprint);
  // Or, if you happy to ignore the SSL certificate, then use the following line instead:
  client->setInsecure();

  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  if (https.begin(*client, "https://diraniyoussef.pythonanywhere.com")) {  // HTTPS
    
    https.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpsCode = https.POST("{\"image\":\"aaabbb\"}");
  
    // httpCode will be negative on error
    if (httpsCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpsCode);
  
      // file found at server
      if (httpsCode == HTTP_CODE_OK) { //|| httpsCode == HTTP_CODE_MOVED_PERMANENTLY) {
        /*
        const String& payload = https.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
        */
        
        // get length of document (is -1 when Server sends no Content-Length header)
        int len = https.getSize();

        // create buffer for read
        static uint8_t buff[128] = { 0 };

        // read all data from server
        while (https.connected() && (len > 0 || len == -1)) {
          // get available data size
          size_t size = client->available();

          if (size) {
            // read up to 128 byte
            int c = client->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

            // write it to Serial
            Serial.write(buff, c);

            if (len > 0) {
              len -= c;
            }
          }
          delay(1);
        }
        
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", https.errorToString(httpsCode).c_str());
    }
  
    https.end();

  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  //}

  Serial.println("Wait 10s before next round...");
  delay(10000);
}
