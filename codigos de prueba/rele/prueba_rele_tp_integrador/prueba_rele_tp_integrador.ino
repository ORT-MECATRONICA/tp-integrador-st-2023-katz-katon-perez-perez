//katz-Katon-Perez-Perez

#define PIN_RELE 1

void setup() {
  pinMode(PIN_RELE, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  while(Serial.available()){
    if(Serial.read() == 'rele'){
      digitalWrite(PIN_RELE, HIGH);
      delay(500);
      digitalWrite(PIN_RELE, LOW);
    }
  }

}
