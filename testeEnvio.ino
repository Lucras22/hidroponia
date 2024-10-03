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
String url = "https://devicesserver.onrender.com/api/devices/666840141508522b72de6983";

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

// Credenciais do Telegram
const String botToken = "7330394567:AAFF6ArePK4t3QbCLtZMHgFFEGsyyWoJSVo";
const String chatId = "7003158288";

// Inicializando sensores
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT22);

// Função para enviar mensagem para o Telegram via POST
void sendMessage(String message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";

        http.begin(url);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Montando os parâmetros da requisição POST
        String postData = "chat_id=" + chatId + "&text=" + message;

        // Enviando a requisição
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

    String message = "Monitoramento Hidropônico\n\n";
    
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

    // Condição do nível de água (baixo, médio, alto)
    if (containerLevel > 23.0) {
        message += "Status do nível de água: Baixo\n";
    } else if (containerLevel <= 23.0 && containerLevel > 11.5) {
        message += "Status do nível de água: Médio\n";
    } else {
        message += "Status do nível de água: Alto\n";
    }

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

    message += "Índice UV: " + String(uv) + " V\n";

    // Leitura do sensor DHT22
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    if (isnan(humidity) || isnan(temperature)) {
        message += "Falha ao ler do sensor DHT!\n";
    } else {
        message += "Umidade do AR: " + String(humidity) + " %\n";
        message += "Temperatura do AR: " + String(temperature) + " °C\n";
    }

    // Leitura do sensor de condutividade da água (TDS)
    int tdsValue = analogRead(TDS_PIN);
    float conductivity = tdsValue * 2;
    message += "Valor PPM: " + String(tdsValue) + "\n";
    message += "Valor ECC: " + String(conductivity) + " \n";

    // Leitura do sensor LDR (luminosidade)
    int luminosity = analogRead(LDR_PIN);
    message += "Intensidade Luminosa: " + String(luminosity) + "\n";

    // Condição do período (dia ou noite)
    if (luminosity < 2000) {
        message += "Período do dia: Dia\n";
    } else {
        message += "Período do dia: Noite\n";
    }

    // Fluxo de água utilizando o estado do botão
    waterFlux = digitalRead(BUTTON_PIN);
    if (waterFlux == LOW) { 
        message += "Há fluxo de água\n";
    } else {
        message += "Não há fluxo de água\n";
    }

    // Geração da string JSON para enviar
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
        
        int httpResponseCode = http.POST(json);

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

    // Enviar mensagem para o Telegram
    sendMessage(message);
    
    delay(10000); // Delay de 10 segundos antes da próxima leitura
}
