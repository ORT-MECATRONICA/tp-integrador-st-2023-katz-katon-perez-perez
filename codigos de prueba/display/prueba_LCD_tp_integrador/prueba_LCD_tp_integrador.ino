  //katz-Katon-Perez-Perez

//incluir la libreria dentro de display LCD en librerias
#include <LiquidCrystal_I2C.h>
#include <Wire.h> //libreria opcional

//remplazar con los valores del display
#define DISPLAY_HIGHT 2
#define DISPLAY_WIDTH 16

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x3F, DISPLAY_WIDTH, DISPLAY_HIGHT);  

void setup() {
  // Inicializar el LCD
 lcd.init();  REVISAR ESTA LINEA SI ALGO NO FUNCIONA

  //Encender la luz de fondo.
  lcd.backlight();

  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.setCursor(0, 1);
  
  // Escribimos el Mensaje en el LCD.
  lcd.print("Hola Mundo");
} 

void loop() {
  
}
