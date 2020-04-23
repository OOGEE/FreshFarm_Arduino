FreshFarm_Arduino
====

> 와이파이 쉴드 (ESP8266) 사용

## 와이파이 쉴드를 사용한 데이터 송신 및 주기 조절

> 와이파이 쉴드는 시리얼 통신 방식으로 데이터를 송수신 함

> 실질적으로 온습도값, 토양습도 값은 그리 자주 바뀌지 않기 때문에 일정 주기를 두고 전송

~~~
#include <SoftwareSerial.h> // 시리얼 통신을 위한 헤더 선언
#include "ESP8266.h" // ESP8266을 사용하기 위한 라이브러리

#define SSID        "WIFI-NAME" // WIFI의 이름을 SSID에 정의해놓음
#define PASSWORD    "PASSWORD" // WIFI의 비밀번호를 PASSWORD에 정의해놓음
#define HOST_NAME   "IP" // 서버의 주소를 정의해놓음
#define HOST_PORT   (****) // 포트번호를 지정해놓음

SoftwareSerial WIFI = SoftwareSerial(2, 3); // RX와 TX를 각각 2, 3번 핀에 연결
ESP8266 wifi = ESP8266(WIFI); // ESP8266을 사용하기 위해 

//통신을 하기 위한 와이파이 쉴드 세팅
if (wifi.setOprToStationSoftAP()) {
  Serial.print("to station + softap ok\r\n");
} else {
  Serial.print("to station + softap err\r\n");
}

// WIFI에 연결하기
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

// tcp 연결
if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
   Serial.print("create tcp ok\r\n");
} else {
  Serial.print("create tcp err\r\n\n");
}

  // sens에 측정한 데이터 값들을 json 형태로 결합
  String sens = "temperature=";
  sens += temp;
  sens += "&humidity=";
  sens += hum;
  sens += "&g_humidity=";
  sens += g_hum;
  sens += "&machine_num=4";
  
  // POST를 하기 위한 http 프로토콜 작성
  String cmd = "POST /upload HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
  cmd += String(sens.length());
  cmd += "\r\n\r\n";
  cmd += sens;
  
  // 서버에 string 전송
  wifi.send(cmd.c_str(), cmd.length());

  // GET을 하기 위한 http 프로토콜 작성
  String mmd = "GET /getRecentData/1 HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";

  // 서버에 string 전송
  wifi.send(mmd.c_str(), mmd.length());
  
  // 서버에서 받아온 string의 값을 c에 저장
  int leng;
  String c;
  
  if((leng = WIFI.available())> 0) {
     c = WIFI.readString();
  }
