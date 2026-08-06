#define touch 7
#define buzzerLed 5
void setup() {
  pinMode(touch, INPUT);
  pinMode(buzzerLed, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int sentuhan = digitalRead(touch);
  if (sentuhan == HIGH) {
    Serial.println("terjadi sentuhan");
    for (int i = 300; i >0; i=i-5) {
      float pangkat = pow(i,-i);
      digitalWrite(buzzerLed, HIGH);
      delay(i);
      digitalWrite(buzzerLed, LOW);
      delay(i/2);}
  }else {
  Serial.println("tidak ada sentuhan");
  digitalWrite(buzzerLed, LOW);
  }
  delay(1000);

}
