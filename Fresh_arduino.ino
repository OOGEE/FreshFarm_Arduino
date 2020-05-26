#include <SoftwareSerial.h>
#include "ESP8266.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

SoftwareSerial WIFI = SoftwareSerial(2, 3);
ESP8266 wifi = ESP8266(WIFI);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(25, 13, NEO_GRB + NEO_KHZ800);
DHT dht(5, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int stack = 3;
float hum, temp;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pixels.setBrightness(255);
  pixels.begin();  // 네오픽셀 제어 시작
  pixels.show();  // 네오픽셀 초기화
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  delay(1000);
}

void loop() {
  digitalWrite(4, LOW);

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
 
  if (wifi.createTCP("175.208.85.188" , (8421))) {
       Serial.print(F("create tcp ok\r\n"));
   } else {
       Serial.print(F("create tcp err\r\n\n"));
   }
  if(stack < 5){
    String mmd = "GET /ARgetRecentData/1 HTTP/1.1\r\nHost: 175.208.85.188:8421\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
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
      for (int i = 0; i < 25; i++) {
        pixels.setPixelColor(i, 0);
        pixels.show();
      }
    }
  
    else if(set_illu == 1) {
      for (int i = 0; i < 25; i++) {
        pixels.setPixelColor(i, 0);
        pixels.show();
      }
      pixels.setPixelColor(12, 255, 255, 255);
      pixels.show();
    }
  
    else if(set_illu == 2) {
      for (int i = 0; i < 25; i++) {
        pixels.setPixelColor(i, 0);
        pixels.show();
      }
      for(int i = 5; i < 10; i++){
        pixels.setPixelColor(i, 255, 255, 255);
        pixels.show();
      }
      for(int i = 15; i < 20; i++) {
        pixels.setPixelColor(i, 255, 255, 255);
        pixels.show();
      }
    }
  
    if(temp > set_temp+1) {
        digitalWrite(6, HIGH);
        digitalWrite(7, LOW);
        digitalWrite(8, LOW);
    }
    else if(temp < set_temp-1) {
        digitalWrite(6, LOW);
        digitalWrite(7, HIGH);
        digitalWrite(8, LOW);
    }
    else {
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
    }
    
    if(hum > set_hum+1) {
        digitalWrite(9, HIGH);
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
    }
    else if(hum < set_hum-1) {
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
        digitalWrite(11, HIGH);
    }
    else {
      digitalWrite(9, LOW);
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
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
    String cmd = "POST /ARuploadData HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
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
