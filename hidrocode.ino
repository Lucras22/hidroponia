#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

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

int buttonState = 0;

// Credenciais da rede Wi-Fi
const char* ssid = "WIFI LISA";
const char* password = "34Eua7WYBn";

// Credenciais do Telegram
// BOT LUCAS
//const String botToken = "7330394567:AAFF6ArePK4t3QbCLtZMHgFFEGsyyWoJSVo";
//const String chatId = "7003158288";

// BOT RENATO
//const String botToken = "7432114712:AAFNhmK2mkbTUROiS8qbVYknGBc1vRPKVJY";
//const String chatId = "1237176867";

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
  String message = "Monitoramento Hidropônico\n.\n";

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

//De acordo com a leitura define o indice UV corrrspondente
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
    message += "Umidade: " + String(humidity) + " %\n";
    message += "Temperatura do ar: " + String(temperature) + " °C\n";
  }

  // Leitura do sensor de condutividade da água (TDS)
  int tdsValue = analogRead(TDS_PIN);
  float conductivity = (tdsValue * 2;
  message += "TDS Value: " + String(tdsValue) + "PPM\n";
  message += "Tensão: " + String(conductivity) + " ECC\n";

  // Leitura do sensor LDR (luminosidade)
  int luminosity = analogRead(LDR_PIN);
  message += "Valor do LDR: " + String(luminosity) + "\n";

  // Condição do período (dia ou noite)
  bool isDay = (luminosity < 2000);  // Ajuste o valor de corte conforme necessário
  if (isDay) {
    message += "Período do dia: Dia\n";
  } else {
    message += "Período do dia: Noite\n";
  }

  // Fluxo de água utilizando o estado do botão
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) { 
    message += "Há fluxo de água\n";
  } else {
    message += "Não há fluxo de água\n";
  }

  // Exibir e enviar a mensagem
  Serial.println(message);  // Exibe no monitor serial
  sendMessage(message);  // Envia para o Telegram

  delay(10000);  // Envia a cada 10 segundos
}
