//katz-Katon-Perez-Perez
void setup() {
  Serial.begin(9600);
  pinMode(23, OUTPUT);

}

void loop() {
digitalWrite(23, HIGH);
Serial.println("buzzer on");
delay(1000);
digitalWrite(23, LOW);
delay(1000);
Serial.println("buzzer off");
}
