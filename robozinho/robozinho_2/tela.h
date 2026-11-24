// =============================================================================================
// tela.h — LCD, tela virtual, caracteres personalizados e desenhos compartilhados
// =============================================================================================
// Cada cena escreve na matriz tela[][] e chama desenha(), que envia ao LCD só o que mudou.
// O LCD guarda 8 caracteres personalizados (slots 0 a 7); cada cena carrega os seus.
// Todos os desenhos ficam na memória flash (PROGMEM) para economizar RAM.
#pragma once

LiquidCrystal_I2C lcd(LCD_ENDERECO, LCD_COLS, LCD_LINHAS);

byte tela[LCD_LINHAS][LCD_COLS];         // código do caractere de cada posição
byte telaAnt[LCD_LINHAS][LCD_COLS];      // o que já está no LCD
bool telaToda = true;                    // true = reenvia a tela inteira no próximo desenha()
int  linhaForcada = -1;                  // linha reenviada inteira neste quadro (-1 = nenhuma)

// Efeito de clima desenhado por cima das cenas, só nas posições vazias:
// 0 = nenhum | 1 = neve caindo (frio) | 2 = solzinho no canto (calor)
byte climaSobreposto = 0;

void limpaTela() { memset(tela, ' ', sizeof(tela)); }

void poe(int l, int c, byte cod) {
  if (l >= 0 && l < LCD_LINHAS && c >= 0 && c < LCD_COLS) tela[l][c] = cod;
}

// escreve um texto guardado na flash: use sempre a macro TXT(linha, coluna, "texto")
void texto_P(byte l, byte c, const char* s) {
  char ch;
  while ((ch = pgm_read_byte(s++)) != 0) poe(l, c++, ch);
}
#define TXT(l, c, s) texto_P((l), (c), PSTR(s))

void texto(byte l, byte c, const char* s) { while (*s) poe(l, c++, *s++); }   // texto da RAM

// texto da RAM centralizado na linha
void textoCentro(byte l, const char* s) {
  int n = strlen(s);
  texto(l, n >= LCD_COLS ? 0 : (LCD_COLS - n) / 2, s);
}

// texto da flash centralizado na linha
void textoCentro_P(byte l, const char* s) {
  int n = strlen_P(s);
  texto_P(l, n >= LCD_COLS ? 0 : (LCD_COLS - n) / 2, s);
}

// escreve o número v em s com pelo menos "dig" dígitos (completa com zeros à esquerda) e
// devolve onde o texto terminou. Substitui o sprintf, que ocupa muita memória no Uno.
char* numero(char* s, unsigned int v, byte dig) {
  char t[6]; byte n = 0;
  do { t[n++] = '0' + v % 10; v /= 10; } while (v && n < 5);
  while (n < dig) t[n++] = '0';
  while (n) *s++ = t[--n];
  *s = 0;
  return s;
}

void aplicaClima() {
  if (climaSobreposto == 1) {                          // 5 flocos descendo em colunas fixas
    unsigned long q = millis() / 600;
    for (byte i = 0; i < 5; i++) {
      byte c = (i * 7 + 3) % LCD_COLS;
      byte l = (q + i * 2) % (LCD_LINHAS - 1);
      if (tela[l][c] == ' ') tela[l][c] = (i % 2) ? '.' : '*';
    }
  } else if (climaSobreposto == 2) {                   // sol com raios piscando
    byte c = LCD_COLS - 2;
    bool raio = (millis() / 500) % 2;
    if (tela[0][c] == ' ') tela[0][c] = 'O';
    if (raio && tela[0][c - 1] == ' ') tela[0][c - 1] = '-';
    if (raio && tela[1][c] == ' ')     tela[1][c] = '|';
  }
}

// envia ao LCD só o que mudou; reposiciona o cursor apenas quando pula posições
void desenha() {
  aplicaClima();
  for (byte l = 0; l < LCD_LINHAS; l++) {
    int ultima = -2;
    for (byte c = 0; c < LCD_COLS; c++) {
      if (!telaToda && l != linhaForcada && tela[l][c] == telaAnt[l][c]) continue;
      if (c != ultima + 1) lcd.setCursor(c, l);
      lcd.write(tela[l][c]);
      telaAnt[l][c] = tela[l][c];
      ultima = c;
    }
  }
  telaToda = false;
}

// desenha reenviando uma linha inteira por quadro (corrige caracteres corrompidos por ruído)
void desenhaQuadro(unsigned int passo) {
  linhaForcada = passo % LCD_LINHAS;
  desenha();
  linhaForcada = -1;
}

// ---------------------------------------------------------------------------------------------
// Caracteres personalizados
// ---------------------------------------------------------------------------------------------
byte espelha(byte b) {                   // inverte esquerda <-> direita (5 pixels)
  byte r = 0;
  for (byte i = 0; i < 5; i++) if (b & (1 << i)) r |= 1 << (4 - i);
  return r;
}

void criaChar(byte slot, const byte* p, bool esp) {   // p aponta para 8 bytes em PROGMEM
  byte t[8];
  for (byte i = 0; i < 8; i++) {
    byte b = pgm_read_byte(p + i);
    t[i] = esp ? espelha(b) : b;
  }
  lcd.createChar(slot, t);
}

// ---------------------------------------------------------------------------------------------
// TUBARÃO (6 colunas x 2 linhas) — slots 0 a 6
//   0 = barbatana | 1 = cauda (2 quadros) | 2 a 6 = corpo e cabeça. Desenhado para a direita.
// ---------------------------------------------------------------------------------------------
const byte T_BARBATANA[8]  PROGMEM = {B00000,B00000,B10000,B11000,B10100,B10010,B10010,B10001};
const byte T_CAUDA_A[8]    PROGMEM = {B11000,B01110,B01001,B01000,B01001,B01110,B11000,B00000};
const byte T_CAUDA_B[8]    PROGMEM = {B00000,B01110,B00101,B00100,B00101,B01110,B00000,B00000};
const byte T_CORPO1[8]     PROGMEM = {B00011,B00100,B11001,B00001,B00000,B10000,B01111,B00000};
const byte T_CORPO2[8]     PROGMEM = {B00001,B00000,B01000,B01000,B00000,B00000,B11111,B00000};
const byte T_CORPO_MEIO[8] PROGMEM = {B11111,B00000,B00000,B00000,B00000,B00000,B11111,B00000};
const byte T_CORPO3[8]     PROGMEM = {B11000,B00110,B00001,B01000,B01000,B00000,B11111,B00000};
const byte T_CORPO4[8]     PROGMEM = {B00000,B00000,B00000,B10000,B01000,B01000,B10000,B00000};
const byte T_CORPO3_SONO[8]PROGMEM = {B11000,B00110,B00001,B00000,B01100,B00000,B11111,B00000}; // olho fechado

#define T_LARG 6

void carregaCauda(bool dir, bool caudaB) { criaChar(1, caudaB ? T_CAUDA_B : T_CAUDA_A, !dir); }

void carregaTubarao(bool dir, bool caudaB) {          // dir = true -> olhando para a direita
  criaChar(0, T_BARBATANA, !dir);
  carregaCauda(dir, caudaB);
  criaChar(2, T_CORPO1, !dir);
  criaChar(3, T_CORPO2, !dir);
  criaChar(4, T_CORPO_MEIO, !dir);
  criaChar(5, T_CORPO3, !dir);
  criaChar(6, T_CORPO4, !dir);
}

// l = linha do corpo (a barbatana fica na linha de cima); x = coluna da ponta esquerda
void poeTubarao(int l, int x, bool dir) {
  for (byte c = 0; c < T_LARG; c++) poe(l, x + c, dir ? 1 + c : T_LARG - c);
  poe(l - 1, dir ? x + 2 : x + 3, 0);
}

// ---------------------------------------------------------------------------------------------
// TUBARÃO PEQUENO (5 colunas x 2 linhas) — 4 slots a partir de "base"
//   base+0 = cauda | base+1 = corpo | base+2 = cabeça | base+3 = barbatana
// ---------------------------------------------------------------------------------------------
const byte TP_CAUDA_A[8]   PROGMEM = {B10000,B11001,B01101,B00111,B01101,B11001,B10000,B00000};
const byte TP_CAUDA_B[8]   PROGMEM = {B00000,B10001,B11001,B01111,B11001,B10001,B00000,B00000};
const byte TP_CORPO[8]     PROGMEM = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B00000};
const byte TP_CABECA[8]    PROGMEM = {B11100,B11110,B11011,B11111,B11110,B11100,B11000,B00000};
const byte TP_BARBATANA[8] PROGMEM = {B00000,B01000,B01100,B01100,B01110,B01110,B11110,B11111};

void carregaTubaraoP(byte base, bool dir, bool caudaB) {
  criaChar(base,     caudaB ? TP_CAUDA_B : TP_CAUDA_A, !dir);
  criaChar(base + 1, TP_CORPO, !dir);
  criaChar(base + 2, TP_CABECA, !dir);
  criaChar(base + 3, TP_BARBATANA, !dir);
}

void poeTubaraoP(byte base, int l, int x, bool dir) {
  poe(l, x,     base + (dir ? 0 : 2));
  poe(l, x + 1, base + 1); poe(l, x + 2, base + 1); poe(l, x + 3, base + 1);
  poe(l, x + 4, base + (dir ? 2 : 0));
  poe(l - 1, x + 2, base + 3);
}

// ---------------------------------------------------------------------------------------------
// ALGA (slot 7) e GELO
// ---------------------------------------------------------------------------------------------
// A alga balança alternando 3 desenhos (esquerda = ALGA_DIR espelhada, reta, direita).
// Trocar o desenho do slot muda todas as algas da tela sem redesenhar.
const byte ALGA_RETA[8] PROGMEM = {B00100,B01000,B01000,B00100,B00010,B00010,B00100,B00100};
const byte ALGA_DIR[8]  PROGMEM = {B00001,B00001,B00010,B00010,B00010,B00100,B00100,B00100};
const char SEQ_ALGA[4] = {'E', 'R', 'D', 'R'};
// no frio as algas viram cristais de gelo, com um brilho que pisca
const byte GELO_A[8]    PROGMEM = {B00000,B00100,B10101,B01110,B11111,B01110,B10101,B00100};
const byte GELO_B[8]    PROGMEM = {B00000,B00100,B00100,B01110,B11111,B01110,B00100,B00100};

void carregaAlga(byte passo) {
  char q = SEQ_ALGA[passo % 4];
  if (q == 'R') criaChar(7, ALGA_RETA, false);
  else          criaChar(7, ALGA_DIR, q == 'E');
}

void carregaGelo(byte passo) { criaChar(7, (passo % 2) ? GELO_A : GELO_B, false); }
