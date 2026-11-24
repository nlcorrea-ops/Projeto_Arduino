// =============================================================================================
// app.h — conversa com o aplicativo pelo HC-06 (CONEXAO_APP)
// =============================================================================================
// Usa o mesmo formato do link.h:  $<tipo><dados>*<conferência>\n   (detalhes em PROTOCOLO.md)
//
// Do app para o robô:
//   I<pet>        portal: o bichinho chega ao robô. pet = T (tubarão), S (sapinho), R (recruta)
//   O             portal: o bichinho sai do robô e vai para o app
//   E<modo>       começa a estudar. modo = P (Pomodoro) ou C (cronômetro)
//   X             encerra o estudo (só o app encerra)
//   C<n><id>      chegaram n comidas compradas na loja (n de 1 a 9; id evita contar duas vezes)
//
// Do robô para o app:
//   P<onde><pet><estudo><fase><saude><fome><felicidade><higiene><energia><lanches><vivo>
//      a cada 1 s. onde = R (no robô), A (no app), V (viajando pelo portal)
//      estudo = N (nenhum), P (Pomodoro), C (cronômetro); fase = F (foco), P (pausa), Z (parado)
//      necessidades com 3 dígitos (000 a 100); lanches de 0 a 9; vivo = 1 ou 0
//      ex.: $PRTNF100080080080080211*xx
//   K<o que>      confirmação: KI chegou, KO saiu, KE estudo começou, KX estudo acabou,
//                 KC<id> comida guardada, KN pedido negado (ex.: bichinho sem vidas)
//
// O app reenvia o pedido até receber a confirmação, por isso pedidos repetidos são ignorados.
#pragma once

char app_pedido = 0;                    // último pedido recebido (0 = nenhum)
char app_dado = 0, app_dado2 = 0;       // dados do pedido
byte app_ultimoLanche = 255;            // id da última entrega de comida (evita repetir)

// estado que vai para o app (o programa principal atualiza)
char app_onde = 'R';
char app_estudo = 'N';
char app_fase = 'F';

void app_recebe(const char* m) {
  char t = m[0];
  if (t == 'I' || t == 'O' || t == 'E' || t == 'X' || t == 'C') {
    app_pedido = t;
    app_dado = m[1];
    app_dado2 = m[1] ? m[2] : 0;
  }
}

char app_pegaPedido() { char c = app_pedido; app_pedido = 0; return c; }

void app_responde(char oque, char dado) {
  if (CONEXAO != CONEXAO_APP) return;
  char c[4] = {'K', oque, dado, 0};
  link_envia(c);
}

char app_letraPet(byte tipo) { return tipo == PET_SAPO ? 'S' : (tipo == PET_RECRUTA ? 'R' : 'T'); }

byte app_petDaLetra(char l) { return l == 'S' ? PET_SAPO : (l == 'R' ? PET_RECRUTA : PET_TUBARAO); }

void app_enviaEstado() {
  char c[24];
  c[0] = 'P'; c[1] = app_onde; c[2] = app_letraPet(petTipo); c[3] = app_estudo; c[4] = app_fase;
  char* p = c + 5;
  p = numero(p, pet.saude, 3); p = numero(p, pet.fome, 3); p = numero(p, pet.felicidade, 3);
  p = numero(p, pet.higiene, 3); numero(p, pet.energia, 3);
  c[20] = '0' + lanches;
  c[21] = pet_vivo() ? '1' : '0';
  c[22] = 0;
  link_envia(c);
}
