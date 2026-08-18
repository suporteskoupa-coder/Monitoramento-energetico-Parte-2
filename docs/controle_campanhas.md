# Controle de Campanhas

## Finalidade

Este documento define como registrar testes e campanhas para que cada CSV possa ser associado
à montagem, ao firmware, ao equipamento monitorado e às ocorrências da coleta.

## Classificações

- **Teste de bancada:** valida uma função ou componente isolado.
- **Teste-piloto:** valida a cadeia completa e a qualidade do arquivo, ainda sem conclusões de consumo.
- **Campanha diagnóstica:** dados preservados para investigar problemas de aquisição.
- **Campanha definitiva:** coleta realizada com montagem confirmada, firmware oficial e calibração válida.

## Campanha 01

| Campo | Registro atual |
|---|---|
| Identificador | `CAMPANHA_PILOTO_01` nos arquivos processados; padronização pendente |
| Classificação | **Campanha diagnóstica** |
| Início | 04/08/2026 16:03:55 |
| Término | 05/08/2026 08:37:56 |
| Local | Residência |
| Equipamento monitorado | Não registrado de forma verificável no controle atual |
| Dispositivo de aquisição | ESP32 |
| Responsável registrado | Nathália Barbosa Feitosa |
| Firmware/commit | Não registrado na campanha |
| Frequência planejada | 1 Hz |
| Frequência observada | Intervalos de 1 s nos trechos contínuos, com múltiplas linhas por timestamp |
| Calibração | Não validada com referência independente |
| Uso permitido | Diagnóstico de duplicidades, lacunas e organização dos dados |

### Qualidade dos dados

| Indicador | Valor |
|---|---:|
| Linhas brutas | 101.244 |
| Timestamps distintos | 16.709 |
| Duplicações exatas excedentes | 80.965 |
| Registros excedentes por timestamp | 84.535 |
| Timestamps com seis registros | 15.674 |
| Interrupções | 4 |

### Interrupções

| # | Último registro antes da lacuna | Primeiro registro depois da lacuna | Intervalo entre timestamps |
|---:|---|---|---:|
| 1 | 04/08/2026 16:30:10 | 04/08/2026 22:57:46 | 6 h 27 min 36 s |
| 2 | 04/08/2026 23:06:44 | 04/08/2026 23:07:21 | 37 s |
| 3 | 04/08/2026 23:12:54 | 04/08/2026 23:20:11 | 7 min 17 s |
| 4 | 05/08/2026 01:23:04 | 05/08/2026 06:43:11 | 5 h 20 min 7 s |

As causas não estão comprovadas e não devem ser inferidas sem evidência. O relatório
corrigido está em [Campanha Experimental Corrigida](../experimentos/2026-08-04_campanha-01-diagnostica/relatorios/campanha-experimental-corrigida.pdf).

## Regra de consolidação

O documento [Regra de Consolidação](../experimentos/2026-08-04_campanha-01-diagnostica/relatorios/regra-de-consolidacao.pdf) registra que a
Campanha 01 deve manter o primeiro registro de cada timestamp. O script atual calcula a média
dos registros do segundo. A equipe precisa fechar essa divergência em
[`decisoes.md`](decisoes.md).

## Registro obrigatório para novas campanhas

Copiar e preencher antes do início:

```text
ID da campanha:
Classificação:
Objetivo/hipótese:
Equipamento monitorado:
Local:
Responsáveis:
Data e hora planejadas:
Duração planejada:
Frequência planejada:
Sensor e número de identificação:
Montagem/esquema:
Firmware, versão e commit:
Método de timestamp:
Método de armazenamento:
Instrumento de referência:
Estado da calibração:
Arquivo bruto esperado:
Critérios de interrupção:
```

Ao encerrar, registrar:

```text
Início e término observados:
Quantidade de linhas:
Timestamps únicos:
Frequência observada:
Duplicidades:
Lacunas e reinícios:
Ocorrências:
Causa comprovada ou presumida, identificada como tal:
Arquivo bruto:
Arquivo processado:
Relatório de qualidade:
Conclusão sobre validade:
```

## Regras de governança

1. não sobrescrever o CSV bruto;
2. usar identificador único de campanha em todos os artefatos;
3. registrar a versão do firmware antes da coleta;
4. distinguir equipamento monitorado de dispositivo de aquisição;
5. documentar interrupções sem inventar causas;
6. não promover teste-piloto ou campanha diagnóstica a campanha definitiva;
7. manter dados, relatório de qualidade e conclusão vinculados.
