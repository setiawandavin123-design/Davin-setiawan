#define BLYNK_TEMPLATE_ID "TMPL6KBHYIu_B"
#define BLYNK_TEMPLATE_NAME "mencoba led"
#define BLYNK_AUTH_TOKEN "HOuzqhk8y5AfZGRMsIP2d3Ch5XVWx4zg"

#define typeDHT DHT11
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <LCD_I2C.h>

LCD_I2C LCD(0x27, 16,2);
BlynkTimer timer;
char ssid[] = "BPVP-SIDOARJO";
char pass[] = "bpvpsda2024";

const int  DHTPIN = D7;
 int buzzer = D8;
DHT suhu(DHTPIN,typeDHT);

int tombol = 0;
int batasSuhu = 50;
float celcius = 0;
float kelembapan = 0;
float celcuisHindex = 0;
BLYNK_WRITE(V0) {
 tombol = param.asInt(); // membaca tombol dari blynk
}

BLYNK_WRITE(V9) {
 batasSuhu = param.asInt(); // membaca batas suhu dari bllynk
}

void bacaSuhu(){
   kelembapan = suhu.readHumidity();
   celcius = suhu.readTemperature();
   celcuisHindex = suhu.computeHeatIndex(celcius,kelembapan,false);
  
  Blynk.virtualWrite(V7, celcuisHindex); //menampilkan ke V7

  if (tombol == 1 && celcuisHindex >= batasSuhu) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}
void bacaLCD(){
  LCD.clear();
  LCD.setCursor(0,0);
  LCD.print("s= ");
  LCD.print(batasSuhu);
  LCD.print("  ");

  LCD.setCursor(6,0);
  LCD.print("e= ");
  LCD.print(tombol);

  int nilaiBuzzer = digitalRead(buzzer);
  LCD.setCursor(12,0);
  LCD.print("b= ");
  LCD.print(nilaiBuzzer);

  LCD.setCursor(0,1);
  LCD.print("h= ");
  LCD.print(kelembapan);
  LCD.print("    ");

  LCD.setCursor(9,1);
  LCD.print("t= ");
  LCD.print(celcius);
  LCD.print("    ");


}
void setup() {
pinMode(buzzer, OUTPUT);
Serial.begin(115200);
Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
suhu.begin();
LCD.begin();
LCD.backlight();
timer.setInterval(2000L, bacaSuhu);
timer.setInterval(2000L, bacaLCD);

LCD.setCursor(4, 0);
LCD.print("welcome");
LCD.setCursor(4, 1);
LCD.print("mendeteksi");
delay(1000);
LCD.clear();
delay(500);
}

void loop() {
  Blynk.run();
  timer.run();
}