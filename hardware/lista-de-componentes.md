# Lista de Componentes (BOM)

> Campos marcados com `<preencher>` dependem de conferência física do material em
> bancada. **Não preencher por estimativa** — o valor real dos resistores e o modelo
> exato do sensor mudam a calibração.

## Componentes principais

| # | Componente | Especificação | Qtd. | Função no circuito | Custo (R$) |
|---|-----------|---------------|------|--------------------|-----------|
| 1 | Placa de desenvolvimento | ESP32 WROOM (DevKit v1, 30 ou 38 pinos) | 1 | Aquisição, processamento e futura transmissão Wi-Fi | `<preencher>` |
| 2 | Sensor de corrente | SCT-013 — **modelo a confirmar**, ver nota abaixo | 1 | Medição não invasiva da corrente | `<preencher>` |
| 3 | Resistor R1 | `<preencher>` Ω, tolerância `<preencher>` % | 1 | Braço superior do divisor de tensão | `<preencher>` |
| 4 | Resistor R2 | `<preencher>` Ω, tolerância `<preencher>` % | 1 | Braço inferior do divisor de tensão | `<preencher>` |
| 5 | Capacitor eletrolítico | 10 µF / ≥ 16 V | 1 | Estabiliza o ponto médio de 1,65 V | `<preencher>` |
| 6 | Protoboard | 400 ou 830 pontos | 1 | Montagem do protótipo | `<preencher>` |
| 7 | Cabos jumper | macho-macho / macho-fêmea | ~10 | Interligação | `<preencher>` |
| 8 | Cabo USB | USB-A para micro-USB (dados, não só carga) | 1 | Alimentação e gravação | `<preencher>` |
| | | | | **Total** | `<preencher>` |

## Nota importante — qual SCT-013?

Os dois modelos citados no projeto **não são intercambiáveis** e exigem circuitos
diferentes:

| Modelo | Saída | Burden interno | Fator de conversão | Circuito extra |
|--------|-------|----------------|--------------------|----------------|
| **SCT-013-030** | Tensão — 1 V para 30 A | **Sim** | 30 A/V | Só o divisor de offset |
| **SCT-013-000** | Corrente — 50 mA para 100 A | **Não** | Depende do burden | Divisor **+ resistor de burden** |

O README original do projeto cita o **SCT-013-000 (100 A/50 mA)**, mas o firmware que
existiu em versões de trabalho assumia `AMPS_POR_VOLT = 30.0`, que corresponde ao
**SCT-013-030**. **Confirme o modelo impresso no corpo do sensor e registre aqui** —
usar o fator errado escala toda a medição de corrente incorretamente.

Se o sensor for o **-000**, é obrigatório calcular e instalar um resistor de burden.
Ver [`docs/calibracao.md`](../docs/calibracao.md).

## Instrumentos utilizados

| Instrumento | Modelo | Uso |
|-------------|--------|-----|
| Multímetro | `<preencher>` | Conferir valores de R1/R2 e a tensão do ponto médio |
| Alicate amperímetro (referência) | `<preencher>` | Medida de referência para calibração |
| Carga de teste | `<preencher>` (potência conhecida) | Ponto de calibração |
