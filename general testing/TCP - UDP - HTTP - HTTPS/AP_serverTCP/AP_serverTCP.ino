#include <ESP8266WiFi.h>

IPAddress local_IP(192,168,4,201);
IPAddress gateway(192,168,4,200); 
IPAddress subnet(255,255,255,0);

WiFiServer server(3551); //this is the server socket I guess
#define MAX_SRV_CLIENTS 3 //related to socket only. Don't know if better to use const keyword.
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup()
{
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_AP);
  Serial.println("WIFI Mode : AccessPoint");

  char* ssid = "ESPsoftAP_01";
  char* pass = "AP01_123456"; //can be empty string 
  if (WiFi.softAPConfig(local_IP, gateway, subnet)){
    Serial.print("Setting soft-AP ... ");  
    boolean result = WiFi.softAP( ssid, pass ); //WiFi.softAP(ssid, password, 1 ,0, MAXSC); // WiFi.softAP(ssid, password, channel, hidden, max_connection)
    if(result == true)
    {
      Serial.println("WIFI < " + String(ssid) + " > ... Started");
      Serial.println("Ready"); //as a server, it's probably better to let the client close the connection after that he is satisfied.
      Serial.print("Soft-AP IP address = ");
      Serial.println(WiFi.softAPIP());
      //WiFi.softAPdisconnect(wifioff);
      
      server.begin();
      server.setNoDelay(true); //I think everything you send is directly sent without the need to wait for the buffer to be full
    }
    else
    {
      Serial.println("Failed! Couldn't establish an AP");
    }  
  } else {
    Serial.println("Failed! Couldn't configure an AP");
  }  
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
  serverClients[0] = server.available(); //here the module will accept up to specific number of connections determined by the sold version...
  if (serverClients[0]) {
    Serial.print("some data is available fromt the mobile client.\n");      
    while(serverClients[0].available())
      Serial.write(serverClients[0].read());//only reads one byte?
  }
}
