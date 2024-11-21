//Codigo feito por Lucas Galindo

//Estudos dos sensores:

/*conversão de valores UV: https://www.usinainfo.com.br/blog/projeto-sensor-uv-guva-s12sd-com-arduino/
Sensor TDS: https://www.usinainfo.com.br/blog/projeto-medidor-de-tds-arduino-para-condutividade-da-agua/
Sensor LDR: https://www.usinainfo.com.br/ldr/ldr-sensor-de-luminosidade-5mm-2982.html?search_query=LDR&results=24
Sensor Temp_Agua: https://www.usinainfo.com.br/blog/sensor-de-temperatura-arduino-ds18b20-comunicacao-onewire/
Sensor DHT22: https://www.usinainfo.com.br/sensor-de-umidade-arduino/sensor-de-umidade-e-temperatura-dht22-am2302-original-40-a-80c-2813.html?search_query=dht&results=20
Sensor Ultra_Sonico: https://www.usinainfo.com.br/blog/projeto-sensor-ultrassonico-hc-sr04-com-arduino/
Sensor Fluxo: https://www.usinainfo.com.br/blog/sensor-de-fluxo-de-agua-para-arduino-1-30-lmin/
*/

//NTP Cliente (Estudos):
// https://curtocircuito.com.br/blog/Categoria%20IoT/protocolo-ntp-para-ESP32


//Bibliotecas usadas
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// Credenciais da rede Wi-Fi LISA
const char* ssid = "WIFI LISA";
const char* password = "34Eua7WYBn";

// conexão da rede Wi-Fi DISCENTES
// const char* ssid = "IFCE_DISCENTES"; 
// const char* password = "ifce@bvg";

// conexão da rede Wi-Fi TAUA
// const char* ssid = "Festival Gastronomico Privado"; 
// const char* password = "gastronomia10";

// teste
// Configurações para o NTP (Time)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // UTC-3 para o Brasil



// Defina a URL do servidor
String url = "https://devicesserver.onrender.com/api/devices/673ddc26eb9737c99228c69b"; // Substitua pelo ID da hidroponia que foi criada

// Definindo os nomes dos pinos para os sensores
#define ONE_WIRE_BUS 4
#define TRIG_PIN 13
#define ECHO_PIN 14
#define LDR_PIN 34
#define DHT_PIN 15
#define TDS_PIN 32
#define UV_PIN 33
#define waterFlux_Pin 35
#define relayPin  19
#define LED_BUILTIN 2

int waterFlux = 1;

// Inicializando sensores
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT22);


// Constantes para os tempos de ligar e desligar (em milissegundos)
const unsigned long intervalOn = 1000 * 60; //15 * 60 * 1000;  // 15 minutos
const unsigned long intervalOff = 1000 * 60; //15 * 60 * 1000; // 15 minutos

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
    pinMode(waterFlux_Pin, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);


    // Inicia a tarefa para o controle do relé
    xTaskCreatePinnedToCore(
        relayTask,        // Função que executa a tarefa
        "Relay Control",  // Nome da tarefa
        1000,             // Tamanho da pilha (em palavras)
        NULL,             // Parâmetro para passar à tarefa (não utilizado aqui)
        1,                // Prioridade da tarefa
        NULL,             // Handle da tarefa (não utilizado aqui)
        1                 // Executar no núcleo 1 (para separar do loop)
    );
}

// Função da tarefa para controlar o relé
void relayTask(void *parameter) {
    bool relayState = false; // Estado inicial do relé
    unsigned long previousMillis = millis();

    while (true) {
        unsigned long currentMillis = millis();

        if (relayState && (currentMillis - previousMillis >= intervalOn)) {
            // Desliga o relé após 15 minutos ligado
            relayState = false;
            digitalWrite(relayPin, HIGH);
            digitalWrite(LED_BUILTIN, LOW);
            previousMillis = currentMillis;
        } else if (!relayState && (currentMillis - previousMillis >= intervalOff)) {
            // Liga o relé após 15 minutos desligado
            relayState = true;
            digitalWrite(relayPin, LOW);
            digitalWrite(LED_BUILTIN, HIGH);
            previousMillis = currentMillis;
        }

        // Delay para evitar loop desnecessário, ajustável para precisão
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
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

    // Geração da mensagem em JSON para enviar
    String json = "{";
    json += "\"espStatus\": true,";
    json += "\"measures\": [";
    json += "{";

    // Temperatura da água
    sensors.requestTemperatures();
    float waterTemperature = sensors.getTempCByIndex(0);
    json += "\"waterTemperature\": " + String(waterTemperature) + ",";


    // Strings para os estados de nível
    String Baixo = "Baixo";
    String Medio = "Medio";
    String Alto = "Alto";
    
    // Nível de água
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    float nivel = (duration * 0.0343) / 2;

  // Condição do nível de água
  if (nivel > 23.0) {
    json += "\"containerLevel\": \"" + Baixo + "\",";
  } else if (nivel <= 23.0 && nivel > 11.5) {
    json += "\"containerLevel\": \"" + Medio + "\",";
  } else {
    json += "\"containerLevel\": \"" + Alto + "\",";
  }

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
    int waterFlux = digitalRead(waterFlux_Pin);
    bool engineStatus = (waterFlux == LOW);
    json += "\"waterFlux\": " + String(waterFlux) + ",";
    json += "\"engineStatus\": " + String(engineStatus ? "false" : "true") + ",";

    json += "\"ph\": null,";  // sem sensor de pH

    json += "\"date\": \"" + Date + "\",";
    json += "\"time\": \"" + Time + "\"";

    json += "}";
    json += "]}";


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
