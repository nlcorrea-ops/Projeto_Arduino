// =============================================================================================
// pet.h — necessidades, saúde, vidas, personalidade e memória (EEPROM)
// =============================================================================================
// Necessidades vão de 0 (péssimo) a 100 (ótimo): FOME (100 = de barriga cheia), FELICIDADE,
// HIGIENE e ENERGIA. A cada TICK_MS elas caem conforme a personalidade e o clima.
//
// Níveis de cada necessidade:  BEM (>= NIVEL_BOM) | ATENÇÃO | CRÍTICO (< NIVEL_CRITICO)
// Consequências por tick:
//   fome crítica       -> saúde -2 e felicidade -1
//   higiene crítica    -> saúde -1 (fica doente)
//   felicidade crítica -> saúde -1
//   energia crítica    -> felicidade -1 e dorme de cansaço (tratado no programa principal)
//   tudo bem           -> saúde +2; depois de TICKS_RECUPERA_VIDA seguidos ganha 1 vida
// Saúde chegando a 0 tira uma vida; sem vidas é fim de jogo.
//
// Bichinhos: o app escolhe qual mora na tela (tubarão, sapinho ou recruta). Cada um tem as
// próprias necessidades guardadas na EEPROM. Quando o bichinho está no app (petAqui = false),
// o tempo dele fica parado aqui no robô.
#pragma once

// ---------------------------------------------------------------------------------------------
// Qual bichinho está no robô
// ---------------------------------------------------------------------------------------------
enum { PET_TUBARAO, PET_SAPO, PET_RECRUTA, N_PETS };
byte petTipo = PET_TUBARAO;
bool petAqui = true;                    // false = o bichinho foi para o app pelo portal

// ---------------------------------------------------------------------------------------------
// Personalidades (a placa escolhe pela ligação do pino A3)
// ---------------------------------------------------------------------------------------------
struct Personalidade {
  char nome[5];
  byte perdeFome, perdeFelicidade, perdeHigiene, perdeEnergia;   // queda por tick
  byte bonusComer, bonusBrincar;                                  // ganho extra nas atividades
};
const Personalidade PERSONALIDADES[2] = {
  {"TUBA", 3, 1, 1, 1, 10, 0},          // robô 1: guloso (fome cai rápido, ama comer)
  {"FINN", 2, 2, 1, 1, 0, 10},          // robô 2: brincalhão (precisa de mais diversão)
};

// ---------------------------------------------------------------------------------------------
// Estado salvo (um por bichinho)
// ---------------------------------------------------------------------------------------------
#define PET_MARCA  0x5442               // "TB": identifica dados válidos na EEPROM
#define PET_VERSAO 1

struct DadosPet {
  unsigned int marca;
  byte versao;
  byte fome, felicidade, higiene, energia, saude, vidas;
  unsigned int recorde;                 // maior pontuação do minigame
  unsigned int ticksBons;               // ticks seguidos com todas as necessidades bem
  byte conferencia;                     // XOR de todos os bytes anteriores a este campo
};

// Mapa da EEPROM: [tubarão][sapinho][recruta][último bichinho][lanches]
#define EEPROM_PET(t)      (EEPROM_ENDERECO + (t) * sizeof(DadosPet))
#define EEPROM_TIPO        (EEPROM_ENDERECO + N_PETS * sizeof(DadosPet))
#define EEPROM_LANCHES     (EEPROM_TIPO + 1)

DadosPet pet;
const Personalidade* perso = &PERSONALIDADES[0];
byte meuId = 1;
byte lanches = 0;                       // comidas compradas no app esperando na lancheira (0 a 9)

enum { EV_NADA, EV_PERDEU_VIDA, EV_GANHOU_VIDA, EV_FIM_DE_JOGO };

bool pet_mudou = false;                 // há alterações ainda não gravadas
unsigned long pet_tTick = 0, pet_tSalvo = 0;

// nome curto (até 4 letras) que aparece na tela
const char* pet_nome() {
  if (petTipo == PET_SAPO)    return "SAPO";
  if (petTipo == PET_RECRUTA) return "RECO";
  return perso->nome;                   // o tubarão usa o nome da personalidade (TUBA ou FINN)
}

byte nivelDe(byte v) { return v >= NIVEL_BOM ? HUMOR_BEM : (v >= NIVEL_CRITICO ? HUMOR_ATENCAO : HUMOR_CRITICO); }

// humor geral = pior nível entre saúde e necessidades (é o que os LEDs mostram)
byte pet_humor() {
  byte h = nivelDe(pet.saude);
  byte n[4] = {pet.fome, pet.felicidade, pet.higiene, pet.energia};
  for (byte i = 0; i < 4; i++) if (nivelDe(n[i]) > h) h = nivelDe(n[i]);
  return h;
}

bool pet_vivo() { return pet.vidas > 0; }

void pet_ajusta(byte& v, int d) {
  int n = (int)v + d;
  byte novo = (byte)constrain(n, 0, 100);
  if (novo != v) { v = novo; pet_mudou = true; }
}

byte pet_conferencia(const DadosPet& d) {
  const byte* p = (const byte*)&d;
  byte x = 0;
  for (byte i = 0; i < offsetof(DadosPet, conferencia); i++) x ^= p[i];   // todos antes da conferência
  return x;
}

// grava só os bytes que mudaram (EEPROM.put usa update), preservando a vida útil da memória
void pet_salvar() {
  pet.conferencia = pet_conferencia(pet);
  EEPROM.put(EEPROM_PET(petTipo), pet);
  pet_mudou = false;
  pet_tSalvo = millis();
}

void pet_novo() {
  pet.marca = PET_MARCA; pet.versao = PET_VERSAO;
  pet.fome = pet.felicidade = pet.higiene = pet.energia = 80;
  pet.saude = 100; pet.vidas = VIDAS_MAX;
  pet.recorde = 0; pet.ticksBons = 0;
  pet_salvar();
}

bool pet_dadosValidos(const DadosPet& d) {
  return d.marca == PET_MARCA && d.versao == PET_VERSAO && d.conferencia == pet_conferencia(d) &&
         d.fome <= 100 && d.felicidade <= 100 && d.higiene <= 100 && d.energia <= 100 &&
         d.saude <= 100 && d.vidas <= VIDAS_MAX;
}

void pet_carregar() {
  EEPROM.get(EEPROM_PET(petTipo), pet);
  if (!pet_dadosValidos(pet)) pet_novo();             // primeira vez ou memória corrompida
  pet_mudou = false;
  pet_tTick = pet_tSalvo = millis();
}

void pet_iniciar(byte id) {
  meuId = id;
  perso = &PERSONALIDADES[id == 2 ? 1 : 0];
  petTipo = EEPROM.read(EEPROM_TIPO);
  if (petTipo >= N_PETS) petTipo = PET_TUBARAO;
  lanches = EEPROM.read(EEPROM_LANCHES);
  if (lanches > 9) lanches = 0;
  pet_carregar();
}

// troca o bichinho que mora no robô (quando chega um diferente pelo portal)
void pet_trocar(byte tipo) {
  if (tipo >= N_PETS || tipo == petTipo) return;
  if (pet_mudou) pet_salvar();
  petTipo = tipo;
  EEPROM.update(EEPROM_TIPO, petTipo);
  pet_carregar();
}

void pet_guardaLanches() { EEPROM.update(EEPROM_LANCHES, lanches); }

// um passo do tempo: aplica quedas, consequências, saúde e vidas
byte pet_tick(bool dormindo, byte climaAtual) {
  if (dormindo) {
    pet_ajusta(pet.energia, ENERGIA_RECUPERA);
    pet_ajusta(pet.fome, -(int)(perso->perdeFome / 2 + 1));
    pet_ajusta(pet.higiene, -(int)perso->perdeHigiene);
  } else {
    pet_ajusta(pet.fome, -(int)perso->perdeFome);
    pet_ajusta(pet.felicidade, -(int)perso->perdeFelicidade);
    pet_ajusta(pet.higiene, -(int)perso->perdeHigiene);
    pet_ajusta(pet.energia, -(int)perso->perdeEnergia);
    if (climaAtual == CLIMA_FRIO)  pet_ajusta(pet.energia, -1);   // frio cansa
    if (climaAtual == CLIMA_CALOR) pet_ajusta(pet.higiene, -1);   // calor faz suar
  }

  if (nivelDe(pet.fome) == HUMOR_CRITICO)       { pet_ajusta(pet.saude, -2); pet_ajusta(pet.felicidade, -1); }
  if (nivelDe(pet.higiene) == HUMOR_CRITICO)    pet_ajusta(pet.saude, -1);
  if (nivelDe(pet.felicidade) == HUMOR_CRITICO) pet_ajusta(pet.saude, -1);
  if (nivelDe(pet.energia) == HUMOR_CRITICO && !dormindo) pet_ajusta(pet.felicidade, -1);

  bool tudoBem = nivelDe(pet.fome) == HUMOR_BEM && nivelDe(pet.felicidade) == HUMOR_BEM &&
                 nivelDe(pet.higiene) == HUMOR_BEM && nivelDe(pet.energia) == HUMOR_BEM;
  byte evento = EV_NADA;
  if (tudoBem) {
    pet_ajusta(pet.saude, 2);
    pet.ticksBons++; pet_mudou = true;
    if (pet.ticksBons >= TICKS_RECUPERA_VIDA && pet.vidas < VIDAS_MAX) {
      pet.vidas++; pet.ticksBons = 0;
      evento = EV_GANHOU_VIDA;
    }
  } else if (pet.ticksBons) {
    pet.ticksBons = 0; pet_mudou = true;
  }

  if (pet.saude == 0) {
    pet.vidas--;
    if (pet.vidas == 0) {
      evento = EV_FIM_DE_JOGO;
    } else {                            // recomeça mais fraco, mas sem morrer de novo na hora
      evento = EV_PERDEU_VIDA;
      pet.saude = 60;
      if (pet.fome < 50) pet.fome = 50;
      if (pet.felicidade < 50) pet.felicidade = 50;
      if (pet.higiene < 50) pet.higiene = 50;
      if (pet.energia < 50) pet.energia = 50;
    }
    pet_mudou = true;
  }
  if (evento != EV_NADA) pet_salvar();                // vidas são gravadas na hora
  return evento;
}

// chamar em todo loop; devolve um evento EV_* quando acontece algo importante
byte pet_atualizar(bool dormindo, byte climaAtual) {
  byte ev = EV_NADA;
  if (millis() - pet_tTick >= TICK_MS) {
    pet_tTick += TICK_MS;
    if (millis() - pet_tTick >= TICK_MS) pet_tTick = millis();   // não acumula atraso
    ev = pet_tick(dormindo, climaAtual);
  }
  if (pet_mudou && millis() - pet_tSalvo >= SALVAR_A_CADA_MS) pet_salvar();
  return ev;
}

// pausa o tempo do bichinho (aparelho desligado ou bichinho no app): sem "atraso" acumulado
void pet_pausar()   { if (pet_mudou) pet_salvar(); }
void pet_retomar()  { pet_tTick = millis(); }
