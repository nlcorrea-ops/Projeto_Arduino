// =============================================================================================
// sensores.h — temperatura (NTC), luminosidade (LDR) e distância (HC-SR04)
// =============================================================================================
// Cada sensor é lido no seu próprio intervalo, sem delay(). Os resultados ficam em variáveis
// globais e os "eventos" (mudou o clima, escureceu, alguém chegou) valem por um ciclo do loop.
#pragma once

enum { CLIMA_NORMAL, CLIMA_FRIO, CLIMA_CALOR };

// --- temperatura ---
float temperatura = 25.0;               // °C, filtrada
bool  tempFalha = false;                // NTC desconectado ou em curto
bool  tempPrimeira = true;
byte  clima = CLIMA_NORMAL;
bool  climaMudou = false;
unsigned long sn_tTemp = 0;

// --- luz ---
int   luz = 1023;                       // 0 = escuro, 1023 = claro (filtrado)
bool  escuro = false;                   // estado confirmado
bool  escuroMudou = false;
unsigned long sn_tLuz = 0, sn_tLuzCandidato = 0;
bool  luzCandidata = false;             // há uma mudança esperando confirmação

// --- distância ---
int   distancia = 999;                  // cm; 999 = nada à frente
bool  pessoaPerto = false;
bool  pessoaChegou = false;
byte  sn_pertoSeguidas = 0;
unsigned long sn_tDist = 0;

void sensores_iniciar() {
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  digitalWrite(PINO_TRIG, LOW);
}

// converte a leitura do divisor (5V - R fixo - A0 - NTC - GND) em °C pela equação beta
float ntc_celsius(int adc) {
  float r = NTC_R_FIXO * adc / (1023.0 - adc);
  float invT = 1.0 / 298.15 + log(r / NTC_R_25) / NTC_BETA;
  return 1.0 / invT - 273.15;
}

void sensores_temperatura() {
  int adc = analogRead(PINO_NTC);
  tempFalha = (adc < 5 || adc > 1018);
  if (tempFalha) {                                     // sem leitura confiável: clima neutro
    if (clima != CLIMA_NORMAL) { clima = CLIMA_NORMAL; climaMudou = true; }
    tempPrimeira = true;
    return;
  }
  float t = ntc_celsius(adc);
  temperatura = tempPrimeira ? t : temperatura * 0.8 + t * 0.2;
  tempPrimeira = false;

  byte novo = clima;
  if (clima == CLIMA_NORMAL) {
    if (temperatura <= TEMP_FRIO) novo = CLIMA_FRIO;
    else if (temperatura >= TEMP_CALOR) novo = CLIMA_CALOR;
  } else if (clima == CLIMA_FRIO) {
    if (temperatura > TEMP_FRIO + TEMP_HISTERESE) novo = CLIMA_NORMAL;
  } else {
    if (temperatura < TEMP_CALOR - TEMP_HISTERESE) novo = CLIMA_NORMAL;
  }
  if (novo != clima) { clima = novo; climaMudou = true; }
}

void sensores_luz() {
  int l = analogRead(PINO_LDR);
  luz = (luz * 3 + l) / 4;
  bool pedeMudanca = escuro ? (luz > LDR_CLARO) : (luz < LDR_ESCURO);
  if (!pedeMudanca) { luzCandidata = false; return; }
  if (!luzCandidata) { luzCandidata = true; sn_tLuzCandidato = millis(); return; }
  if (millis() - sn_tLuzCandidato >= LDR_CONFIRMA_MS) {
    escuro = !escuro; escuroMudou = true; luzCandidata = false;
  }
}

// HC-SR04: espera o eco no máximo 12 ms (~2 m) para não atrasar as animações
void sensores_distancia() {
  digitalWrite(PINO_TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);
  unsigned long us = pulseIn(PINO_ECHO, HIGH, 12000UL);
  distancia = us ? (int)(us / 58) : 999;              // 58 us por cm (ida e volta)

  bool perto = distancia <= PRESENCA_CM;               // precisa de 2 leituras seguidas
  if (perto) { if (sn_pertoSeguidas < 2) sn_pertoSeguidas++; } else sn_pertoSeguidas = 0;
  bool confirmado = sn_pertoSeguidas >= 2;
  if (confirmado && !pessoaPerto) pessoaChegou = true;
  pessoaPerto = confirmado;
}

// temperatura arredondada como texto, ex.: "25°C" (ou "??C" se o NTC estiver desconectado)
void temperaturaTexto(char* s) {
  if (tempFalha) { strcpy_P(s, PSTR("??C")); return; }
  int tc = (int)(temperatura + (temperatura >= 0 ? 0.5 : -0.5));
  if (tc < 0) { *s++ = '-'; tc = -tc; }
  s = numero(s, tc, 1);
  *s++ = (char)0xDF; *s++ = 'C'; *s = 0;   // 0xDF é o "°" do LCD
}

// distância simulada para MODO_DEMO_SENSOR: aproxima em 8 s, fica perto 6 s, afasta em 6 s
int distanciaDemo() {
  unsigned long t = millis() % 20000;
  if (t < 8000)  return 120 - (int)(t * 110 / 8000);
  if (t < 14000) return 10;
  return 10 + (int)((t - 14000) * 110 / 6000);
}

void sensores_atualizar() {
  climaMudou = escuroMudou = pessoaChegou = false;
  unsigned long agora = millis();
  if (agora - sn_tTemp >= 1000) { sn_tTemp = agora; sensores_temperatura(); }
  if (agora - sn_tLuz  >= 200)  { sn_tLuz  = agora; sensores_luz(); }
  if (agora - sn_tDist >= 200)  { sn_tDist = agora; sensores_distancia(); }
}
