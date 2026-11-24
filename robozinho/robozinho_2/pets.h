// =============================================================================================
// pets.h — desenho dos três bichinhos: tubarão, sapinho e recruta
// =============================================================================================
// Todas as cenas desenham o bichinho por estas funções, então a mesma animação serve para os três.
//
//   tubarão  6 colunas x 2 linhas, slots 0 a 6 (desenhos em tela.h)
//   sapinho  3 colunas x 2 linhas, slots 0 a 5
//   recruta  3 colunas x 2 linhas, slots 0 a 5
//
// O slot 7 fica para o cenário (alga, taboa ou bandeira) e o slot 6 sobra para a comida.
// Cada bichinho tem 3 quadros: A (parado), B (nadando/pulando/andando) e Z (dormindo).
// Os desenhos de 15x16 pixels que geraram estes números estão no README.
#pragma once

enum { QUADRO_A, QUADRO_B, QUADRO_SONO };

const byte SAPO_A[6][8] PROGMEM = {
  {B00000,B00111,B01000,B01010,B01000,B10000,B10010,B10001},
  {B00000,B00000,B10001,B10001,B11111,B00000,B00000,B11111},
  {B00000,B11100,B00010,B01010,B00010,B00001,B01001,B10001},
  {B10000,B01000,B01100,B10010,B10001,B01110,B00000,B00000},
  {B00000,B00000,B00000,B00000,B11111,B00000,B00000,B00000},
  {B00001,B00010,B00110,B01001,B10001,B01110,B00000,B00000},
};
const byte SAPO_B[6][8] PROGMEM = {
  {B00000,B00111,B01000,B01010,B01000,B10000,B10010,B10001},
  {B00000,B00000,B10001,B10001,B11111,B00000,B00000,B11111},
  {B00000,B11100,B00010,B01010,B00010,B00001,B01001,B10001},
  {B10000,B01000,B01100,B01010,B10000,B11000,B00000,B00000},
  {B00000,B00000,B00000,B00000,B11111,B00000,B00000,B00000},
  {B00001,B00010,B00110,B01010,B00001,B00011,B00000,B00000},
};
const byte SAPO_Z[6][8] PROGMEM = {
  {B00000,B00111,B01000,B01000,B01111,B10000,B10010,B10000},
  {B00000,B00000,B10001,B10001,B10001,B11111,B00000,B00000},
  {B00000,B11100,B00010,B00010,B11110,B00001,B01001,B00001},
  {B10000,B01000,B01100,B10010,B10001,B01110,B00000,B00000},
  {B11111,B00000,B00000,B00000,B11111,B00000,B00000,B00000},
  {B00001,B00010,B00110,B01001,B10001,B01110,B00000,B00000},
};
const byte RECRUTA_A[6][8] PROGMEM = {
  {B00000,B00011,B00100,B01111,B01011,B01010,B01011,B00100},
  {B11111,B00000,B00000,B11111,B10001,B10001,B10001,B00000},
  {B00000,B11000,B00100,B11110,B11010,B01010,B11010,B00100},
  {B00100,B00010,B00001,B00010,B00100,B00000,B00000,B00001},
  {B10001,B01110,B11111,B11111,B11111,B10001,B10001,B10001},
  {B00100,B01000,B10000,B01000,B00100,B00000,B00000,B10000},
};
const byte RECRUTA_B[6][8] PROGMEM = {
  {B00000,B00011,B00100,B01111,B01011,B01010,B01011,B00100},
  {B11111,B00000,B00000,B11111,B10001,B10001,B10001,B00000},
  {B00000,B11000,B00100,B11110,B11010,B01010,B11010,B00100},
  {B00100,B00010,B00001,B00110,B00000,B00001,B00010,B00110},
  {B10001,B01110,B11111,B11111,B11111,B00000,B00000,B00000},
  {B00100,B01000,B10000,B01100,B00000,B10000,B01000,B01100},
};
const byte RECRUTA_Z[6][8] PROGMEM = {
  {B00000,B00011,B00100,B01111,B01000,B01011,B01000,B00100},
  {B11111,B00000,B00000,B11111,B00000,B10001,B00000,B00000},
  {B00000,B11000,B00100,B11110,B00010,B11010,B00010,B00100},
  {B00100,B00010,B00001,B00010,B00100,B00000,B00000,B00001},
  {B00000,B01110,B11111,B11111,B11111,B10001,B10001,B10001},
  {B00100,B01000,B10000,B01000,B00100,B00000,B00000,B10000},
};

// ---------------------------------------------------------------------------------------------
// Medidas e desenho do corpo
// ---------------------------------------------------------------------------------------------
byte pet_larg() { return petTipo == PET_TUBARAO ? T_LARG : 3; }

const byte (*pet_quadro(byte q))[8] {
  if (petTipo == PET_SAPO) return q == QUADRO_B ? SAPO_B : (q == QUADRO_SONO ? SAPO_Z : SAPO_A);
  return q == QUADRO_B ? RECRUTA_B : (q == QUADRO_SONO ? RECRUTA_Z : RECRUTA_A);
}

// carrega os desenhos do bichinho; dir = true -> olhando para a direita
void pet_carrega(bool dir, byte quadro) {
  if (petTipo == PET_TUBARAO) {
    carregaTubarao(dir, quadro == QUADRO_B);
    if (quadro == QUADRO_SONO) criaChar(5, T_CORPO3_SONO, !dir);
    return;
  }
  const byte (*g)[8] = pet_quadro(quadro);
  for (byte i = 0; i < 6; i++) criaChar(i, g[i], !dir);
}

// só troca o que mexe entre os quadros A e B (mais rápido que recarregar tudo)
void pet_anima(bool dir, bool quadroB) {
  if (petTipo == PET_TUBARAO) { carregaCauda(dir, quadroB); return; }
  const byte (*g)[8] = pet_quadro(quadroB ? QUADRO_B : QUADRO_A);
  for (byte i = 3; i < 6; i++) criaChar(i, g[i], !dir);   // pernas e braços ficam na linha de baixo
}

// só a parte de cima (cabeça) na linha l
void pet_poeTopo(int l, int x, bool dir) {
  if (petTipo == PET_TUBARAO) { poe(l, dir ? x + 2 : x + 3, 0); return; }
  for (byte c = 0; c < 3; c++) poe(l, x + (dir ? c : 2 - c), c);
}

// l = linha do corpo (a cabeça ou a barbatana fica na linha de cima); x = coluna da esquerda
void pet_poe(int l, int x, bool dir) {
  if (petTipo == PET_TUBARAO) { poeTubarao(l, x, dir); return; }
  pet_poeTopo(l - 1, x, dir);
  for (byte c = 0; c < 3; c++) poe(l, x + (dir ? c : 2 - c), 3 + c);
}

// coluna da frente do bichinho (onde ficam a boca e os balões)
int pet_frente(int x, bool dir) {
  if (petTipo == PET_TUBARAO) return dir ? x + T_LARG - 1 : x;
  return x + 1;
}

// ---------------------------------------------------------------------------------------------
// Cenário (slot 7): alga no mar, taboa no pântano, bandeira no quartel; gelo quando faz frio
// ---------------------------------------------------------------------------------------------
const byte TABOA_A[8]    PROGMEM = {B00100,B01110,B01110,B01110,B00100,B00101,B00110,B00100};
const byte TABOA_B[8]    PROGMEM = {B00010,B00111,B00111,B00111,B00010,B00100,B10100,B01100};
const byte BANDEIRA_A[8] PROGMEM = {B11110,B11111,B11110,B11100,B10000,B10000,B10000,B11000};
const byte BANDEIRA_B[8] PROGMEM = {B11100,B11111,B11111,B11000,B10000,B10000,B10000,B11000};

void pet_carregaFundo(byte passo) {
  if (clima == CLIMA_FRIO)          carregaGelo(passo);
  else if (petTipo == PET_SAPO)     criaChar(7, (passo % 4 < 2) ? TABOA_A : TABOA_B, false);
  else if (petTipo == PET_RECRUTA)  criaChar(7, (passo % 4 < 2) ? BANDEIRA_A : BANDEIRA_B, false);
  else                              carregaAlga(passo);
}

// põe os enfeites do cenário na última linha
void pet_poeFundo() {
  if (petTipo == PET_TUBARAO) { const byte c[4] = {2, 7, 12, 17}; for (byte i = 0; i < 4; i++) poe(3, c[i], 7); }
  else if (petTipo == PET_SAPO) { const byte c[4] = {0, 3, 16, 19}; for (byte i = 0; i < 4; i++) poe(3, c[i], 7); }
  else { poe(3, 0, 7); poe(3, 19, 7); }
}

// ---------------------------------------------------------------------------------------------
// Comida de cada bichinho (slot 6): peixinho, mosca e maçã
// ---------------------------------------------------------------------------------------------
const byte COMIDA_PEIXE[8] PROGMEM = {B00000,B00000,B10110,B11111,B10110,B00000,B00000,B00000};
const byte COMIDA_MOSCA[8] PROGMEM = {B00000,B11011,B11011,B00100,B01110,B00100,B00000,B00000};
const byte COMIDA_MACA[8]  PROGMEM = {B00100,B01000,B01110,B11111,B11111,B11111,B01110,B00000};

const byte* pet_comida() {
  if (petTipo == PET_SAPO)    return COMIDA_MOSCA;
  if (petTipo == PET_RECRUTA) return COMIDA_MACA;
  return COMIDA_PEIXE;
}

// ---------------------------------------------------------------------------------------------
// ROSTO de perto (só olhos e boca), nas linhas 1 a 3 — usado no estudo, na festa e no sensor
// ---------------------------------------------------------------------------------------------
const byte F3_OLHO[8]        PROGMEM = {B01110,B10001,B10011,B10111,B10111,B10001,B01110,B00000};
const byte F3_OLHO_FECH[8]   PROGMEM = {B00000,B00000,B00000,B01110,B10001,B00000,B00000,B00000};
const byte F3_BOCA_CE[8]     PROGMEM = {B00000,B00000,B00011,B00100,B01000,B01000,B10000,B10000};
const byte F3_DENTE_CIMA[8]  PROGMEM = {B00000,B00000,B11111,B01110,B00100,B00000,B00000,B00000};
const byte F3_BOCA_BE[8]     PROGMEM = {B10000,B10000,B01000,B01000,B00100,B00011,B00000,B00000};
const byte F3_DENTE_BAIXO[8] PROGMEM = {B00000,B00000,B00000,B00100,B01110,B11111,B00000,B00000};

const byte ROSTO_CANTO[8]    PROGMEM = {B00000,B00000,B10000,B01000,B00111,B00000,B00000,B00000};
const byte ROSTO_SORRISO[8]  PROGMEM = {B00000,B00000,B00000,B00000,B11111,B00000,B00000,B00000};
const byte ROSTO_BOCHECHA[8] PROGMEM = {B00000,B00000,B00000,B01001,B10010,B00000,B00000,B00000};
const byte CAPACETE_CANTO[8] PROGMEM = {B00011,B00111,B01111,B11111,B11111,B11111,B11111,B11111};
const byte OCULOS[8]         PROGMEM = {B00000,B11111,B10001,B10011,B10011,B10001,B11111,B00000};
const byte OCULOS_FECH[8]    PROGMEM = {B00000,B00000,B00000,B00000,B11111,B00000,B00000,B00000};
const byte RECO_CANTO[8]     PROGMEM = {B00000,B10000,B01000,B00111,B00000,B00000,B00000,B00000};
const byte RECO_SORRISO[8]   PROGMEM = {B00000,B00000,B00000,B11111,B00000,B00000,B00000,B00000};

void pet_carregaRosto(bool pisca) {
  if (petTipo == PET_TUBARAO) {
    criaChar(0, pisca ? F3_OLHO_FECH : F3_OLHO, false);
    criaChar(1, pisca ? F3_OLHO_FECH : F3_OLHO, true);
    criaChar(2, F3_BOCA_CE, false);  criaChar(3, F3_DENTE_CIMA, false);  criaChar(4, F3_BOCA_CE, true);
    criaChar(5, F3_BOCA_BE, false);  criaChar(6, F3_DENTE_BAIXO, false); criaChar(7, F3_BOCA_BE, true);
  } else if (petTipo == PET_SAPO) {
    criaChar(0, pisca ? F3_OLHO_FECH : F3_OLHO, false);
    criaChar(1, pisca ? F3_OLHO_FECH : F3_OLHO, true);
    criaChar(2, ROSTO_CANTO, false); criaChar(3, ROSTO_SORRISO, false); criaChar(4, ROSTO_CANTO, true);
    criaChar(5, ROSTO_BOCHECHA, false);
  } else {
    criaChar(0, CAPACETE_CANTO, false); criaChar(1, CAPACETE_CANTO, true);
    criaChar(2, pisca ? OCULOS_FECH : OCULOS, false);
    criaChar(3, pisca ? OCULOS_FECH : OCULOS, true);
    criaChar(4, RECO_CANTO, false); criaChar(5, RECO_SORRISO, false); criaChar(6, RECO_CANTO, true);
  }
}

void pet_poeRosto() {
  if (petTipo == PET_TUBARAO) {
    poe(1, 6, 0); poe(1, 13, 1);
    poe(2, 5, 2); poe(2, 14, 4);
    poe(3, 5, 5); poe(3, 14, 7);
    for (byte c = 6; c <= 13; c++) { poe(2, c, 3); poe(3, c, 6); }
  } else if (petTipo == PET_SAPO) {
    poe(1, 7, 0); poe(1, 12, 1);
    poe(2, 6, 5); poe(2, 13, 5);
    poe(2, 9, '.'); poe(2, 10, '.');
    poe(3, 6, 2); for (byte c = 7; c <= 12; c++) poe(3, c, 3); poe(3, 13, 4);
  } else {
    poe(1, 6, 0); for (byte c = 7; c <= 12; c++) poe(1, c, 0xFF); poe(1, 13, 1);
    TXT(2, 7, "- -- -"); poe(2, 8, 2); poe(2, 11, 3);
    poe(3, 8, 4); poe(3, 9, 5); poe(3, 10, 5); poe(3, 11, 6);
  }
}

// ---------------------------------------------------------------------------------------------
// PORTAL (slot 7): oval brilhante com o miolo piscando
// ---------------------------------------------------------------------------------------------
const byte PORTAL_A[8] PROGMEM = {B01110,B11011,B10101,B11011,B10101,B11011,B10101,B01110};
const byte PORTAL_B[8] PROGMEM = {B01110,B10101,B11011,B10101,B11011,B10101,B11011,B01110};
void carregaPortal(byte passo) { criaChar(7, (passo % 2) ? PORTAL_A : PORTAL_B, false); }
