// =============================================================================================
// config.h — pinagem e todos os ajustes do robozinho
// =============================================================================================
// Os dois robozinhos usam exatamente o mesmo programa. O que diferencia um do outro é o
// jumper de identificação no pino A3 (veja PINO_ID).
#pragma once

// ---------------------------------------------------------------------------------------------
// COMUNICAÇÃO: com o app (HC-06) ou com o outro robozinho
// ---------------------------------------------------------------------------------------------
// As duas usam os mesmos pinos D2/D3, então só uma pode estar ligada por vez.
//   CONEXAO_APP   = HC-06 ligado no D2/D3, conversa com o aplicativo (padrão)
//   CONEXAO_AMIGO = fios cruzados até a outra placa, ativa o item VISITAR do menu
// Com CONEXAO_APP o robozinho funciona normalmente mesmo sem o app conectado.
#define CONEXAO_APP    1
#define CONEXAO_AMIGO  2
#define CONEXAO        CONEXAO_APP

// ---------------------------------------------------------------------------------------------
// PINAGEM (Arduino Uno R3)
// ---------------------------------------------------------------------------------------------
//  D0, D1  livres: são a USB (gravação do programa e monitor serial)
//  D2      RX da comunicação  <- HC-06 TXD   (ou D3 da outra placa, com CONEXAO_AMIGO)
//  D3      TX da comunicação  -> HC-06 RXD pelo divisor 1k/2k (o HC-06 recebe 3,3 V)
//          (ou D2 da outra placa, com CONEXAO_AMIGO; as placas precisam ter o GND em comum)
//  D4      HC-SR04 TRIG
//  D5      HC-SR04 ECHO
//  D6      buzzer ativo (+)     (- no GND)
//  D7      botão POWER          (outro terminal no GND)
//  D8      botão SETA ESQUERDA  (outro terminal no GND)
//  D9      botão SETA DIREITA   (outro terminal no GND)
//  D10     botão ENTER          (outro terminal no GND)
//  D11     LED verde    (com resistor de 220 ohms até o GND)
//  D12     LED amarelo  (com resistor de 220 ohms até o GND)
//  D13     LED vermelho (com resistor de 220 ohms até o GND)
//  A0      NTC 10k: divisor 5V -- resistor 10k -- A0 -- NTC -- GND
//  A1      LDR:     divisor 5V -- LDR -- A1 -- resistor 10k -- GND
//  A2      desconectado (gera a semente dos números aleatórios)
//  A3      identificação: solto = robô 1 | jumper para o GND = robô 2
//  A4      LCD I2C SDA
//  A5      LCD I2C SCL
#define PINO_LINK_RX       2
#define PINO_LINK_TX       3
#define PINO_TRIG          4
#define PINO_ECHO          5
#define PINO_BUZZER        6
#define PINO_BTN_POWER     7
#define PINO_BTN_ESQ       8
#define PINO_BTN_DIR       9
#define PINO_BTN_ENTER     10
#define PINO_LED_VERDE     11
#define PINO_LED_AMARELO   12
#define PINO_LED_VERMELHO  13
#define PINO_NTC           A0
#define PINO_LDR           A1
#define PINO_SEMENTE       A2
#define PINO_ID            A3

// ---------------------------------------------------------------------------------------------
// LCD
// ---------------------------------------------------------------------------------------------
#define LCD_ENDERECO  0x27       // endereço I2C (os mais comuns são 0x27 e 0x3F)
#define LCD_COLS      20
#define LCD_LINHAS    4

// ---------------------------------------------------------------------------------------------
// TEMPERATURA (NTC 10k)
// ---------------------------------------------------------------------------------------------
#define NTC_R_FIXO      10000.0  // resistor do divisor (ohms)
#define NTC_R_25        10000.0  // resistência do NTC a 25 °C (ohms)
#define NTC_BETA        3950.0   // coeficiente beta do NTC (veja a ficha do componente)
#define TEMP_FRIO       18.0     // °C: igual ou abaixo disso, cenário de FRIO
#define TEMP_CALOR      30.0     // °C: igual ou acima disso, cenário de CALOR
#define TEMP_HISTERESE  1.0      // °C a mais/menos para sair do cenário (evita ficar trocando)

// ---------------------------------------------------------------------------------------------
// LUMINOSIDADE (LDR) — valores de 0 (escuro) a 1023 (claro)
// ---------------------------------------------------------------------------------------------
#define LDR_ESCURO       300     // abaixo disso por LDR_CONFIRMA_MS: ficou escuro (dorme)
#define LDR_CLARO        450     // acima disso por LDR_CONFIRMA_MS: clareou (acorda)
#define LDR_CONFIRMA_MS  5000UL  // tempo que a luz precisa ficar estável para valer

// ---------------------------------------------------------------------------------------------
// PRESENÇA (HC-SR04)
// ---------------------------------------------------------------------------------------------
#define PRESENCA_CM       60     // pessoa a esta distância ou menos = alguém perto
#define PRESENCA_PAUSA_MS 60000UL // intervalo mínimo entre duas reações de "OI!"
#define MODO_DEMO_SENSOR  false  // true = cena SENSOR usa distância simulada (sem o HC-SR04)

// limites da cena SENSOR (bichinho de frente), em cm
#define DIST_PEQUENO  80
#define DIST_MEDIO    60
#define DIST_GRANDE   42
#define DIST_CLOSE    25
#define DIST_CORACAO  15

// ---------------------------------------------------------------------------------------------
// NECESSIDADES (Tamagotchi) — cada necessidade vai de 0 (péssimo) a 100 (ótimo)
// ---------------------------------------------------------------------------------------------
#define TICK_MS            30000UL // de quanto em quanto tempo as necessidades mudam
#define NIVEL_BOM          60      // daqui para cima: bem
#define NIVEL_CRITICO      30      // abaixo disso: crítico
#define ENERGIA_RECUPERA   4       // energia ganha por tick dormindo
#define ENERGIA_ACORDA     90      // dormindo de cansaço, acorda quando chega aqui
#define GANHO_COMER        35      // fome recuperada ao comer
#define GANHO_LANCHE       10      // felicidade extra quando come uma comida comprada no app
#define GANHO_BRINCAR      15      // felicidade ganha no pega-pega
#define GANHO_VISITA       25      // felicidade ganha brincando com o outro robozinho
#define GANHO_PRESENCA     2       // felicidade ganha quando alguém se aproxima
#define CUSTO_BRINCAR      5       // energia gasta numa brincadeira

// ---------------------------------------------------------------------------------------------
// ESTUDO (Pomodoro e cronômetro, iniciados pelo app)
// ---------------------------------------------------------------------------------------------
#define POMODORO_FOCO_MIN   25     // minutos de foco
#define POMODORO_PAUSA_MIN  5      // minutos de pausa automática entre os focos
#define ESTUDO_TEMPO_MS     6000UL // tempo mostrando o relógio antes de mostrar o rostinho
#define ESTUDO_ROSTO_MS     3000UL // tempo mostrando o rostinho

// ---------------------------------------------------------------------------------------------
// VIDAS E MEMÓRIA
// ---------------------------------------------------------------------------------------------
#define VIDAS_MAX            3
#define TICKS_RECUPERA_VIDA  120   // ticks seguidos com tudo bem para ganhar 1 vida (120 x 30 s = 1 h)
#define SALVAR_A_CADA_MS     600000UL // grava na EEPROM no máximo a cada 10 min (se mudou algo)
#define EEPROM_ENDERECO      0

// ---------------------------------------------------------------------------------------------
// SOM, MENU E COMUNICAÇÃO
// ---------------------------------------------------------------------------------------------
#define SOM_LIGADO         true
#define ALERTA_REPETE_MS   120000UL // repete o aviso sonoro de atenção a cada 2 min
#define MENU_VOLTA_MS      20000UL  // sem apertar nada, o menu volta para a casa
#define POWER_SEGURAR_MS   1000UL   // segurar POWER por este tempo para desligar
#define NOVO_PET_SEGURAR_MS 3000UL  // segurar ENTER no fim de jogo para começar de novo

#define LINK_BAUD          9600     // o HC-06 vem de fábrica em 9600
#define LINK_PULSO_MS      1000UL   // envia o próprio estado a cada 1 s
#define LINK_SUMIU_MS      3500UL   // sem notícias por este tempo = amigo desligado/desconectado
#define LINK_RESPOSTA_MS   3000UL   // espera pela resposta de um convite de visita
#define LINK_REENVIO_MS    500UL    // reenvia o convite enquanto espera
