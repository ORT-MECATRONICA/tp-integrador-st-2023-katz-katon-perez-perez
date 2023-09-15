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

int VU_TEMP = ;
int VU_HUMEDAD = ;

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

  //verificamos el VU de los sensores
  switch (estadoAlerta) {
    case PREGUNTA_VU:
      if (lecturaHumedad > VU_HUMEDAD || lecturaTemp > VU_TEMP) {
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
      if (lecturaHumedad > VU_HUMEDAD || lecturaTemp < VU_TEMP) {
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
      if (lecturaHumedad < VU_HUMEDAD || lecturaTemp > VU_TEMP) {
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
