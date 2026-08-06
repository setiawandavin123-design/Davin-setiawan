#define BLYNK_TEMPLATE_ID "TMPL6QekSqsSy"
#define BLYNK_TEMPLATE_NAME "projek akhir smart fish akuarium"
#define BLYNK_AUTH_TOKEN "Hn3cmnrzv1fB53cwwCnIJsEQtl30QrDW"
#define BLYNK_PRINT Serial

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define RELAY_KIPAS 26
#define RELAY_LAMPU 27
#define DS18B20_PIN 4

BlynkTimer timer;
char ssid[] = "BPVP-SIDOARJO";
char pass[] = "bpvpsda2024";

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensorSuhu(&oneWire);

float suhu = 0;
int tombol_kipas = 0;
int tombol_lampu = 0;
int tombolmanual = 0;
BLYNK_WRITE(V10) {
 tombolmanual = param.asInt(); // membaca tombol dari blynk
}
BLYNK_WRITE(V7) {
 tombol_kipas = param.asInt(); // membaca tombol dari blynk
 if(tombolmanual == 1) {
  if(tombol_kipas ==1 ){
    digitalWrite(RELAY_KIPAS, HIGH);   // Kipas ON
  }else{
    digitalWrite(RELAY_KIPAS, LOW);
  }
 }
} 

BLYNK_WRITE(V8) {
 tombol_lampu = param.asInt(); // membaca tombol dari blynk
 if(tombolmanual == 1){
  if(tombol_lampu ==1 ){
    digitalWrite(RELAY_LAMPU, HIGH);   // Kipas ON
  }else{
    digitalWrite(RELAY_LAMPU, LOW);
  }
 }
} 

void bacaSuhu(){
  sensorSuhu.requestTemperatures(); //perintah untuk membaca suhu
  suhu = sensorSuhu.getTempCByIndex(0); // hasil pembacaan suhu
  Blynk.virtualWrite(V0, suhu); //menampilkan ke V7
  if (tombolmanual==0) {
    if (suhu >= 26) {

      // Air panas
      digitalWrite(RELAY_KIPAS, HIGH);   // Kipas ON
      digitalWrite(RELAY_LAMPU, LOW);  // Lampu OFF

      Serial.println("PANAS: Kipas ON, Lampu OFF");
      Blynk.virtualWrite(V1, "menyala"); //menampilkan ke V1
      Blynk.virtualWrite(V2, "mati"); //menampilkan ke V2
    } else { 

      // Air dingin
      digitalWrite(RELAY_KIPAS, LOW);  // Kipas OFF
      digitalWrite(RELAY_LAMPU, HIGH);   // Lampu ON

      Serial.println("DINGIN: Kipas OFF, Lampu ON");
      Blynk.virtualWrite(V1, "mati"); //menampilkan ke V1
      Blynk.virtualWrite(V2, "menyala"); //menampilkan ke V2
    }
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(RELAY_KIPAS, OUTPUT);
  pinMode(RELAY_LAMPU, OUTPUT);

  // Kondisi awal: semua OFF
  digitalWrite(RELAY_KIPAS, LOW);
  digitalWrite(RELAY_LAMPU, LOW);

  sensorSuhu.begin();

  Serial.println("Smart Fish Tank Aktif");
  timer.setInterval(2000L, bacaSuhu);
}

void loop() {
    timer.run();
    Blynk.run();
}