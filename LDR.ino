#define LDR_PIN 2  // Pino analógico onde o LDR está conectado
#define LDR_THRESHOLD 2000  // Limite para determinar se é dia ou noite

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial
  pinMode(LDR_PIN, INPUT);  // Configura o pino do LDR como entrada
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);  // Lê o valor do LDR
  
  Serial.print("Valor do LDR: ");
  Serial.println(ldrValue);
  
  if (ldrValue < LDR_THRESHOLD) {
    Serial.println("É dia.");
  } else {
    Serial.println("É noite.");
  }

  delay(1000);  // Atraso de 1 segundo entre as leituras
}
