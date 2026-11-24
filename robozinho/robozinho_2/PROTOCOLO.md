# Protocolo entre o app Desk Companions e o robozinho

Comunicação serial em texto pelo HC-06 (Bluetooth Classic SPP, 9600 baud).
No iPhone, o Mac faz a ponte: recebe as mensagens por Wi-Fi e repassa ao HC-06 sem mudar nada.

## Formato

```text
$<tipo><dados>*<conferência>\n
```

- A conferência é o XOR de todos os caracteres entre `$` e `*`, escrito em 2 dígitos hexadecimais
  maiúsculos. Exemplo: o corpo `O` tem conferência `4F`, então a mensagem é `$O*4F\n`.
- Mensagens com conferência errada são descartadas.
- O robô aceita `\n` ou `\r` no fim da linha.

## Do app para o robô

| Mensagem | Significado | Resposta |
|---|---|---|
| `I<pet>` | o bichinho chega ao robô pelo portal. `pet`: `T` tubarão, `S` sapinho, `R` recruta | `KI` quando a animação termina (3 a 4 s), ou `KN` se já houver outro bichinho no robô |
| `O` | o bichinho sai do robô e vai para o app | `KO` quando a animação termina (3,2 s), ou `KN` se ele não tiver vidas |
| `E<modo>` | começa a estudar. `modo`: `P` Pomodoro, `C` cronômetro | `KEP` ou `KEC` |
| `X` | encerra o estudo (só o app encerra) | `KX` |
| `C<n><id>` | chegaram `n` comidas compradas na loja (1 a 9). `id` é um caractere qualquer que muda a cada compra | `KC<id>` |

O app deve **repetir o pedido a cada 1 s até receber a resposta**, desistindo depois de 8 s.
O robô ignora pedidos repetidos: um `I` que chega no meio da viagem é descartado, e a mesma
compra (mesmo `id`) não é contada duas vezes.

## Do robô para o app

Estado, enviado a cada 1 s:

```text
P<onde><pet><estudo><fase><saude><fome><felicidade><higiene><energia><lanches><vivo>
```

| Campo | Valores |
|---|---|
| `onde` | `R` no robô, `A` no app, `V` viajando pelo portal |
| `pet` | `T`, `S` ou `R` (o último bichinho que esteve no robô) |
| `estudo` | `N` nenhum, `P` Pomodoro, `C` cronômetro |
| `fase` | `F` foco, `P` pausa do Pomodoro, `Z` cronômetro pausado pelo ENTER |
| necessidades | 3 dígitos cada, de `000` a `100` |
| `lanches` | comidas esperando na lancheira, de `0` a `9` |
| `vivo` | `1` ou `0` |

Exemplo: `$PRTNF100080080080080021*xx` → tubarão no robô, sem estudo, saúde 100, as outras
necessidades em 80, 2 lanches, vivo.

**O robô é a referência** para saber onde o bichinho está. Se o robô reiniciar, o bichinho
volta para ele e o app deve se ajustar pelo campo `onde`.

## Regras de tempo

- O Pomodoro tem 25 min de foco e 5 min de pausa automática (`config.h`). Quem conta é o robô.
- O cronômetro conta para cima; o ENTER do robô pausa e continua.
- Enquanto o bichinho está no app, as necessidades dele ficam paradas no robô.
