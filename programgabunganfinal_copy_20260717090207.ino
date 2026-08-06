#include <Wire.h>
#include <LCD_I2C.h>
#include "DHT.h"

#define DHTPIN 7
#define DHTTYPE DHT11

#define RELAY 4

LCD_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);   // Relay OFF (Active LOW)

  dht.begin();

  lcd.begin();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Monitoring");
  lcd.setCursor(0,1);
  lcd.print("DHT11");
  delay(2000);
  lcd.clear();
}

void loop() {

  float suhu = dht.readTemperature();
  float hum = dht.readHumidity();

  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(suhu);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.setCursor(9,0);
  lcd.print("H:");
  lcd.print(hum);
  lcd.print("%");

  if (suhu > 29.5 && hum < 60) {

    digitalWrite(RELAY, LOW);   // Relay ON

    lcd.setCursor(0,1);
    lcd.print("Relay : ON ");

  }
  else {

    digitalWrite(RELAY, HIGH);  // Relay OFF

    lcd.setCursor(0,1);
    lcd.print("Relay : OFF");

  }

  delay(1000);
}