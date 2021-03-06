/**
   PostHTTPClient.ino

    Created on: 21.11.2016

*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

/* this can be run with an emulated server on host:
        cd esp8266-core-root-dir
        cd tests/host
        make ../../libraries/ESP8266WebServer/examples/PostServer/PostServer
        bin/PostServer/PostServer
   then put your PC's IP address in SERVER_IP below, port 9080 (instead of default 80):
*/
//#define SERVER_IP "10.0.1.7:9080" // PC address with emulation on host
#define SERVER_IP "diraniyoussef.pythonanywhere.com:80" 

#ifndef STASSID
#define STASSID "TP-LINK_2B5E"
#define STAPSK  "87654321"
#endif

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {

  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  http.begin(client, "http://" SERVER_IP "/"); //HTTP
  http.addHeader("Content-Type", "application/json");

  Serial.print("[HTTP] POST...\n");
  // start connection and send HTTP header and body
  int httpCode = http.POST("{\"image\":\"fffeee\"}");

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      /*
      const String& payload = http.getString();
      Serial.println("received payload:\n<<");
      Serial.println(payload);
      Serial.println(">>");
      */
            
      // get length of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();

      // create buffer for read
      static uint8_t buff[128] = { 0 };

      // read all data from server
      while (http.connected() && (len > 0 || len == -1)) {
        // get available data size
        size_t size = client.available();

        if (size) {
          // read up to 128 byte
          int c = client.readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

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
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  
  delay(10000);
}
