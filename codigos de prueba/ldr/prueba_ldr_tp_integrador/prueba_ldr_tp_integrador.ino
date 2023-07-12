//katz katopn perez perez
void setup() {
  Serial.begin(9600);
  pinMode(22, INPUT);

}

void loop() {
Serial.println(analogRead(22));

}
