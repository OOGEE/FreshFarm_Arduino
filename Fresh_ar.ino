#include <SoftwareSerial.h>
#include "ESP8266.h"

#define SSID      "304-1"
#define PASSWORD  "99341790!@"
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
  String sens = "temperature=";
  sens += temp;
  sens += "&humidity=";
  sens += hum;
  sens += "&g_humidity=";
  sens += g_hum;
  sens += "&machine_num=4";
  String cmd = "POST /upload HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
  cmd += String(sens.length());
  cmd += "\r\n\r\n";
  cmd += sens;
  wifi.send(cmd.c_str(), cmd.length());

  delay(3000);
}
