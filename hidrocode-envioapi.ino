//Codigo feito por Lucas Galindo
//Artigos estudados:




//Bibliotecas usadas
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// conexão da rede Wi-Fi
const char* ssid = "IFCE_DISCENTES"; 
const char* password = "ifce@bvg";

// Defina a URL do servidor
String url = "https://devicesserver.onrender.com/api/devices/665f4fdb8c75c69202d26331"; // Substitua pelo ID da hidroponia que foi criada

// Definindo os nomes dos pinos para os sensores
#define ONE_WIRE_BUS 4
#define TRIG_PIN 13
#define ECHO_PIN 14
#define LDR_PIN 34
#define DHT_PIN 15
#define TDS_PIN 32
#define UV_PIN 33
#define waterFlux_Pin 12
#define RELE_PIN 19

int waterFlux = 0;

// Inicializando sensores
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT22);

void setup() {
    Serial.begin(115200);

    // Conectando ao Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    // Inicializando sensores
    sensors.begin();
    dht.begin();

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);
    pinMode(TDS_PIN, INPUT);
    pinMode(UV_PIN, INPUT);
    pinMode(waterFlux_Pin, INPUT_PULLUP);
}

void loop() {
    // Geração da mensagem em JSON para enviar
    String json = "{";
    json += "\"espStatus\": true,";
    json += "\"measures\": [";
    json += "{";

    // Temperatura da água
    sensors.requestTemperatures();
    float waterTemperature = sensors.getTempCByIndex(0);
    json += "\"waterTemperature\": " + String(waterTemperature) + ",";

    // Nível de água
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    float containerLevel = (duration * 0.0343) / 2;
    json += "\"containerLevel\": " + String(containerLevel) + ",";

    // Temperatura e Umidade
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    if (isnan(humidity) || isnan(temperature)) {
        json += "\"humidity\": null,";
        json += "\"temperature\": null,";
    } else {
        json += "\"humidity\": " + String(humidity) + ",";
        json += "\"temperature\": " + String(temperature) + ",";
    }

    // condutividade da água
    int tdsValue = analogRead(TDS_PIN);
    float conductivity = tdsValue * 2;
    json += "\"conductivity\": " + String(conductivity) + ",";

    // luminosidade
    int luminosity = analogRead(LDR_PIN);
    json += "\"luminosity\": " + String(luminosity) + ",";

    // índice UV
    int leitura_porta = analogRead(UV_PIN);
    int uv;

    // De acordo com a leitura define o indice UV correspondente
    if (leitura_porta <= 10) {
        uv = 0;
    } else if (leitura_porta > 10 && leitura_porta <= 46) {
        uv = 1;
    } else if (leitura_porta > 46 && leitura_porta <= 65) {
        uv = 2;
    } else if (leitura_porta > 65 && leitura_porta <= 83) {
        uv = 3;
    } else if (leitura_porta > 83 && leitura_porta <= 103) {
        uv = 4;
    } else if (leitura_porta > 103 && leitura_porta <= 124) {
        uv = 5;
    } else if (leitura_porta > 124 && leitura_porta <= 142) {
        uv = 6;
    } else if (leitura_porta > 142 && leitura_porta <= 162) {
        uv = 7;
    } else if (leitura_porta > 162 && leitura_porta <= 180) {
        uv = 8;
    } else if (leitura_porta > 180 && leitura_porta <= 200) {
        uv = 9;
    } else if (leitura_porta > 200 && leitura_porta <= 221) {
        uv = 10;
    } else {
        uv = 11;
    }

    json += "\"uv\": " + String(uv) + ",";

  //fluxo de agua + status do motor
    int waterFlux = LOW;
    waterFlux = digitalRead(waterFlux_Pin);

  if (waterFlux == HIGH) {
      json += "\"waterFlux\": " + String(waterFlux) + ",";
      json += "\"engineStatus\": " + String(waterFlux) + ",";
  } else {
    json += "\"waterFlux\": " + String(waterFlux) + ",";
    json += "\"engineStatus\": " + String(waterFlux) + ",";
  }

    json += "}";
    json += "]";
    json += "}";

    // Envio dos dados para a interface web
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        
        int httpResponseCode = http.PUT(json);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Código de resposta do servidor: " + String(httpResponseCode));
            Serial.println("Resposta do servidor: " + response);
        } else {
            Serial.println("Erro ao enviar dados: " + String(httpResponseCode));
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado");
    }

    delay(10000); // Delay de 10 segundos antes da próxima leitura
}
