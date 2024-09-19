#include <TFT_eSPI.h> // Biblioteca para displays TFT com SPI

// Inicializando o display TFT
TFT_eSPI tft = TFT_eSPI(); // Inicializa o display com a configuração no User_Setup.h

// Função para exibir dados no display
void mostrarDados() {
  // Limpar a tela e definir a cor de fundo
  tft.fillScreen(TFT_BLACK); // Preto
  
  // Definir o tamanho e a cor do texto
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE); // Texto em branco
  
  // Exibir o título
  tft.setCursor(10, 10);
  tft.println("Monitoramento Hidrop.");

  // Exibir as informações de monitoramento
  tft.setCursor(10, 40);
  tft.printf("Temp. Agua: %.2f C", 29.31);

  tft.setCursor(10, 70);
  tft.printf("Nivel Agua: %.2f cm", 32.77);
  
  tft.setCursor(10, 100);
  tft.printf("Status: Baixo");
  
  tft.setCursor(10, 130);
  tft.printf("Indice UV: %.2f V", 0.0);
  
  tft.setCursor(10, 160);
  tft.printf("Umidade: %.2f %%", 46.80);
  
  tft.setCursor(10, 190);
  tft.printf("Temp. Ar: %.2f C", 29.00);
  
  tft.setCursor(10, 220);
  tft.printf("TDS Value: %d PPM", 0);
  
  tft.setCursor(10, 250);
  tft.printf("Tensao: %.2f ECC", 0.00);
  
  tft.setCursor(10, 280);
  tft.printf("Valor LDR: %d", 0);
  
  tft.setCursor(10, 310);
  tft.printf("Periodo: Dia");
  
  tft.setCursor(10, 340);
  tft.printf("Fluxo: Nao ha agua");
}

void setup() {
  Serial.begin(115200);
  
  // Inicializar o display
  tft.init();
  
  // Configurar a rotação para que o display funcione de cima para baixo (em pé)
  tft.setRotation(1); // 0, 1, 2, 3 alteram a rotação. Teste para ver qual se ajusta melhor.
  
  // Mostrar os dados na tela
  mostrarDados();
}

void loop() {
  // Atualizar a tela a cada 10 segundos com novos dados (simulados ou reais)
  delay(10000); // 10 segundos
  mostrarDados(); // Atualizar os dados mostrados na tela
}
