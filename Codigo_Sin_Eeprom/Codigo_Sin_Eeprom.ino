//katz-perez-perez-katon
//INCLUIMOS LIBRERIAS
#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h> //libreria general adafruit
#include <Adafruit_BMP280.h> //libreria sensor temperatura
#include <LiquidCrystal_I2C.h> //libreria del display
#include <WiFi.h>
#include <WiFiManager.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP32Time.h>
#include "time.h"

//wifi sin web server
//const char* ssid = "REPLACE_WITH_YOUR_SSID";
//const char* password = "REPLACE_WITH_YOUR_PASSWORD";


//CONFIGURACION TELEGRAM
#define BOTtoken "6524164842:AAGCe368_vFVZWBZ6amrgBrFOzF2XhA8YYY"  //Bot Token
#define CHAT_ID "5388508527" //Remplazar por tu chat id
WiFiClientSecure client; //Revisar linea si el WIFI no funciona
UniversalTelegramBot bot(BOTtoken, client); //cambiar linea por las credenciales wifi (esto por no usar wificlientsecure)
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//CONFIGURACION DE LA HORA
void pedir_lahora(void);
void setup_rtc_ntp(void);
struct tm timeinfo;
ESP32Time rtc;
/// time
long unsigned int timestamp; // hora
long gmtOffset_sec = -10800;
const int daylightOffset_sec = 0;
const char *servidor = "south-america.pool.ntp.org";


//DEFINIMOS PINES
#define PIN_HUMEDAD  //pin sensor humedad

#define PIN_SCL_TEMP //pin scl sensor de temperatura
#define PIN_SDA_TEMP  //pin sda sensor temperatura
Adafruit_BMP280 bmp; // definimos el id I2C del sensor de temp

#define DISPLAY_HIGHT 2
#define DISPLAY_WIDTH 16
LiquidCrystal_I2C lcd(0x3F, DISPLAY_WIDTH, DISPLAY_HIGHT);  // definimos el id I2C del sensor del display

#define PIN_RELE
#define PIN_LDR //pin sensor de luz
#define PIN_BUZZER
#define PIN_BOTON_1
#define PIN_BOTON_2
#define PIN_BOTON_3
#define PIN_BOTON_4
#define PIN_BOTON_5
#define PIN_LED_ROJO
#define PIN_LED_AMA
#define PIN_LED_VERDE 

bool estadoBoton1 = false;
bool estadoBoton2 = false;
bool estadoBoton3 = false;}

//Máquina de Estados
#define PANTALLA_INICIAL 1 
#define CHECK_BOTON 2   
#define AJUSTAR_TEMPERATURA 3
#define AJUSTAR_HUMEDAD 4
#define AJUSTAR_GMT 5

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  


//Estado inicial de la máquina
int estado = 1;  

//Variables
int temperaturaUmbral = 0;
int humedadUmbral = 0;
int gmt = 0;
int pantalla = 1;


//DEFINO ESTADOS DE LA MAQUINA DE ALERTA
int aestadoAlerta = 0;
#define PREGUNTA_VU 0
#define ALARMA 1
#define AVISO 2
#define CHEQUEO_BTN_5 3

void setup() {
  Serial.begin(9600); //inicializo puerto serie
  bmp.begin(); //inicializo I2C del sensor de temperatura
  //lcd.init(); // Inicializar el LCD     REVISAR SI ALGO NO FUNCIONA CON EL DISPLAY
  lcd.backlight(); //Encender la luz de fondo
  pinMode(PIN_RELE, OUTPUT); //configuramos el pin del rele
  pinMode(PIN_LDR, INPUT); //configuramos el pin del ldr
  pinMode(PIN_BUZZER, OUTPUT); //configuramos el pin del buzzer
  pinMode(PIN_BOTON_1, INPUT);
  pinMode(PIN_BOTON_2, INPUT);
  pinMode(PIN_BOTON_3, INPUT);
  pinMode(PIN_BOTON_4, INPUT);
  pinMode(PIN_BOTON_5, INPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_AMA, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);

  digitalWrite(PIN_LED_VERDE, HIGH); //INICIAMOS CON EL LED VERDE ENCENDIDO

  // initialize LCD
  lcd.begin();
  // turn on LCD backlight                      
  lcd.backlight();

  //SETEO WIFI MANAGER
  WiFi.mode(WIFI_STA); //seteamos el modo del wifi
  WiFiManager wm; //creamos un objeto WM (wifi manager)
  bool respuesta;
  res = wm.autoConnect("WiFi_server_HuertaIoT", "12345"); // password protected ap
  if (!respuesta) {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else {
    //if you get here you have connected to the WiFi
    Serial.println("connected");
  }

  setup_rtc_ntp(); //igualo el rtc a la hora del servidor
  //initWiFi(); descomentar si no funciona el wifi manager
}


void loop() {
  pedirHora(); //le pido la hora al servidor o al rtc              //gmtOffset_sec = gmtOffset_sec - 3600;  setup_rtc_ntp();  colocar estas lineas para cambiar el valor GMT en config
  //println(&timeinfo, "%H:%M:%S");  Forma de imprimir solo hora minuto segundo


  //leemos los 3 sensores
  int lecturaHumedad = analogRead(PIN_HUMEDAD);
  float lecturaTemp = bmp.readTemperature();
  int lecturaLdr = analogRead(PIN_LDR);

  //preguntamos por nuevos mensajes de telegram
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages, lecturaHumedad, lecturaTemp, lecturaLdr);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }


  //Maquina de estados de las pantallas

  switch(estado)
  {
    //Pantalla que muestra hora y temperatura
    case PANTALLA_INICIAL: 
   
{      
    Serial.println("PANTALLA INICIAL"); 
    delay(2000);
    pedir_lahora();
    
    //IMPRIMIR VALORES EN EL DISPLAY

  lcd.setCursor(0, 0);
  lcd.print("Temp:");

  lcd.setCursor(5,0);
  lcd.print(lecturaTemp);
  
  lcd.setCursor(9,0);
  lcd.print("Hum:");
  
  lcd.setCursor(13,0);
  lcd.print(lecturaHumedad);

  lcd.setCursor(0,1);
  lcd.print("LED:");

  lcd.setCursor(4,1);
  lcd.print(LecturaLdr);

  lcd.setCursor(9,1);
  lcd.print("Al:");

  if(estadoAlerta == ALARMA) {
  lcd.setCursor(12,1);
  lcd.print("ON");
  }
  else {
  lcd.setCursor(12,1);
  lcd.print("OFF");
  }
     
    //IMPRIMIR VALORES POR PUERTO SERIE
    Serial.println(lecturaTemp); 
    Serial.println(lecturaHumedad); 
    Serial.println(LecturaLdr);  
   
      if(digitalRead(PIN_BOTON_1) == LOW)
      {
        estado = CHECK_BOTON;
      }
      
}
    break;

    case CHECK_BOTON: 
    { 
      Serial.println("CHECK BOTON"); 

      //DISPLAY A TEMPERATURA
      if(digitalRead(PIN_BOTON_1) == HIGH && estadoBoton1 == false)
      {
        lcd.clear(); 
        estadoBoton1 = true;
        estado = AJUSTAR_TEMPERATURA;
      }

      //CUALQUIERA A DISPLAY
      if(digitalRead(PIN_BOTON_1) == HIGH && estadoBoton1 == true)
      {
        lcd.clear(); 
        estadoBoton1 = false;
        estado = PANTALLA_INICIAL;
      }

      //TEMPERATURA A HUMEDAD
      if(digitalRead(PIN_BOTON_4) == HIGH && pantalla == 2)
      {
        lcd.clear(); 
        estado = AJUSTAR_HUMEDAD;
      }

      //HUMEDAD A GMT
      if(digitalRead(PIN_BOTON_4) == HIGH && pantalla == 3)
      {
        lcd.clear(); 
        estado = AJUSTAR_GMT;
      }

      //GMT A TEMPERATURA
      if(digitalRead(PIN_BOTON_4) == HIGH && pantalla == 1)
      {
        lcd.clear(); 
        estado = AJUSTAR_TEMPERATURA;
      }

    }
    break;

    //Pantalla que permite cambiar la temperatura
    case AJUSTAR_TEMPERATURA: 
    { 

    Serial.println("AJUSTAR TEMPERATURA");
    Serial.println(temperaturaUmbral); 

    lcd.setCursor(0, 0);
    lcd.print("Temp Umbral:");

    lcd.setCursor(0,1);
    lcd.print(temperaturaUmbral);
    
      //SUMA
      if(digitalRead(PIN_BOTON_2) == LOW) 
      {
        estadoBoton2 = true;
        Serial.println("BOTON_PRESIONADO");
      }
     
      if(digitalRead(PIN_BOTON_2) == HIGH && estadoBoton2 == true) 
      {
        estadoBoton2 = false;
        Serial.println("BOTON_SUELTO");
        temperaturaUmbral = temperaturaUmbral++;
        Serial.println(temperaturaUmbral);  
        
      }    

      //RESTA
      if(digitalRead(PIN_BOTON_3) == LOW)
      {
        estadoBoton3 = true;
        Serial.println("BOTON_PRESIONADO");
      }
      if(digitalRead(PIN_BOTON_3) == HIGH && estadoBoton3 == true)
      {
        estadoBoton3 = false;
        Serial.println("BOTON_SUELTO");
        temperaturaUmbral = temperaturaUmbral--;
        Serial.println(temperaturaUmbral);  
      }    
      
      //TEMPERATURA A DISPLAY
      if(digitalRead(PIN_BOTON_1) == LOW && estadoBoton1 == true) 
      {
        estado = CHECK_BOTON;
      }

      //TEMPERATURA A HUMEDAD
      if(digitalRead(PIN_BOTON_4) == LOW && pantalla == 1) 
      {
        pantalla = 2;
        estado = CHECK_BOTON;
      }
    }
    break;

    case AJUSTAR_HUMEDAD: 
    { 

    Serial.println("AJUSTAR HUMEDAD");
    Serial.println(humedadUmbral); 

     
    lcd.setCursor(0, 0);
    lcd.print("Hum Umbral:");

    lcd.setCursor(0,1);
    lcd.print(HumedadUmbral);
    
      //SUMA
      if(digitalRead(PIN_BOTON_2) == LOW) 
      {
        estadoBoton2 = true;
        Serial.println("BOTON_PRESIONADO");
      }
     
      if(digitalRead(PIN_BOTON_2) == HIGH && estadoBoton2 == true) 
      {
        estadoBoton2 = false;
        Serial.println("BOTON_SUELTO");
        humedadUmbral = humedadUmbral++;
        Serial.println(humedadUmbral);  
        
      }    

      //RESTA
      if(digitalRead(PIN_BOTON_3) == LOW)
      {
        estadoBoton3 = true;
        Serial.println("BOTON_PRESIONADO");
      }
      if(digitalRead(PIN_BOTON_3) == HIGH && estadoBoton3 == true)
      {
        estadoBoton3 = false;
        Serial.println("BOTON_SUELTO");
        humedadUmbral = humedadUmbral--;
        Serial.println(humedadUmbral);  
      }    
      
      //HUMEDAD A DISPLAY
      if(digitalRead(PIN_BOTON_1) == LOW && estadoBoton1 == true) 
      {
        estado = CHECK_BOTON;
      }

      //HUMEDAD A GMT
      if(digitalRead(PIN_BOTON_4) == LOW && pantalla == 2) 
      {
        pantalla = 3;
        estado = CHECK_BOTON;
      }
    }
    break;

case AJUSTAR_GMT: 
    { 

    Serial.println("AJUSTAR GMT");
    Serial.println(gmt);

    
    lcd.setCursor(0, 0);
    lcd.print("GMT:");

    lcd.setCursor(0,1);
    lcd.print(gmt);
    
      //SUMA
      if(digitalRead(PIN_BOTON_2) == LOW) 
      {
        estadoBoton2 = true;
        Serial.println("BOTON_PRESIONADO");
      }
     
      if(digitalRead(PIN_BOTON_2) == HIGH && estadoBoton2 == true) 
      {
        estadoBoton2 = false;
        Serial.println("BOTON_SUELTO");
        gmt = gmt++;
        Serial.println(gmt);  
        
      }    

      //RESTA
      if(digitalRead(PIN_BOTON_3) == LOW)
      {
        estadoBoton3 = true;
        Serial.println("BOTON_PRESIONADO");
      }
      if(digitalRead(PIN_BOTON_3) == HIGH && estadoBoton3 == true)
      {
        estadoBoton3 = false;
        Serial.println("BOTON_SUELTO");
        gmt = gmt--;
        Serial.println(gmt);  
      }    
      
      //GMT A DISPLAY
      if(digitalRead(PIN_BOTON_1) == LOW && estadoBoton1 == true) 
      {
        estado = CHECK_BOTON;
      }

      //GMT A TEMPERATURA
      if(digitalRead(PIN_BOTON_4) == LOW && pantalla == 3) 
      {
        pantalla = 1;
        estado = CHECK_BOTON;
      }
    }
    break;

}

  //verificamos el VU de los sensores
  switch (estadoAlerta) {
    case PREGUNTA_VU:
      if (lecturaHumedad > humedadUmbral || lecturaTemp > temperaturaUmbral) {
        estadoAlerta = ALARMA;
        alertaTelegram();
        digitalWrite(PIN_LED_ROJO, HIGH);
        digitalWrite(PIN_LED_VERDE, LOW);
        digitalWrite(PIN_BUZZER, HIGH);
        digitalWrite(PIN_RELE, HIGH);
        flagAlerta = 0;
      }
      break;
    case ALARMA:
      if (lecturaHumedad > humedadUmbral || lecturaTemp < temperaturaUmbral) {
        estadoAlerta = AVISO;
        digitalWrite(PIN_LED_ROJO, LOW);
        digitalWrite(PIN_LED_AMA, HIGH);
        digitalWrite(PIN_BUZZER, LOW);
        digitalWrite(PIN_RELE, LOW);
        flagAlerta = 1;
      }
      if (digitalRead(PIN_BTN_5) == LOW) {
        estadoAlerta = CHEQUEO_BTN_5;
      }
      break;
    case AVISO:
      if (lecturaHumedad < humedadUmbral || lecturaTemp > temperaturaUmbral) {
        estadoAlerta = ALARMA;
        alertaTelegram();
        digitalWrite(PIN_LED_ROJO, HIGH);
        digitalWrite(PIN_LED_AMA, LOW);
        digitalWrite(PIN_BUZZER, HIGH);
        digitalWrite(PIN_RELE, HIGH);
        flagAlerta = 0;
      }
      if (digitalRead(PIN_BTN_5) == LOW) {
        estadoAlerta = CHEQUEO_BTN_5;
        digitalWrite(PIN_LED_AMA, LOW);
      }
      break;
    case CHEQUEO_BTN_5:
      if (flagAlerta == 0) {
        if (digitalRead(PIN_BTN_5) == HIGH) {
          estadoAlerta = ALARMA;
          digitalWrite(PIN_BUZZER, LOW);
        }
      } else if (flagAlerta = 1) {
        if (digitalRead(PIN_BTN_5) == HIGH) {
          estadoAlerta = PREGUNTA_VU;
          digitalWrite(PIN_LED_AMA, LOW);
          digitalWrite(PIN_LED_VERDE, HIGH);
        }
      }
      break;
  }
}


void alertaTelegram() {
  String mensajeAlerta = "ALERTA!!!\n"
                         mensajeAlerta += "ALERTA!!!\n";
  mensajeAlerta += "ALERTA!!!\n\n";
  mensajeAlerta += "VALORES HUMBRALES SUPERADOS\n";
  mensajeAlerta += "REVISAR HUERTA HURGENTE\n";
  mensajeAlerta += "RIESGO TERMINAL PARA LA HUERTA\n\n";
  mensajeAlerta += "Revise los valores de los sensores en la pantalla o mediante el comando /lectura_sensores\n";
  mensajeAlerta += "si la humedad es muy baja se debe regar la huerta.\n";
  mensajeAlerta += "si la temperatura es muy alta verifique si necesita agua y revise si el cooler esta encendido.\n";
  botSendMessage(chat_id, mensajeAlerta, "");
}

void handleNewMessages(int numNewMessages, int lecturaHumedad, float lecturaTemp, int lecturaLdr) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Hola, " + from_name + ".\n";
      welcome += "Use estos comandos para solicitar informacion.\n\n";
      welcome += "/lectura_sensores para recibir la ultima lectura de cada sensor \n";
      welcome += "/sensor_humedad para recibir la lectura del sensor de humedad\n";
      welcome += "/sensor_temperatura para recibir la lectura del sensor de temperatura\n";
      welcome += "/sensor_luz para recibir la lectura del sensor de luz\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/lectura_sensores") {
      bot.sendMessage(chat_id, "valor sensor humedad =");
      bot.sendMessage(chat_id, lecturaHumedad, "\n");
      bot.sendMessage(chat_id, "valor sensor temperatura =");
      bot.sendMessage(chat_id, lecturaTemp, "\n");
      bot.sendMessage(chat_id, "valor sensor luz =");
      bot.sendMessage(chat_id, lecturaLdr, "\n");
    }

    if (text == "/sensor_humedad") {
      bot.sendMessage(chat_id, "valor sensor humedad =");
      bot.sendMessage(chat_id, lecturaHumedad, "\n");
    }

    if (text == "/sensor_temperatura") {
      bot.sendMessage(chat_id, "valor sensor temperatura =");
      bot.sendMessage(chat_id, lecturaTemp, "\n");
    }

    if (text == "/sensor_luz") {
      bot.sendMessage(chat_id, "valor sensor luz =");
      bot.sendMessage(chat_id, lecturaLdr, "\n");
    }
  }
}

void setup_rtc_ntp()
{
  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, servidor);
  timestamp = time(NULL);
  rtc.setTime(timestamp + gmtOffset_sec);
}

void pedirHora()
{
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("veo la hora del rtc interno ");
    timestamp = rtc.getEpoch() - gmtOffset_sec;
    timeinfo = rtc.getTimeStruct();
    Serial.println(&timeinfo, "%H:%M:%S");
  }
  else
  {
    Serial.print("NTP Time:");
    timestamp = time(NULL);
    Serial.println(&timeinfo, "%H:%M:%S");
  }
}

void initWiFi() {
  WiFi.begin(ssid , password);
  Serial.println(WiFi.localIP());
  Serial.println();
}
