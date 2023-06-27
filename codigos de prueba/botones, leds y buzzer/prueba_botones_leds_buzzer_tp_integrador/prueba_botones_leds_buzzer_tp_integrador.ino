//katz-Katon-Perez-Perez

#define PIN_BOTON_1 1
#define PIN_BOTON_2 1
#define PIN_BOTON_3 1
#define PIN_BOTON_4 1
#define PIN_BOTON_5 1
#define PIN_LED_1 1
#define PIN_LED_2 1
#define PIN_LED_3 1
#define PIN_BUZZER 1

String lectura;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BOTON_1, INPUT_PULLUP);
  pinMode(PIN_BOTON_2, INPUT_PULLUP);
  pinMode(PIN_BOTON_3, INPUT_PULLUP);
  pinMode(PIN_BOTON_4, INPUT_PULLUP);
  pinMode(PIN_BOTON_5, INPUT_PULLUP);

  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);

  pinMode(PIN_BUZZER, OUTPUT);

}

void loop() { 
  
    void serial_input();
    void button_input();
}

void serial_input(){
  lectura = Serial.read();
  if (lectura == 'buzzer'){
    digitalWrite(PIN_BUZZER, HIGH);
    Serial.println("buzzer on");
    delay(1000);
    digitalWrite(PIN_BUZZER, LOW);
    Serial.println("buzzer off");
  } else if (lectura == 'led'){
    digitalWrite(PIN_LED_1, HIGH);
    digitalWrite(PIN_LED_2, HIGH);
    digitalWrite(PIN_LED_3, HIGH);
    Serial.println("leds on");
    delay(1000);
    digitalWrite(PIN_LED_1, LOW);
    digitalWrite(PIN_LED_2, LOW);
    digitalWrite(PIN_LED_3, LOW);
    Serial.println("leds off");
  }
}
void button_input(){
    while (digitalRead(PIN_BOTON_1) == 0){
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_LED_1, HIGH);
    digitalWrite(PIN_LED_2, HIGH);
    digitalWrite(PIN_LED_3, HIGH);
    Serial.println("buzzer on");
    Serial.println("leds on");
  }
    while (digitalRead(PIN_BOTON_2) == 0){
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_LED_1, HIGH);
    digitalWrite(PIN_LED_2, HIGH);
    digitalWrite(PIN_LED_3, HIGH);
    Serial.println("buzzer on");
    Serial.println("leds on");
  }
    while (digitalRead(PIN_BOTON_3) == 0){
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_LED_1, HIGH);
    digitalWrite(PIN_LED_2, HIGH);
    digitalWrite(PIN_LED_3, HIGH);
    Serial.println("buzzer on");
    Serial.println("leds on");
  }
    while (digitalRead(PIN_BOTON_4) == 0){
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_LED_1, HIGH);
    digitalWrite(PIN_LED_2, HIGH);
    digitalWrite(PIN_LED_3, HIGH);
    Serial.println("buzzer on");
    Serial.println("leds on");
  }
    while (digitalRead(PIN_BOTON_5) == 0){
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_LED_1, HIGH);
    digitalWrite(PIN_LED_2, HIGH);
    digitalWrite(PIN_LED_3, HIGH);
    Serial.println("buzzer on");
    Serial.println("leds on");
  }
}
