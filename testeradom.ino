#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Informações da rede Wi-Fi
const char* ssid = "WIFI LISA";
const char* password = "34Eua7WYBn";

// Defina a URL do servidor
String url = "https://devicesserver.onrender.com/api/devices/666840141508522b72de6983";

// Configurações para o NTP (Time)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // UTC-3 para o Brasil

void setup() {
  Serial.begin(115200);

  // Conectando ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  // Iniciar NTP Client
  timeClient.begin();
}

void loop() {
  // Atualizando o tempo
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  int year = ptm->tm_year + 1900;
  int month = ptm->tm_mon + 1;
  int day = ptm->tm_mday;
  int hour = ptm->tm_hour;
  int minute = ptm->tm_min;
  int second = ptm->tm_sec;

  String Date = String(day) + "/" + String(month) + "/" + String(year);
  String Time = String(hour) + ":" + String(minute) + ":" + String(second);

  unsigned long onlineTimeMillis = millis();
  unsigned long onlineTimeSeconds = onlineTimeMillis / 60000;

  // Gera dados fictícios dos sensores
  float temperature = random(20, 30);         // Temperatura ambiente fictícia
  float waterTemperature = random(18, 28);    // Temperatura da água fictícia
  float waterFlux = random(0, 10);            // Fluxo de água fictício
  float containerLevel = random(10, 100);     // Nível de água fictício
  float conductivity = random(400, 800);      // Condutividade fictícia
  float humidity = random(40, 80);            // Umidade do ar fictícia
  float luminosity = random(100, 1000);       // Luminosidade fictícia
  float uv = random(0, 10);                   // Índice UV fictício
  float ph = random(6, 8);                    // pH da água fictício
  bool engineStatus = true;                   // Status do motor fictício

  // Criação da string JSON para enviar
  String json = "{";
  json += "\"espStatus\": true,";
  json += "\"measures\": [";
  json += "{";
  json += "\"temperature\": " + String(temperature) + ",";
  json += "\"waterTemperature\": " + String(waterTemperature) + ",";
  json += "\"waterFlux\": " + String(waterFlux) + ",";
  json += "\"containerLevel\": " + String(containerLevel) + ",";
  json += "\"conductivity\": " + String(conductivity) + ",";
  json += "\"humidity\": " + String(humidity) + ",";
  json += "\"luminosity\": " + String(luminosity) + ",";
  json += "\"ph\": " + String(ph) + ",";
  json += "\"uv\": " + String(uv) + ",";
  json += "\"hour\": \"" + Time + "\",";
  json += "\"day\": \"" + Date + "\",";
  json += "\"onlineTime\": " + String(onlineTimeSeconds) + ",";
  json += "\"engineStatus\": " + String(engineStatus ? "true" : "false");
  json += "}";
  json += "]}";

  // Envio dos dados ao servidor
  HTTPClient http;
  Serial.println("Enviando dados para o servidor...");
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.PUT(json); // Use o método PUT
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Resposta do servidor:");
    Serial.println(payload);
  } else {
    Serial.printf("Falha ao enviar requisição, código de erro: %d\n", httpCode);
  }
  http.end();

  // Aguardar 5 segundos antes da próxima atualização
  delay(5000);
}
