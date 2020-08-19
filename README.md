FreshFarm_Arduino
====

> 와이파이 쉴드 (ESP8266) 사용
> 온습도센서(DHT-22), 토양습도센서(DM2082) 

## 와이파이 쉴드를 사용한 데이터 송신 및 주기 조절

> 와이파이 쉴드는 시리얼 통신 방식으로 데이터를 송수신 함

> 실질적으로 온습도, 토양 습도값은 크게 변하지 않기때문에 일정한 주기에 한 번씩 전송

> if문으로 GET과 POST를 나눠놓은 이유는 아두이노 우노 자체의 메모리의 양이 적기 때문에 이를 해결하기 

~~~
#include <SoftwareSerial.h> // 시리얼 통신을 위한 헤더 선언
#include "ESP8266.h" // ESP8266을 사용하기 위한 라이브러리

SoftwareSerial WIFI = SoftwareSerial(2, 3); // RX와 TX를 각각 2, 3번 핀에 연결
ESP8266 wifi = ESP8266(WIFI); // ESP8266을 사용하기 위해 

//일정한 주기에 한번씩 전송하기위해 stack
int stack = 3;

void loop(){
  //통신을 하기 위한 와이파이 쉴드 세팅
  WIFI.begin(9600);
  
  if (wifi.setOprToStationSoftAP()) {
  } else {
  }

  // WIFI에 연결하기
  if (wifi.joinAP("WI-FI 이름", "WI-FI 비밀번호")) {    
  } else {
  }

  if (wifi.disableMUX()) {
  } else {
  }

  // tcp 연결, 시리얼문을 플래시메모리에 올려서 사용
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print(F("create tcp ok\r\n"));
  } else {
    Serial.print(F("create tcp err\r\n\n"));
  }
  // stack이 5 미만일때 GET 문장 구현
  if(stack < 5){
    // GET을 하기 위해 mmd에 명령어 입력
    String mmd = "GET /ARgetRecentData/1 HTTP/1.1\r\nHost: dalreak-nas.duckdns.org:999\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
    // GET을 send 사용하여 전송
    wifi.send(mmd.c_str(), mmd.length());
    
    // c에 서버에서 넘어온 문장 저장, leng은 문장을 끝까지 받아오기 위함
    int leng;
    String c;
  
    if((leng = WIFI.available())> 0) {
       c = WIFI.readString();
    }
    
    // c의 불필요한 부분 지우기위해 {에서 }까지만 분리
    c = c.substring(c.indexOf('{')+1, c.indexOf('}'));
    c += ',';
  
    // 각각의 set값에 값들을 분리해 넣기위한 작업
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
  }

  // stack 값이 5가 되면 POST문 실행
  if(stack == 5){
    //String sens에 측정한 온도, 습도, 토양 습도값을 붙임
    String sens = "temperature=";
    sens += temp;
    sens += "&humidity=";
    sens += hum;
    sens += "&g_humidity=";
    sens += g_hum;
    sens += "&machine_num=1";
    // POST를 하기 위해 cmd에 명령어 입력후 앞에 sens 값들 합치기
    String cmd = "POST /ARuploadData HTTP/1.1\r\nHost: dalreak-nas.duckdns.org\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
    cmd += String(sens.length());
    cmd += "\r\n\r\n";
    cmd += sens;
    
    // POST를 send를 사용하여 전송
    wifi.send(cmd.c_str(), cmd.length());
    
    // stack값 초기화
    stack = 0;
  }

  // 인터넷 연결 해제
  WIFI.end();

  // 문장을 잘 실행하기위해 stack 
  stack++;
}
~~~

## 식물 생장용 LED를 사용 및 조도 제어

> 12V의 전압을 필요로 하는 식물 생장용 LED를 사용하기 위해 외부 전원 사용

> 릴레이 모듈을 사용함으로써 아두이노로 LED의 ON/OFF 제어를 가능하게 함

~~~
void setup() {
  pinMode(6, OUTPUT)
}

void loop() {
  if(stack < 5) {
    // GET해서 set_illu가 0일때 LED 소등
    if(set_illu == 0) {
      digitalWrite(6, LOW);
    }
    // GET해서 set_illu가 1일때 점등
    else if(set_illu == 1) {}
      digitalWrite(6, HIGH);
    }
  }
}
~~~

## 온습도센서, 토양습도센서를 이용한 데이터 측정 및 제어

> 온습도센서는 전용 라이브러리를 사용하여 디지털 포트로 데이터를 측정

> 토양습도센서는 아두이노의 기능 중 하나인 아날로그 리드를 사용하여 데이터 측정

> 아날로그값은 0~1024사이의 값이 나오므로 그에 따른 적당한 수식으로 값을 %로 바꿈

> 측정한 온습도값과 토양습도값에따라 3색 LED와 펌프를 제어

> 측정한 온습도값을 LCD에 표시

> 펌프때문에 아두이노의 전압이 부족해져 다른 장치들의 동작이 불안해 지는 것을 방지하기 위해 외부 전압을 사용하고 이를 제어하기위해 릴레이 모듈 사용

~~~
// DHT-22, LCD를 사용하기 위한 라이브러리 선언
#include<Adafruit_Sensor.h>
#include<DHT.h>
#include<DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

DHT dht(5, DHT22); // DHT22를 5번 디지털포트에 연결

float hum,temp; // 습도와 온도값을 각각 hum과 temp값에 입력받기위해 변수 선언

void setup() {
  dht.begin() // dht 켜기
  
  // 펌프 제어를 위한 4번 디지털 포트 정의
  pinMode(4, OUTPUT);
  
  // 측정한 온도값과 세팅값을 비교해 3색 LED를 키기위한 6, 7, 8번 디지털포트 정의
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  // 측정한 습도값과 세팅값을 비교해 3색 LED를 키기위한 9, 10, 11번 디지털포트 정의
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  
  // lcd를 사용하기 위한 초기화작업 진행
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  digitalWrite(4, LOW); // 펌프가 너무 오래 가동되지않게 반복 문장을 실행할때 펌프 정지, 펌프는 4번핀에 연결되어있음
  
  // 습도와 온도값 dht 라이브러리를 사용하여 읽어옴
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  
  // 토양 습도값을 아날로그값으로 읽어와 물에 넣었을때 저항(620)과 공기중의 저항(300) 값을 구해 그에 따른 퍼센트 수식으로 변환
  float g_hum = analogRead(A3);
  g_hum = 100 - (100 * (g_hum - 300) / 320);
  
  // lcd에 온도와 습도, 토양습도 표시
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
  
  if(stack < 5) {
    // 현재 토양 습도값이 세팅값보다 낮을 시 펌프 가동
    // 너무 예민하게 반응하지 않기 위해 2의 오차값을 둠
    if(g_hum < set_g_hum-2) {
      digitalWrite(4, HIGH);
    }
    
    // 현재 온도값과 세팅값의 차이에따른 LED 점등
    // 너무 예민하게 반응하지 않기 위해 1의 오차값을 둠
    // 현재 온도가 세팅값보다 높을 시 적색등 점등
    if(temp > set_temp+1) {
        digitalWrite(7, HIGH);
        digitalWrite(8, LOW);
        digitalWrite(9, LOW);
    }
    // 현재 온도가 세팅값보다 낮을 시 청색등 점등
    else if(temp < set_temp-1) {
        digitalWrite(7, LOW);
        digitalWrite(8, HIGH);
        digitalWrite(9, LOW);
    }
    // 현재 온도가 세팅값과 비슷할시 녹색등 점등
    else {
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
    }
    
    // 현재 습도값과 세팅값의 차이에따른 LED 점등
    // 너무 예민하게 반응하지 않기 위해 1의 오차값을 둠
    // 현재 습도가 세팅값보다 높을 시 적색등 점등
    if(hum > set_hum+1) {
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
    }
    // 현재 습도가 세팅값보다 낮을 시 청색등 점등
    else if(hum < set_hum-1) {
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, HIGH);
    }
    // 현재 습도가 세팅값과 비슷할시 녹색등 점등
    else {
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);
    }
  }
