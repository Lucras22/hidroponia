// Define o pino ao qual o sensor TDS está conectado
const int tdsPin = 34;  // Alterar conforme o pino de conexão

void setup() {
  Serial.begin(115200);
  pinMode(tdsPin, INPUT);
}

void loop() {
  // Leitura do valor do sensor TDS
  int tdsValue = analogRead(tdsPin);

  // Converte o valor do sensor em milivolts (mV)
  float voltage = (tdsValue / 4095.0) * 3300; // 4095 é o valor máximo de 12 bits, 3300 é a referência de 3.3V
  Serial.print("TDS Value (Raw): ");
  Serial.println(tdsValue);
  Serial.print("Voltage (mV): ");
  Serial.println(voltage);

  // Aqui você pode adicionar uma fórmula para converter a tensão em TDS em ppm se necessário
  // Exemplo: TDS (ppm) = (Voltagem em mV - Valor de calibração) / Fator de conversão

  delay(1000); // Espera 1 segundo antes da próxima leitura
}
