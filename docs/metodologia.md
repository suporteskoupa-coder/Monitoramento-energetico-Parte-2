# Metodologia

## 1. Delineamento

O desenvolvimento é dividido em etapas dependentes: instrumentação, aquisição, teste-piloto,
calibração, campanha definitiva e análise. Uma etapa só é considerada concluída quando há
evidência reproduzível no repositório.

```text
montagem confirmada -> firmware oficial -> teste-piloto -> relatório de qualidade
                    -> calibração -> campanha definitiva -> análise de séries temporais
```

## 2. Instrumentação

O sistema utiliza um ESP32 e um transformador de corrente SCT-013. O sinal alternado do
sensor é condicionado para permanecer dentro da faixa de entrada do ADC. Um divisor cria um
nível de referência próximo de 1,65 V, e o firmware estima a componente alternada pela
dispersão das amostras em torno da média.

O candidato atual a firmware usa o **GPIO 34**, pertencente ao ADC1. Antes de qualquer
campanha, a equipe deve confirmar fisicamente:

- modelo exato do SCT-013;
- necessidade e valor do resistor de burden;
- valores medidos de R1 e R2;
- polaridade e valor do capacitor;
- tensão do ponto médio;
- pinagem realmente utilizada;
- correspondência entre esquema e bancada.

Os procedimentos de montagem e segurança estão em [`../hardware/`](../hardware/).

## 3. Aquisição do sinal

O arquivo [`../firmware/src/main.cpp`](../firmware/src/main.cpp) é o candidato
atual a firmware oficial. Ele:

1. configura o ADC em 12 bits e atenuação de 11 dB;
2. coleta 600 amostras com intervalo nominal de 100 microssegundos;
3. calcula mínimo, máximo, média e RMS da componente alternada;
4. converte o RMS para corrente por um fator provisório;
5. estima potência aparente usando tensão nominal de 127 V;
6. grava uma linha CSV no SPIFFS em intervalo nominal de um segundo.

Esse fluxo ainda não foi validado de ponta a ponta. Os protótipos de bancada e o coletor
serial anterior permanecem em [`../historico/firmware/bancada/`](../historico/firmware/bancada/)
e não devem ser confundidos com o candidato atual.

## 4. Cálculo RMS

Para uma janela com amostras `x`, o firmware usa:

```text
média = E[x]
variância = E[x²] - E[x]²
RMS_AC = sqrt(variância)
```

A remoção da média elimina matematicamente a componente DC do circuito de offset. O valor
obtido ainda depende da resposta do ADC, do condicionamento e da calibração do sensor.

## 5. Registro temporal e metadados

O fluxo definitivo deve registrar, no mínimo:

| Campo | Finalidade |
|---|---|
| timestamp real | ordenar e detectar lacunas entre reinícios |
| timestamp relativo | diagnosticar o tempo desde o boot |
| campanha | associar o registro ao protocolo experimental |
| equipamento monitorado | identificar a carga, não o ESP32 |
| local | contextualizar a coleta |
| versão/commit do firmware | permitir reprodução |
| corrente RMS | grandeza estimada em ampères |
| potência aparente | estimativa em VA |
| estado de qualidade | indicar saturação ou falha de hardware |

O firmware atual não produz o mesmo esquema esperado pelo script de consolidação. A escolha
entre SPIFFS, captura serial ou combinação dos dois permanece uma decisão técnica aberta.

## 6. Organização dos dados

- dados originais: `dados/brutos/`;
- dados transformados: `dados/processados/`;
- relatórios e interpretações: `resultados/`;
- evidências específicas de testes: `experimentos/`.

Dados brutos não devem ser editados ou sobrescritos. Toda transformação precisa indicar o
arquivo de entrada, a regra aplicada, o código utilizado e o arquivo de saída.

## 7. Consolidação da Campanha 01

A base bruta possui múltiplos registros dentro do mesmo segundo. O script atual agrupa por
`data` e `hora` e calcula a média de corrente e potência. O documento de controle registra a
regra de manter o primeiro registro.

Essa divergência está aberta e impede declarar uma regra metodológica definitiva. Para
campanhas futuras, o objetivo é produzir uma única janela RMS e uma única linha por segundo,
evitando consolidação corretiva.

## 8. Teste-piloto

Antes da calibração, deve ser realizada uma coleta curta para validar a cadeia de dados, sem
interpretar os valores como consumo calibrado.

Critérios mínimos:

- firmware e versão identificados;
- timestamp real e metadados completos;
- aproximadamente uma linha por segundo;
- ausência de duplicidades indevidas;
- lacunas e reinícios detectados e registrados;
- CSV bruto preservado;
- relatório automático concordante com o CSV;
- saída serial e fotos arquivadas como evidência.

## 9. Calibração e validação

A calibração seguirá [`calibracao.md`](calibracao.md) após a disponibilização de alicate
amperímetro AC. Devem ser usados pelo menos três pontos de calibração e uma carga adicional
para validação. O fator experimental, o erro percentual e as condições do ensaio devem ser
registrados.

Até essa etapa, corrente e potência são valores diagnósticos, não medições validadas.

## 10. Campanha definitiva

A campanha definitiva deve possuir protocolo anterior à coleta, contendo equipamento, local,
hipótese, duração, frequência, firmware, responsáveis, instrumentos e critérios de
interrupção. A base bruta, a base processada, o controle de ocorrências e o relatório de
qualidade devem permanecer vinculados pelo identificador da campanha.

## 11. Análise de séries temporais

Análise exploratória ampla, clusterização e detecção de anomalias só serão iniciadas depois de
uma campanha calibrada e temporalmente válida. A Campanha 01 pode ser usada apenas para
desenvolver verificações de qualidade e estudar falhas da aquisição.

## 12. Reprodutibilidade

Cada resultado deve informar:

- arquivo de origem;
- versão do código;
- parâmetros utilizados;
- data da execução;
- regra de transformação;
- responsável;
- limitações conhecidas.

O controle das decisões metodológicas está em [`decisoes.md`](decisoes.md).
