#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// Definindo os pinos para os sensores
#define ONE_WIRE_BUS 4
#define TRIG_PIN 13
#define ECHO_PIN 14
#define LDR_PIN 34
#define DHT_PIN 15
#define TDS_PIN 32
#define UV_PIN 33
#define BUTTON_PIN 12

#define RELAY_PIN 19

int waterFlux = 0;

// Credenciais da rede Wi-Fi
const char* ssid = "IFCE_DISCENTES";
const char* password = "ifce@bvg";

// URL para enviar os dados
const String serverUrl = "https://devicesserver.onrender.com/api/devices/666840141508522b72de6983";

// Inicializando sensores
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT22);

// Inicializando NTP para obter a data e hora
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800); // Fuso horário de Brasília (-3h)

// Função para enviar dados via PUT
void sendDataToServer(float temperature, float humidity, float waterTemperature, float containerLevel, float uv, float conductivity, int luminosity, unsigned long onlineTimeSeconds) {
  timeClient.update();
  
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  String Date = String(ptm->tm_mday) + "/" + String(ptm->tm_mon + 1) + "/" + String(ptm->tm_year + 1900);
  String Time = String(ptm->tm_hour) + ":" + String(ptm->tm_min) + ":" + String(ptm->tm_sec);

  HTTPClient http;
  Serial.print("Enviando dados para URL: ");
  Serial.println(serverUrl);
  http.begin(serverUrl);

  String json = "{";
  json += "\"espStatus\": true,";
  json += "\"measures\": [";
  json += "{";
  json += "\"temperature\": " + String(temperature) + ",";
  json += "\"humidity\": " + String(humidity) + ",";
  json += "\"waterTemperature\": " + String(waterTemperature) + ",";
  json += "\"containerLevel\": " + String(containerLevel) + ",";
  json += "\"uv\": " + String(uv) + ",";
  json += "\"conductivity\": " + String(conductivity) + ",";
  json += "\"luminosity\": " + String(luminosity) + ",";
  json += "\"onlineTime\": " + String(onlineTimeSeconds) + ",";
  json += "\"hour\": \"" + Time + "\",";
  json += "\"day\": \"" + Date + "\"";
  json += "}";
  json += "]}";

  http.addHeader("Content-Type", "application/json");

  int httpCode = http.PUT(json);
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Resposta:");
    Serial.println(payload);
  } else {
    Serial.printf("HTTP PUT falhou, código de erro: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);

  // Conectar ao Wi-Fi
  Serial.print("Conectando-se ao Wi-Fi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado ao Wi-Fi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicializando sensores e NTP
  sensors.begin();
  dht.begin();
  timeClient.begin();
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(TDS_PIN, INPUT);
  pinMode(UV_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Leitura dos sensores
  sensors.requestTemperatures();
  float waterTemperature = sensors.getTempCByIndex(0);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Leitura de outros sensores (nível de água, UV, TDS, luminosidade, etc.)
  // Simulação dos valores para fins de exemplo
  float containerLevel = 20.0;
  float uv = 5.0;
  float conductivity = 300.0;
  int luminosity = analogRead(LDR_PIN);

  // Tempo de operação do ESP32
  unsigned long onlineTimeMillis = millis();
  unsigned long onlineTimeSeconds = onlineTimeMillis / 60000;

  // Enviar dados para o servidor
  sendDataToServer(temperature, humidity, waterTemperature, containerLevel, uv, conductivity, luminosity, onlineTimeSeconds);

  delay(900000);  // Espera 15 minutos antes de enviar novamente
}
