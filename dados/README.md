# Dados

Esta área separa evidência bruta, resultados processados e scripts. A base bruta não deve ser
editada ou substituída; qualquer transformação deve produzir um arquivo novo em
`processados/` e indicar a regra usada.

## Campanha 01

| Artefato | Caminho | Situação |
|---|---|---|
| base bruta | [`brutos/campanha-01/leituras.csv`](brutos/campanha-01/leituras.csv) | evidência original, 101.244 linhas de dados |
| consolidação pela média | [`processados/campanha-01/base_consolidada_media.csv`](processados/campanha-01/base_consolidada_media.csv) | saída reproduzível atual, 16.709 linhas |
| primeiro registro | [`processados/campanha-01/base_consolidada_primeiro_registro_legada.csv`](processados/campanha-01/base_consolidada_primeiro_registro_legada.csv) | saída legada preservada |
| script | [`scripts/consolidar_base.py`](scripts/consolidar_base.py) | gera a saída pela média |

## Esquemas

Base bruta:

```text
data,hora,corrente A,potencia VA
```

Base processada pela média:

```text
data,hora,corrente_a,potencia_va,campanha
```

O firmware candidato produz outro esquema (`timestamp_ms`, `tempo_relativo`,
`corrente_a`, `potencia_va`, `campanha`). A integração ainda deve ser definida antes da
próxima campanha.

## Reproduzir

A partir da raiz do repositório:

```bash
pip install -r requirements.txt
python dados/scripts/consolidar_base.py
```

O script funciona independentemente da pasta atual, valida as colunas obrigatórias e grava
somente em `dados/processados/campanha-01/base_consolidada_media.csv`.

## Regra ainda aberta

O pipeline atual calcula a média de todas as leituras do mesmo segundo. Um relatório legado
determina manter a primeira leitura. Nenhuma das alternativas deve ser declarada definitiva
até a equipe registrar a decisão em [`../docs/decisoes.md`](../docs/decisoes.md).
