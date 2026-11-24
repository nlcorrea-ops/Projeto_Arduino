// =============================================================================================
// cenas.h — todas as telas animadas
// =============================================================================================
// Cada cena tem <nome>_iniciar() (chamada ao entrar) e <nome>_atualizar() (chamada em todo
// loop, só desenha quando passa o tempo do quadro). As que terminam sozinhas devolvem true.
// O bichinho é desenhado pelas funções de pets.h, então as cenas servem para os três.
//
//   CASA        animação padrão: o tubarão nada, o sapinho pula, o recruta anda; muda com o frio
//               (gelo, neve, "BRR") e o calor (sol, suor, "UFA"); balões, "!" de atenção e a
//               temperatura no canto
//   FORA        o bichinho está no app: cenário vazio e "FUI PRO CELULAR!"
//   PORTAL      saída e chegada pelo portal mágico
//   ESTUDO      relógio grande do Pomodoro ou do cronômetro, alternando com o rostinho
//   FESTA       rostinho comemorando (início e fim do estudo)
//   COMER       a comida cai e o bichinho come
//   BRINCAR     o bichinho joga bolinha
//   DORMIR      dormindo no fundo; espia quando alguém chega perto
//   TRANSIÇÃO   adormecer ("UAAAH") e despertar ("BOM DIA!")
//   BANHO       ENTER faz o bichinho pular na banheira
//   MINIGAME    ENTER pula os obstáculos
//   INTERAÇÃO   pega-pega com o tubarão do outro robozinho (visita, só com CONEXAO_AMIGO)
//   SENSOR      o bichinho se aproxima conforme a distância
//   STATUS      barras das necessidades, vidas, temperatura e amigo
//   MENSAGEM    duas linhas de texto, com o bichinho saindo ou chegando
#pragma once


// ############################################################################################
// CASA — animação padrão com clima
// ############################################################################################
#define CASA_TEMPO_NADAR  300           // ms por passo com clima normal
#define CASA_TEMPO_LENTO  450           // no frio e no calor ele anda mais devagar
#define CASA_TEMPO_FUNDO  400           // ms por passo do balanço do cenário / brilho do gelo

int  cs_x = 0;
bool cs_dir = true, cs_caudaB = false;
byte cs_fundo = 0, cs_manut = 0;
unsigned int cs_passo = 0;
unsigned long cs_tNadar = 0, cs_tFundo = 0;
char cs_balao[12] = "";                 // texto do balão sobre a cabeça
unsigned long cs_balaoAte = 0;
bool casaPausa = false;                 // true = cronômetro pausado: mostra o sinal de pausa

void casa_balao(const char* s, unsigned long ms) {
  strncpy(cs_balao, s, sizeof(cs_balao) - 1); cs_balao[sizeof(cs_balao) - 1] = 0;
  cs_balaoAte = millis() + ms;
}
void casa_balao_P(const char* s, unsigned long ms) {
  strncpy_P(cs_balao, s, sizeof(cs_balao) - 1); cs_balao[sizeof(cs_balao) - 1] = 0;
  cs_balaoAte = millis() + ms;
}

void casa_carregaFundo() { pet_carregaFundo(cs_fundo); }

// linha do corpo: o tubarão nada no meio, o sapinho pula, o recruta anda no chão
int casa_linha() {
  if (petTipo == PET_TUBARAO) return 2;
  if (petTipo == PET_SAPO) return (cs_passo % 4 == 1 || cs_passo % 4 == 2) ? 2 : 3;
  return 3;
}

void casa_montaTela() {
  limpaTela();
  pet_poeFundo();

  if (clima == CLIMA_CALOR) {                          // sol perto do canto, raios piscando
    poe(0, 13, 'O');                                   // (sem '\\': no LCD ela vira o símbolo do iene)
    if (cs_passo % 2) { poe(0, 12, '-'); poe(0, 14, '-'); poe(1, 13, '|'); poe(1, 12, '.'); poe(1, 14, '.'); }
    else              { poe(1, 13, '.'); }
  }

  int l = casa_linha();
  pet_poe(l, cs_x, cs_dir);
  int cabeca = pet_frente(cs_x, cs_dir);
  int frente = cs_dir ? cabeca + 1 : cabeca - 1;

  if (clima == CLIMA_NORMAL && petTipo == PET_TUBARAO) {   // bolinhas atrás da cauda
    int d = cs_dir ? -1 : 1;
    int cauda = cs_dir ? cs_x : cs_x + T_LARG - 1;
    poe(2, cauda + d, '.');
    poe(1, cauda + 3 * d, 0xDF);
    poe(0, cauda + 5 * d, '.');
  } else if (clima == CLIMA_CALOR) {                   // gota de suor pingando na frente
    poe(l - 1 + cs_passo % 2, frente, ',');
  }

  if (!casaPausa) {                                     // temperatura no canto direito
    char t[6]; temperaturaTexto(t);
    texto(0, LCD_COLS - strlen(t), t);
  }

  bool balao = millis() < cs_balaoAte;
  if (!balao && clima == CLIMA_FRIO  && (cs_passo / 4) % 3 == 0) { strcpy_P(cs_balao, PSTR("BRR")); balao = true; }
  if (!balao && clima == CLIMA_CALOR && (cs_passo / 5) % 3 == 0) { strcpy_P(cs_balao, PSTR("UFA")); balao = true; }
  if (balao) {
    int n = strlen(cs_balao);
    int c = constrain(cabeca - n / 2, 0, LCD_COLS - n);
    texto(max(0, l - 2), c, cs_balao);
  }

  if (casaPausa) TXT(0, 0, "|| PAUSA");
  else if (pet_humor() == HUMOR_CRITICO && (cs_passo % 2)) poe(0, 0, '!');   // pede atenção
  desenha();
}

void casa_iniciar() {
  cs_x = (LCD_COLS - pet_larg()) / 2;                 // começa no meio da tela
  cs_dir = true; cs_caudaB = false; cs_fundo = 0; cs_manut = 0; cs_passo = 0;
  cs_tNadar = cs_tFundo = millis();
  pet_carrega(cs_dir, QUADRO_A);
  casa_carregaFundo();
  casa_montaTela();
}

void casa_atualizar() {
  unsigned long agora = millis();
  if (agora - cs_tFundo >= CASA_TEMPO_FUNDO) {        // cenário: só troca o desenho do slot 7
    cs_tFundo = agora;
    cs_fundo = (cs_fundo + 1) % 4;
    casa_carregaFundo();
  }

  unsigned int tempo = clima == CLIMA_NORMAL ? CASA_TEMPO_NADAR : CASA_TEMPO_LENTO;
  if (agora - cs_tNadar < tempo) return;
  cs_tNadar = agora;
  cs_passo++;

  int maxX = LCD_COLS - pet_larg();
  if (cs_dir) { if (cs_x < maxX) cs_x++; else { cs_dir = false; pet_carrega(cs_dir, QUADRO_A); } }
  else        { if (cs_x > 0)    cs_x--; else { cs_dir = true;  pet_carrega(cs_dir, QUADRO_A); } }
  cs_caudaB = !cs_caudaB;
  pet_anima(cs_dir, cs_caudaB);

  // manutenção gradual: reenvia 1 linha da tela por passo e recarrega os desenhos de vez em quando
  cs_manut++;
  linhaForcada = cs_manut % LCD_LINHAS;
  if (cs_manut % 6 == 0) pet_carrega(cs_dir, cs_caudaB ? QUADRO_B : QUADRO_A);
  casa_montaTela();
  linhaForcada = -1;
}


// ############################################################################################
// FORA — o bichinho foi para o app: cenário vazio esperando ele voltar
// ############################################################################################
byte fr_passo = 0;
unsigned long fr_t = 0;

void fora_iniciar() { fr_passo = 0; fr_t = 0; pet_carregaFundo(0); }

void fora_atualizar() {
  if (millis() - fr_t < 400) return;
  fr_t = millis();
  fr_passo++;
  pet_carregaFundo(fr_passo);
  limpaTela();
  pet_poeFundo();
  TXT(1, 2, "FUI PRO CELULAR!");
  TXT(2, 5, "volto ja");
  if (fr_passo % 2) TXT(2, 14, "<3");
  desenhaQuadro(fr_passo);
}


// ############################################################################################
// PORTAL — o bichinho entra no portal (saída) ou sai dele (chegada). Devolve true ao terminar.
// ############################################################################################
enum { PORTAL_SAIDA, PORTAL_CHEGADA };
#define PT_QUADROS    20                // saída: 20 x 160 ms = 3,2 s
#define PT_COL_SAIDA  17
#define PT_COL_CHEGA  2

byte pt_tipo = PORTAL_SAIDA, pt_passo = 0;
unsigned long pt_t = 0;

void portal_iniciar(byte tipo) {
  pt_tipo = tipo; pt_passo = 0; pt_t = 0;
  pet_carrega(true, QUADRO_A);
  som_tocar(SOM_PORTAL);
}

// a chegada dura mais: o bichinho sai do portal devagar e depois comemora
byte portal_total() {
  if (pt_tipo == PORTAL_SAIDA) return PT_QUADROS;
  return 2 * (8 - pet_larg() / 2 - (PT_COL_CHEGA + 1 - pet_larg())) + 10;
}

bool portal_atualizar() {
  if (pt_passo >= portal_total()) return true;
  if (millis() - pt_t < 160) return false;
  pt_t = millis();
  pt_passo++;

  carregaPortal(pt_passo);
  pet_anima(true, pt_passo % 2);
  bool saida = pt_tipo == PORTAL_SAIDA;
  int col = saida ? PT_COL_SAIDA : PT_COL_CHEGA;
  int l = petTipo == PET_TUBARAO ? 2 : 3;
  int parada = 8 - pet_larg() / 2;                     // onde o bichinho para depois de chegar
  int inicio = col + 1 - (int)pet_larg();
  int x = saida ? 1 + pt_passo : min(inicio + pt_passo / 2, parada);
  bool chegou = !saida && x == parada;
  if (chegou && pt_passo == 2 * (parada - inicio)) som_tocar(SOM_FESTA);

  limpaTela();
  pet_poe(l, x, true);
  for (byte r = 0; r < LCD_LINHAS; r++)                // o portal "esconde" o que passou por ele
    for (byte c = 0; c < LCD_COLS; c++)
      if (saida ? c >= col : c <= col) tela[r][c] = ' ';

  for (byte r = 1; r <= 3; r++) poe(r, col, 7);        // o portal
  const char brilho[4] = {'*', '+', '.', (char)0xDF};
  for (byte i = 0; i < 5; i++) {                       // brilhinhos em volta
    byte r = (pt_passo + i * 3) % 4;
    int c = col + ((i % 2) ? 1 : -1) * (1 + (pt_passo + i) % 2);
    if (tela[r][c] == ' ') poe(r, c, brilho[(pt_passo + i) % 4]);
  }

  if (saida) {
    TXT(0, 0, "OUTRA DIMENSAO");
    for (byte i = 0; i < (pt_passo / 2) % 4; i++) poe(0, 14 + i, '.');
  } else if (chegou) {
    TXT(0, 0, "CHEGUEI!");
    if (pt_passo % 2) TXT(1, 9, "<3");
  } else {
    TXT(0, 0, "CHEGANDO");
    for (byte i = 0; i < (pt_passo / 2) % 4; i++) poe(0, 8 + i, '.');
  }
  desenhaQuadro(pt_passo);
  return false;
}


// ############################################################################################
// FESTA — rostinho comemorando, com corações. Devolve true ao terminar.
// ############################################################################################
const char* ft_texto = 0;               // texto na flash (PSTR)
unsigned long ft_t0 = 0, ft_dur = 0, ft_t = 0;
byte ft_passo = 0;

void festa_iniciar(const char* textoP, unsigned long dur) {
  ft_texto = textoP; ft_dur = dur; ft_t0 = millis(); ft_t = 0; ft_passo = 0;
  if (petAqui) pet_carregaRosto(false);
  som_tocar(SOM_FESTA);
}

bool festa_atualizar() {
  if (millis() - ft_t0 >= ft_dur) return true;
  if (millis() - ft_t < 200) return false;
  ft_t = millis();
  ft_passo++;
  if (petAqui && ft_passo % 6 == 5) pet_carregaRosto(true);   // pisca
  if (petAqui && ft_passo % 6 == 0) pet_carregaRosto(false);

  limpaTela();
  textoCentro_P(0, ft_texto);
  if (petAqui) pet_poeRosto();
  bool a = ft_passo % 2;
  TXT(a ? 1 : 2, 1, "<3");
  TXT(a ? 2 : 1, 17, "<3");
  if (a) TXT(3, 2, "*"); else TXT(3, 17, "*");
  desenhaQuadro(ft_passo);
  return false;
}


// ############################################################################################
// ESTUDO — relógio grande alternando com o rostinho. Quem controla o tempo é o robô.
// ############################################################################################
// Números grandes (3 colunas x 2 linhas) montados com 8 desenhos nos slots 0 a 7.
const byte BIG[8][8] PROGMEM = {
  {B00111,B01111,B11111,B11111,B11111,B11111,B11111,B11111},   // 0 canto de cima à esquerda
  {B11111,B11111,B11111,B00000,B00000,B00000,B00000,B00000},   // 1 barra de cima
  {B11100,B11110,B11111,B11111,B11111,B11111,B11111,B11111},   // 2 canto de cima à direita
  {B11111,B11111,B11111,B11111,B11111,B11111,B01111,B00111},   // 3 canto de baixo à esquerda
  {B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111},   // 4 barra de baixo
  {B11111,B11111,B11111,B11111,B11111,B11111,B11110,B11100},   // 5 canto de baixo à direita
  {B11111,B11111,B11111,B00000,B00000,B00000,B11111,B11111},   // 6 barra de cima e do meio
  {B11111,B00000,B00000,B00000,B00000,B11111,B11111,B11111},   // 7 barra do meio e de baixo
};
#define BB 0xFF                         // bloco cheio (já existe no LCD)
#define BV ' '
const byte BIG_DIGITO[10][6] PROGMEM = {
  {0, 1, 2,   3, 4, 5},                 // 0
  {1, 2, BV,  4, BB, 4},                // 1
  {6, 6, 2,   3, 4, 4},                 // 2
  {6, 6, 2,   4, 4, 5},                 // 3
  {3, 4, BB,  BV, BV, BB},              // 4
  {3, 6, 6,   4, 4, 5},                 // 5
  {0, 6, 6,   3, 4, 5},                 // 6
  {1, 1, 2,   BV, BV, BB},              // 7
  {0, 6, 2,   3, 4, 5},                 // 8
  {0, 6, 2,   4, 4, 5},                 // 9
};

enum { EST_NADA, EST_TROCOU_FASE };
enum { ET_RELOGIO, ET_ROSTO, ET_CASA };

char est_modo = 'N';                    // 'P' Pomodoro, 'C' cronômetro, 'N' nenhum
bool est_pausaPomodoro = false;         // true = na pausa automática do Pomodoro
bool est_parado = false;                // true = cronômetro pausado pelo ENTER
byte est_ciclo = 1;
unsigned long est_seg = 0, est_tSeg = 0;
byte est_tela = 255;
unsigned long est_tTela = 0, est_t = 0;
byte est_passo = 0;

bool estudoAtivo() { return est_modo != 'N'; }

unsigned long estudo_duracao() { return (est_pausaPomodoro ? POMODORO_PAUSA_MIN : POMODORO_FOCO_MIN) * 60UL; }

void estudo_comecar(char modo) {
  est_modo = modo; est_pausaPomodoro = false; est_parado = false;
  est_ciclo = 1; est_seg = 0; est_tSeg = millis();
}

void estudo_parar() { est_modo = 'N'; est_parado = false; est_pausaPomodoro = false; casaPausa = false; }

// chamar ao entrar na tela de estudo (também depois de uma festa ou de um portal)
void estudo_entrar() { est_tela = 255; est_t = 0; }

void estudo_alternaPausa() {
  if (est_modo != 'C') return;
  est_parado = !est_parado;
  est_tSeg = millis();
  est_tela = 255;
}

void big_carrega() { for (byte i = 0; i < 8; i++) criaChar(i, BIG[i], false); }

void big_digito(byte d, byte col) {
  for (byte i = 0; i < 3; i++) {
    poe(1, col + i, pgm_read_byte(&BIG_DIGITO[d][i]));
    poe(2, col + i, pgm_read_byte(&BIG_DIGITO[d][3 + i]));
  }
}

void estudo_rotulo(byte l, byte c) {
  if (est_modo == 'P') {
    if (est_pausaPomodoro) TXT(l, c, "PAUSA");
    else { TXT(l, c, "FOCO #"); char s[4]; numero(s, est_ciclo, 1); texto(l, c + 6, s); }
  } else {
    if (est_parado) TXT(l, c, "PAUSADO"); else TXT(l, c, "CRONOMETRO");
  }
}

// tempo que aparece: o que falta no Pomodoro, o que passou no cronômetro
unsigned long estudo_tempoMostrado() {
  if (est_modo == 'P') { unsigned long d = estudo_duracao(); return est_seg >= d ? 0 : d - est_seg; }
  return est_seg;
}

void estudo_relogio() {
  unsigned long t = estudo_tempoMostrado();
  bool horas = t >= 3600;                              // a partir de 1 h mostra horas:minutos
  unsigned int a = horas ? t / 3600 : t / 60, b = horas ? (t / 60) % 60 : t % 60;
  if (a > 99) a = 99;
  limpaTela();
  estudo_rotulo(0, 0);
  big_digito(a / 10, 2); big_digito(a % 10, 6);
  if ((millis() / 500) % 2 || est_parado) { poe(1, 10, 0xA5); poe(2, 10, 0xA5); }
  big_digito(b / 10, 12); big_digito(b % 10, 16);
  if (est_modo == 'P') { if (est_pausaPomodoro) TXT(3, 0, "respira um pouco"); else TXT(3, 0, "foco total!"); }
  else if (est_parado) TXT(3, 0, "ENTER continua");
  else if (horas) TXT(3, 0, "horas:min");
  else TXT(3, 0, "ENTER pausa");
  desenhaQuadro(est_passo);
}

void estudo_rosto(bool pisca) {
  unsigned long t = estudo_tempoMostrado();
  char s[8];
  char* p = numero(s, min(t / 60, 99UL), 2); *p++ = ':'; numero(p, t % 60, 2);
  limpaTela();
  estudo_rotulo(0, 0);
  texto(0, 15, s);
  pet_poeRosto();
  desenhaQuadro(est_passo);
}

// conta o tempo; chamar em todo loop enquanto houver estudo (mesmo fora da tela de estudo)
// devolve EST_TROCOU_FASE quando o Pomodoro passa do foco para a pausa ou da pausa para o foco
byte estudo_tique() {
  if (est_parado) { est_tSeg = millis(); return EST_NADA; }
  while (millis() - est_tSeg >= 1000) { est_tSeg += 1000; est_seg++; }
  if (est_modo == 'P' && est_seg >= estudo_duracao()) {
    est_pausaPomodoro = !est_pausaPomodoro;
    if (!est_pausaPomodoro) est_ciclo++;
    est_seg = 0; est_tSeg = millis();
    return EST_TROCOU_FASE;
  }
  return EST_NADA;
}

// desenha a tela de estudo
void estudo_atualizar() {
  // qual tela mostrar
  byte quer;
  if (est_parado && petAqui) quer = ET_CASA;
  else if (!petAqui) quer = ET_RELOGIO;
  else {
    unsigned long ciclo = millis() % (ESTUDO_TEMPO_MS + ESTUDO_ROSTO_MS);
    quer = ciclo < ESTUDO_TEMPO_MS ? ET_RELOGIO : ET_ROSTO;
  }
  if (quer != est_tela) {
    est_tela = quer; telaToda = true; est_t = 0;
    casaPausa = (quer == ET_CASA);
    if (quer == ET_RELOGIO) big_carrega();
    else if (quer == ET_ROSTO) pet_carregaRosto(false);
    else casa_iniciar();
  }

  if (est_tela == ET_CASA) { casa_atualizar(); return; }
  if (millis() - est_t < 250) return;
  est_t = millis();
  est_passo++;
  if (est_tela == ET_RELOGIO) estudo_relogio();
  else {
    bool pisca = est_passo % 12 == 0;
    if (pisca || est_passo % 12 == 1) pet_carregaRosto(pisca);
    estudo_rosto(pisca);
  }
}


// ############################################################################################
// COMER — a comida cai e o bichinho come. Devolve true ao terminar.
// ############################################################################################
// Slots: 0-6 o bichinho (o mesmo desenho da casa) | 7 a comida
#define COL_TUBO 12

// estados: 0 tubo aparece | 1 comida caindo | 2 indo até a comida | 3 boca na comida | 4 "NHAC!" | 5 fim
byte cm_estado = 0, cm_cont = 0;
int  cm_x = 0, cm_fc = COL_TUBO, cm_fl = 1;
unsigned int  cm_passo = 0;
unsigned long cm_t = 0;
bool cm_lanche = false;                 // true = comida comprada no app

void comer_iniciar(bool lanche) {
  cm_lanche = lanche;
  pet_carrega(true, QUADRO_A);
  criaChar(7, pet_comida(), false);
  cm_estado = 0; cm_cont = 0; cm_x = 0;
  cm_fc = COL_TUBO; cm_fl = 1; cm_passo = 0; cm_t = 0;
}

bool comer_atualizar() {
  if (cm_estado == 5) return true;
  if (millis() - cm_t < 150) return false;
  cm_t = millis();
  cm_passo++; cm_cont++;

  if (cm_estado == 0 && cm_cont >= 4) { cm_estado = 1; cm_cont = 0; cm_fl = 1; }
  else if (cm_estado == 1) {
    if (cm_cont == 2) cm_fl = 2;
    if (cm_cont >= 3) { cm_estado = 2; cm_cont = 0; cm_fl = 3; }
  }
  else if (cm_estado == 2) {
    cm_x++;
    if (cm_cont % 2 == 0 && cm_fc < LCD_COLS - 1) cm_fc++;
    if (cm_fc - pet_frente(cm_x, true) <= 1) { cm_estado = 3; cm_cont = 0; }
  }
  else if (cm_estado == 3 && cm_cont >= 2) { cm_estado = 4; cm_cont = 0; }
  else if (cm_estado == 4 && cm_cont >= 8) { cm_estado = 5; }

  limpaTela();
  if (cm_lanche) TXT(0, 0, "LANCHE!"); else TXT(0, 0, "COMER");
  if (cm_estado > 0 || cm_cont >= 2) TXT(0, COL_TUBO - 1, "[_]");
  pet_anima(true, cm_passo % 2);
  pet_poe(3, cm_x, true);
  if (cm_estado >= 1 && cm_estado <= 3) poe(cm_fl, cm_fc, 7);
  if (cm_estado == 4) TXT(1, min(pet_frente(cm_x, true), LCD_COLS - 5), "NHAC!");
  desenhaQuadro(cm_passo);
  return false;
}


// ############################################################################################
// BRINCAR — o bichinho joga bolinha; depois de BOLA_TOQUES toques a cena devolve true
// ############################################################################################
// Slots: 0-6 o bichinho | 7 a bolinha. A bola voa em arco, quica nas bordas da tela e o
// bichinho corre atrás dela para dar outro toque.
const byte BOLA[8] PROGMEM = {B00000,B01110,B10101,B11111,B10101,B01110,B00000,B00000};
const byte BOLA_ARCO[10] = {3, 2, 1, 1, 0, 0, 1, 1, 2, 3};   // linha da bola em cada passo do voo
#define BOLA_TOQUES 5

int  bl_x, bl_px;                       // coluna da bola e coluna da esquerda do bichinho
int  bl_vx;                             // direção da bola: +1 direita, -1 esquerda
byte bl_arco, bl_toques, bl_fim, bl_boing;
bool bl_dir;
unsigned int  bl_passo;
unsigned long bl_t;

void bola_iniciar() {
  pet_carrega(true, QUADRO_A);
  criaChar(7, BOLA, false);
  bl_px = 1; bl_dir = true;
  bl_x = 14; bl_vx = -1; bl_arco = 9;   // a bola começa rolando na direção do bichinho
  bl_toques = 0; bl_fim = 0; bl_boing = 0; bl_passo = 0; bl_t = 0;
}

bool bola_atualizar() {
  if (millis() - bl_t < 180) return false;
  bl_t = millis();
  bl_passo++;
  int larg = pet_larg();

  if (bl_fim) {
    if (++bl_fim > 14) return true;
  } else {
    bl_x += bl_vx;                                     // a bola anda e quica nas bordas
    if (bl_x <= 0)                { bl_x = 0; bl_vx = 1; }
    else if (bl_x >= LCD_COLS - 1) { bl_x = LCD_COLS - 1; bl_vx = -1; }
    if (bl_arco < 9) bl_arco++;                        // depois do voo, rola no chão

    bool dir = bl_x >= bl_px + larg / 2;               // o bichinho olha para a bola
    if (dir != bl_dir) { bl_dir = dir; pet_carrega(bl_dir, QUADRO_A); }
    int alvo = dir ? bl_x - larg : bl_x + 1;           // corre até encostar na bola
    if (bl_passo % 2 == 0) {                           // anda na metade da velocidade da bola
      if (bl_px < alvo) bl_px++; else if (bl_px > alvo) bl_px--;
    }
    bl_px = constrain(bl_px, 0, LCD_COLS - larg);

    bool baixa = bl_arco >= 7;                         // só toca a bola quando ela desce
    if (baixa && bl_x >= bl_px - 1 && bl_x <= bl_px + larg) {
      bl_vx = (bl_x >= bl_px + larg / 2) ? 1 : -1;     // joga para longe de si
      bl_arco = 0; bl_boing = 3; bl_toques++;
      if (bl_toques >= BOLA_TOQUES) { bl_fim = 1; som_tocar(SOM_FESTA); }
      else som_tocar(SOM_CLIQUE);
    }
    if (bl_boing) bl_boing--;
  }

  limpaTela();
  TXT(0, 0, "BRINCAR");
  for (byte i = 0; i < bl_toques; i++) poe(0, LCD_COLS - 1 - i, '*');
  pet_anima(bl_dir, bl_passo % 2);
  pet_poe(3, bl_px, bl_dir);
  if (bl_fim) {
    TXT(0, 8, "QUE LEGAL!");
    if (bl_passo % 2) TXT(1, constrain(bl_px + larg / 2, 0, LCD_COLS - 2), "<3");
  } else {
    poe(BOLA_ARCO[bl_arco], bl_x, 7);
    if (bl_boing) TXT(0, 8, "BOING!");
  }
  desenhaQuadro(bl_passo);
  return false;
}


// ############################################################################################
// DORMIR — dormindo no fundo, "Zzz" e bolha de nariz; espia quem chega perto
// ############################################################################################
#define TD_TEMPO 250
#define TD_COL   5

unsigned int  td_passo = 0;
unsigned long td_t = 0;
unsigned long td_espiarAte = 0;         // até quando fica de olho aberto, espiando
unsigned long td_avisoAte = 0;          // até quando mostra "SHHH" (apertaram botão)
bool td_olhoAberto = false;

void dormir_espiar()  { td_espiarAte = millis() + 2000; }
void dormir_avisar()  { td_avisoAte = millis() + 2000; }

void td_carrega() { pet_carrega(true, td_olhoAberto ? QUADRO_A : QUADRO_SONO); }

void dormir_iniciar() {
  td_olhoAberto = false;
  td_carrega();
  pet_carregaFundo(0);
  td_passo = 0; td_t = 0; td_espiarAte = td_avisoAte = 0;
}

void dormir_atualizar() {
  if (millis() - td_t < TD_TEMPO) return;
  td_t = millis();
  td_passo++;

  bool espiando = millis() < td_espiarAte;
  if (espiando != td_olhoAberto) { td_olhoAberto = espiando; td_carrega(); }
  if (petTipo == PET_TUBARAO) carregaCauda(true, (td_passo / 4) % 2);   // respiração lenta
  if (td_passo % 16 == 0) td_carrega();
  if (td_passo % 2 == 0) pet_carregaFundo(td_passo / 2);

  limpaTela();
  pet_poeFundo();
  pet_poe(3, TD_COL, true);
  int fim = TD_COL + pet_larg();                       // primeira coluna depois do bichinho

  if (millis() < td_avisoAte) {
    TXT(0, 0, "SHHH! DORMINDO");
  } else if (espiando) {
    poe(2, fim, '?');
  } else {
    byte f = td_passo % 16;                            // bolha de nariz
    byte b = f < 4 ? '.' : (f < 8 ? 'o' : (f < 12 ? 'O' : (f == 12 ? '*' : ' ')));
    poe(3, fim, b);
    byte z = td_passo % 14;                            // "Zzz" subindo
    if (z >= 1 && z <= 11) poe(2, fim + 1, 'z');
    if (z >= 4 && z <= 11) poe(1, fim + 2, 'z');
    if (z >= 7 && z <= 11) poe(0, fim + 4, 'Z');
  }
  desenhaQuadro(td_passo);
}


// ############################################################################################
// TRANSIÇÃO — adormecer e despertar (cerca de 3 s). Devolve true ao terminar.
// ############################################################################################
enum { TR_ADORMECER, TR_DESPERTAR };
#define TR_QUADROS 12

byte tr_tipo = TR_ADORMECER;
byte tr_passo = 0;
unsigned long tr_t = 0;

void transicao_iniciar(byte tipo) {
  tr_tipo = tipo; tr_passo = 0; tr_t = 0;
  pet_carrega(true, tipo == TR_DESPERTAR ? QUADRO_SONO : QUADRO_A);
  pet_carregaFundo(0);
}

bool transicao_atualizar() {
  if (tr_passo >= TR_QUADROS) return true;
  if (millis() - tr_t < 250) return false;
  tr_t = millis();
  tr_passo++;

  limpaTela();
  pet_poeFundo();
  pet_poe(3, TD_COL, true);
  int fim = TD_COL + pet_larg();
  if (tr_tipo == TR_ADORMECER) {
    if (petTipo == PET_TUBARAO) carregaCauda(true, tr_passo % 4 < 2);
    if (tr_passo <= 5)       TXT(1, TD_COL + 4, "UAAAH...");
    else if (tr_passo == 6)  pet_carrega(true, QUADRO_SONO);   // fecha o olho
    else                     poe(2, fim + 1, 'z');
  } else {
    pet_anima(true, tr_passo % 2);                     // espreguiçando
    if (tr_passo == 5) pet_carrega(true, QUADRO_A);    // abre o olho
    if (tr_passo >= 5 && tr_passo <= 7) poe(1, pet_frente(TD_COL, true), '!');
    if (tr_passo >= 8) TXT(0, 6, "BOM DIA!");
  }
  desenhaQuadro(tr_passo);
  return false;
}


// ############################################################################################
// BANHO — ENTER faz o bichinho pular na banheira; o banho dura uns segundos e devolve true
// ############################################################################################
// Esperando: 7 espuma | Entrando: bichinho + 7 espuma
// No banho (tubarão): 0 barbatana | 1-5 metade de cima do corpo "cortada" pela borda
// No banho (sapinho e recruta): 0-2 cabeça para fora da espuma
// Sempre: 6 espuma | 7 chuveiro
const byte BORDA[3]      PROGMEM = {B11111,B00000,B11111};
const byte ESPUMA_A[5]   PROGMEM = {B00000,B01100,B10010,B10001,B01110};
const byte ESPUMA_B[5]   PROGMEM = {B00110,B01001,B10001,B01110,B00000};
const byte CHUVEIRO_A[8] PROGMEM = {B11100,B00100,B01110,B11111,B00000,B01010,B00000,B10101};
const byte CHUVEIRO_B[8] PROGMEM = {B11100,B00100,B01110,B11111,B00000,B10101,B00000,B01010};

#define COL_TUB_BANHO   8
#define BANHO_QUADROS   20               // quadros de banho (300 ms cada) até ficar limpinho
const int  ENTRADA_X[] = {-6, -5, -4, -3, -1,  1,  3,  5,  7};
const byte ENTRADA_L[] = { 1,  1,  1,  1,  0,  0,  0,  0,  1};
#define ENTRADA_QUADROS 9

byte bn_estado = 0, bn_quadro = 0, bn_splash = 0, bn_noBanho = 0;
unsigned int  bn_passo = 0;
unsigned long bn_t = 0;

void banho_metade(byte slot, const byte* cima) {
  byte t[8];
  for (byte i = 0; i < 5; i++) t[i] = pgm_read_byte(cima + i);
  for (byte i = 0; i < 3; i++) t[5 + i] = pgm_read_byte(BORDA + i);
  lcd.createChar(slot, t);
}

void banho_carregaPet() {
  if (petTipo != PET_TUBARAO) { pet_carrega(true, QUADRO_A); return; }
  criaChar(0, T_BARBATANA, false);
  banho_metade(1, T_CORPO1);
  banho_metade(2, T_CORPO2);
  banho_metade(3, T_CORPO_MEIO);
  banho_metade(4, T_CORPO3);
  banho_metade(5, T_CORPO4);
}

void banho_iniciar() { bn_estado = 0; bn_quadro = 0; bn_splash = 0; bn_noBanho = 0; bn_passo = 0; bn_t = 0; }

void banho_entrar() {
  if (bn_estado != 0) return;
  bn_estado = 1; bn_quadro = 0;
  pet_carrega(true, QUADRO_A);
  bn_t = 0;
}

void banho_desenhaBanheira(byte slotEspuma) {
  for (byte c = 3; c <= 16; c++) poe(1, c, slotEspuma);
  poe(2, 4, '|'); poe(2, 15, '|');
  poe(3, 4, '|'); poe(3, 15, '|');
  for (byte c = 5; c <= 14; c++) poe(3, c, '_');
}

bool banho_atualizar() {
  if (bn_noBanho >= BANHO_QUADROS) return true;
  if (millis() - bn_t < (bn_estado == 1 ? 180 : 300)) return false;
  bn_t = millis();
  bn_passo++;

  limpaTela();
  TXT(0, 0, "BANHO");

  if (bn_estado == 0) {
    banho_metade(7, (bn_passo % 2) ? ESPUMA_A : ESPUMA_B);
    banho_desenhaBanheira(7);
    if (bn_passo % 4 < 3) TXT(0, 13, "[ENTER]");
  } else if (bn_estado == 1) {
    banho_metade(7, (bn_passo % 2) ? ESPUMA_A : ESPUMA_B);
    pet_anima(true, bn_passo % 2);
    banho_desenhaBanheira(7);
    int x = ENTRADA_X[bn_quadro];
    if (petTipo != PET_TUBARAO) x += 2;                // bichinhos menores caem no meio da banheira
    pet_poe(ENTRADA_L[bn_quadro] + (petTipo == PET_TUBARAO ? 0 : 1), x, true);
    if (bn_quadro == ENTRADA_QUADROS - 1) { poe(0, COL_TUB_BANHO - 1, '*'); poe(0, COL_TUB_BANHO + 5, '*'); }
    if (++bn_quadro >= ENTRADA_QUADROS) { bn_estado = 2; bn_splash = 4; banho_carregaPet(); }
  } else {
    bn_noBanho++;
    banho_metade(6, (bn_passo % 2) ? ESPUMA_A : ESPUMA_B);
    criaChar(7, (bn_passo % 2) ? CHUVEIRO_A : CHUVEIRO_B, false);
    if (bn_passo % 8 == 0) banho_carregaPet();
    poe(0, 6, 7);
    banho_desenhaBanheira(6);
    if (petTipo == PET_TUBARAO) {
      for (byte c = 0; c < 5; c++) poe(1, COL_TUB_BANHO + c, 1 + c);
      poe(0, COL_TUB_BANHO + 1, 0);
    } else {
      pet_poeTopo(0, COL_TUB_BANHO + 1, true);         // só a cabeça para fora da espuma
    }
    if (bn_splash > 0) {
      bn_splash--;
      const byte resp[2] = {'o', '*'};
      poe(0, COL_TUB_BANHO - 1, resp[bn_splash % 2]);
      poe(0, COL_TUB_BANHO + 5, resp[(bn_splash + 1) % 2]);
    }
    const byte colDentro[4] = {6, 9, 11, 13};
    for (byte i = 0; i < 4; i++) {
      byte f = (bn_passo + i * 3) % 6;
      if (f == 0) poe(2, colDentro[i], '.');
      if (f == 1) poe(2, colDentro[i], 'o');
    }
    if (bn_splash == 0) {
      const byte formas[4] = {'.', 'o', '*', ' '};
      const byte colFora[3] = {13, 15, 17};
      for (byte i = 0; i < 3; i++) poe(0, colFora[i], formas[(bn_passo + i) % 4]);
    }
  }
  desenhaQuadro(bn_passo);
  return false;
}


// ############################################################################################
// MINIGAME — ENTER pula os obstáculos; jg_acabouAgora fica true no ciclo em que bate
// ############################################################################################
const byte FORMA_BOLHA[2] = {'.', 0xDF};
#define COL_TUBARAO 1
#define FRAMES_PULO 7

byte jg_fim = 0;
bool jg_acabouAgora = false;
int  jg_pulo = -1;
int  jg_obst[3];
int  jg_bolC[2], jg_bolL[2];
unsigned int  jg_passo, jg_espera, jg_periodo, jg_pontos;
unsigned long jg_t, jg_tFim;

void minigame_reinicia() {
  jg_fim = 0; jg_pulo = -1; jg_pontos = 0;
  for (byte i = 0; i < 3; i++) jg_obst[i] = -1;
  jg_bolC[0] = 12;           jg_bolL[0] = 1;
  jg_bolC[1] = LCD_COLS - 1; jg_bolL[1] = 2;
  jg_passo = 0; jg_espera = 10; jg_periodo = 150; jg_t = 0;
}

void minigame_iniciar() { pet_carrega(true, QUADRO_A); minigame_reinicia(); }

bool minigame_podeReiniciar() { return jg_fim && millis() - jg_tFim > 1000; }

void minigame_pular() {
  if (jg_fim) { if (minigame_podeReiniciar()) minigame_reinicia(); }
  else if (jg_pulo < 0) jg_pulo = 0;
}

void minigame_atualizar() {
  jg_acabouAgora = false;
  if (millis() - jg_t < jg_periodo) return;
  jg_t = millis();
  if (jg_fim) return;
  jg_passo++;

  jg_pontos = jg_passo / 2;
  jg_periodo = constrain(150 - (int)jg_pontos, 70, 150);

  for (byte i = 0; i < 3; i++) if (jg_obst[i] >= 0) jg_obst[i]--;
  if (--jg_espera == 0) {
    for (byte i = 0; i < 3; i++) if (jg_obst[i] < 0) { jg_obst[i] = LCD_COLS - 1; break; }
    jg_espera = random(12, 20);
  }
  if (jg_passo % 2 == 0) {
    for (byte i = 0; i < 2; i++) {
      jg_bolC[i]--;
      if (jg_bolC[i] < 0) { jg_bolC[i] = LCD_COLS - 1; jg_bolL[i] = random(1, 3); }
    }
  }

  bool noAr = (jg_pulo >= 0);
  for (byte i = 0; i < 3; i++) {
    if (!noAr && jg_obst[i] >= COL_TUBARAO + 1 && jg_obst[i] <= COL_TUBARAO + (int)pet_larg() - 1) {
      jg_fim = 1; jg_tFim = millis(); jg_acabouAgora = true;
    }
  }

  limpaTela();
  TXT(0, 0, "MINIGAME");
  char buf[7];
  buf[0] = 'R'; numero(buf + 1, pet.recorde > jg_pontos ? pet.recorde : jg_pontos, 5);
  texto(0, 9, buf);
  numero(buf, jg_pontos, 5);
  texto(0, LCD_COLS - 5, buf);
  if (petTipo == PET_TUBARAO) for (byte i = 0; i < 2; i++) poe(jg_bolL[i], jg_bolC[i], FORMA_BOLHA[i]);
  pet_carregaFundo((jg_passo / 2) % 4);
  for (byte i = 0; i < 3; i++) if (jg_obst[i] >= 0) poe(3, jg_obst[i], 7);
  pet_anima(true, jg_passo % 2);
  pet_poe(noAr ? 2 : 3, COL_TUBARAO, true);
  if (jg_fim) { TXT(1, 5, "GAME OVER"); TXT(2, 3, "ENTER: DE NOVO"); }
  desenhaQuadro(jg_passo);

  if (jg_pulo >= 0) { jg_pulo++; if (jg_pulo >= FRAMES_PULO) jg_pulo = -1; }
}


// ############################################################################################
// INTERAÇÃO — pega-pega com o outro robozinho (visita); depois de 4 pegadas a cena devolve true
// ############################################################################################
// Só existe com CONEXAO_AMIGO. Cada um vê o seu tubarão pequeno (slots 0-3) e o do amigo (4-7).
#if CONEXAO == CONEXAO_AMIGO
#define IC_MAX_TUB (LCD_COLS - 5)

// estados: 0 entram | 1 pega-pega | 2 "PEGOU!" | 3 amigos
int  ic_sx, ic_fx;                      // coluna esquerda do meu tubarão (linha 2) e do amigo (linha 3)
bool ic_sdir, ic_fdir, ic_tubPega;
byte ic_estado, ic_cont, ic_rodadas;
unsigned int  ic_passo;
unsigned long ic_t;

void inter_iniciar() {
  ic_sx = -5; ic_fx = LCD_COLS;
  ic_sdir = true; ic_fdir = false; ic_tubPega = true;
  ic_estado = 0; ic_cont = 0; ic_rodadas = 0;
  ic_passo = 0; ic_t = 0;
}

void inter_foge(bool tubFoge) {
  if (tubFoge) {
    int d = (ic_sx + 2 >= ic_fx) ? 1 : -1;
    if (ic_sx + d < 0 || ic_sx + d > IC_MAX_TUB) d = -d;
    ic_sdir = (d > 0); ic_sx += d;
  } else {
    int d = (ic_fx >= ic_sx + 2) ? 1 : -1;
    if (ic_fx + d < 0 || ic_fx + d > IC_MAX_TUB) d = -d;
    ic_fdir = (d > 0); ic_fx += d;
  }
}

void inter_persegue() {                 // perseguidor anda 1 coluna por quadro, fugitivo a cada 2
  if (ic_tubPega) {
    ic_sdir = (ic_fx > ic_sx + 2); ic_sx += ic_sdir ? 1 : -1;
    if (ic_passo % 2 == 0) inter_foge(false);
  } else {
    ic_fdir = (ic_sx + 2 > ic_fx); ic_fx += ic_fdir ? 1 : -1;
    if (ic_passo % 2 == 0) inter_foge(true);
  }
}

bool inter_encostou() { return ic_sx <= ic_fx + 4 && ic_fx <= ic_sx + 4; }

bool inter_atualizar() {
  if (millis() - ic_t < 200) return false;
  ic_t = millis();
  ic_passo++;

  if (ic_estado == 0) {
    if (ic_sx < 2) ic_sx++;
    if (ic_fx > IC_MAX_TUB - 4) ic_fx--;
    if (ic_sx >= 2 && ic_fx <= IC_MAX_TUB - 4) ic_estado = 1;
  } else if (ic_estado == 1) {
    inter_persegue();
    if (inter_encostou()) { ic_estado = 2; ic_cont = 0; ic_rodadas++; ic_tubPega = !ic_tubPega; }
  } else if (ic_estado == 2) {
    inter_foge(!ic_tubPega);
    if (++ic_cont >= 5) { ic_cont = 0; ic_estado = (ic_rodadas >= 4) ? 3 : 1; }
  } else if (ic_estado == 3) {
    if (++ic_cont >= 12) return true;
  }

  limpaTela();
  texto(0, 0, pet_nome()); TXT(0, 5, "x"); texto(0, 7, amigoNome);
  carregaTubaraoP(0, ic_sdir, ic_passo % 2);
  poeTubaraoP(0, 2, ic_sx, ic_sdir);
  carregaTubaraoP(4, ic_fdir, (ic_passo + 1) % 2);
  poeTubaraoP(4, 3, ic_fx, ic_fdir);

  if (ic_estado == 1) {
    if (ic_tubPega) poe(1, ic_sdir ? ic_sx + 4 : ic_sx, '!');
    else            poe(1, ic_fdir ? ic_fx + 4 : ic_fx, '!');
  }
  if (ic_estado == 2) TXT(0, 12, "PEGOU!");
  if (ic_estado == 3) TXT(0, 12, "AMIGOS<3");
  desenhaQuadro(ic_passo);
  return false;
}
#endif


// ############################################################################################
// SENSOR — o bichinho se aproxima conforme a distância
// ############################################################################################
// Tubarão: 0 mini | 1 pequeno (3x2) | 2 médio (4x3) | 3 grande (4x3) | 4 close (olhos e boca)
// Sapinho e recruta: de longe o corpo inteiro; de perto (DIST_GRANDE) o rostinho
const byte CAUDA_FRENTE[3][8] PROGMEM = {
  {B00011,B00110,B01100,B11000,B11000,B01100,B00110,B00011},
  {B00001,B00011,B00110,B11100,B11000,B01100,B00110,B00000},
  {B00000,B00110,B01100,B11000,B11100,B00110,B00011,B00001}
};
const byte SEQ_CAUDA[4] = {0, 1, 0, 2};

const byte E0_G[8] PROGMEM = {B00010,B00110,B01001,B10001,B10101,B11111,B10101,B01110};

const byte E1_N = 6;
const byte E1_G[6][8] PROGMEM = {
  {B00000,B00000,B00000,B00000,B00001,B00111,B01000,B10000},
  {B00000,B01000,B11000,B10100,B00100,B00110,B00001,B00000},
  {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B10000},
  {B10010,B10010,B10000,B10011,B10010,B10001,B01000,B00111},
  {B00100,B00100,B00000,B11100,B10100,B11000,B00001,B11110},
  {B00011,B00110,B01100,B11000,B11000,B01100,B00110,B00011},
};
const byte E1_POS[6][2] = {{0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {1,2}};
const byte E1_CAUDA = 5;

const byte E2_N = 8;
const byte E2_G[8][8] PROGMEM = {
  {B00000,B00000,B00000,B00010,B00110,B01010,B01001,B10001},
  {B00001,B00010,B00100,B01000,B01001,B01001,B01000,B01001},
  {B10001,B00000,B00000,B00000,B00000,B00000,B00000,B11111},
  {B10000,B01000,B00100,B00010,B10010,B10010,B00010,B10010},
  {B00011,B00110,B01100,B11000,B11000,B01100,B00110,B00011},
  {B01001,B01000,B01000,B00100,B01011,B01100,B00000,B00000},
  {B01010,B10001,B01110,B00000,B00000,B11111,B00000,B00000},
  {B10010,B00010,B00010,B00100,B11010,B00110,B00000,B00000},
};
const byte E2_POS[8][2] = {{0,1}, {1,0}, {1,1}, {1,2}, {1,3}, {2,0}, {2,1}, {2,2}};
const byte E2_CAUDA = 4;
const byte E2_PISCA_SLOT[2] = {1, 3};
const byte E2_PISCA_G[2][8] PROGMEM = {
  {B00001,B00010,B00100,B01000,B01000,B01001,B01000,B01001},
  {B10000,B01000,B00100,B00010,B00010,B10010,B00010,B10010}
};

const byte E3_N = 8;
const byte E3_G[8][8] PROGMEM = {
  {B00000,B00010,B00110,B01010,B01001,B10001,B10001,B10001},
  {B00011,B00100,B01000,B10000,B10001,B10001,B10000,B10011},
  {B10001,B00000,B00000,B00000,B00000,B00000,B00000,B11111},
  {B11000,B00100,B00010,B00001,B10001,B10001,B00001,B11001},
  {B00011,B00110,B01100,B11000,B11000,B01100,B00110,B00011},
  {B10010,B10010,B10001,B10000,B01000,B10110,B11001,B00000},
  {B10101,B00000,B01010,B11111,B00000,B00000,B11111,B00000},
  {B01001,B01001,B10001,B00001,B00010,B01101,B10011,B00000},
};
const byte E3_POS[8][2] = {{0,1}, {1,0}, {1,1}, {1,2}, {1,3}, {2,0}, {2,1}, {2,2}};
const byte E3_CAUDA = 4;
const byte E3_PISCA_SLOT[2] = {1, 3};
const byte E3_PISCA_G[2][8] PROGMEM = {
  {B00011,B00100,B01000,B10000,B10000,B10001,B10000,B10011},
  {B11000,B00100,B00010,B00001,B00001,B10001,B00001,B11001}
};

#define COL_CENTRO 8

byte sn_estagio = 0, sn_espera = 0, sn_carregado = 255;
bool sn_piscaCarregado = false;
bool sn_coracoes = false;               // true no ciclo em que os corações aparecem pela 1ª vez
bool sn_coracoesVistos = false;
unsigned int  sn_passo = 0;
unsigned long sn_t = 0;

byte        sensor_n(byte e)           { return e == 1 ? E1_N : (e == 2 ? E2_N : E3_N); }
const byte* sensor_g(byte e, byte i)   { return e == 1 ? E1_G[i] : (e == 2 ? E2_G[i] : E3_G[i]); }
const byte* sensor_pos(byte e, byte i) { return e == 1 ? E1_POS[i] : (e == 2 ? E2_POS[i] : E3_POS[i]); }
byte        sensor_cauda(byte e)       { return e == 1 ? E1_CAUDA : (e == 2 ? E2_CAUDA : E3_CAUDA); }

void sensor_carrega(byte e, bool pisca) {
  if (e == sn_carregado && pisca == sn_piscaCarregado) return;
  sn_carregado = e; sn_piscaCarregado = pisca;
  if (e == 0) {
    criaChar(0, E0_G, false);
  } else if (e <= 3) {
    for (byte i = 0; i < sensor_n(e); i++) if (i != sensor_cauda(e)) criaChar(i, sensor_g(e, i), false);
    if (pisca && e == 2) for (byte i = 0; i < 2; i++) criaChar(E2_PISCA_SLOT[i], E2_PISCA_G[i], false);
    if (pisca && e == 3) for (byte i = 0; i < 2; i++) criaChar(E3_PISCA_SLOT[i], E3_PISCA_G[i], false);
  } else {
    pet_carregaRosto(pisca);
  }
}

void sensor_iniciar() {
  sn_estagio = 0; sn_espera = 0; sn_carregado = 255; sn_passo = 0; sn_t = 0;
  sn_coracoes = sn_coracoesVistos = false;
}

void sensor_coracoes(int distanciaCm) {
  if (distanciaCm <= DIST_CORACAO && sn_passo % 6 < 4) {
    if (!sn_coracoesVistos) { sn_coracoesVistos = true; sn_coracoes = true; }
    TXT(0, 9, "<3");
    if ((sn_passo / 2) % 2) { TXT(1, 2, "<3"); TXT(1, 16, "<3"); }
  }
}

// sapinho e recruta: corpo inteiro de longe, rostinho de perto
void sensor_atualizarPet(int distanciaCm) {
  byte alvo = distanciaCm <= DIST_GRANDE ? 4 : 1;
  if (alvo != sn_estagio) {
    if (++sn_espera >= 2) { sn_estagio = alvo; sn_espera = 0; }
  } else sn_espera = 0;

  bool pisca = (sn_passo % 20) < 2;
  if (sn_estagio == 4) sensor_carrega(4, pisca);
  else if (sn_carregado != 1) { sn_carregado = 1; pet_carrega(true, QUADRO_A); }
  else pet_anima(true, (sn_passo / 2) % 2);

  limpaTela();
  TXT(0, 0, "SENSOR");
  if (sn_estagio == 4) { pet_poeRosto(); sensor_coracoes(distanciaCm); }
  else pet_poe((sn_passo / 4) % 2 ? 2 : 3, COL_CENTRO, true);
  desenhaQuadro(sn_passo);
}

void sensor_atualizar(int distanciaCm) {
  sn_coracoes = false;
  if (millis() - sn_t < 150) return;
  sn_t = millis();
  sn_passo++;
  if (petTipo != PET_TUBARAO) { sensor_atualizarPet(distanciaCm); return; }

  byte alvo = 0;
  if (distanciaCm <= DIST_CLOSE)        alvo = 4;
  else if (distanciaCm <= DIST_GRANDE)  alvo = 3;
  else if (distanciaCm <= DIST_MEDIO)   alvo = 2;
  else if (distanciaCm <= DIST_PEQUENO) alvo = 1;
  if (alvo != sn_estagio) {             // 1 tamanho por vez, só se o pedido durar 2 quadros
    if (++sn_espera >= 2) { sn_estagio += (alvo > sn_estagio) ? 1 : -1; sn_espera = 0; }
  } else sn_espera = 0;

  bool pisca = (sn_passo % 20) < 2;
  sensor_carrega(sn_estagio, pisca && sn_estagio >= 2);
  if (sn_estagio >= 1 && sn_estagio <= 3)
    criaChar(sensor_cauda(sn_estagio), CAUDA_FRENTE[SEQ_CAUDA[sn_passo % 4]], false);

  limpaTela();
  TXT(0, 0, "SENSOR");
  bool sobe = (sn_passo / 4) % 2;
  if (sn_estagio == 0) {
    poe(sobe ? 2 : 3, COL_CENTRO + 1, 0);
  } else if (sn_estagio == 1) {
    int l = sobe ? 1 : 2;
    for (byte i = 0; i < E1_N; i++) poe(l + E1_POS[i][0], COL_CENTRO + E1_POS[i][1], i);
  } else if (sn_estagio <= 3) {
    for (byte i = 0; i < sensor_n(sn_estagio); i++)
      poe(1 + sensor_pos(sn_estagio, i)[0], COL_CENTRO + sensor_pos(sn_estagio, i)[1], i);
  } else {
    pet_poeRosto();
    sensor_coracoes(distanciaCm);
  }
  desenhaQuadro(sn_passo);
}


// ############################################################################################
// STATUS — barras das necessidades, vidas, temperatura e amigo
// ############################################################################################
// Linha 0: nome, vidas (corações), temperatura e clima
// Linhas 1-3: FOM (fome), FEL (felicidade), HIG (higiene), ENE (energia), SAU (saúde), amigo/app
const byte VIDA_CHEIA[8] PROGMEM = {B00000,B01010,B11111,B11111,B01110,B00100,B00000,B00000};
const byte VIDA_VAZIA[8] PROGMEM = {B00000,B01010,B10101,B10001,B01010,B00100,B00000,B00000};
unsigned long st_t = 0;
unsigned int st_passo = 0;

void status_iniciar() {
  criaChar(0, VIDA_CHEIA, false);
  criaChar(1, VIDA_VAZIA, false);
  st_t = 0; st_passo = 0;
}

void status_barra(byte l, byte c, const char* rotulo, byte v) {
  texto_P(l, c, rotulo);
  byte cheias = (v + 10) / 20;                         // 5 blocos, cada um vale 20
  for (byte i = 0; i < 5; i++) poe(l, c + 4 + i, i < cheias ? 0xFF : '.');
  if (nivelDe(v) == HUMOR_CRITICO && (st_passo % 2)) for (byte i = 0; i < 3; i++) poe(l, c + i, ' ');
}

void status_atualizar() {
  if (millis() - st_t < 500) return;
  st_t = millis();
  st_passo++;

  limpaTela();
  texto(0, 0, pet_nome());
  for (byte i = 0; i < VIDAS_MAX; i++) poe(0, 5 + i, i < pet.vidas ? 0 : 1);
  char buf[8];
  temperaturaTexto(buf);
  texto(0, 10, buf);
  if (clima == CLIMA_FRIO)       TXT(0, 15, "FRIO");
  else if (clima == CLIMA_CALOR) TXT(0, 15, "CALOR");

  status_barra(1, 0,  PSTR("FOM"), pet.fome);
  status_barra(1, 10, PSTR("FEL"), pet.felicidade);
  status_barra(2, 0,  PSTR("HIG"), pet.higiene);
  status_barra(2, 10, PSTR("ENE"), pet.energia);
  status_barra(3, 0,  PSTR("SAU"), pet.saude);

  if (CONEXAO == CONEXAO_APP) {                        // lanches que chegaram do app
    TXT(3, 10, "LANCHE ");
    poe(3, 17, '0' + lanches);
  } else if (idRepetido)   TXT(3, 10, "MESMO ID!");
  else if (!amigoPresente) TXT(3, 10, "AMIGO --");
  else {
    texto(3, 10, amigoNome);
    if (amigoDormindo)                    TXT(3, 15, "zz");
    else if (amigoHumor == HUMOR_BEM)     TXT(3, 15, ":)");
    else if (amigoHumor == HUMOR_ATENCAO) TXT(3, 15, ":|");
    else                                  TXT(3, 15, ":(");
  }
  desenhaQuadro(st_passo);
}


// ############################################################################################
// MENSAGEM — duas linhas de texto; opcionalmente o bichinho saindo ou chegando
// ############################################################################################
enum { MSG_PARADO, MSG_SAINDO, MSG_CHEGANDO };

char msg_l1[LCD_COLS + 1] = "", msg_l2[LCD_COLS + 1] = "";
byte msg_anim = MSG_PARADO;
unsigned long msg_t0 = 0, msg_dur = 0, msg_t = 0;

void mensagem_iniciar(byte anim, unsigned long dur) {
  msg_anim = anim; msg_dur = dur; msg_t0 = millis(); msg_t = 0;
  if (anim != MSG_PARADO) pet_carrega(true, QUADRO_A);
}

bool mensagem_atualizar() {
  unsigned long passou = millis() - msg_t0;
  if (passou >= msg_dur) return true;
  if (millis() - msg_t < 120) return false;
  msg_t = millis();

  limpaTela();
  if (msg_anim == MSG_PARADO) {
    textoCentro(1, msg_l1);
    textoCentro(2, msg_l2);
  } else {
    textoCentro(0, msg_l1);
    textoCentro(1, msg_l2);
    int x = (int)(passou / 120);                       // 1 coluna a cada 120 ms
    int meio = 10 - pet_larg() / 2;
    if (msg_anim == MSG_SAINDO) x = meio + x;          // sai pela direita
    else x = min(-(int)pet_larg() + x, meio);          // entra pela esquerda e para no meio
    pet_anima(true, (passou / 120) % 2);
    pet_poe(3, x, true);
  }
  desenhaQuadro(passou / 120);
  return false;
}
