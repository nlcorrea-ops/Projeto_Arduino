# Robozinho — bichinho virtual do Desk Companions

Um bichinho de mesa no estilo Tamagotchi: Arduino Uno R3 + LCD 20x4 I2C, conversando com o
aplicativo **Desk Companions** pelo módulo Bluetooth **HC-06**.

O bichinho pode ser o **tubarão** (fundo do mar), o **sapinho** (pântano) ou o **recruta**
(quartel-general). O app escolhe qual vai para a tela pelo portal mágico, começa o Pomodoro e
o cronômetro, e manda as comidas compradas na loja. Sem o app, o robozinho funciona sozinho.

Ver `previa_lcd.png` para uma prévia de como cada tela aparece no LCD.

---

## 1. Materiais

| Qtd | Componente | Observação |
|-----|------------|------------|
| 1 | Arduino Uno R3 | |
| 1 | LCD 20x4 com módulo I2C (PCF8574) | |
| 1 | Módulo Bluetooth HC-06 | Bluetooth Classic, vem em 9600 baud |
| 1 | Resistor de 1 kΩ e 1 de 2 kΩ | divisor para o RX do HC-06 (ele recebe 3,3 V) |
| 4 | Botões de pressão | esquerda, direita, enter e power |
| 1 | Buzzer **ativo** | apita sozinho com 5 V; o passivo só faz "clique" |
| 3 | LEDs: verde, amarelo e vermelho + 3 resistores de 220 Ω | |
| 1 | Sensor ultrassônico HC-SR04 | |
| 1 | NTC 10k + 1 resistor de 10 kΩ | temperatura |
| 1 | LDR + 1 resistor de 10 kΩ | luminosidade |

## 2. Pinagem

| Pino | Ligação |
|------|---------|
| D0, D1 | livres (USB: gravação e Monitor Serial) |
| D2 | **HC-06 TXD** |
| D3 | **HC-06 RXD**, passando pelo divisor (veja abaixo) |
| D4 | HC-SR04 TRIG |
| D5 | HC-SR04 ECHO |
| D6 | buzzer ativo (+), (−) no GND |
| D7 | botão POWER (outro terminal no GND) |
| D8 | botão SETA ESQUERDA |
| D9 | botão SETA DIREITA |
| D10 | botão ENTER |
| D11, D12, D13 | LEDs verde, amarelo e vermelho (perna longa no pino, curta → 220 Ω → GND) |
| A0 | NTC: 5V → 10 kΩ → **A0** → NTC → GND |
| A1 | LDR: 5V → LDR → **A1** → 10 kΩ → GND |
| A2 | solto (números aleatórios) |
| A3 | identificação: solto = TUBA (guloso), jumper no GND = FINN (brincalhão) |
| A4, A5 | LCD SDA e SCL |

Os botões não precisam de resistor: o programa usa o pull-up interno.
Para mudar algum pino, troque o número na linha `#define PINO_...` do `config.h`.

### Ligando o HC-06

```text
HC-06 VCC ─── 5V
HC-06 GND ─── GND
HC-06 TXD ─── D2
D3 ── 1 kΩ ──┬── HC-06 RXD
             └── 2 kΩ ── GND
```

O divisor baixa os 5 V do Arduino para cerca de 3,3 V, que é o que o RX do HC-06 aceita.

## 3. Como gravar

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software) e a biblioteca **LiquidCrystal I2C**
   (Frank de Brabander) pelo Gerenciador de Bibliotecas. `Wire`, `EEPROM` e `SoftwareSerial`
   já vêm com a IDE.
2. Deixe todos os arquivos juntos na pasta `robozinho_1` e abra o `robozinho_1.ino`.
3. Escolha *Arduino Uno* e a porta, e clique em **Carregar**.

O programa ocupa cerca de 95% da memória do Uno (com `CONEXAO_AMIGO`, quase 100%). Se depois de alguma mudança a IDE disser
"Sketch too big", desligue o som (`SOM_LIGADO false`) ou fale comigo para enxugar uma cena.

Se o LCD acender sem texto, troque `LCD_ENDERECO` de `0x27` para `0x3F` e ajuste o contraste no
parafusinho azul atrás do módulo.

## 4. App ou outro robozinho

Os pinos D2/D3 servem para **uma** de duas coisas, escolhida no `config.h`:

- `#define CONEXAO CONEXAO_APP` (padrão): conversa com o app pelo HC-06.
- `#define CONEXAO CONEXAO_AMIGO`: conversa com outro robozinho (fios D2↔D3 cruzados e GND em
  comum) e ativa o item VISITAR do menu. Opcional: só use se quiser testar a visita.

## 5. Botões

| Onde | ESQ / DIR | ENTER | POWER |
|------|-----------|-------|-------|
| Casa | abre o menu | abre o menu | segurar 1 s: desliga |
| Menu | navega | abre o item | segurar 1 s: desliga |
| Numa atividade | ESQ volta ao menu | ação (pular, entrar no banho) | segurar 1 s: desliga |
| Estudando | — (só o app encerra) | no cronômetro: pausa e continua | segurar 1 s: desliga |
| Bichinho no app | mostra "ESTOU NO CELULAR" | igual | segurar 1 s: desliga |
| Fim de jogo | — | segurar 3 s: novo bichinho | — |

## 6. Itens do menu

| Item | O que faz |
|------|-----------|
| CASA | volta para a tela inicial |
| STATUS | fome, felicidade, higiene, energia, saúde, vidas, temperatura e lanches do app |
| COMER | a comida cai do tubo e o bichinho vai buscar. Se houver lanche comprado no app, ele usa um e ganha +10 de felicidade |
| BANHO | ENTER: o bichinho pula na banheira |
| BRINCAR | o bichinho joga bolinha: a bola voa, quica nas bordas e ele corre para dar 5 toques |
| MINIGAME | ENTER pula os obstáculos; guarda o recorde |
| DORMIR | tira um cochilo |
| VISITAR | só com `CONEXAO_AMIGO` |
| SENSOR | o bichinho se aproxima conforme a distância; de perto aparece o rostinho |

## 7. Funções ligadas ao app

### Portal mágico
- O bichinho só existe em um lugar por vez.
- **Saída:** ele anda até o portal brilhante na direita, com "OUTRA DIMENSAO..." no alto, e some.
  A tela fica com o cenário vazio e "FUI PRO CELULAR! volto ja <3".
- **Chegada:** o portal abre na esquerda, o bichinho sai dele e diz "CHEGUEI!".
- O robô só confirma para o app quando a animação termina.
- Enquanto o bichinho está no app, o tempo dele fica parado no robô (ele não passa fome lá).
- Cada bichinho tem a própria ficha (fome, vidas, recorde...) guardada na EEPROM.

### Estudo
- **Pomodoro:** 25 min de foco e 5 min de pausa, repetindo até o app encerrar.
- **Cronômetro:** conta para cima. O ENTER pausa: o bichinho volta à animação de casa com
  "|| PAUSA" no alto.
- A tela alterna 6 s de relógio grande e 3 s do rostinho (só olhos e boca), com o tempo no canto.
- O bichinho comemora ao começar, ao trocar de fase e ao terminar.
- Só o app encerra a sessão.

### Lanches
- As comidas compradas na loja do app vão para a lancheira do robô (até 9).
- O item COMER usa um lanche, se houver.

O formato das mensagens está em `PROTOCOLO.md`.

## 8. Necessidades, sensores e vidas

Continuam como antes:
- As necessidades caem a cada 30 s (`TICK_MS`); níveis bem (60+), atenção (30–59) e crítico (<30).
- Os LEDs mostram o pior nível; no vermelho o buzzer avisa a cada 2 min e a tela pisca "!".
- No escuro (LDR) ele dorme; no frio aparecem gelo e neve; no calor, sol e suor.
- Na tela de casa, a temperatura do NTC fica no canto direito (ex.: 25°C).
- Alguém a menos de 60 cm (HC-SR04): "OI!" e um pouco de felicidade.
- 3 vidas; sem vidas é fim de jogo. Tudo é salvo na EEPROM, no máximo a cada 10 min.

Para testar mais rápido, diminua `TICK_MS` no `config.h` (por exemplo, `3000UL`).

## 9. Arquivos

```text
robozinho_1/
├── robozinho_1.ino  programa principal: estados, menu, eventos e pedidos do app
├── config.h         pinagem e todos os ajustes
├── tela.h           LCD, tela virtual e desenhos do tubarão
├── pets.h           desenhos do sapinho e do recruta, rostinhos, cenários, comidas e portal
├── som.h            buzzer
├── botoes.h         botões com antirrepique
├── leds.h           LEDs de humor
├── sensores.h       NTC, LDR e HC-SR04
├── pet.h            necessidades, vidas e EEPROM (uma ficha por bichinho)
├── link.h           serial nos pinos D2/D3
├── app.h            mensagens do aplicativo
├── cenas.h          todas as animações
├── PROTOCOLO.md     mensagens entre o app e o robô
├── previa_lcd.png   prévia das telas
```

## 10. Limitações conhecidas

- O código foi compilado para o Uno e testado num simulador no computador, ainda não numa placa
  de verdade.
- A `SoftwareSerial` não envia e recebe ao mesmo tempo; por isso o app repete os pedidos até
  receber a confirmação.
- Sem relógio (RTC), o tempo para quando a placa está desligada.
