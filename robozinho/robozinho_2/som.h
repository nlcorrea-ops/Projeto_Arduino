// =============================================================================================
// som.h — buzzer ativo (só liga e desliga) tocando sequências sem travar o programa
// =============================================================================================
// Cada som é uma lista de durações em ms: liga, desliga, liga, desliga... terminando em 0.
// Um som novo interrompe o que estiver tocando.
#pragma once

const unsigned int SOM_CLIQUE[]  PROGMEM = {25, 0};
const unsigned int SOM_ENTRAR[]  PROGMEM = {50, 40, 80, 0};
const unsigned int SOM_VOLTAR[]  PROGMEM = {80, 0};
const unsigned int SOM_NEGADO[]  PROGMEM = {150, 80, 150, 0};
const unsigned int SOM_ALERTA[]  PROGMEM = {200, 100, 200, 100, 200, 0};
const unsigned int SOM_FESTA[]   PROGMEM = {50, 50, 50, 50, 50, 50, 300, 0};
const unsigned int SOM_TRISTE[]  PROGMEM = {400, 150, 700, 0};
const unsigned int SOM_DORMIR[]  PROGMEM = {300, 200, 150, 250, 60, 0};
const unsigned int SOM_ACORDAR[] PROGMEM = {60, 60, 60, 60, 200, 0};
const unsigned int SOM_FRIO[]    PROGMEM = {40, 40, 40, 40, 40, 40, 40, 0};   // tremendo
const unsigned int SOM_CALOR[]   PROGMEM = {500, 0};                          // "ufa" longo
const unsigned int SOM_AMIGO[]   PROGMEM = {80, 80, 80, 80, 160, 0};
const unsigned int SOM_OI[]      PROGMEM = {60, 60, 120, 0};
const unsigned int SOM_PORTAL[]  PROGMEM = {20, 40, 20, 40, 20, 40, 20, 40, 20, 40, 250, 0};  // brilhinhos

const unsigned int* som_atual = 0;
byte som_idx = 0;
unsigned int som_dur = 0;
unsigned long som_t = 0;

void som_iniciar() {
  pinMode(PINO_BUZZER, OUTPUT);
  digitalWrite(PINO_BUZZER, LOW);
}

void som_tocar(const unsigned int* s) {
  if (!SOM_LIGADO) return;
  som_atual = s; som_idx = 0; som_t = millis();
  som_dur = pgm_read_word(s);
  digitalWrite(PINO_BUZZER, HIGH);
}

void som_parar() {
  som_atual = 0;
  digitalWrite(PINO_BUZZER, LOW);
}

void som_atualizar() {
  if (!som_atual || millis() - som_t < som_dur) return;
  som_t = millis();
  som_idx++;
  som_dur = pgm_read_word(som_atual + som_idx);
  if (som_dur == 0) { som_parar(); return; }
  digitalWrite(PINO_BUZZER, (som_idx % 2 == 0) ? HIGH : LOW);   // posições pares = ligado
}
