//katz-Katon-Perez-Perez

#include <Wire.h>
#include <SPI.h> //Why? Because library supports SPI and I2C connection
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h> //encontrar en el administrador de librerias

//Setup connection of the sensor
Adafruit_BMP280 bmp; // I2C


//Variables
float temperature; 

void setup() {
  bmp.begin();    //Begin the sensor
    Serial.begin(9600); //Begin serial communication at 9600bps
    Serial.println("Adafruit BMP280 test:");
}

void loop() {
  //Read values from the sensor:
  temperature = bmp.readTemperature();
  
  //Print values to serial monitor:
    Serial.print(("Temp: "));
    Serial.print(temperature);
    Serial.print(" oC");
    Serial.println();
}
