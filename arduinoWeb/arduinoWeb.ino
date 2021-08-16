#include <SPI.h> // SPI 통신을 위한 헤더
#include <Ethernet.h> // 이더넷 쉴드를 사용하기 위한 헤더
#include <SD.h> // SD카드를 사용하기 위한 FILE 클래스 헤더
#include "pitches.h" // 부저 음역대
#include <DHT11.h> // 온습도를 사용하기 위한 헤더

#define CS_PIN 4 // SD카드를 사용하기 위한 핀 번호 지정
#define FILENAME F("dust.htm") // HTML 파일을 불러올 파일의 이름을 플래시메모리에 저장
#define cdspin A0 // 조도센서 값 받기
#define gaspin A1 // 가스센서 값 받기

#define TRIG 9 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO 8 //ECHO 핀 설정 (초음파 받는 핀)

#define led1_pin 7 // 선풍기를 제어하기 위한 핀 설정
#define led2_pin 6 // 조명을 제어하기 위한 핀 설정





// 미세먼지 설정 값


byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02}; // 범용 맥주소
IPAddress ip(192, 168, 1, 110); // 고정 IP
EthernetServer server(80); // 기본 80번 포트에 포워딩 했음.

unsigned long previousRequestMiilis; // 서버 응답 함수 delay 설정
int requestInterval = 1;
String HTTP_REQUEST; // 서버로부터 요청한 파라미터 확인

unsigned long previousGasMiilis; // 가스 센서 값을 받아오기
int gasInterval = 10;
int gas = 0;

int music[] = { // 부저를 울리기 위한 비트 설정
  NOTE_F7,NOTE_DS8,NOTE_F7,NOTE_DS8,NOTE_F7,NOTE_DS8,NOTE_F7,NOTE_DS8,NOTE_F7,NOTE_DS8,NOTE_F7,NOTE_DS8
};
int beats[] = { 8,8,8,8,8,8,8,8,8,8,8,8 };
int thisNote = 0;

unsigned long MusicpreviousMillis = 0;
int Musicinterval = 0;
boolean Musicsound = false;

unsigned long previousCdsMillis; // 조도 센서 값을 받아오기
int cdsInterval = 10;
int cds = 0;

int pin = 22; // 온습도 센서 값을 받아오기
DHT11 dht11(pin);
unsigned long previousDhtMillis;
int dhtInterval = 10;
float humi;
float temp;

int measurePin = A2; //먼지 센서 A2에 연결
int ledPower = 2;   //적외선 led
int timeStatus = 0;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

unsigned long previousMeasureMillis = 0; // 슈퍼루프를 구현하기 위한 변수들로 구분 할 수 있게 가운데 부분의 영문을 바꿨다.  
unsigned long measureinterval; // 미세먼지 함수 설정 값

unsigned long cdsFile = 1802; // 조도 센서 값이 html 파일 내의 갱신 되는 위치
char cdsBuffer[BUFSIZ];

unsigned long dhtFile = 2273; // 온습도 센서 값이 html 파일 내의 갱신 되는 위치
char dhtHumiBuffer[BUFSIZ]; // humi 값 버퍼
char dhtTempBuffer[BUFSIZ]; // temp 값 버퍼

unsigned long gasFile = 2610; // 가스 센서 값이 html 파일 내의 갱신 되는 위치
char gasBuffer[BUFSIZ];

unsigned long dustFile = 3149; // 미세먼지 값이 위치할 버퍼 위치
char dustBuffer[BUFSIZ];

void setup()
{ 
    pinMode(led1_pin , OUTPUT);
    pinMode(led2_pin , OUTPUT);
    pinMode(ledPower, OUTPUT);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    initializeSdCard(); // SD 카드 init 
    fileExistSdCard(); // html 파일이 존재하는지 확인
    
    Ethernet.begin(mac, ip);  // 이더넷 쉴드를 사용하기 위한 준비
    server.begin(); // 웹 서버 시작.
}

void loop() {
    cdsValue();
    dhtValue();
    gasValue();
    clientRequest();
    echo();
    dust();

    if(Musicsound == true) {
      Music_loop();
    }
    else {
      Music_stop();
    }
}

void initializeSdCard() { // SD 카드를 준비합니다. 만약 실패했다면 인식할 수 없는걸 알 수 있도록 표시합니다.
    Serial.begin(115200);
    Serial.println(F("SD 카드를 준비중입니다..."));

    if(!SD.begin(CS_PIN)) {
      Serial.println("SD카드를 인식할 수 없습니다.");
      while(1);
    }

    Serial.println(F("SD카드 준비가 완료되었습니다."));
}

void fileExistSdCard() {
    if (!SD.exists(FILENAME)) {
        Serial.println(F("ERROR - Can't find index.htm file!"));
        while (1); // 파일이 없는 경우 무한 루프
    }
    Serial.println(F("SUCCESS - Found index.htm file."));
}

void clientRequest() { // 클라이언트로써 서버로 HTTP 요청을 했을 때 응답하는 함수.
    unsigned long currentRequestMiilis = millis();

    if( currentRequestMiilis - previousRequestMiilis >= requestInterval) {
        previousRequestMiilis = currentRequestMiilis;

        EthernetClient client = server.available();  // try to get client
     
        if (client) {  // got client?
            boolean currentLineIsBlank = true;
            while (client.connected()) {
                if (client.available()) {   // client data available to read
                    char c = client.read(); // read 1 byte (character) from client
                    HTTP_REQUEST += c;
                    // last line of client request is blank and ends with \n
                    // respond to client only after last line received
                    if (c == '\n' && currentLineIsBlank) {
                        // send a standard http response header
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");
                        client.println();
                        // send web page
                        File webFile = SD.open(FILENAME);
    
                        if(webFile) {
                          while(webFile.available()) {
                            client.write(webFile.read());
                          }
                          webFile.close();
                        }

                        if(HTTP_REQUEST.indexOf("fan=on") > -1) {
                          digitalWrite(led1_pin , HIGH);
                        }
                        if(HTTP_REQUEST.indexOf("fan=off") > -1) {
                          digitalWrite(led1_pin , LOW);
                        }
                        if(HTTP_REQUEST.indexOf("bulb=on") > -1) {
                          digitalWrite(led2_pin , HIGH);
                        }
                        if(HTTP_REQUEST.indexOf("bulb=off") > -1) {
                          digitalWrite(led2_pin , LOW);
                        }
                        HTTP_REQUEST = "";
                        break;
                    }
                    if (c == '\n') {
                        currentLineIsBlank = true;
                    } 
                    else if (c != '\r') {
                        currentLineIsBlank = false;
                    }
                } 
            } 
             client.stop(); // close the connection
        }
          
      }
}

void cdsValue() {
  unsigned long currentCdsMillis = millis();

  if(currentCdsMillis - previousCdsMillis >= cdsInterval) {
    previousCdsMillis = currentCdsMillis;

    cds = map(analogRead(cdspin),0,1024,0,255);
    sprintf(cdsBuffer, "%d", cds);

    File dataFile = SD.open(FILENAME , O_RDWR);

        if(dataFile) {
          
          dataFile.seek(cdsFile);
          dataFile.write("   ");
          dataFile.seek(cdsFile);
          dataFile.write(cdsBuffer);
          
        }
    dataFile.close();
  }
}

void dhtValue() {
  unsigned long currentDhtMillis = millis();
  
  if(currentDhtMillis - previousDhtMillis >= dhtInterval) {
    previousDhtMillis = currentDhtMillis; 

    dht11.read(humi , temp);

    char str_humi[6];
    char str_temp[6];
    
    dtostrf(temp, 3, 2, str_temp);
    sprintf(dhtTempBuffer,"%s ", str_temp);
   
    dtostrf(humi, 3, 2, str_humi);
    sprintf(dhtHumiBuffer,"%s ", str_humi);
    
    File dataFile = SD.open(FILENAME , O_RDWR); // 습도 데이터 갱신

      if(dataFile) {
        
        dataFile.seek(dhtFile);
        dataFile.write("                                  ");
        dataFile.seek(dhtFile);

        char str[20] = "<sup>o</sup>C / ";
        strcat(dhtTempBuffer , str);
        strcat(dhtTempBuffer , dhtHumiBuffer);
        strcat(dhtTempBuffer , "%");
        dataFile.write(dhtTempBuffer);
        
      }
    dataFile.close();
  }
}

void gasValue() {
  unsigned long currentGasMiilis = millis();
  
  if( currentGasMiilis - previousGasMiilis >= gasInterval) {
        previousGasMiilis = currentGasMiilis;
        
        gas = analogRead(gaspin);

        if(gas >= 200) {
          Musicsound = true;
        }
        else {
          Musicsound = false;
        }
        
        sprintf(gasBuffer, "%d", gas);

        File dataFile = SD.open(FILENAME , O_RDWR);

        if(dataFile) {
          
          dataFile.seek(gasFile);
          dataFile.write("                              ");
          dataFile.seek(gasFile);
          dataFile.write(gasBuffer);
          
        }
        dataFile.close();
  }
}

void Music_loop() {
  unsigned long MusiccurrentMillis = millis();

  if(MusiccurrentMillis - MusicpreviousMillis >= Musicinterval) {
      MusicpreviousMillis =  MusiccurrentMillis;

        int duration = 1000 / beats[thisNote % 12];
        tone(12,music[thisNote%12]);
        int MusicTime = duration * 1.3;
        Musicinterval = MusicTime;
        thisNote++;
      
  }
}

void Music_stop() {
  noTone(12);
}





unsigned long previousMillis = 0;
unsigned long interval;

int echoTimeStatus = 0;

void echo() {
  long duration, distance; //우리가 거리 측정에 사용하게 될 변수 : distance 단위 : cm

  unsigned long currentMillis = micros();

  if (echoTimeStatus == 0) {
    interval = 2;
  }
  if (echoTimeStatus == 1) {
    interval = 10;
  }
  if (echoTimeStatus == 2) {
    interval = 1000000; //1초
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (echoTimeStatus == 0) {
      digitalWrite(TRIG, LOW);
      echoTimeStatus += 1;
    } else if (echoTimeStatus == 1) {
      digitalWrite(TRIG, HIGH);
      echoTimeStatus += 1;
    } else if (echoTimeStatus == 2) {
      digitalWrite(TRIG, LOW);
      duration = pulseIn (ECHO, HIGH); //물체에 반사되어돌아온 초음파의 시간을 변수에 저장합니다.
      //34000*초음파가 물체로 부터 반사되어 돌아오는시간 /1000000 / 2(왕복값이아니라 편도값이기때문에 나누기2를 해줍니다.)
      
      //*************************************************distance로 거리 측정(cm)**********************************
      distance = duration * 17 / 1000; //초음파센서의 거리값이 위 계산값과 동일하게 Cm로 환산되는 계산공식 입니다. 수식이 간단해지도록 적용했습니다.
      
      //시리얼모니터로 초음파 거리값을 확인 하는 코드 입니다.
      Serial.print(distance); //측정된 물체로부터 거리값(cm값)을 보여줍니다.

      if(distance <= 30) {
        digitalWrite(led1_pin,HIGH);
      }

      
      Serial.println(" Cm");
      echoTimeStatus = 0;

    }
  }
}

void dust() {

  unsigned long currentMeasureMillis = micros();
   
  if (timeStatus == 0) { interval = 280;}
  if (timeStatus == 1) { interval = 40;}
  if (timeStatus == 2) { interval = 9680;}

  if (currentMeasureMillis - previousMeasureMillis >= measureinterval) {
    previousMeasureMillis = currentMeasureMillis;

    if (timeStatus == 0) { 
         digitalWrite(ledPower, LOW); // power on the LED
        

         timeStatus += 1;
    }
    else if (timeStatus == 1) {
         voMeasured = analogRead(measurePin); // read the dust value
         
         
         timeStatus += 1;
    }
    else if (timeStatus == 2) {
      digitalWrite(ledPower, HIGH); // turn the LED off
          // 0 - 5V mapped to 0 - 1023 integer values
    // recover voltage
    calcVoltage = voMeasured * (5.0 / 1024.0);

    dustDensity = 0.17 * calcVoltage - 0.1;
    //먼지 농도에 아날로그 출력 값과 전압 변화 (참고 용임)
    Serial.print("Raw Signal Value (0-1023): ");
    Serial.print(voMeasured);
    Serial.print(" - Voltage: ");
    Serial.print(calcVoltage);
    //먼지 농도 (우리가 사용할 값 (float)) ******************
    Serial.print(" - Dust Density: ");
    Serial.println(dustDensity); // unit: mg/m3
    timeStatus = 0;

    char str_dust[6];
    
    dtostrf(dustDensity, 3, 2, str_dust);
    sprintf(dustBuffer,"%s ", str_dust);

    File dataFile = SD.open(FILENAME , O_RDWR); // 습도 데이터 갱신

      if(dataFile) {
        
        dataFile.seek(dustFile);
        dataFile.write("                                        ");
        dataFile.seek(dustFile);

        char str[30] = "mg/m<sup>3</sup> / SAFE";
        strcat(dustBuffer , str);
        dataFile.write(dustBuffer);
        
      }
    dataFile.close();
    }

   

  }
}
