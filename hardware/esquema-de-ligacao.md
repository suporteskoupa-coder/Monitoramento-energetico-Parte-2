# Esquema de Ligação

> **Status provisório:** diagrama conceitual alinhado ao GPIO do firmware atual. A topologia,
> a variante do SCT-013 e as conexões reais devem ser conferidas por Pedro na bancada antes
> de este arquivo ser tratado como esquemático final.

## Diagrama do circuito

```
                    3,3 V (ESP32)
                        │
                        ├──────────────┐
                        │              │
                       ┌┴┐            ─┴─  (opcional)
                    R1 │ │            ─┬─  100 nF de desacoplamento
                       └┬┘              │
                        │               │
     SCT-013            ├───────────────┼──────────────► GPIO 34 (ADC1)
   ╭─────────╮          │               │
   │  grampo │          │              ┌┴┐
   │  ao redor├─── P1 ──┤              │ │ (nada aqui — ilustrativo)
   │  de UM  │          │              └┬┘
   │ condutor├─── P2 ──┐│               │
   ╰─────────╯         ││               │
                       │├──── C1 ───────┤
                       ││   10 µF       │
                       ││  (+ no        │
                       ││   ponto médio)│
                       ││               │
                      ┌┴┴┐              │
                   R2 │  │              │
                      └┬─┘              │
                       │                │
                      GND ──────────────┘
                   (ESP32)
```

**Leitura do diagrama:**

- `R1` e `R2` formam o divisor: com R1 = R2, o ponto médio fica em **3,3 V ÷ 2 = 1,65 V**.
- O **ponto médio** do divisor é o nó indicado para o **GPIO 34** no candidato atual a firmware.
- `C1` (10 µF) liga o ponto médio ao GND — **perna longa (positivo) no ponto médio**.
- Os dois terminais do plugue do sensor (`P1`/`P2`) entram um no ponto médio e outro no
  GND, de modo que o sinal AC do sensor "monte" sobre o offset de 1,65 V.

> ⚠️ Este diagrama reflete a topologia descrita pelas mensagens de diagnóstico do
> firmware (R1 superior, R2 inferior, capacitor com o positivo no ponto médio).
> **Confirme contra a montagem física e substitua por um esquemático real** — coloque o
> arquivo em [`esquematicos/`](esquematicos/) e referencie aqui.

## Mapa de pinos

| Pino do ESP32 | Ligação | Observação |
|---------------|---------|------------|
| `3V3` | Topo do divisor (R1) | **Nunca usar 5 V** — estoura o offset e pode danificar o ADC |
| `GND` | Base do divisor (R2), negativo de C1, um terminal do sensor | Referência comum |
| `GPIO 34` | Ponto médio do divisor / entrada do sinal | ADC1_CH6 no candidato atual a firmware |
| `USB` | Alimentação e gravação | — |

### Confirmação do GPIO

O candidato atual a firmware já usa **GPIO 34 / ADC1_CH6**. A documentação anterior usava
GPIO 4, pertencente ao ADC2 no ESP32 WROOM e inadequado ao futuro uso simultâneo de Wi-Fi.
Antes da coleta, conferir se o fio da bancada foi realmente movido para o GPIO 34.

## Verificação após montar

Com o ESP32 alimentado e **sem carga** no condutor:

| Medida com multímetro | Valor esperado |
|-----------------------|----------------|
| Entre `3V3` e `GND` | 3,3 V ± 0,1 V |
| Entre o ponto médio e `GND` | **1,65 V ± 0,2 V** |
| R1 isolado (fora do circuito) | Valor nominal ± tolerância |
| R2 isolado (fora do circuito) | Valor nominal ± tolerância |

Se o ponto médio não estiver em ~1,65 V, o **Teste 1** do firmware vai falhar — e as
causas prováveis estão listadas na saída do próprio teste
(ver [`docs/firmware.md`](../docs/firmware.md#teste-1--teste1_offset-o-divisor-resistivo-está-correto)).
