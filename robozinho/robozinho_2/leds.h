// =============================================================================================
// leds.h — indicador de humor e saúde com 3 LEDs (verde, amarelo, vermelho)
// =============================================================================================
#pragma once

enum { HUMOR_BEM, HUMOR_ATENCAO, HUMOR_CRITICO, HUMOR_APAGADO };

void leds_iniciar() {
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_AMARELO, OUTPUT);
  pinMode(PINO_LED_VERMELHO, OUTPUT);
}

void leds_mostrar(byte humor) {
  digitalWrite(PINO_LED_VERDE,    humor == HUMOR_BEM     ? HIGH : LOW);
  digitalWrite(PINO_LED_AMARELO,  humor == HUMOR_ATENCAO ? HIGH : LOW);
  digitalWrite(PINO_LED_VERMELHO, humor == HUMOR_CRITICO ? HIGH : LOW);
}
