# Registro de Decisões

## Finalidade

Registrar decisões técnicas e metodológicas que afetam mais de uma frente. Uma decisão só é
considerada fechada quando há evidência no repositório e concordância dos responsáveis.

## Decisões registradas

### D-001 — Campanha 01 classificada como diagnóstica

- **Status:** adotada nos relatórios corrigidos;
- **Motivo:** há duplicidades, quatro interrupções e ausência de calibração independente;
- **Consequência:** a campanha pode apoiar diagnóstico da aquisição, mas não conclusões
  quantitativas sobre consumo;
- **Evidências:** `resultados/Campanha Experimental Corrigida.pdf` e
  `controle/Duplicidades.pdf`.

### D-002 — Grandeza denominada potência aparente estimada

- **Status:** aplicada no firmware e nos dados atuais;
- **Motivo:** o cálculo usa corrente multiplicada por tensão nominal de 127 V;
- **Consequência:** usar unidade VA e não apresentar o resultado como potência ativa em W;
- **Evidência:** `firmware/src/main.cpp`.

### D-003 — GPIO 34 no candidato atual a firmware

- **Status:** implementada no código, pendente de confirmação na montagem física;
- **Motivo:** o GPIO 34 pertence ao ADC1 e evita o conflito do ADC2 com futuro uso de Wi-Fi;
- **Consequência:** toda documentação de montagem deve ser conferida contra a bancada antes
  de ser declarada final;
- **Evidência:** `firmware/src/main.cpp`.

### D-004 — Preservação da base bruta

- **Status:** aplicada;
- **Motivo:** manter a evidência original para auditoria e reprodução;
- **Consequência:** transformações são salvas separadamente em `dados/processados/`;
- **Evidências:** `dados/brutos/campanha-01/leituras.csv` e `dados/processados/campanha-01/`.

## Decisões pendentes

### P-001 — Firmware e arquitetura oficial de aquisição

- **Opções existentes:** `main.cpp` com SPIFFS; protótipo `.ino`; captura serial por Python;
- **Decisão necessária:** escolher um fluxo oficial e definir se os demais serão apenas histórico;
- **Responsável principal:** Mel;
- **Participantes:** Pedro e Nathália;
- **Evidência para fechamento:** execução integrada, instrução única e CSV gerado.

### P-002 — Esquema canônico do CSV

- **Problema:** firmware atual e script de consolidação usam colunas incompatíveis;
- **Decisão necessária:** definir timestamp, campanha, equipamento, local, firmware, corrente,
  potência aparente e indicadores de qualidade;
- **Responsáveis:** Mel e Nathália;
- **Evidência para fechamento:** mesmo cabeçalho no produtor, coletor, exemplo e pipeline.

### P-003 — Regra da Campanha 01: primeiro registro ou média

- **Problema:** o PDF de controle manda manter o primeiro registro; o script usa média;
- **Impacto verificado:** as duas abordagens produzem valores diferentes em parte dos segundos;
- **Responsável principal:** Nathália;
- **Validação:** orientador;
- **Evidência para fechamento:** regra aprovada, código alinhado e base regenerada se necessário.

### P-004 — Modelo do SCT-013 e circuito correspondente

- **Problema:** o repositório menciona SCT-013-000 e SCT-013-030;
- **Responsável:** Pedro;
- **Evidência para fechamento:** foto legível do sensor, valores de R1/R2, burden e esquema real.

### P-005 — Critérios metrológicos

- **Itens:** faixa de corrente, erro máximo, repetibilidade, limiar e protocolo de validação;
- **Responsável pela definição:** orientador com a equipe;
- **Dependência:** alicate amperímetro AC;
- **Evidência para fechamento:** protocolo aprovado e tabela de calibração.

### P-006 — Licença do repositório

- **Problema:** nenhuma licença foi declarada;
- **Responsáveis:** equipe e orientador;
- **Evidência para fechamento:** arquivo `LICENSE` e indicação no README.

## Modelo para novas decisões

```text
ID e título:
Data:
Status: proposta | aprovada | rejeitada | substituída
Contexto:
Opções consideradas:
Decisão:
Justificativa:
Responsáveis:
Artefatos afetados:
Evidência de validação:
```
