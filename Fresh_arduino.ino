#include <SoftwareSerial.h>
#include "ESP8266.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

SoftwareSerial WIFI = SoftwareSerial(2, 3);
ESP8266 wifi = ESP8266(WIFI);
DHT dht(5, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int stack = 3;
float hum, temp;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(4, OUTPUT); // motor
  pinMode(6, OUTPUT); // LED
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  delay(1000);
}

void loop() {
  digitalWrite(4, HIGH);
  digitalWrite(6, HIGH);

  hum = dht.readHumidity();
  temp = dht.readTemperature();
  float g_hum = analogRead(A3);
  g_hum = 100 - (100 * (g_hum - 300) / 320);

  lcd.setCursor(0, 0);
  lcd.print("TEMP : ");
  lcd.print(temp, 1);
  lcd.print(" C deg");
  lcd.setCursor(0,1);
  lcd.print("HUM : ");
  lcd.print(hum);
  lcd.print(" %");
  lcd.setCursor(0, 2);
  lcd.print("G_HUM : ");
  lcd.print(g_hum);
  lcd.print(" %");
  
  WIFI.begin(9600);
  
  if (wifi.setOprToStationSoftAP()) {
  } else {
  }


  if (wifi.joinAP("304-1", "99341790!@")) {     
  } else {
  }
  
  if (wifi.disableMUX()) {
  } else {
  }
 
  if (wifi.createTCP("darleak-nas.duckdns.org" , (9999))) {
       Serial.print(F("create tcp ok\r\n"));
   } else {
       Serial.print(F("create tcp err\r\n\n"));
   }
  if(stack < 5){
    String mmd = "GET /ARgetRecentData/1 HTTP/1.1\r\nHost: dalreak-nas.duckdns.org:9999\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
    wifi.send(mmd.c_str(), mmd.length());
  
    int leng;
    String c;
  
    if((leng = WIFI.available())> 0) {
       c = WIFI.readString();
    }
    
    c = c.substring(c.indexOf('{')+1, c.indexOf('}'));
    c += ',';
  
    int set_illu, set_temp, set_hum, set_g_hum;
    int index1_1, index1_2, index1_3, index1_4, index2_1, index2_2, index2_3, index2_4;

  
    index1_1 = c.indexOf(':');
    index1_2 = c.indexOf(':', index1_1+1);
    index1_3 = c.indexOf(':', index1_2+1);
    index1_4 = c.indexOf(':', index1_3+1);
    index2_1 = c.indexOf(',');
    index2_2 = c.indexOf(',', index2_1+1);
    index2_3 = c.indexOf(',', index2_2+1);
    index2_4 = c.indexOf(',', index2_3+1);

    set_illu = c.substring(index1_1+1, index2_1).toInt();
    set_temp = c.substring(index1_2+1, index2_2).toInt();
    set_hum = c.substring(index1_3+1, index2_3).toInt();
    set_g_hum = c.substring(index1_4+1, index2_4).toInt();

    Serial.println(set_g_hum);
  
    if(g_hum < set_g_hum-2) {
      digitalWrite(4, HIGH);
    }

    if(set_illu == 0) {
      digitalWrite(6, HIGH);
    }
  
    else if(set_illu == 1) {
      digitalWrite(6, LOW);
    }
  
    if(temp > set_temp+1) {
        digitalWrite(7, HIGH);
        digitalWrite(8, LOW);
        digitalWrite(9, LOW);
    }
    else if(temp < set_temp-1) {
        digitalWrite(7, LOW);
        digitalWrite(8, HIGH);
        digitalWrite(9, LOW);
    }
    else {
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
    }
    
    if(hum > set_hum+1) {
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
    }
    else if(hum < set_hum-1) {
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, HIGH);
    }
    else {
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);
    }

    delay(3000);
  }
  
  if(stack == 5){
    String sens = "temperature=";
    sens += temp;
    sens += "&humidity=";
    sens += hum;
    sens += "&g_humidity=";
    sens += g_hum;
    sens += "&machine_num=1";
    String cmd = "POST /ARuploadData HTTP/1.1\r\nHost: dalreak-nas.duckdns.org\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
    cmd += String(sens.length());
    cmd += "\r\n\r\n";
    cmd += sens;
    wifi.send(cmd.c_str(), cmd.length());
    stack = 0;
  }

  WIFI.end();

  stack++;
  
  delay(3000);
}
