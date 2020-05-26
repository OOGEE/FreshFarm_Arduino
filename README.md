FreshFarm_Arduino
====

> 와이파이 쉴드 (ESP8266) 사용

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
    String mmd = "GET /ARgetRecentData/1 HTTP/1.1\r\nHost: 175.208.85.188:8421\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";
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
    String cmd = "POST /ARuploadData HTTP/1.1\r\nHost: 175.208.85.188\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
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
