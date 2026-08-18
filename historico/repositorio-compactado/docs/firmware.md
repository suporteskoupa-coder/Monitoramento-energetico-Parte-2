# Documentação Técnica do Firmware

Arquivo documentado: [`firmware/monitoramento_energetico/monitoramento_energetico.ino`](../firmware/monitoramento_energetico/monitoramento_energetico.ino)

Este documento explica **linha a linha o que o código faz hoje**, a matemática por trás
da medição, os parâmetros ajustáveis e as limitações conhecidas.

> **Status:** o sketch atual é um **programa de bancada / validação**, não o firmware
> final de campo. Ele prova que o circuito de condicionamento está correto e mostra o
> sinal do sensor ao vivo. Ainda **não** converte para Ampères, não envia dados por
> Wi-Fi e não grava nada.

---

## 1. Visão geral

O sketch tem duas fases:

| Fase | Onde | O que faz |
|------|------|-----------|
| **Validação** | `setup()` | Roda 3 testes em sequência e imprime um laudo de aprovação/falha para cada um |
| **Monitor ao vivo** | `loop()` | A cada 2 s, imprime o offset DC e o sinal AC (RMS) medidos no sensor |

Fluxo de execução:

```
  boot
   │
   ├─ Serial.begin(115200)          → abre o monitor serial
   ├─ analogReadResolution(12)      → ADC em 12 bits (0–4095)
   ├─ analogSetAttenuation(ADC_11db)→ faixa de entrada até ~3,3 V
   ├─ delay(3000)                   → espera o capacitor de 10 µF carregar
   │
   ├─ teste1_offset()               → o divisor está em ~1,65 V?
   ├─ teste2_ruido()                → o capacitor está filtrando?
   ├─ teste3_deteccaoSinal()        → o sensor responde a corrente?
   │
   └─ loop() ─────────────────┐
        amostrarADC(500)      │  repete para sempre,
        imprime offset + RMS  │  uma linha a cada 2 s
        delay(2000) ──────────┘
```

---

## 2. O problema que o circuito resolve (e que o código compensa)

O sensor **SCT-013** é um transformador de corrente: ele produz um sinal **alternado**,
que oscila em torno de zero — ou seja, tem semiciclos **negativos**.

O ADC do ESP32 só lê tensões de **0 V a 3,3 V**. Tensão negativa é descartada (e pode
danificar o pino).

**Solução em hardware:** um divisor resistivo cria um ponto médio em **1,65 V** (metade
de 3,3 V) e o sinal do sensor passa a oscilar *em torno desse ponto* em vez de em torno
de zero. Um capacitor de 10 µF estabiliza esse ponto médio. Ver
[`hardware/README.md`](../hardware/README.md).

**Consequência em software:** toda leitura vem somada a um offset DC de ~2048 (metade
de 4095). O código precisa separar de novo as duas coisas:

- a **média** das leituras = o offset DC (deve ficar em ~2048);
- o **desvio em torno da média** = o sinal AC real do sensor.

É exatamente isso que a função `amostrarADC()` faz.

---

## 3. Parâmetros de configuração

Todos declarados no topo do arquivo:

| Constante | Valor | Significado | Quando mudar |
|-----------|-------|-------------|--------------|
| `PINO_ADC` | `4` | GPIO onde o sinal condicionado entra | Ao mudar a montagem — **ver alerta de ADC2 na seção 8** |
| `NUM_AMOSTRAS_TESTE` | `500` | Amostras por janela de medição | Mais amostras = leitura mais estável, porém mais lenta |
| `VREF` | `3,3` | Tensão de referência do ADC (V) | Só se alimentar o divisor com outra tensão |
| `ADC_MAX` | `4095` | Fundo de escala em 12 bits (2¹²−1) | Só se mudar `analogReadResolution()` |
| `OFFSET_ESPERADO` | `2048` | Metade da escala = 1,65 V | Se o divisor não for 50/50 |
| `OFFSET_TOLERANCIA` | `250` | Desvio aceito no Teste 1 (±250 ≈ ±0,2 V) | Para apertar/afrouxar o critério |
| `RUIDO_MAXIMO_OK` | `50` | Variação pico-a-pico aceita no Teste 2 (≈ 40 mV) | Para apertar/afrouxar o critério |

Valor padrão de `atrasoMicros` (no protótipo de `amostrarADC`): **200 µs**.

---

## 4. As estruturas e funções

### 4.1 `struct AmostragemStats`

Empacota o resultado de uma janela de amostragem, para que uma única leitura sirva aos
três testes e ao monitor:

| Campo | Tipo | Conteúdo |
|-------|------|----------|
| `minimo` | `int` | Menor valor bruto lido (0–4095) |
| `maximo` | `int` | Maior valor bruto lido |
| `variacao` | `int` | `maximo - minimo` — a amplitude pico-a-pico, em unidades de ADC |
| `media` | `float` | Média das leituras — **é o offset DC do circuito** |
| `rmsTensao` | `float` | Componente AC em **Volts RMS**, já com o offset removido |

### 4.2 `amostrarADC(pino, numAmostras, atrasoMicros)` — o coração do código

É a única função que fala com o hardware. Ela roda um laço de `numAmostras` iterações e,
em cada uma:

1. lê o ADC com `analogRead(pino)`;
2. atualiza `minimo` e `maximo`;
3. acumula a soma das leituras (`soma`) e a **soma dos quadrados** (`somaQuadrados`);
4. espera `atrasoMicros` microssegundos.

Terminado o laço, calcula:

```cpp
media          = soma / numAmostras;            // E[x]  → o offset DC
mediaQuadrados = somaQuadrados / numAmostras;   // E[x²]
variancia      = mediaQuadrados - (media*media) // E[x²] − E[x]²
rmsADC         = sqrt(variancia)
```

**Por que isso funciona.** A identidade `Var(x) = E[x²] − E[x]²` é a variância; sua raiz
é o desvio padrão. Como o sinal medido é `offset_DC + sinal_AC`, e a variância ignora
qualquer constante somada, `sqrt(variancia)` devolve **exatamente o valor RMS do sinal
AC, com o offset DC eliminado matematicamente**. Não é preciso conhecer o valor exato
do offset — o que torna a medida imune a um divisor resistivo levemente desbalanceado.

A linha `if (variancia < 0) variancia = 0;` protege contra o caso em que erro de
arredondamento de ponto flutuante produz um valor negativo minúsculo (o que faria
`sqrt()` retornar `NaN`) quando o sinal é praticamente nulo.

Por fim, `rmsADC` (em unidades de ADC) é convertido para Volts por `adcParaTensao()` e
guardado em `stats.rmsTensao`.

> **Nota sobre `variacao` vs `rmsTensao`:** `variacao` (pico-a-pico) é sensível a um
> único pico de ruído isolado; `rmsTensao` usa todas as amostras e por isso é a medida
> confiável. `variacao` fica no struct apenas como diagnóstico do Teste 2.

### 4.3 `adcParaTensao(valorADC)`

Regra de três simples entre a escala do ADC e a tensão de referência:

```cpp
return (valorADC / 4095.0) * 3.3;
```

Resolução resultante: **1 unidade de ADC ≈ 0,806 mV**.

### 4.4 `separador(char c)`

Utilitário de formatação: imprime 50 vezes o caractere recebido e quebra a linha. Só
serve para deixar a saída do monitor serial legível.

---

## 5. A bateria de testes

### Teste 1 — `teste1_offset()`: o divisor resistivo está correto?

Mede uma janela e compara `stats.media` com `OFFSET_ESPERADO` (2048).

| Condição | Veredito | Diagnóstico impresso |
|----------|----------|----------------------|
| \|diferença\| ≤ 250 | **PASSOU** | Divisor entregando ~1,65 V |
| média > 2298 | **FALHOU — offset alto** | Divisor alimentado com 5 V; R2 > R1; mau contato em R2 |
| média < 1798 | **FALHOU — offset baixo** | R1 > R2; mau contato em R1; capacitor invertido |

O valor de comparação é a **média**, e não um valor instantâneo — por isso o teste não
é enganado por ruído.

### Teste 2 — `teste2_ruido()`: o capacitor está filtrando?

Mede uma janela **sem carga no condutor** e olha para `stats.variacao` (pico-a-pico):

| Variação | Veredito | Interpretação |
|----------|----------|---------------|
| ≤ 50 (≈ 40 mV) | **PASSOU** | Ruído aceitável, capacitor filtrando bem |
| 51 – 200 | **ALERTA** | Funciona, mas não ideal: capacitor pequeno, EMI próxima ou fonte USB ruidosa |
| > 200 | **FALHOU** | Capacitor ausente/invertido/defeituoso, ou fio longo demais até o ADC |

### Teste 3 — `teste3_deteccaoSinal()`: o sensor responde?

Faz **10 medições** consecutivas (uma a cada 500 ms) e imprime uma tabela com a variação
e o RMS em mV de cada uma. É um teste **manual/interativo**: o operador liga e desliga a
carga durante as 10 medições e observa o RMS acompanhar.

Faixas de interpretação impressas pelo próprio código:

| RMS medido | Leitura |
|------------|---------|
| < 30 mV | Sem sinal significativo (ou sem corrente no condutor) |
| 30 – 150 mV | Corrente baixa |
| > 150 mV | Corrente moderada/alta |

---

## 6. O monitor ao vivo (`loop()`)

```cpp
AmostragemStats monitor = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);
Serial.printf("Monitor ao vivo RMS -> Offset DC: %.3fV | Sinal AC (RMS): %.4fV\n",
              adcParaTensao(monitor.media), monitor.rmsTensao);
delay(2000);
```

Cada iteração imprime duas grandezas:

- **Offset DC** — deve permanecer estável em ~1,65 V. Se derivar durante a operação, há
  problema de alimentação ou contato.
- **Sinal AC (RMS)** — sobe e desce conforme a corrente no condutor pinçado.

---

## 7. Temporização e taxa de amostragem

| Grandeza | Cálculo | Valor |
|----------|---------|-------|
| Intervalo entre amostras | `delayMicroseconds(200)` + tempo do `analogRead()` | ≳ 200 µs |
| Taxa de amostragem | 1 / 200 µs | ≲ 5 kHz |
| Duração de uma janela | 500 × 200 µs | ≈ 100 ms |
| Ciclos da rede por janela (60 Hz) | 100 ms ÷ 16,7 ms | ≈ **6 ciclos** |

Cobrir vários ciclos inteiros é o que faz o RMS ser estável: a janela sempre contém
semiciclos positivos e negativos em quantidade equilibrada. Pelo critério de Nyquist,
5 kHz permite representar componentes até ~2,5 kHz — suficiente para a fundamental de
60 Hz e suas primeiras harmônicas.

O tempo real de `analogRead()` no ESP32 (dezenas de µs) **não** é descontado do
`delayMicroseconds(200)`, então a taxa efetiva é um pouco menor que 5 kHz. Isso não
afeta o cálculo do RMS, mas deve ser considerado se algum dia houver análise espectral
(FFT), que exige taxa de amostragem conhecida com precisão.

---

## 8. Limitações conhecidas e pendências

Registradas aqui para não se perderem; acompanhar em
[`docs/diario-de-desenvolvimento.md`](diario-de-desenvolvimento.md).

1. **Linha 1 do sketch contém a palavra solta `cpp`** — sobra de uma colagem de bloco de
   código Markdown. O arquivo **não compila** enquanto ela existir. É a primeira coisa a
   corrigir.

2. **GPIO 4 pertence ao ADC2 no ESP32 clássico (WROOM).** O ADC2 fica **indisponível
   enquanto o Wi-Fi está ligado** — `analogRead()` passa a falhar. Como o projeto é de
   IoT e vai ligar o Wi-Fi, é necessário migrar o sinal para um pino do **ADC1
   (GPIO 32–39)** antes da fase de transmissão. O comentário `// GPIO 4 = ADC10` no
   código também está incorreto (GPIO 4 = ADC2 canal 0).

3. **Não há conversão para Ampères nem para Watts.** O sketch para no valor RMS em
   Volts. Falta o fator de calibração (A/V) — ver [`docs/calibracao.md`](calibracao.md).

4. **Não há aquisição de tempo (timestamp).** Para série temporal é indispensável;
   depende de sincronização NTP após o Wi-Fi.

5. **Não há persistência nem transmissão.** Os dados só existem no monitor serial e se
   perdem ao desconectar. Ver [`dados/README.md`](../dados/README.md) para o formato
   pretendido.

6. **`analogSetAttenuation(ADC_11db)` está depreciado** nas versões novas do core
   ESP32 (substituído por `ADC_ATTEN_DB_12`). Funciona, mas gera aviso de compilação; o
   valor de atenuação é o mesmo.

7. **O ADC do ESP32 é notoriamente não linear**, sobretudo perto de 0 V e de 3,3 V.
   Trabalhar em torno de 1,65 V (como o circuito faz) é a região mais bem comportada,
   mas para precisão maior seria necessário usar a calibração de fábrica (eFuse) ou uma
   curva de correção levantada experimentalmente.

8. **Potência aparente ≠ potência real.** Multiplicar corrente por tensão nominal dá
   **VA**, não W. Sem medir a tensão da rede e a defasagem, o fator de potência fica
   desconhecido — o que subestima ou superestima o consumo de cargas indutivas
   (motores, geladeira, ar-condicionado). Assumir tensão fixa de 127 V também ignora a
   variação real da rede.

---

## 9. Como compilar e gravar

Ver [`firmware/README.md`](../firmware/README.md).
