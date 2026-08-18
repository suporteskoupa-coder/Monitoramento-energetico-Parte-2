# Metrologia e Validade das Medições

## Status

> **Medição ainda não calibrada com referência independente.** Os valores da Campanha 01
> servem ao diagnóstico da aquisição, não a conclusões quantitativas sobre consumo.

## Grandezas do projeto

| Grandeza | Unidade | Como é obtida | Situação |
|---|---|---|---|
| leitura ADC | contagem | conversor de 12 bits do ESP32 | observada, não calibrada em tensão |
| tensão RMS do sinal | V | conversão linear provisória das contagens | dependente do ADC e Vref |
| corrente RMS | A | tensão RMS multiplicada por fator A/V | fator experimental pendente |
| potência aparente estimada | VA | corrente RMS × 127 V nominais | não é potência ativa medida |
| potência ativa | W | exigiria tensão e relação de fase | não medida |
| energia | Wh/kWh | exigiria potência ativa integrada no tempo | não medida |

## Cadeia de medição

```text
corrente no condutor
  -> transformador de corrente SCT-013
  -> burden, se aplicável, e circuito de offset
  -> ADC do ESP32
  -> cálculo RMS
  -> fator de conversão A/V
  -> corrente indicada
```

Cada elemento contribui para o erro total. A calibração do fator final não substitui a
necessidade de registrar montagem, modelo do sensor, faixa, saturação e condições do ensaio.

## Parâmetros atuais e pendências

| Parâmetro | Valor no firmware | Situação metrológica |
|---|---:|---|
| resolução | 12 bits | configurada, mas sem validação do ADC da unidade |
| Vref usada no cálculo | 3,3 V | hipótese; não representa calibração individual do ADC |
| fator de corrente | 29,0 A/V | provisório, sem tabela experimental no repositório |
| tensão da rede | 127 V | nominal e fixa, não medida durante a campanha |
| limiar de corrente | 0,05 A | precisa ser comparado ao piso de ruído medido |
| ponto médio esperado | aproximadamente 1,65 V | medição atual da bancada precisa ser confirmada |
| modelo SCT-013 | não confirmado | impede validar fator e circuito de burden |

## Principais fontes de incerteza

- variação e não linearidade do ADC;
- valor real da tensão de alimentação e da referência do ADC;
- tolerância e temperatura de R1, R2 e burden;
- modelo e classe de exatidão do SCT-013;
- posição do condutor dentro do núcleo;
- ruído elétrico e interferência eletromagnética;
- saturação do sensor ou do ADC;
- frequência e duração da janela RMS;
- limiar que transforma leituras pequenas em zero;
- uso de tensão nominal no cálculo da potência aparente;
- exatidão e validade de calibração do instrumento de referência.

## Rastreabilidade mínima

Cada calibração ou campanha deve registrar:

- identificação e faixa do sensor;
- valores medidos dos componentes;
- placa e pino utilizados;
- firmware e commit;
- identificação do alicate amperímetro e validade de calibração, se disponível;
- cargas utilizadas;
- data, local, responsáveis e condições do ensaio;
- dados brutos e cálculos do fator;
- erro de validação.

## Calibração planejada

O procedimento completo está em [`calibracao.md`](calibracao.md). Em resumo:

1. confirmar montagem e ausência de saturação;
2. medir o piso de ruído sem carga;
3. aplicar ao menos três níveis de corrente;
4. registrar simultaneamente referência e indicação do protótipo;
5. ajustar o fator A/V;
6. validar com uma carga não usada no ajuste;
7. calcular erro absoluto, relativo e percentual;
8. atualizar firmware e documentação.

A execução depende de alicate amperímetro AC fornecido pelo orientador.

## Critérios de aceitação

Ainda precisam ser definidos com o orientador:

- faixa de corrente de interesse;
- erro máximo admissível;
- repetibilidade mínima;
- limiar de detecção;
- duração de estabilização;
- frequência e número de pontos de validação.

Até que esses limites sejam aprovados, o projeto não deve empregar termos como “medição
precisa”, “consumo real” ou “instrumento validado”.

## Forma correta de apresentar os resultados

Antes da calibração:

> Valores diagnósticos estimados pela cadeia ESP32/SCT-013, sem validação metrológica
> independente.

Depois da calibração:

> Corrente RMS estimada na faixa validada, acompanhada do método, erro observado e condições
> de calibração.

A potência deve permanecer identificada como **potência aparente estimada em VA** enquanto
não houver medição sincronizada de tensão e corrente.
