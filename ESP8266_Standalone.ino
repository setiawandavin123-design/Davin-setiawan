/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  NOTE: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6asLmRSXM"
#define BLYNK_TEMPLATE_NAME "UJK"
#define BLYNK_AUTH_TOKEN "bX-d-86sc9TgSzCfJvw0qbU1og9BjlTF"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "BPVP-SIDOARJO";
char pass[] = "bpvpsda2024";

#define relay D7
#define pTrig D2
#define pEcho D1
#define switch1 D5


int jarak;
long durasi = 0;
int setPointPercent;
bool modeSetPoint;

BlynkTimer timer;

BLYNK_WRITE(V2) {
  setPointPercent = param.asInt();
}

BLYNK_WRITE(V3) {
  modeSetPoint = param.asInt();
}

void fitur() {
  digitalWrite(pTrig, 1);
  delayMicroseconds(10);
  digitalWrite(pTrig, 0);

  durasi = pulseIn(pEcho, 1);
  jarak = (durasi*0.034)/2;

  int val = map(jarak, 45, 5, 0, 100);

  Serial.println(jarak);
  Blynk.virtualWrite(V0, val);

  // fitur untuk indikator ketinggian air
  if(jarak < 15){
    Blynk.virtualWrite(V1,1);
    Blynk.setProperty(V1, "color", "#23C48E");
  }else if(jarak >= 15 && jarak <= 30){
    Blynk.virtualWrite(V1,1);
    Blynk.setProperty(V1, "color", "#ED9D00");
  }else if(jarak > 30){
    Blynk.virtualWrite(V1,1);
    Blynk.setProperty(V1, "color", "#D3435C");
  }

  // fitur nyala mati menggunakan saklar
  if(digitalRead(switch1) == 1 && modeSetPoint == 0){
    digitalWrite(relay, 1);
    Blynk.virtualWrite(V4, "POMPA MATI");
  }else if(digitalRead(switch1) == 0 && modeSetPoint == 0){
    Blynk.virtualWrite(V4, "POMPA MENYALA");
    digitalWrite(relay, 0);
  }

  // fitur mode otomatatis 
  if(modeSetPoint == 1 && val < setPointPercent){
    digitalWrite(relay, 0);
    Blynk.virtualWrite(V4, "POMPA MENYALA");
  }else if(val > 99){
    digitalWrite(relay, 1);
    Blynk.virtualWrite(V4, "POMPA MATI");    
  }else{
    
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(pTrig, OUTPUT);
  pinMode(pEcho, INPUT);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  timer.setInterval(1000L, fitur);
  Blynk.virtualWrite(V2, setPointPercent);
  Blynk.virtualWrite(V3, modeSetPoint);
}

void loop() {
  Blynk.run();
  timer.run();
}
