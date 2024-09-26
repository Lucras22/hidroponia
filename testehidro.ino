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

// Credenciais do Telegram
const String botToken = "7330394567:AAFF6ArePK4t3QbCLtZMHgFFEGsyyWoJSVo";
const String chatId = "7003158288";

// Inicializando sensores
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT22);

// Configurações de NTP para obter a hora
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// URL do servidor para envio dos dados
const String url = "http://seu-servidor.com/endpoint";

void sendMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String urlTelegram = "https://api.telegram.org/bot" + botToken + "/sendMessage";
    
    http.begin(urlTelegram);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String postData = "chat_id=" + chatId + "&text=" + message;
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Código de resposta: " + String(httpResponseCode));
      Serial.println("Resposta: " + response);
    } else {
      Serial.print("Erro na requisição: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi desconectado");
  }
}

void sendSensorDataToServer(String jsonData) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.PUT(jsonData);
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Resposta do servidor:");
      Serial.println(payload);
    } else {
      Serial.printf("HTTP PUT falhou, código de erro: %d\n", httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado");
  }
}

void setup() {
  Serial.begin(115200);

  Serial.print("Conectando-se ao Wi-Fi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConectado ao Wi-Fi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  sensors.begin();
  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(TDS_PIN, INPUT);
  pinMode(UV_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  timeClient.begin();
}

void loop() {
  String message = "Monitoramento Hidropônico\n\n";

  // Atualizando o tempo
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  String Date = String(ptm->tm_mday) + "/" + String(ptm->tm_mon + 1) + "/" + String(ptm->tm_year + 1900);
  String Time = String(ptm->tm_hour) + ":" + String(ptm->tm_min) + ":" + String(ptm->tm_sec);

  unsigned long onlineTimeMillis = millis();
  unsigned long onlineTimeMinutes = onlineTimeMillis / 60000;

  // Leitura da temperatura da água (DS18B20)
  sensors.requestTemperatures();
  float waterTemperature = sensors.getTempCByIndex(0);
  message += "Temperatura da água: " + String(waterTemperature) + " °C\n";

  // Leitura do nível de água (HC-SR04)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float containerLevel = (duration * 0.0343) / 2;
  message += "Nível de água: " + String(containerLevel) + " cm\n";

  // Leitura do sensor UV
  int leitura_porta = analogRead(UV_PIN);
  int uv = map(leitura_porta, 0, 1023, 0, 11);
  message += "Índice UV: " + String(uv) + "\n";

  // Leitura do sensor DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    message += "Falha ao ler o sensor DHT!\n";
  } else {
    message += "Umidade do AR: " + String(humidity) + " %\n";
    message += "Temperatura do AR: " + String(temperature) + " °C\n";
  }

  // Leitura do sensor de condutividade da água (TDS)
  int tdsValue = analogRead(TDS_PIN);
  float conductivity = tdsValue * 2;
  message += "Valor PPM: " + String(tdsValue) + "\n";
  message += "Condutividade: " + String(conductivity) + "\n";

  // Leitura do sensor LDR
  int luminosity = analogRead(LDR_PIN);
  message += "Intensidade Luminosa: " + String(luminosity) + "\n";

  // Condição do fluxo de água
  waterFlux = digitalRead(BUTTON_PIN);
  message += (waterFlux == LOW) ? "Há fluxo de água\n" : "Não há fluxo de água\n";

  // Montando JSON para enviar para o servidor
  String jsonData = "{";
  jsonData += "\"date\": \"" + Date + "\",";
  jsonData += "\"time\": \"" + Time + "\",";
  jsonData += "\"onlineTime\": " + String(onlineTimeMinutes) + ",";
  jsonData += "\"temperature\": " + String(temperature) + ",";
  jsonData += "\"humidity\": " + String(humidity) + ",";
  jsonData += "\"waterTemperature\": " + String(waterTemperature) + ",";
  jsonData += "\"containerLevel\": " + String(containerLevel) + ",";
  jsonData += "\"conductivity\": " + String(conductivity) + ",";
  jsonData += "\"luminosity\": " + String(luminosity) + ",";
  jsonData += "\"uvIndex\": " + String(uv) + ",";
  jsonData += "\"waterFlux\": " + String(waterFlux == LOW);
  jsonData += "}";

  Serial.println(message);  // Exibe no monitor serial
  sendMessage(message);  // Envia para o Telegram
  sendSensorDataToServer(jsonData);  // Envia para o servidor

  delay(900000);  // Espera por 15 minutos (900.000 ms)
}
