# Projeto de Sistema Embarcado para Hidroponia Automatizada

## 1. Introdução
Este sistema embarcado para hidroponia oferece uma solução integrada para monitoramento remoto e controle de um sistema hidropônico. Ao automatizar o processo de irrigação e monitorar as condições ambientais em tempo real, ele permite uma maior eficiência no uso dos recursos, melhora o controle sobre a qualidade da água e da solução nutritiva, além de oferecer maior segurança na manutenção do cultivo. A integração com o Telegram proporciona um controle acessível e em tempo real para o operador, independentemente de sua localização.

## 2. Arquitetura do Sistema Embarcado
O núcleo do projeto é um ESP32, responsável pela aquisição dos dados dos sensores, processamento e envio das informações via rede Wi-Fi para o Telegram. Além disso, o ESP32 também pode controlar atuadores, como bombas de irrigação, via um relé, possibilitando automação do sistema hidropônico com base nos parâmetros monitorados.

## 3. Sensores Utilizados e Suas Funcionalidades
O sistema faz uso de diversos sensores para garantir o monitoramento abrangente do ambiente e da solução nutritiva da hidroponia:

- DS18B20 (Temperatura da Água): Monitora a temperatura da solução nutritiva, um fator crucial para o crescimento saudável das plantas.

- HC-SR04 (Nível de Água): Mede o nível de água no reservatório, garantindo que a solução nutritiva esteja sempre disponível em quantidades adequadas. O sistema classifica o nível como baixo, médio ou alto.

- GUVA-S12SD (Índice UV): Mede a intensidade da radiação ultravioleta, fornecendo informações importantes sobre a quantidade de luz solar que as plantas estão recebendo.

- DHT22 (Temperatura e Umidade do Ar): Mede a umidade e a temperatura do ar, fatores que influenciam diretamente o desenvolvimento das plantas.

- TDS (Sólidos Dissolvidos Totais): Monitora a condutividade elétrica da solução nutritiva, permitindo avaliar a concentração de nutrientes. Essa medição é feita em PPM (partículas por milhão) e condutividade elétrica (EC).

- LDR (Sensor de Luminosidade): Detecta a intensidade da luz ambiente para determinar se é dia ou noite, e esse dado pode ser utilizado para controlar a bomba de irrigação e outras ações automatizadas.

- Sensor de Fluxo de Água: utilizado para monitorar se há ou não fluxo de água no sistema de irrigação automatizado. Esse sensor detecta a passagem da água e converte a rotação das lâminas internas em pulsos que representam a vazão da água. Sempre que a água fluir, o sensor gera sinais de pulso, que indicam a presença de fluxo. Além disso, o sistema também vai monitorar o status do motor de irrigação (controlado por relé). Assim, será possível verificar se o fluxo de água está presente quando o motor está ligado..

## 4. Integração com o Telegram
O sistema utiliza a conectividade Wi-Fi do ESP32 para enviar relatórios periódicos ao Telegram. Esses relatórios contêm dados detalhados sobre o estado dos sensores, incluindo a temperatura da água e do ar, umidade, índice UV, nível de água e status do fluxo de irrigação. O envio é realizado via HTTP utilizando a API do Telegram, permitindo que o usuário receba atualizações diretamente em seu dispositivo móvel a cada 10 segundos (esse intervalo pode ser ajustado conforme a necessidade).

<img src="https://github.com/user-attachments/assets/acc0729b-dbcf-4f8c-99d3-2215f762f5bc" alt="simulador beta" width="250"/>

## 5. Integração Avançada com API via HTTP PUT e Exibição em Tela

O sistema foi aprimorado para realizar requisições HTTP PUT, enviando os dados coletados pelos sensores diretamente para uma API REST. Utilizando a conectividade Wi-Fi do ESP32, o sistema envia as informações em formato JSON, que incluem a temperatura da água, nível de água, temperatura do ar, umidade, condutividade, luminosidade, e o índice UV. Esse avanço permite uma integração mais robusta com sistemas externos, onde os dados são registrados e podem ser analisados de forma centralizada.

<img src="https://github.com/user-attachments/assets/24fb915d-1832-4dff-abc8-0e90ef81bf71" alt="simulador beta" width="500"/>


## 6. Benefícios
- Eficiência no uso de recursos: O monitoramento constante das condições da solução nutritiva e do ambiente reduz o desperdício de água e nutrientes, otimizando o crescimento das plantas.
- Automação: O sistema automatiza o controle da irrigação e pode ser ajustado para tomar decisões com base nos parâmetros ambientais, liberando o operador para outras atividades.
- Monitoramento remoto: O envio de relatórios para o Telegram permite que o operador esteja sempre informado sobre o estado do sistema, podendo tomar decisões rápidas em caso de problemas.
- Sustentabilidade: O uso otimizado da água e dos nutrientes contribui para uma agricultura mais sustentável, o que é especialmente importante em regiões com recursos limitados.
- Este sistema pode ser expandido e adaptado para diferentes tipos de cultivos hidropônicos, representando uma solução versátil e de fácil acesso para pequenos e grandes produtores.


## 7. Sensores e suas portas

Sensor de Temperatura da Água (DS18B20)

Porta: ONE_WIRE_BUS (GPIO 4)

Sensor de Nível de Água (HC-SR04) 

Pino do Trig: TRIG_PIN (GPIO 13)

Pino do Echo: ECHO_PIN (GPIO 14)

Sensor LDR (Luminosidade)

Porta: LDR_PIN (GPIO 34)

Sensor DHT22 (Temperatura e Umidade)

Porta: DHT_PIN (GPIO 15)

Sensor de Condutividade da Água (TDS)

Porta: TDS_PIN (GPIO 32)

Sensor de Índice UV

Porta: UV_PIN (GPIO 33)

Sensor de Fluxo de Agua

Porta: WalterFlux_PIN (GPIO 35)

Relé (para controle da Bompa de agua)

Porta: RELAY_PIN (GPIO 19)

## 8. Bibliotecas Utilizadas
<WiFi.h>: Para conectar o ESP32 à rede Wi-Fi.

<HTTPClient.h>: Para enviar requisições HTTP, neste caso, para enviar mensagens via Telegram.

<OneWire.h>: Para comunicação com o sensor DS18B20 que utiliza o protocolo OneWire.

<DallasTemperature.h>: Biblioteca para facilitar a leitura da temperatura do sensor DS18B20.

"DHT.h": Biblioteca para ler dados do sensor DHT22, que mede temperatura e umidade.


## 9. Alimentação (Componentes e Função)

#### Duas Pilhas de 3.7V/3800mAh:
Ligadas em série: Quando você conecta essas pilhas em série, a tensão total fornecida ao circuito será a soma das tensões individuais, ou seja, 7.4V (3.7V + 3.7V). A capacidade de 3800mAh permanecerá a mesma. Essas baterias alimentam todo o sistema.

#### Regulador de Tensão LM2596 (DC-DC HW-411):
Esse regulador é responsável por pegar a tensão fornecida pelas pilhas (7.4V) e regulá-la para 5V, que é a tensão ideal para a alimentação da ESP32 pelo pino VIN.
- Entrada: O regulador recebe a tensão das duas baterias (7.4V).
- Saída: Ele regula essa tensão para 5V.
- Conexões: O negativo (GND) da saída do regulador está conectado ao GND da ESP32, e o positivo (Vout) está ligado ao pino VIN da ESP32 por meio de um interruptor, permitindo o controle da alimentação (liga/desliga).

#### Placa de Carregamento DDTCCRUB-2S-1A:
Essa placa é um carregador de baterias de lítio 2S (para duas baterias em série). Sua função é gerenciar o carregamento das duas baterias de 3.7V, controlando a tensão e a corrente durante o processo de carregamento, para evitar sobrecargas e proteger as células.
- Entrada: Alimentada via porta USB-C, o que possibilita o carregamento das baterias por meio de um carregador USB padrão.
- Saída: Conecta-se às pilhas para carregá-las diretamente. Durante o processo de carregamento, a placa protege contra sobrecarga e controla o processo automaticamente.
- Conexões: As baterias estão ligadas à placa de carregamento para que possam ser carregadas quando conectadas à fonte de alimentação USB-C.

#### Placa Solar (para ser especificada):
Essa placa solar será conectada ao módulo de carregamento para fornecer energia de carregamento às baterias de lítio. Quando estiver completamente configurada, essa placa solar poderá alimentar o circuito e carregar as baterias de maneira sustentável. No entanto, ainda é necessário definir as características dessa placa solar, como:
- Tensão de saída: 
- Corrente de saída: 
- Potência (Watt): 

## Diagrama Simplificado do Circuito:
* Baterias (7.4V) → Regulador de Tensão (7.4V → 5V) → Interruptor (Liga/Desliga) → ESP32 (pino VIN e GND).
* Baterias (7.4V) ← Placa de Carregamento DDTCCRUB-2S-1A ← Entrada USB-C/Placa Solar.
