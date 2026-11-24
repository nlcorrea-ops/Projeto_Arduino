/*
  =============================================================================================
  ROBOZINHO — bichinho virtual (estilo Tamagotchi) para Arduino Uno R3 + LCD 20x4 I2C
  =============================================================================================
  Um bichinho de mesa que conversa com o aplicativo Desk Companions pelo Bluetooth (HC-06).
  O bichinho pode ser o tubarão, o sapinho ou o recruta: o app escolhe qual vem pelo portal.
  Pelo app também começam o Pomodoro e o cronômetro, e chegam as comidas compradas na loja.
  Sem o app, o robozinho funciona sozinho normalmente.

  Opcional: com CONEXAO_AMIGO (config.h), dois robozinhos se visitam pela serial em vez do app.
  O jumper no pino A3 define a personalidade:
    A3 solto        -> robô 1 "TUBA" (guloso)
    A3 ligado ao GND -> robô 2 "FINN" (brincalhão)

  Módulos (arquivos .h nesta pasta, incluídos na ordem abaixo):
    config.h    pinagem e todos os ajustes (limites de temperatura, luz, necessidades...)
    tela.h      LCD, tela virtual, caracteres personalizados, desenhos do tubarão
    som.h       buzzer ativo com sequências que não travam o programa
    botoes.h    4 botões com antirrepique
    leds.h      indicador de humor (verde, amarelo, vermelho)
    sensores.h  NTC (temperatura), LDR (luz) e HC-SR04 (presença)
    pet.h       necessidades, saúde, vidas, personalidade e EEPROM (uma ficha por bichinho)
    pets.h      desenhos do tubarão, do sapinho e do recruta
    link.h      comunicação serial pelos pinos D2/D3 (com o app ou com o outro robozinho)
    app.h       mensagens do aplicativo (portal, estudo e comida)
    cenas.h     todas as animações

  Botões:
    SETA ESQ / SETA DIR  na casa: abre o menu | no menu: navega | numa cena: ESQ volta ao menu
    ENTER                no menu: abre o item | numa cena: ação (pular, entrar no banho)
                         estudando no cronômetro: pausa e continua
    POWER                segurar 1 s: desliga (salva tudo) | apertar desligado: liga

  Bibliotecas: LiquidCrystal I2C (Frank de Brabander). Wire, EEPROM e SoftwareSerial já vêm
  com a Arduino IDE.
  =============================================================================================
*/

#include <Wire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#include "config.h"
#include "tela.h"
#include "som.h"
#include "botoes.h"
#include "leds.h"
#include "sensores.h"
#include "pet.h"
#include "pets.h"
#include "link.h"
#include "app.h"
#include "cenas.h"

// ---------------------------------------------------------------------------------------------
// Estados do aparelho
// ---------------------------------------------------------------------------------------------
enum {
  MODO_CASA,          // animação padrão (tela inicial)
  MODO_MENU,          // escolhendo um item
  MODO_CENA,          // uma cena aberta pelo menu
  MODO_MENSAGEM,      // aviso temporário; depois vai para msg_proxModo
  MODO_TRANSICAO,     // adormecendo ou acordando
  MODO_SONO,          // dormindo
  MODO_ESPERA,        // convidou o amigo e espera a resposta
  MODO_DESLIGADO,
  MODO_FIM,           // sem vidas
  MODO_FORA,          // o bichinho está no app
  MODO_PORTAL,        // viajando pelo portal
  MODO_ESTUDO,        // Pomodoro ou cronômetro
  MODO_FESTA          // rostinho comemorando
};
enum { CENA_STATUS, CENA_COMER, CENA_BANHO, CENA_BRINCAR, CENA_MINIGAME, CENA_SENSOR, CENA_VISITA };
enum { SONO_ESCURO, SONO_CANSACO, SONO_COCHILO };   // por que está dormindo

// itens do menu, na ordem em que aparecem (VISITAR só existe com CONEXAO_AMIGO)
enum { M_CASA, M_STATUS, M_COMER, M_BANHO, M_BRINCAR, M_MINIGAME, M_DORMIR, M_VISITAR, M_SENSOR, N_ITENS };
#define N_MENU (CONEXAO == CONEXAO_AMIGO ? N_ITENS : N_ITENS - 1)
byte menu_id(byte i) { return (CONEXAO != CONEXAO_AMIGO && i >= M_VISITAR) ? i + 1 : i; }

byte modo = MODO_CASA;
byte cena = CENA_STATUS;
byte motivoSono = SONO_ESCURO;
byte menuItem = M_STATUS;
byte msg_proxModo = MODO_CASA, msg_proxCena = 0;
bool visitaSouVisitante = false;        // true = eu fui visitar; false = recebi a visita
unsigned long tMenu = 0, tEspera = 0, tReenvio = 0, tAlerta = 0, tPresenca = 0;
bool alertaAtivo = false;

void terminouPortal();                  // usada antes de aparecer no código

// ---------------------------------------------------------------------------------------------
// Troca de tela
// ---------------------------------------------------------------------------------------------
void irEstudo() {
  modo = MODO_ESTUDO;
  telaToda = true;
  estudo_entrar();
}

void irFora() {
  modo = MODO_FORA;
  telaToda = true;
  fora_iniciar();
}

// "casa" depende de onde o bichinho está: estudando, no app ou aqui
void irCasa() {
  if (estudoAtivo()) { irEstudo(); return; }
  if (!petAqui) { irFora(); return; }
  modo = MODO_CASA;
  telaToda = true;
  casaPausa = false;
  casa_iniciar();
}

void irPortal(byte tipo) {
  modo = MODO_PORTAL;
  telaToda = true;
  app_onde = 'V';
  portal_iniciar(tipo);
}

void irFesta(const char* textoP, unsigned long dur) {
  modo = MODO_FESTA;
  telaToda = true;
  festa_iniciar(textoP, dur);
}

void abrirCena(byte c) {
  modo = MODO_CENA; cena = c;
  telaToda = true;
  switch (c) {
    case CENA_STATUS:   status_iniciar(); break;
    case CENA_COMER:    comer_iniciar(lanches > 0); break;
    case CENA_BANHO:    banho_iniciar(); break;
    case CENA_BRINCAR:  bola_iniciar(); break;
    case CENA_MINIGAME: minigame_iniciar(); break;
    case CENA_SENSOR:   sensor_iniciar(); break;
#if CONEXAO == CONEXAO_AMIGO
    case CENA_VISITA:   inter_iniciar(); break;
#endif
  }
}

// mostra duas linhas (textos na flash, use PSTR) e depois vai para "proximo"
void mensagem_P(const char* l1, const char* l2, byte anim, unsigned long dur, byte proximo, byte proxCena = 0) {
  strncpy_P(msg_l1, l1, LCD_COLS); msg_l1[LCD_COLS] = 0;
  strncpy_P(msg_l2, l2, LCD_COLS); msg_l2[LCD_COLS] = 0;
  msg_proxModo = proximo; msg_proxCena = proxCena;
  modo = MODO_MENSAGEM;
  telaToda = true;
  mensagem_iniciar(anim, dur);
}

// mesma coisa, com o nome do amigo antes ou depois do texto da 2ª linha
void mensagemAmigo_P(const char* l1, const char* antes, const char* depois, byte anim, unsigned long dur,
                     byte proximo, byte proxCena = 0) {
  mensagem_P(l1, antes, anim, dur, proximo, proxCena);
  strcat(msg_l2, amigoNome);
  strncat_P(msg_l2, depois, LCD_COLS - strlen(msg_l2));
}

void irMenu() {
  modo = MODO_MENU;
  telaToda = true;
  tMenu = millis();
}

void irDormir(byte motivo) {
  motivoSono = motivo;
  modo = MODO_TRANSICAO;
  telaToda = true;
  transicao_iniciar(TR_ADORMECER);
  som_tocar(SOM_DORMIR);
}

void acordar() {
  modo = MODO_TRANSICAO;
  telaToda = true;
  transicao_iniciar(TR_DESPERTAR);
  som_tocar(SOM_ACORDAR);
}

void desligar() {
  if (modo == MODO_CENA && cena == CENA_VISITA) link_comando('F', '0');
  if (modo == MODO_PORTAL) terminouPortal();          // termina a viagem antes de apagar
  pet_pausar();
  som_parar();
  leds_mostrar(HUMOR_APAGADO);
  lcd.clear();
  lcd.noBacklight();
  lk_ligado = false;                    // para de avisar o amigo: ele percebe que desligamos
  modo = MODO_DESLIGADO;
}

void ligar() {
  lcd.backlight();
  lk_ligado = true;
  pet_retomar();
  est_tSeg = millis();                                 // o estudo não conta enquanto desligado
  som_tocar(SOM_ENTRAR);
  if (pet_vivo()) irCasa(); else { modo = MODO_FIM; telaToda = true; }
}

// ---------------------------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------------------------
void menu_nome(byte i, char* s) {
  switch (menu_id(i)) {
    case M_CASA:     strcpy_P(s, PSTR("CASA")); break;
    case M_STATUS:   strcpy_P(s, PSTR("STATUS")); break;
    case M_COMER:    strcpy_P(s, PSTR("COMER")); break;
    case M_BANHO:    strcpy_P(s, PSTR("BANHO")); break;
    case M_BRINCAR:  strcpy_P(s, PSTR("BRINCAR")); break;
    case M_MINIGAME: strcpy_P(s, PSTR("MINIGAME")); break;
    case M_DORMIR:   strcpy_P(s, PSTR("DORMIR")); break;
    case M_VISITAR:  strcpy_P(s, PSTR("VISITAR")); break;
    default:         strcpy_P(s, PSTR("SENSOR")); break;
  }
}

void menu_descricao(byte i, char* s) {
  switch (menu_id(i)) {
    case M_CASA:     strcpy_P(s, PSTR("voltar pra casa")); break;
    case M_STATUS:   strcpy_P(s, PSTR("ver necessidades")); break;
    case M_COMER:    strcpy_P(s, lanches ? PSTR("tem lanche do app!") : PSTR("hora de comer")); break;
    case M_BANHO:    strcpy_P(s, PSTR("tomar banho")); break;
    case M_BRINCAR:  strcpy_P(s, PSTR("jogar bolinha")); break;
    case M_MINIGAME: strcpy_P(s, PSTR("pular obstaculos")); break;
    case M_DORMIR:   strcpy_P(s, PSTR("tirar um cochilo")); break;
    case M_VISITAR:  strcpy_P(s, amigoPresente ? PSTR("ir ver o amigo") : PSTR("amigo desligado")); break;
    default:         strcpy_P(s, PSTR("chegue perto!")); break;
  }
}

void menu_desenha() {
  char s[LCD_COLS + 1];
  limpaTela();
  TXT(0, 0, "MENU");
  texto(0, 6, pet_nome());
  char* p = numero(s, menuItem + 1, 1); *p++ = '/'; numero(p, N_MENU, 1);
  texto(0, LCD_COLS - strlen(s), s);
  poe(1, 0, '<'); poe(1, LCD_COLS - 1, '>');
  menu_nome(menuItem, s);      textoCentro(1, s);
  menu_descricao(menuItem, s); textoCentro(2, s);
  TXT(3, 0, "ENTER abre");
  if (pet_humor() == HUMOR_CRITICO) TXT(3, 14, "!AJUDA");
  desenha();
}

void menu_abrir() {
  som_tocar(SOM_ENTRAR);
  byte item = menu_id(menuItem);
  switch (item) {
    case M_CASA:   irCasa(); break;
    case M_STATUS: abrirCena(CENA_STATUS); break;
    case M_COMER:
      if (pet.fome >= 95) { som_tocar(SOM_NEGADO); mensagem_P(PSTR("SEM FOME!"), PSTR("barriga cheia"), MSG_PARADO, 1800, MODO_MENU); }
      else abrirCena(CENA_COMER);
      break;
    case M_BANHO:  abrirCena(CENA_BANHO); break;
    case M_BRINCAR:
    case M_MINIGAME:
      if (pet.energia < NIVEL_CRITICO) { som_tocar(SOM_NEGADO); mensagem_P(PSTR("CANSADO DEMAIS"), PSTR("precisa dormir"), MSG_PARADO, 1800, MODO_MENU); }
      else abrirCena(item == M_BRINCAR ? CENA_BRINCAR : CENA_MINIGAME);
      break;
    case M_DORMIR: irDormir(SONO_COCHILO); break;
#if CONEXAO == CONEXAO_AMIGO
    case M_VISITAR:
      if (idRepetido) {
        som_tocar(SOM_NEGADO); mensagem_P(PSTR("AS DUAS PLACAS"), PSTR("TEM O MESMO ID (A3)"), MSG_PARADO, 2500, MODO_MENU);
      } else if (!amigoPresente) {
        som_tocar(SOM_NEGADO); mensagem_P(PSTR("AMIGO NAO"), PSTR("ENCONTRADO"), MSG_PARADO, 1800, MODO_MENU);
      } else if (amigoDormindo) {
        som_tocar(SOM_NEGADO); mensagemAmigo_P(PSTR("SHHH..."), PSTR(""), PSTR(" DORME"), MSG_PARADO, 1800, MODO_MENU);
      } else if (pet.energia < NIVEL_CRITICO) {
        som_tocar(SOM_NEGADO); mensagem_P(PSTR("CANSADO DEMAIS"), PSTR("precisa dormir"), MSG_PARADO, 1800, MODO_MENU);
      } else {
        link_comando('V', '0');
        modo = MODO_ESPERA; telaToda = true;
        tEspera = tReenvio = millis();
      }
      break;
#endif
    default: abrirCena(CENA_SENSOR); break;
  }
}

// ---------------------------------------------------------------------------------------------
// Fim das atividades: aplica os efeitos nas necessidades
// ---------------------------------------------------------------------------------------------
void terminouComer() {
  pet_ajusta(pet.fome, GANHO_COMER + perso->bonusComer);
  pet_ajusta(pet.felicidade, 3 + perso->bonusComer / 2);
  pet_ajusta(pet.higiene, -5);
  if (cm_lanche && lanches > 0) {                      // comida comprada no app: ganha um extra
    lanches--; pet_guardaLanches();
    pet_ajusta(pet.felicidade, GANHO_LANCHE);
  }
  som_tocar(SOM_FESTA);
  irCasa(); casa_balao_P(cm_lanche ? PSTR("DELICIA!") : PSTR("NHAM!"), 2500);
}

void terminouBanho() {
  pet_ajusta(pet.higiene, 100);
  pet_ajusta(pet.felicidade, 5);
  som_tocar(SOM_FESTA);
  irCasa(); casa_balao_P(PSTR("LIMPINHO!"), 2500);
}

void terminouMinigame() {
  unsigned int pts = jg_pontos;
  pet_ajusta(pet.felicidade, min((int)(pts / 5), 30) + perso->bonusBrincar);
  pet_ajusta(pet.energia, -CUSTO_BRINCAR);
  pet_ajusta(pet.fome, -3);
  if (pts > pet.recorde) { pet.recorde = pts; pet_mudou = true; som_tocar(SOM_FESTA); }
  else som_tocar(SOM_TRISTE);
}

void terminouBrincar() {
  pet_ajusta(pet.felicidade, GANHO_BRINCAR + perso->bonusBrincar);
  pet_ajusta(pet.energia, -CUSTO_BRINCAR);
  som_tocar(SOM_FESTA);
  irCasa(); casa_balao_P(PSTR("QUE LEGAL!"), 2500);
}

#if CONEXAO == CONEXAO_AMIGO
void terminouVisita(bool avisarAmigo) {
  if (avisarAmigo) link_comando('F', '0');
  pet_ajusta(pet.felicidade, GANHO_VISITA + perso->bonusBrincar);
  pet_ajusta(pet.energia, -CUSTO_BRINCAR);
  som_tocar(SOM_FESTA);
  if (visitaSouVisitante) mensagemAmigo_P(PSTR("VOLTANDO PRA CASA"), PSTR("tchau "), PSTR("!"), MSG_CHEGANDO, 2500, MODO_CASA);
  else                    mensagemAmigo_P(PSTR("FOI MUITO LEGAL!"), PSTR("tchau "), PSTR("!"), MSG_PARADO, 2000, MODO_CASA);
}
#endif

// fim da viagem pelo portal: avisa o app e vai para a tela certa
void terminouPortal() {
  if (pt_tipo == PORTAL_SAIDA) {
    petAqui = false; pet_pausar();
    app_onde = 'A'; app_responde('O', 0);
  } else {
    petAqui = true; pet_retomar();
    app_onde = 'R'; app_responde('I', 0);
  }
  if (!pet_vivo() && petAqui) { modo = MODO_FIM; telaToda = true; }
  else irCasa();
}

// ---------------------------------------------------------------------------------------------
// Eventos: necessidades, sensores e comunicação
// ---------------------------------------------------------------------------------------------
bool acordado() { return modo != MODO_SONO && modo != MODO_TRANSICAO && modo != MODO_DESLIGADO; }
bool emVisita() { return (modo == MODO_CENA && cena == CENA_VISITA) || modo == MODO_ESPERA; }

void trataEventoPet(byte ev) {
  if (ev == EV_FIM_DE_JOGO) {
    if (emVisita()) link_comando('F', '0');
    som_tocar(SOM_TRISTE);
    modo = MODO_FIM; telaToda = true;
  } else if (ev == EV_PERDEU_VIDA) {
    som_tocar(SOM_TRISTE);
    if (acordado() && !emVisita()) mensagem_P(PSTR("OH NAO!"), PSTR("perdeu uma vida"), MSG_PARADO, 2500, MODO_CASA);
  } else if (ev == EV_GANHOU_VIDA) {
    if (acordado()) som_tocar(SOM_FESTA);
    if (acordado() && !emVisita()) mensagem_P(PSTR("OBA!"), PSTR("ganhou uma vida"), MSG_PARADO, 2500, MODO_CASA);
  }
}

void trataSensores() {
  if (climaMudou && acordado() && petAqui) {
    som_tocar(clima == CLIMA_FRIO ? SOM_FRIO : (clima == CLIMA_CALOR ? SOM_CALOR : SOM_CLIQUE));
    if (modo == MODO_CASA) {
      casa_carregaFundo();
      if (clima == CLIMA_FRIO)       casa_balao_P(PSTR("BRRR! FRIO"), 2500);
      else if (clima == CLIMA_CALOR) casa_balao_P(PSTR("UFA! CALOR"), 2500);
      else                           casa_balao_P(PSTR("AH, BOM!"), 2000);
    }
  }

  if (pessoaChegou) {
    if (modo == MODO_SONO) dormir_espiar();
    else if (modo == MODO_CASA && millis() - tPresenca >= PRESENCA_PAUSA_MS) {
      tPresenca = millis();
      pet_ajusta(pet.felicidade, GANHO_PRESENCA);
      som_tocar(SOM_OI);
      casa_balao_P(PSTR("OI!"), 2500);
    }
  }

  // escuro: dorme (menos no meio de uma visita); energia no fim: dorme de cansaço
  if (escuro && (modo == MODO_CASA || modo == MODO_MENU || modo == MODO_CENA || modo == MODO_MENSAGEM) && !emVisita())
    irDormir(SONO_ESCURO);
  else if (modo == MODO_CASA && pet.energia < NIVEL_CRITICO)
    irDormir(SONO_CANSACO);

  // regras para acordar:
  //   dormiu porque escureceu -> acorda quando clareia (se a energia estiver no fim, vira cansaço)
  //   dormiu de cansaço       -> acorda com luz e energia >= ENERGIA_ACORDA
  //   cochilo pelo menu       -> acorda com luz e energia cheia, ou com um botão (em rodaModo)
  if (modo == MODO_SONO) {
    if (escuro && motivoSono == SONO_COCHILO) motivoSono = SONO_ESCURO;
    if (!escuro) {
      if (motivoSono == SONO_ESCURO) {
        if (pet.energia < NIVEL_CRITICO) motivoSono = SONO_CANSACO; else acordar();
      } else if (motivoSono == SONO_CANSACO && pet.energia >= ENERGIA_ACORDA) acordar();
      else if (motivoSono == SONO_COCHILO && pet.energia >= 100) acordar();
    }
  }
}

void trataLink() {
#if CONEXAO == CONEXAO_AMIGO
  if (amigoChegou && acordado() && (modo == MODO_CASA || modo == MODO_MENU)) {
    som_tocar(SOM_AMIGO);
    if (modo == MODO_CASA) { char s[12] = "OI "; strncat(s, amigoNome, 4); strcat(s, "!"); casa_balao(s, 3000); }
  }
  if (amigoSaiu && emVisita()) {
    som_tocar(SOM_TRISTE);
    mensagem_P(PSTR("CONEXAO PERDIDA"), PSTR("amigo sumiu..."), MSG_PARADO, 2500, MODO_CASA);
  }

  char cmd = link_pegaComando();
  if (modo == MODO_DESLIGADO) return;
  if (cmd == 'V') {                                    // convite do amigo
    if (modo == MODO_CENA && cena == CENA_VISITA) link_comando('A', '0');   // já estamos brincando
    else if (modo == MODO_CASA || modo == MODO_MENU) {
      link_comando('A', '0');
      visitaSouVisitante = false;
      som_tocar(SOM_AMIGO);
      mensagemAmigo_P(PSTR("VISITA!"), PSTR(""), PSTR(" CHEGOU"), MSG_PARADO, 2000, MODO_CENA, CENA_VISITA);
    } else {
      link_comando('R', (modo == MODO_SONO || modo == MODO_TRANSICAO) ? 'D' : 'O');
    }
  } else if (cmd == 'A' && modo == MODO_ESPERA) {
    visitaSouVisitante = true;
    som_tocar(SOM_AMIGO);
    mensagemAmigo_P(PSTR("INDO VISITAR"), PSTR(""), PSTR("..."), MSG_SAINDO, 2200, MODO_CENA, CENA_VISITA);
  } else if (cmd == 'R' && modo == MODO_ESPERA) {
    som_tocar(SOM_NEGADO);
    if (lk_motivo == 'D') mensagemAmigo_P(PSTR("SHHH..."), PSTR(""), PSTR(" DORME"), MSG_PARADO, 2000, MODO_CASA);
    else                  mensagemAmigo_P(PSTR("AGORA NAO DA"), PSTR(""), PSTR(" OCUPADO"), MSG_PARADO, 2000, MODO_CASA);
  } else if (cmd == 'F' && modo == MODO_CENA && cena == CENA_VISITA) {
    terminouVisita(false);
  }
#endif
}

// pedidos do aplicativo (o app repete o pedido até receber a confirmação)
void trataApp() {
#if CONEXAO == CONEXAO_APP
  char p = app_pegaPedido();
  if (!p) return;

  if (p == 'I') {                                      // bichinho chegando pelo portal
    if (modo == MODO_PORTAL) return;                   // já está viajando
    byte tipo = app_petDaLetra(app_dado);
    if (petAqui) { app_responde(tipo == petTipo ? 'I' : 'N', 0); return; }   // já tem bichinho aqui
    pet_trocar(tipo);
    irPortal(PORTAL_CHEGADA);

  } else if (p == 'O') {                               // bichinho indo para o app
    if (modo == MODO_PORTAL) return;
    if (!petAqui) { app_responde('O', 0); return; }
    if (modo == MODO_FIM) { app_responde('N', 0); return; }
    if (emVisita()) link_comando('F', '0');
    irPortal(PORTAL_SAIDA);

  } else if (p == 'E') {                               // começar a estudar
    if (modo == MODO_FIM) { app_responde('N', 0); return; }
    char m = app_dado == 'C' ? 'C' : 'P';
    if (est_modo != m) {
      estudo_comecar(m);
      if (modo != MODO_PORTAL) irFesta(PSTR("VAMOS ESTUDAR!"), 2500);
    }
    app_responde('E', m);

  } else if (p == 'X') {                               // encerrar o estudo
    if (estudoAtivo()) {
      estudo_parar();
      if (modo == MODO_ESTUDO || modo == MODO_FESTA) irFesta(PSTR("MANDOU BEM!"), 2500);
    }
    app_responde('X', 0);

  } else if (p == 'C') {                               // comida comprada na loja
    byte id = app_dado2;
    if (id != app_ultimoLanche && app_dado >= '1' && app_dado <= '9') {
      app_ultimoLanche = id;
      lanches = min(9, lanches + (app_dado - '0'));
      pet_guardaLanches();
      if (modo == MODO_CASA) { som_tocar(SOM_OI); casa_balao_P(PSTR("OBA, LANCHE!"), 2500); }
    }
    app_responde('C', id);
  }
#endif
}

void trataAlerta() {
  bool critico = pet_humor() == HUMOR_CRITICO && acordado() && modo != MODO_FIM && petAqui;
  if (critico && (!alertaAtivo || millis() - tAlerta >= ALERTA_REPETE_MS)) {
    som_tocar(SOM_ALERTA);
    tAlerta = millis();
  }
  alertaAtivo = critico;
}

// ---------------------------------------------------------------------------------------------
// Cada modo: botões e animação
// ---------------------------------------------------------------------------------------------
void rodaModo() {
  switch (modo) {
    case MODO_CASA:
      if (algum_botao()) { som_tocar(SOM_CLIQUE); irMenu(); break; }
      casa_atualizar();
      break;

    case MODO_MENU:
      if (botao_apertou(BTN_ESQ)) { menuItem = (menuItem + N_MENU - 1) % N_MENU; som_tocar(SOM_CLIQUE); tMenu = millis(); }
      if (botao_apertou(BTN_DIR)) { menuItem = (menuItem + 1) % N_MENU; som_tocar(SOM_CLIQUE); tMenu = millis(); }
      if (botao_apertou(BTN_ENTER)) { menu_abrir(); break; }
      if (millis() - tMenu >= MENU_VOLTA_MS) { irCasa(); break; }
      menu_desenha();
      break;

    case MODO_CENA:
      if (botao_apertou(BTN_ESQ)) {
        som_tocar(SOM_VOLTAR);
        if (cena == CENA_VISITA) { link_comando('F', '0'); irCasa(); }
        else irMenu();
        break;
      }
      switch (cena) {
        case CENA_STATUS:
          if (botao_apertou(BTN_ENTER)) { som_tocar(SOM_VOLTAR); irMenu(); break; }
          status_atualizar();
          break;
        case CENA_COMER:
          if (comer_atualizar()) terminouComer();
          break;
        case CENA_BANHO:
          if (botao_apertou(BTN_ENTER) && bn_estado == 0) { som_tocar(SOM_CLIQUE); banho_entrar(); }
          if (banho_atualizar()) terminouBanho();
          break;
        case CENA_BRINCAR:
          if (bola_atualizar()) terminouBrincar();
          break;
#if CONEXAO == CONEXAO_AMIGO
        case CENA_VISITA:
          if (inter_atualizar()) terminouVisita(true);
          break;
#endif
        case CENA_MINIGAME:
          if (botao_apertou(BTN_ENTER)) {
            if (jg_fim && minigame_podeReiniciar() && pet.energia < NIVEL_CRITICO) {
              som_tocar(SOM_NEGADO);
              mensagem_P(PSTR("CANSADO DEMAIS"), PSTR("precisa dormir"), MSG_PARADO, 1800, MODO_CASA);
              break;
            }
            if (!jg_fim) som_tocar(SOM_CLIQUE);
            minigame_pular();
          }
          minigame_atualizar();
          if (jg_acabouAgora) terminouMinigame();
          break;
        case CENA_SENSOR:
          sensor_atualizar(MODO_DEMO_SENSOR ? distanciaDemo() : distancia);
          if (sn_coracoes) { pet_ajusta(pet.felicidade, 5); som_tocar(SOM_FESTA); }
          break;
      }
      break;

    case MODO_MENSAGEM:
      if (botao_apertou(BTN_ENTER) || mensagem_atualizar()) {
        if (msg_proxModo == MODO_CENA) abrirCena(msg_proxCena);
        else if (msg_proxModo == MODO_MENU) irMenu();
        else irCasa();
      }
      break;

    case MODO_TRANSICAO:
      if (transicao_atualizar()) {
        if (tr_tipo == TR_ADORMECER) { modo = MODO_SONO; telaToda = true; dormir_iniciar(); }
        else irCasa();
      }
      break;

    case MODO_SONO:
      if (algum_botao()) {
        if (motivoSono == SONO_COCHILO && !escuro) { acordar(); break; }
        dormir_avisar();
      }
      dormir_atualizar();
      break;

#if CONEXAO == CONEXAO_AMIGO
    case MODO_ESPERA: {
      if (botao_apertou(BTN_ESQ)) { som_tocar(SOM_VOLTAR); irMenu(); break; }
      if (millis() - tEspera >= LINK_RESPOSTA_MS) {
        som_tocar(SOM_NEGADO);
        mensagemAmigo_P(PSTR("SEM RESPOSTA"), PSTR(""), PSTR(" nao ouviu"), MSG_PARADO, 2000, MODO_MENU);
        break;
      }
      if (millis() - tReenvio >= LINK_REENVIO_MS) { tReenvio = millis(); link_comando('V', '0'); }
      limpaTela();
      TXT(1, 2, "CHAMANDO");
      texto(1, 11, amigoNome);
      for (byte i = 0; i < ((millis() - tEspera) / 500) % 4; i++) poe(2, 9 + i, '.');
      TXT(3, 0, "ESQ cancela");
      desenha();
      break;
    }
#endif

    case MODO_FORA:
      if (algum_botao()) {
        som_tocar(SOM_NEGADO);
        mensagem_P(PSTR("ESTOU NO CELULAR"), PSTR("me chama pelo app"), MSG_PARADO, 2000, MODO_CASA);
        break;
      }
      fora_atualizar();
      break;

    case MODO_PORTAL:
      if (portal_atualizar()) terminouPortal();
      break;

    case MODO_FESTA:
      if (festa_atualizar()) irCasa();
      break;

    case MODO_ESTUDO:                                  // só o app encerra; ENTER pausa o cronômetro
      if (botao_apertou(BTN_ENTER) && est_modo == 'C') { som_tocar(SOM_CLIQUE); estudo_alternaPausa(); }
      estudo_atualizar();
      break;

    case MODO_FIM:
      if (botao_segurou(BTN_ENTER, NOVO_PET_SEGURAR_MS)) {
        pet_novo();
        som_tocar(SOM_FESTA);
        mensagem_P(PSTR("UM NOVO BICHINHO"), PSTR("NASCEU!"), MSG_CHEGANDO, 2500, MODO_CASA);
        break;
      }
      limpaTela();
      TXT(0, 4, "FIM DE JOGO");
      texto(1, 8, pet_nome());
      TXT(1, 13, "x_x");
      TXT(2, 1, "segure ENTER 3 s");
      TXT(3, 1, "para recomecar");
      desenha();
      break;
  }
}

// efeito de clima nas cenas (neve no frio, sol no calor) sem atrapalhar menus e textos
void ajustaClimaSobreposto() {
  bool cenario = (modo == MODO_CENA && cena != CENA_STATUS) || modo == MODO_SONO || modo == MODO_TRANSICAO;
  if (modo == MODO_CASA || modo == MODO_FORA) climaSobreposto = (clima == CLIMA_FRIO) ? 1 : 0;   // a casa desenha o próprio sol
  else if (!cenario)     climaSobreposto = 0;
  else if (clima == CLIMA_FRIO)  climaSobreposto = 1;
  else if (clima == CLIMA_CALOR) climaSobreposto = (modo == MODO_CENA) ? 2 : 0;   // de noite não há sol
  else climaSobreposto = 0;
}

// ---------------------------------------------------------------------------------------------
void setup() {
  pinMode(PINO_ID, INPUT_PULLUP);
  byte id = digitalRead(PINO_ID) == LOW ? 2 : 1;
  randomSeed(analogRead(PINO_SEMENTE));

  lcd.init();
  lcd.backlight();
  som_iniciar();
  botoes_iniciar();
  leds_iniciar();
  sensores_iniciar();
  pet_iniciar(id);
  link_iniciar();

  if (!pet_vivo()) { modo = MODO_FIM; return; }
  som_tocar(SOM_ACORDAR);
  char nome[12];
  strcpy(nome, pet_nome()); strcat(nome, "!");
  mensagem_P(PSTR("OLA, EU SOU O"), PSTR(""), MSG_CHEGANDO, 2500, MODO_CASA);
  strcpy(msg_l2, nome);
}

void loop() {
  botoes_ler();

  if (modo == MODO_DESLIGADO) {                        // desligado: só espera o POWER
    link_atualizar();                                  // descarta o que chegar, sem responder
    link_pegaComando();
    app_pegaPedido();
    if (botao_apertou(BTN_POWER)) ligar();
    return;
  }
  if (botao_segurou(BTN_POWER, POWER_SEGURAR_MS)) { desligar(); return; }

  sensores_atualizar();
  lk_meuHumor = pet_humor();
  lk_euDormindo = !acordado();
  app_estudo = est_modo;
  app_fase = est_parado ? 'Z' : (est_pausaPomodoro ? 'P' : 'F');
  link_atualizar();

  if (modo != MODO_FIM && petAqui) trataEventoPet(pet_atualizar(modo == MODO_SONO, clima));
  if (estudoAtivo() && estudo_tique() == EST_TROCOU_FASE && modo == MODO_ESTUDO)
    irFesta(est_pausaPomodoro ? PSTR("HORA DA PAUSA!") : PSTR("VOLTA AO FOCO!"), 2500);
  trataApp();
  if (modo != MODO_FIM) { trataSensores(); trataLink(); trataAlerta(); }
  else link_pegaComando();

  ajustaClimaSobreposto();
  rodaModo();

  leds_mostrar(petAqui ? pet_humor() : HUMOR_APAGADO);
  som_atualizar();
}
