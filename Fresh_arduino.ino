#include <SoftwareSerial.h>
#include "ESP8266.h"

#define SSID      "304-1"
#define PASSWORD  "99341790!@"
#define HOST_NAME "175.208.85.188"
#define HOST_PORT (8421)

SoftwareSerial WIFI = SoftwareSerial(2, 3);
ESP8266 wifi = ESP8266(WIFI);

int stack = 10;

void setup() {
  Serial.begin(9600);
  WIFI.begin(9600);
  
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
  
  String mmd = "GET /ARgetRecentData/1 HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
  wifi.send(mmd.c_str(), mmd.length());
  
  int leng;
  String c;
  
  if((leng = WIFI.available())> 0) {
     c = WIFI.readString();
  }

  String data = c.substring(c.indexOf('{')+1, c.indexOf('}'));
  data += ',';
  
  int set_illu, set_temp, set_hum, set_g_hum;
  int index1_1, index1_2, index1_3, index1_4, index2_1, index2_2, index2_3, index2_4;

  index1_1 = data.indexOf(':');
  index1_2 = data.indexOf(':', index1_1+1);
  index1_3 = data.indexOf(':', index1_2+1);
  index1_4 = data.indexOf(':', index1_3+1);
  index2_1 = data.indexOf(',');
  index2_2 = data.indexOf(',', index2_1+1);
  index2_3 = data.indexOf(',', index2_2+1);
  index2_4 = data.indexOf(',', index2_3+1);

  set_illu = data.substring(index1_1+1, index2_1).toInt();
  set_temp = data.substring(index1_2+1, index2_2).toInt();
  set_hum = data.substring(index1_3+1, index2_3).toInt();
  set_g_hum = data.substring(index1_4+1, index2_4).toInt();
  
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
