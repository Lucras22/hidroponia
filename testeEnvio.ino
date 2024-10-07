#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// Informações da rede Wi-Fi
const char* ssid = "IFCE_DISCENTES";  // Mantenha o SSID que você deseja
const char* password = "ifce@bvg";

// Defina a URL do servidor
String url = "https://devicesserver.onrender.com/api/devices/666840141508522b72de6983"; // Substitua por seu endpoint PUT

// Configurações para o NTP (Time)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // UTC-3 para o Brasil

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

    // Iniciar NTP Client
    timeClient.begin();

    // Inicializando sensores
    sensors.begin();
    dht.begin();

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);
    pinMode(TDS_PIN, INPUT);
    pinMode(UV_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
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

    // Geração da string JSON para enviar
    String json = "{";
    json += "\"espStatus\": true,";
    json += "\"measures\": [";
    json += "{";

    // Leitura da temperatura da água (DS18B20)
    sensors.requestTemperatures();
    float waterTemperature = sensors.getTempCByIndex(0);
    json += "\"waterTemperature\": " + String(waterTemperature) + ",";

    // Leitura do nível de água (HC-SR04)
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    float containerLevel = (duration * 0.0343) / 2;
    json += "\"containerLevel\": " + String(containerLevel) + ",";

    // Leitura do sensor DHT22
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    if (isnan(humidity) || isnan(temperature)) {
        json += "\"humidity\": null,";
        json += "\"temperature\": null,";
    } else {
        json += "\"humidity\": " + String(humidity) + ",";
        json += "\"temperature\": " + String(temperature) + ",";
    }

    // Leitura do sensor de condutividade da água (TDS)
    int tdsValue = analogRead(TDS_PIN);
    float conductivity = tdsValue * 2;
    json += "\"conductivity\": " + String(conductivity) + ",";

    // Leitura do sensor LDR (luminosidade)
    int luminosity = analogRead(LDR_PIN);
    json += "\"luminosity\": " + String(luminosity) + ",";

    // Leitura do índice UV
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
    json += "\"date\": \"" + Date + "\",";
    json += "\"time\": \"" + Time + "\"";
    json += "}";
    json += "]";
    json += "}";

    // Envio dos dados para o servidor
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
