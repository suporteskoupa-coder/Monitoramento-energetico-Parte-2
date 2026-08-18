# Experimentos e campanhas

Cada teste-piloto, calibração ou campanha deve ter uma pasta própria com o padrão
`AAAA-MM-DD_tipo_identificador-curto`. Copie [`_modelo/`](_modelo/) antes de iniciar.

## Campanhas registradas

| Campanha | Classificação | Situação |
|---|---|---|
| [`2026-08-04_campanha-01-diagnostica/`](2026-08-04_campanha-01-diagnostica/) | diagnóstica | preservada; calibração não validada |

## Organização de cada pasta

```text
experimento/
|-- README.md       # metadados, objetivo, ocorrências e conclusão
|-- evidencias/     # fotos, saída serial e registros dos instrumentos
`-- relatorios/     # análises e documentos derivados
```

Os CSVs ficam centralizados em `dados/brutos/<campanha>/` e
`dados/processados/<campanha>/`, com links no README do experimento.

## Regras mínimas

- registrar equipamento, local, responsáveis, sensor, montagem e firmware/commit;
- documentar frequência, timestamp, armazenamento, calibração e ocorrências;
- nunca mover, editar ou sobrescrever dados brutos depois da coleta;
- produzir um novo arquivo para cada transformação e registrar a regra;
- não classificar como definitiva uma campanha sem montagem e calibração validadas.

O procedimento completo está em [`../docs/controle_campanhas.md`](../docs/controle_campanhas.md).
