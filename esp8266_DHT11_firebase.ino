#define BLYNK_TEMPLATE_ID "TMPL6WK59dHbD"
#define BLYNK_TEMPLATE_NAME "MONITORING PIR dan LDR"
#define BLYNK_AUTH_TOKEN "4BXTjjUuNeMkZUU-0r1QoYZ7lNKKCKjw"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//================ WIFI =================
char ssid[] = "BPVP-SIDOARJO";
char pass[] = "bpvpsda2024";

//============== FIREBASE ===============
// ⚠️ GANTI dengan API Key & Database URL asli dari Firebase Console
#define API_KEY "AIzaSyC9a2Tl8jRFnqaubLiXUyjoeD15E7IoQ8o"
#define DATABASE_URL "https://monitoring-sensor-pir-dan-ldr-default-rtdb.asia-southeast1.firebasedatabase.app/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//================ LCD ==================
LiquidCrystal_I2C lcd(0x27, 16, 2);

//================ PIN ==================
#define LDR_PIN A0
#define PIR_PIN D3
#define RELAY_PIN D5

//=============== SETTING ===============
#define BATAS_GELAP 300
#define RELAY_ON HIGH
#define RELAY_OFF LOW
unsigned long waktuTunggu = 5000;        // 5 detik setelah gerakan hilang lampu mati
unsigned long waktuGerakanTerakhir = 0;
unsigned long lastDisplayUpdate =0;
bool lampuMenyala = false;
BlynkTimer timer;

// Variabel global status (diupdate oleh kontrolRelay)
int nilaiLDR = 0;
bool gerakan = false;
bool gelap = false;
String statusCahaya = "TERANG";
String statusGerakan = "KOSONG";
String statusLampu = "OFF";

//========================================================
// Fungsi untuk mengirim data ke Firebase
//========================================================
void kirimFirebase(int ldr, String cahaya, String gerakan, String lampu) {
  if (Firebase.ready()) {
    Firebase.RTDB.setInt(&fbdo, "Monitoring_PIR_LDR/LDR", ldr);
    Firebase.RTDB.setString(&fbdo, "Monitoring_PIR_LDR/Cahaya", cahaya);
    Firebase.RTDB.setString(&fbdo, "Monitoring_PIR_LDR/Gerakan", gerakan);
    Firebase.RTDB.setString(&fbdo, "Monitoring_PIR_LDR/Lampu", lampu);
    Serial.println("Firebase terkirim");
  } else {
    Serial.println("Firebase belum siap");
  }
}

//========================================================
// Fungsi kontrol relay (real-time di loop)
//========================================================
void kontrolRelay() {
  // Baca sensor
  nilaiLDR = analogRead(LDR_PIN);
  gelap = (nilaiLDR < BATAS_GELAP);
  gerakan = (digitalRead(PIR_PIN) == HIGH);

  // Logika kontrol lampu
  if (!lampuMenyala) {
    if (gelap && gerakan) {
      lampuMenyala = true;
      waktuGerakanTerakhir = millis();
      digitalWrite(RELAY_PIN, RELAY_ON);
    }
  } else {
    if (gerakan) {
      waktuGerakanTerakhir = millis();
    }
    if (millis() - waktuGerakanTerakhir >= waktuTunggu) {
      lampuMenyala = false;
      digitalWrite(RELAY_PIN, RELAY_OFF);
    }
  }

  // Update string status (digunakan oleh tampilan dan cloud)
  statusCahaya = gelap ? "GELAP" : "TERANG";
  statusGerakan = gerakan ? "ADA ORANG" : "KOSONG";
  statusLampu = lampuMenyala ? "ON" : "OFF";
}

//========================================================
// Fungsi update LCD & Serial (real-time, dipanggil di loop)
//========================================================
void updateLCDSerial() {
  // LCD tanpa clear, cukup timpa dengan spasi
  lcd.setCursor(0, 0);
  lcd.print("      "); // 16 spasi
  lcd.setCursor(0, 1);
  lcd.print("         ");
  lcd.setCursor(0, 0);
  lcd.print(statusCahaya);
  lcd.setCursor(0, 1);
  lcd.print(statusGerakan);

  // Serial Monitor
  Serial.print("LDR : "); Serial.print(nilaiLDR);
  Serial.print(" | Cahaya : "); Serial.print(statusCahaya);
  Serial.print(" | PIR : "); Serial.print(statusGerakan);
  Serial.print(" | Lampu : "); Serial.println(statusLampu);
}

//========================================================
// Fungsi kirim ke Blynk & Firebase (dipanggil timer 5 detik)
//========================================================
void kirimCloud() {
  // Blynk
  Blynk.virtualWrite(V0, nilaiLDR);
  Blynk.virtualWrite(V1, gerakan);
  Blynk.virtualWrite(V2, lampuMenyala);
  Blynk.virtualWrite(V3, statusCahaya);
  Blynk.virtualWrite(V4, statusGerakan);
  Blynk.virtualWrite(V5, "MONITORING PIR & LDR");

  // Firebase
  kirimFirebase(nilaiLDR, statusCahaya, statusGerakan, statusLampu);
}

//========================================================
void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 START");

  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Kalibrasi PIR");
  lcd.setCursor(0, 1);
  lcd.print("Tunggu 15 dtk");
  delay(15000);
  lcd.clear();
  lcd.print("Sistem Aktif");
  delay(1000);
  lcd.clear();

  // Blynk
  Serial.println("Blynk Connect");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Blynk OK");

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Anonymous Login OK");
  } else {
    Serial.println("Firebase Login Gagal");
  }
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Timer untuk kirim cloud setiap 5 detik
  timer.setInterval(5000L, kirimCloud);
}

void loop() {
  Blynk.run();
  timer.run();

  // Kontrol relay real-time
  kontrolRelay();

  // Update tampilan LCD & Serial real-time
  if (millis() - lastDisplayUpdate >= 500) {
    lastDisplayUpdate = millis();
    updateLCDSerial();
  }

  yield();
}