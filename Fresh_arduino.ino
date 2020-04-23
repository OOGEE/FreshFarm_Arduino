#include <SoftwareSerial.h>
#include "ESP8266.h"

#define SSID      "304-1"
#define PASSWORD  "99341790!@"
#define HOST_NAME "175.208.85.188"
#define HOST_PORT (8421)

SoftwareSerial WIFI = SoftwareSerial(2, 3);
ESP8266 wifi = ESP8266(WIFI);

void setup() {
  Serial.begin(9600);

  if (wifi.setOprToStationSoftAP()) {
      Serial.print("to station + softap ok\r\n");
  } else {
      Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
      Serial.print("Join AP success\r\n");
      Serial.print("IP:");
      Serial.println(wifi.getLocalIP().c_str());       
  } else {
      Serial.print("Join AP failure\r\n");
  }
  
  if (wifi.disableMUX()) {
      Serial.print("single ok\r\n");
  } else {
      Serial.print("single err\r\n");
  }
 
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
       Serial.print("create tcp ok\r\n");
   } else {
       Serial.print("create tcp err\r\n\n");
   }

  delay(1000);
}

void loop() {
  
  String mmd = "GET /getRecentData/1 HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
  wifi.send(mmd.c_str(), mmd.length());
  
  int leng;
  String c;
  
  if((leng = WIFI.available())> 0) {
     c = WIFI.readString();
  }
  
  String sens = "temperature=";
  sens += temp;
  sens += "&humidity=";
  sens += hum;
  sens += "&g_humidity=";
  sens += g_hum;
  sens += "&machine_num=1";
  String cmd = "POST /uploadData HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
  cmd += String(sens.length());
  cmd += "\r\n\r\n";
  cmd += sens;
  wifi.send(cmd.c_str(), cmd.length());

  delay(3000);
}
