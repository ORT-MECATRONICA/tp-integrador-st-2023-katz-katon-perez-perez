//katz-Katon-Perez-Perez

#define PIN_HUMEDAD 15
//V1.0 Soil_Humidity_Tester  Enjoy!
void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(PIN_HUMEDAD);
  
  //  Prints the condition of soil.  Dry, Wet or Perfect
  if (sensorValue >= 1000)  (Serial.print("SOIL IS TOO DRY!!!!!    "));
  else if ((sensorValue <= 999)  && (sensorValue >=901)) (Serial.print("SOIL IS PERFECT!!!!!    "));
  else  if (sensorValue <= 900) (Serial.print("SOIL IS TOO WET!!!!!    "));
  else;
  
  // print out the value you read:
  Serial.print("Marijuana Soil Humidity  is: ");
  Serial.println(sensorValue);
  delay(500);        // delay in between  reads for stability



}
