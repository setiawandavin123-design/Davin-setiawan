#include "DHT.h"
#include <LCD_I2C.h>
#include <Servo.h>
#define DHTPIN 2
#define typeDHT DHT11
#define buzzer 8
#define LDR A0
#define servopin 3

LCD_I2C LCD(0x27, 16,2);
int LED[] = {10,11,12};
#define jumlahLed 3

//variabel
DHT suhu(DHTPIN,typeDHT);
Servo servoku; // objek servo bernama servoku
int sudutawal = 0;

//Function
void Dhtku(){ 
  float kelembapan = suhu.readHumidity();
  float celcius = suhu.readTemperature();
  float farenheit = suhu.readTemperature(true); // true melambangkan farenheit di library
  float celcuisHindex = suhu.computeHeatIndex(celcius,kelembapan,false); // false berarti bukan farenheit
  float farenheitHindex = suhu.computeHeatIndex(farenheit,kelembapan); 
  
  Serial.print(F("nilai suhu = "));
  Serial.println(celcius);
  Serial.print("");
  LCD.setCursor(0, 0);
  LCD.print("suhu");
  LCD.print(celcuisHindex);
    //buzzer
   if ( celcius >= 31) {
     digitalWrite(buzzer, HIGH);
       delay(2000);
        digitalWrite(buzzer, LOW);
        delay(500);
    }else if (celcius < 31) {
        digitalWrite(buzzer, LOW);
    } 
    // motorservo dan LCD
    if (celcuisHindex < 27) {
      LCD.clear();
      delay(1000);
      LCD.setCursor(0, 0);
      LCD.print("suhu ");
      LCD.print("hangat "); 
      LCD.print(celcuisHindex);
      if (kelembapan <20) {
        for (sudutawal = 0; sudutawal <= 180; sudutawal += 1) { 
        servoku.write(sudutawal);             
        delay(15);}
      }
    }else if (celcuisHindex >= 32 && celcuisHindex <=34) {
      LCD.clear();
      delay(1000);
      LCD.setCursor(0, 0);
      LCD.print("suhu ");
      LCD.print("hangat "); 
      LCD.print(celcuisHindex);
      delay(1000);
      
    }else {
      LCD.clear();
      delay(1000);
      LCD.setCursor(0, 0);
      LCD.print("suhu ");
      LCD.print("hangat "); 
      LCD.print(celcuisHindex);
      if (kelembapan > 40) {
       for (sudutawal = 180; sudutawal >= 0; sudutawal -= 1) { 
        servoku.write(sudutawal);              
        delay(15);} 
      }
    }
    
}// fungsi DHT

void LDRku(){
  int nilaiLDR = analogRead(LDR);
  LCD.setCursor(0,1);
  LCD.print("Ldr = ");
  LCD.print(nilaiLDR);
  LCD.print("       ");
  //malam
  if (nilaiLDR <30) {
    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], HIGH);
    digitalWrite(LED[2], HIGH);

  }else if (nilaiLDR>= 30 && nilaiLDR<= 50) {

    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], HIGH);
    digitalWrite(LED[2], LOW);
    
  }else {
    digitalWrite(LED[0], LOW);
    digitalWrite(LED[1], LOW);
    digitalWrite(LED[2], LOW);
   
  }  
 delay(100);
}


void setup() {
  for (int i= 0; i < jumlahLed; i++) {
  pinMode(LED[i], OUTPUT); }
  pinMode(buzzer, OUTPUT);
  servoku.attach(servopin);

  suhu.begin();
  LCD.begin();
  LCD.backlight();
  Serial.begin(9600);
  
  
}
//looping
void loop() {
  Dhtku();
  delay(500);
  LDRku();
  delay(500);
  

}

