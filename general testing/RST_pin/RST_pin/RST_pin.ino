#include <ESP8266WiFi.h>

const static byte resetting_pin = 9;
static int getRealPinFromD(int D_pin){
     switch (D_pin) {
      case 0:
        return (16);
        break;
      case 1:
        return (5);
        break;
      case 2:
        return (4);
        break;
      case 3:
        return (0);
        break;
      case 4:
        return (2);
        break;
      case 5:
        return (14);
        break;
      case 6:
        return (12);
        break;
      case 7:
        return (13);
        break;
      case 8:
        return (15);
        break;
      case 9:
        return (3);
        break;
      case 10:
        return (1);
        break;
      default:
        return (16);//this should never be used.
        // statements
        break;
     }
  }
  
static void pulsingRST_Pin() {
  digitalWrite( getRealPinFromD( resetting_pin ), LOW ); //at this point RST will be connected to GND (according to PCB design)
  delay(100);
  digitalWrite( getRealPinFromD( resetting_pin ), HIGH ); //at this point it start HW resetting.
}

static void floatingRST() {
  digitalWrite( getRealPinFromD( resetting_pin ), HIGH );
}

void setup() { 
  Serial.begin(115200);  
  Serial.println("Beginning....");
  
  pinMode( getRealPinFromD( resetting_pin ) , OUTPUT );
  floatingRST();

//  if ( WiFi.config(local_IP, gateway, subnet) ) {//this allocates a static IP.  //comment for debugging in case this NodeMCU will use the DHCP of the local network router
    if ( WiFi.mode(WIFI_STA) ) {   //please follow this order: WiFi.config, then WiFi.mode(WIFI_STA) then WiFi.begin. https://github.com/esp8266/Arduino/issues/2371                
      WiFi.begin("Electrotel_Dirani", "onlyforworkpls"); //Nothing would happen if we executed this twice - tested.
      Serial.println("wifi connected");
    }
//  }
  
}

int i = 0;

void loop() {
  Serial.println("looping");
  i++;
  if( i == 10 ) {    
    pulsingRST_Pin();
  }
  
  delay(1000);
}
