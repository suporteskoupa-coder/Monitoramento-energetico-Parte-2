# Projeto de Iniciação Científica

## Título

**Monitoramento Energético de Equipamentos Elétricos com IoT de Baixo Custo e Análise
Inteligente de Séries Temporais**

## Contexto e problema

O acompanhamento do comportamento elétrico de equipamentos pode auxiliar na identificação
de padrões de uso, desperdícios, falhas e alterações de operação. Soluções comerciais podem
ter custo elevado ou não disponibilizar os dados brutos necessários para investigação
científica.

O projeto investiga se uma cadeia de baixo custo, baseada em ESP32 e sensor de corrente não
invasivo SCT-013, pode produzir dados suficientemente rastreáveis e reproduzíveis para
caracterizar séries temporais de corrente e, em etapa posterior, apoiar análises de padrões e
anomalias.

O desafio atual não é apenas medir um sinal. É garantir coerência entre montagem, firmware,
tempo de aquisição, metadados, armazenamento, tratamento e documentação.

## Pergunta de pesquisa

Uma solução de baixo custo baseada em ESP32 e SCT-013 consegue gerar séries temporais de
corrente elétrica com rastreabilidade, frequência conhecida e qualidade suficiente para
análise experimental de equipamentos?

## Hipótese de trabalho

Após confirmação da montagem, calibração com referência independente e controle sistemático
da aquisição, o protótipo poderá produzir uma série temporal útil para identificar estados de
operação e alterações relevantes no comportamento de um equipamento.

Essa hipótese ainda **não foi validada**. A Campanha 01 é diagnóstica e não possui calibração
metrológica comprovada.

## Objetivo geral

Desenvolver e validar uma cadeia de monitoramento de corrente elétrica de baixo custo, desde
a instrumentação até a organização dos dados, como base para estudos posteriores de séries
temporais.

## Objetivos específicos

1. montar e documentar o circuito com ESP32 e SCT-013;
2. adquirir o sinal analógico e calcular seu valor RMS;
3. produzir registros temporais com frequência e metadados verificáveis;
4. preservar dados brutos e gerar bases processadas por regra documentada;
5. medir a qualidade das campanhas por duplicidades, lacunas e estabilidade;
6. calibrar a corrente com instrumento de referência e calcular o erro;
7. realizar uma campanha experimental válida;
8. somente então avaliar técnicas exploratórias, clusterização ou detecção de anomalias.

## Escopo atual

Incluído:

- medição de corrente por transformador de corrente não invasivo;
- condicionamento do sinal para o ADC do ESP32;
- cálculo RMS;
- estimativa de potência aparente com tensão nominal declarada;
- armazenamento e tratamento em CSV;
- diagnóstico de qualidade temporal.

Fora do escopo validado neste momento:

- medição direta de tensão;
- potência ativa em watts e fator de potência;
- certificação do instrumento;
- Wi-Fi, MQTT, dashboard ou operação remota;
- inferências de consumo baseadas na Campanha 01;
- inteligência artificial antes da campanha calibrada.

## Estado das evidências

| Evidência | Estado | Observação |
|---|---|---|
| Montagem e fotos | Parcial | Modelo do sensor, R1/R2 e ponto médio precisam ser confirmados |
| Firmware de aquisição | Parcial | Há candidato atual, mas falta validação integrada e build reproduzível |
| Campanha 01 | Diagnóstica | Quatro interrupções e duplicidades; não calibrada |
| Base bruta | Preservada | 101.244 registros em `dados/brutos/campanha-01/leituras.csv` |
| Base processada | Disponível | 16.709 timestamps, com regra metodológica ainda em discussão |
| Calibração | Pendente externa | Aguarda alicate amperímetro AC do orientador |
| Análise/IA | Não iniciada | Depende de campanha válida e calibrada |

## Produtos esperados

- hardware identificado e esquema fiel à montagem;
- firmware oficial executável;
- teste-piloto com uma linha por segundo e metadados completos;
- relatório automático de qualidade;
- calibração com fator e erro registrados;
- campanha experimental definitiva;
- base bruta, base processada e relatório parcial reproduzíveis;
- documentação científica consistente.

## Critério de sucesso

O protótipo será considerado apto à campanha definitiva quando uma pessoa externa conseguir
identificar a montagem, executar o firmware, localizar o CSV, reproduzir o tratamento e
verificar a calibração e a qualidade temporal sem depender de explicações verbais da equipe.

## Dependências e responsabilidades

- **Hardware:** confirmação física por Pedro;
- **Firmware e aquisição:** definição do fluxo oficial por Mel;
- **Dados:** regra de consolidação e controle por Nathália;
- **Documentação:** integração e rastreabilidade por Fábio;
- **Calibração:** execução pela equipe após disponibilização do alicate pelo orientador.

As decisões ainda abertas estão registradas em [`decisoes.md`](decisoes.md).
