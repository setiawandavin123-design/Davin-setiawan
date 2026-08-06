#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <LCD_I2C.h>
#include <Wire.h>

#define trig D5
#define echo D6
#define led D7
#define DHTPIN D4
#define buzzer D8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LCD_I2C LCD(0x27, 16,2);

//=======================
// WiFi
//=======================
const char* ssid = "BPVP-SIDOARJO";
const char* password = "bpvpsda2024";

//=======================
// Google Apps Script URL
//=======================
String GAS_URL = "https://script.google.com/macros/s/AKfycbwozLaM5CkLtDmVSuU0Xag11BzOPsdWw2q1JxMNACo02ypmdsIptnNKSUjQ8xLs-buk/exec";

// HTTPS Client
WiFiClientSecure client;

//=======================
float bacaJarak()
{
  digitalWrite(trig,LOW);
  delayMicroseconds(2);

  digitalWrite(trig,HIGH);
  delayMicroseconds(10);

  digitalWrite(trig,LOW);

  long durasi=pulseIn(echo,HIGH);

  float jarak=durasi*0.0343/2;

  return jarak;
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  LCD.begin();
  LCD.backlight();
  dht.begin();
  WiFi.begin(ssid, password);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  LCD.print("Menghubungkan WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());

  // Mengabaikan pengecekan sertifikat SSL
  client.setInsecure();

  Serial.println("System Ready");
  LCD.clear();
  LCD.print("WiFi Connected");

  delay(1000);
  LCD.clear();
}

void loop()
{
  float nilaijarak = bacaJarak();
  float suhu = dht.readTemperature();
  float hum  = dht.readHumidity();
  String status;
  if (nilaijarak<10) {
    status= "awas";
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, HIGH);
    }else if (nilaijarak>= 10 && nilaijarak<=30) {
    status = "hati-hati";
    digitalWrite(led,HIGH);
    digitalWrite(buzzer, LOW);
    }else {
   status = "aman";
   digitalWrite(led,LOW);
   digitalWrite(buzzer, LOW);
  }

  if (isnan(suhu) || isnan(hum))
  {
    Serial.println("Sensor DHT Error");
    delay(2000);
    return;
  }
  LCD.setCursor(0,0);
  LCD.print("Jarak:");
  LCD.print(nilaijarak);
  LCD.print("cm ");

  LCD.setCursor(0,1);
  LCD.print(status);
  LCD.print("      ");

  Serial.print("Jarak : ");
  Serial.println(nilaijarak);

  Serial.println(status);


  Serial.println("======================");
  Serial.print("Suhu     : ");
  Serial.print(suhu);
  Serial.println(" C");

  Serial.print("Humidity : ");
  Serial.print(hum);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String url = GAS_URL;
    url += "?suhu=" + String(suhu,1);
    url += "&hum=" + String(hum,1);
    url+="&jarak="+String(nilaijarak,1);
    url+="&status="+ status;
    Serial.println();
    Serial.print("Mengirim ke : ");
    Serial.println(url);

    if (http.begin(client, url))
    {
      int httpCode = http.GET();

      if (httpCode > 0)
      {
        Serial.print("HTTP Code : ");
        Serial.println(httpCode);

        String respon = http.getString();

        Serial.println("Respon Server:");
        Serial.println(respon);
      }
      else
      {
        Serial.print("HTTP Error : ");
        Serial.println(http.errorToString(httpCode));
      }

      http.end();
    }
    else
    {
      Serial.println("http.begin() gagal");
    }
  }
  else
  {
    Serial.println("WiFi Terputus");
  }

  delay(10000);
}