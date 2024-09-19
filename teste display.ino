#include <Adafruit_GFX.h>    // Biblioteca para gráficos no TFT
#include <Adafruit_TFTLCD.h> // Biblioteca para o display TFT
#include <SPI.h>

// Definição dos pinos para ESP32 (ajuste conforme sua conexão)
#define TFT_CS   15  // Chip select
#define TFT_CD   2   // Command/data pin (CD)
#define TFT_WR   4   // Write pin (WR)
#define TFT_RD   16  // Read pin (RD)
#define TFT_RESET 17 // Reset pin (opcional)

// Inicializando o display TFT
Adafruit_TFTLCD tft(TFT_CS, TFT_CD, TFT_WR, TFT_RD, TFT_RESET);

// Função para exibir dados no display
void mostrarDados() {
  // Limpar a tela e definir a cor de fundo
  tft.fillScreen(0x0000); // Preto
  
  // Definir o tamanho e a cor do texto
  tft.setTextSize(2);
  tft.setTextColor(0xFFFF); // Texto em branco
  
  // Exibir o título
  tft.setCursor(10, 10);
  tft.println("Monitoramento Hidrop.");

  // Exibir as informações de monitoramento
  tft.setCursor(10, 40);
  tft.println("Temp. Agua: 29.31 C");

  tft.setCursor(10, 70);
  tft.println("Nivel Agua: 32.77 cm");
  
  tft.setCursor(10, 100);
  tft.println("Status: Baixo");
  
  tft.setCursor(10, 130);
  tft.println("Indice UV: 0 V");
  
  tft.setCursor(10, 160);
  tft.println("Umidade: 46.80 %");
  
  tft.setCursor(10, 190);
  tft.println("Temp. Ar: 29.00 C");
  
  tft.setCursor(10, 220);
  tft.println("TDS Value: 0 PPM");
  
  tft.setCursor(10, 250);
  tft.println("Tensao: 0.00 ECC");
  
  tft.setCursor(10, 280);
  tft.println("Valor LDR: 0");
  
  tft.setCursor(10, 310);
  tft.println("Periodo: Dia");
  
  tft.setCursor(10, 340);
  tft.println("Fluxo: Nao ha agua");
}

void setup() {
  Serial.begin(115200);
  
  // Inicializar o display
  tft.begin();
  
  // Configurar a rotação para que o display funcione de cima para baixo (em pé)
  tft.setRotation(1); // 0, 1, 2, 3 alteram a rotação. Teste para ver qual se ajusta melhor.
  
  // Mostrar os dados na tela
  mostrarDados();
}

void loop() {
  // Aqui você pode adicionar a lógica de atualização dos sensores se necessário
  delay(10000); // Atualizar a cada 10 segundos (por exemplo)
}
