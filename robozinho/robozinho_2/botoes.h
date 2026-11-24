// =============================================================================================
// botoes.h — 4 botões com antirrepique, detecção de aperto e de "segurar"
// =============================================================================================
// Botões ligados entre o pino e o GND, usando o pull-up interno (apertado = LOW).
#pragma once

enum { BTN_ESQ, BTN_DIR, BTN_ENTER, BTN_POWER, N_BOTOES };
const byte BTN_PINO[N_BOTOES] = {PINO_BTN_ESQ, PINO_BTN_DIR, PINO_BTN_ENTER, PINO_BTN_POWER};
#define BTN_ANTIRREPIQUE_MS 30

bool btn_estavel[N_BOTOES];             // estado confirmado (true = apertado)
bool btn_lido[N_BOTOES];                // última leitura crua
bool btn_aperto[N_BOTOES];              // true só no ciclo em que o botão foi apertado
bool btn_longoUsado[N_BOTOES];          // o "segurar" já foi usado neste aperto
unsigned long btn_tMuda[N_BOTOES], btn_tAperto[N_BOTOES];

void botoes_iniciar() {
  for (byte i = 0; i < N_BOTOES; i++) {
    pinMode(BTN_PINO[i], INPUT_PULLUP);
    btn_estavel[i] = btn_lido[i] = false;
    btn_aperto[i] = false;
    btn_longoUsado[i] = true;
  }
}

void botoes_ler() {
  unsigned long agora = millis();
  for (byte i = 0; i < N_BOTOES; i++) {
    btn_aperto[i] = false;
    bool r = digitalRead(BTN_PINO[i]) == LOW;
    if (r != btn_lido[i]) { btn_lido[i] = r; btn_tMuda[i] = agora; }
    if (r != btn_estavel[i] && agora - btn_tMuda[i] >= BTN_ANTIRREPIQUE_MS) {
      btn_estavel[i] = r;
      if (r) { btn_aperto[i] = true; btn_tAperto[i] = agora; btn_longoUsado[i] = false; }
    }
  }
}

bool botao_apertou(byte b) { return btn_aperto[b]; }

// true uma única vez quando o botão fica apertado por "ms"
bool botao_segurou(byte b, unsigned long ms) {
  if (btn_estavel[b] && !btn_longoUsado[b] && millis() - btn_tAperto[b] >= ms) {
    btn_longoUsado[b] = true;
    return true;
  }
  return false;
}

bool algum_botao() { return btn_aperto[BTN_ESQ] || btn_aperto[BTN_DIR] || btn_aperto[BTN_ENTER]; }
