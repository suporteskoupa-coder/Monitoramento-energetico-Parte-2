# Procedimento de Calibração

> **Status: pendente de execução.** Este documento descreve o procedimento a seguir; os
> valores medidos devem ser registrados em um experimento próprio dentro de
> [`experimentos/`](../experimentos/).

Sem calibração, o firmware entrega apenas **Volts RMS** — um número proporcional à
corrente, mas sem unidade física útil. Calibrar significa determinar o fator que
converte esses Volts em **Ampères**.

## 1. Determine o modelo do sensor

| Modelo | Saída | Fator teórico |
|--------|-------|---------------|
| SCT-013-030 | 1 V para 30 A | **30 A/V** |
| SCT-013-000 | 50 mA para 100 A | depende do resistor de burden |

Confirme o que está impresso no corpo do sensor antes de qualquer coisa.

### Se for o SCT-013-000 (saída em corrente)

Ele **não tem resistor de burden interno** — é obrigatório instalar um. O burden
converte a corrente do secundário em tensão:

```
V_pico_desejada = 1,65 V  (metade da faixa do ADC, para não saturar)
I_pico_secundário = I_rms_secundário × √2 = 0,050 A × 1,414 = 0,0707 A
R_burden = 1,65 V ÷ 0,0707 A ≈ 23 Ω     →  usar o valor comercial mais próximo (22 Ω)
```

Com R_burden = 22 Ω, o fundo de escala fica em 100 A e o fator resultante é:

```
A/V = 100 A ÷ (0,050 A × 22 Ω × √2) ≈ 64,3 A/V
```

Meça o resistor real com multímetro e refaça a conta com o valor medido.

## 2. Calibração com carga conhecida

Materiais: uma carga de potência conhecida e estável (uma resistência — chaleira,
secador, lâmpada incandescente; **evite cargas indutivas ou chaveadas** nesta etapa) e,
de preferência, um alicate amperímetro como referência.

| Passo | Ação |
|-------|------|
| 1 | Monte o circuito e confirme que os 3 testes do firmware passaram |
| 2 | Meça o **RMS em Volts sem carga** — este é o piso de ruído (`V_zero`) |
| 3 | Ligue a carga conhecida e aguarde estabilizar (≥ 30 s) |
| 4 | Anote o **RMS em Volts com carga** (`V_carga`) |
| 5 | Meça a corrente de referência com o alicate amperímetro AC (`I_real`) |
| 6 | Calcule o fator: `A/V = I_real ÷ (V_carga − V_zero)` |
| 7 | Repita com pelo menos **3 cargas diferentes** e verifique se o fator se mantém |

Se o fator variar muito entre as cargas, o sistema não é linear na faixa testada —
investigar saturação, ruído ou valor de burden inadequado antes de prosseguir.

O cálculo `P_nominal ÷ V_rede` pode servir apenas como estimativa preliminar para escolher
uma carga. Ele não substitui a referência independente exigida para calibração e validação.

## 3. Limiar de ruído

O piso de ruído (`V_zero`) define abaixo de que valor a leitura deve ser tratada como
zero. Cuidado com o compromisso:

- **Limiar alto demais** → cargas pequenas somem da medição. Exemplo: com 30 A/V, um
  limiar de 30 mV equivale a **0,9 A ≈ 114 W em 127 V** — ou seja, qualquer aparelho
  abaixo de 114 W seria registrado como desligado.
- **Limiar baixo demais** → o ruído do circuito é contabilizado como consumo.

Registre o valor escolhido e a justificativa.

## 4. Validação

Depois de aplicar o fator no firmware, valide com uma carga **diferente** das usadas na
calibração e calcule o erro:

```
erro (%) = |I_medido − I_referência| ÷ I_referência × 100
```

Meta de exatidão para o projeto: `<definir com o orientador>`.

## 5. Registro obrigatório

Toda calibração deve gerar um experimento em [`experimentos/`](../experimentos/) com:

- a tabela de pontos medidos (`dados/`);
- o fator calculado e o erro obtido;
- fotos da bancada e da leitura de referência (`evidencias/`);
- a data e quem executou.
