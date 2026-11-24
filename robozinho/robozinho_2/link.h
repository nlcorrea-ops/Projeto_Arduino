// =============================================================================================
// link.h — comunicação serial pelos pinos D2/D3 (SoftwareSerial)
// =============================================================================================
// Os mesmos dois fios servem para uma de duas coisas, escolhida em CONEXAO (config.h):
//   CONEXAO_APP    conversa com o aplicativo pelo módulo Bluetooth HC-06 (veja app.h)
//   CONEXAO_AMIGO  conversa com o outro robozinho (visitas). Opcional: se não for usar,
//                  deixe CONEXAO_APP e nada desta parte roda.
//
// Mensagens em texto, uma por linha:   $<tipo><dados>*<conferência em 2 dígitos hex>\n
// A conferência é o XOR de todos os caracteres entre '$' e '*'; mensagem com erro é descartada.
//
// Tipos entre os robozinhos (CONEXAO_AMIGO):
//   S  estado, enviado a cada LINK_PULSO_MS: <id><humor><dormindo><clima><nome>
//      ex.: $S1001TUBA*xx  (robô 1, humor bem, acordado, frio, nome TUBA)
//   V  convite de visita        A  visita aceita
//   R  visita recusada: <motivo> D = dormindo, O = ocupado
//   F  fim da brincadeira
// Os tipos do aplicativo estão em app.h e em PROTOCOLO.md.
//
// Se nenhuma mensagem chegar por LINK_SUMIU_MS, o amigo é considerado desligado ou
// desconectado e cada robozinho segue funcionando sozinho.
#pragma once

SoftwareSerial porta(PINO_LINK_RX, PINO_LINK_TX);

#define LK_TAM 32
char lk_buf[LK_TAM];
byte lk_n = 0;
bool lk_lendo = false;
unsigned long lk_tUltimo = 0, lk_tPulso = 0;

bool amigoPresente = false;
bool amigoChegou = false, amigoSaiu = false;   // eventos (valem por um ciclo)
bool idRepetido = false;                       // as duas placas estão com o mesmo jumper de ID
char amigoNome[5] = "";
byte amigoHumor = HUMOR_BEM;
bool amigoDormindo = false;
byte amigoClima = CLIMA_NORMAL;
char lk_comando = 0;                           // último V/A/R/F recebido (0 = nenhum)
char lk_motivo = 0;                            // motivo da recusa

// estado próprio que vai em cada mensagem de estado (o programa principal atualiza)
byte lk_meuHumor = HUMOR_BEM;
bool lk_euDormindo = false;
bool lk_ligado = true;                         // false = aparelho desligado, não envia nada

// definidas em app.h
void app_recebe(const char* m);
void app_enviaEstado();

void link_iniciar() {
  porta.begin(LINK_BAUD);
}

byte link_xor(const char* s) {
  byte x = 0;
  while (*s) x ^= (byte)*s++;
  return x;
}

void link_envia(const char* corpo) {           // corpo = tipo + dados
  char hex[3];
  byte x = link_xor(corpo);
  const char* h = "0123456789ABCDEF";
  hex[0] = h[x >> 4]; hex[1] = h[x & 15]; hex[2] = 0;
  porta.write('$'); porta.print(corpo); porta.write('*'); porta.print(hex); porta.write('\n');
}

void link_comando(char tipo, char dado) {
  if (CONEXAO != CONEXAO_AMIGO) return;        // sem amigo ligado: não manda nada
  char c[3] = {tipo, dado, 0};
  link_envia(c);
}

void link_enviaEstado() {
  char c[10];
  c[0] = 'S';
  c[1] = '0' + meuId;
  c[2] = '0' + lk_meuHumor;
  c[3] = lk_euDormindo ? '1' : '0';
  c[4] = '0' + clima;
  strcpy(c + 5, perso->nome);
  link_envia(c);
}

int hexValor(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

// mensagem do outro robozinho
void link_processaAmigo() {
  char tipo = lk_buf[0];
  if (tipo == 'S') {
    if (strlen(lk_buf) < 6) return;
    byte id = lk_buf[1] - '0';
    if (id == meuId) { idRepetido = true; return; }   // eco ou duas placas com o mesmo ID
    idRepetido = false;
    amigoHumor = constrain(lk_buf[2] - '0', 0, 2);
    amigoDormindo = lk_buf[3] == '1';
    amigoClima = constrain(lk_buf[4] - '0', 0, 2);
    strncpy(amigoNome, lk_buf + 5, 4); amigoNome[4] = 0;
  } else if (tipo == 'V' || tipo == 'A' || tipo == 'R' || tipo == 'F') {
    lk_comando = tipo;
    lk_motivo = lk_buf[1];
  } else {
    return;
  }
  lk_tUltimo = millis();
  if (!amigoPresente) { amigoPresente = true; amigoChegou = true; }
}

// confere e interpreta uma mensagem completa (sem o '$' e sem o '\n')
void link_processa() {
  lk_buf[lk_n] = 0;
  char* estrela = strchr(lk_buf, '*');
  if (!estrela || estrela == lk_buf || strlen(estrela) < 3) return;
  int a = hexValor(estrela[1]), b = hexValor(estrela[2]);
  if (a < 0 || b < 0) return;
  *estrela = 0;
  if (link_xor(lk_buf) != (byte)(a * 16 + b)) return;

  if (CONEXAO == CONEXAO_AMIGO) link_processaAmigo();
  else                          app_recebe(lk_buf);
}

void link_atualizar() {
  amigoChegou = amigoSaiu = false;
  while (porta.available()) {
    char c = porta.read();
    if (c == '$') { lk_lendo = true; lk_n = 0; }
    else if (!lk_lendo) continue;
    else if (c == '\n' || c == '\r') { lk_lendo = false; link_processa(); }
    else if (lk_n < LK_TAM - 1) lk_buf[lk_n++] = c;
    else lk_lendo = false;                     // grande demais: descarta
  }
  if (amigoPresente && millis() - lk_tUltimo > LINK_SUMIU_MS) {
    amigoPresente = false; amigoSaiu = true;
  }
  if (lk_ligado && millis() - lk_tPulso >= LINK_PULSO_MS) {
    lk_tPulso = millis();
    if (CONEXAO == CONEXAO_AMIGO) link_enviaEstado();
    else                          app_enviaEstado();
  }
}

// devolve e apaga o último comando recebido do amigo
char link_pegaComando() { char c = lk_comando; lk_comando = 0; return c; }
