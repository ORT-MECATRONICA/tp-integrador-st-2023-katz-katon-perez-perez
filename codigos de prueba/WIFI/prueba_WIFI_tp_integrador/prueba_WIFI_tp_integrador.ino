//katz-Katon-Perez-Perez

//incluir la libreria de la carpeta de WIFI dentro de librerias
#include <WiFi.h>

//remplazar con las credenciales de wifi correctas
const char* red = "ORT-IoT"; 
const char* clave = "OrtIOTnew22$2";


void setup() {
  //iniciamos puerto serie y wifi
  Serial.begin (9600);
  Serial.println("Iniciando...");
  WiFi.begin(red , clave);
  Serial.println(WiFi.localIP());
  Serial.println("-------------");

}

void loop() {
   
}
