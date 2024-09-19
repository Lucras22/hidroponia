  #include <Adafruit_GFX.h>
  #include <Adafruit_TFTLCD.h>
  #include <SPI.h>
  
  // Definição dos pinos para ESP32
  #define TFT_CS   15  // Chip select (CS)
  #define TFT_CD   2   // Command/data pin (RS)
  #define TFT_WR   4   // Write (WR)
  #define TFT_RD   14  // Read (RD)
  #define TFT_RESET 12 // Reset (opcional)
  
  // Inicializando o display TFT
  Adafruit_TFTLCD tft(TFT_CS, TFT_CD, TFT_WR, TFT_RD, TFT_RESET);
  
  void setup() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(1); // Define a rotação da tela (ajustável)
    tft.fillScreen(0xFFFF); // Limpa a tela (branco)
    
    // Teste básico para mostrar um texto
    tft.setTextSize(2);
    tft.setTextColor(0x0000); // Texto em preto
    tft.setCursor(10, 10);
    tft.print("Hello ESP32!");
  }
  
  void loop() {
    // Código para rodar repetidamente
  }


// https://www.youtube.com/watch?v=H5ZdMPacCbU&t=335s
