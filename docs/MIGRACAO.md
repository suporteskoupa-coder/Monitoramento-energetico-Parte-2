# Migração para a estrutura v2

## Escopo

A estrutura foi criada em 18/08/2026 a partir da cópia de trabalho local do repositório
anterior. O repositório anterior não foi editado. A migração considerou todos os 114 arquivos
fora de `.git`, inclusive arquivos compactados e alterações locais existentes na cópia.

## Critérios adotados

- um único local canônico para firmware, hardware, dados e relatórios atuais;
- dados brutos preservados sem modificação;
- versões antigas e materiais de aprendizagem em `historico/`;
- conteúdo único de arquivos ZIP extraído para pastas normais;
- cópias byte a byte idênticas não duplicadas;
- metadados `.git` encontrados dentro de ZIPs descartados;
- arquivos acidentais chamados `cd`, `git`, `FETCH_HEAD` e um gitlink quebrado excluídos;
- ausência de suposições para preencher dados científicos desconhecidos.

## Mapeamento principal

| Origem anterior | Destino atual |
|---|---|
| `firmware/Firmware/main.cpp` | `firmware/src/main.cpp` |
| `firmware/Firmware/*.md` | `firmware/` e `firmware/docs/` |
| `firmware/Histórico_bancada/` | `historico/firmware/bancada/` |
| `dados/brutos/leituras.csv` | `dados/brutos/campanha-01/leituras.csv` |
| `dados/processados/base_consolidada.csv` | `dados/processados/campanha-01/base_consolidada_media.csv` |
| `dados/consolidar_base.py` | `dados/scripts/consolidar_base.py` |
| `controle/` e `resultados/` | `experimentos/.../relatorios/` |
| `semana/` e `semana8_9/` | `historico/acompanhamento/` |
| `relembrando/` | `historico/aprendizado/` |

O inventário detalhado está em [`migracao_manifesto.csv`](migracao_manifesto.csv), e a
contagem por ação está em [`migracao_resumo.csv`](migracao_resumo.csv). Entradas com `!`
representam arquivos encontrados dentro de um ZIP.

## Interpretação das ações

- `COPIADO`/`RENOMEADO`: conteúdo preservado em um destino explícito;
- `ADAPTADO`: conteúdo preservado e depois ajustado à nova organização;
- `EXTRAIDO`: item único recuperado de um ZIP;
- `DEDUPLICADO`: conteúdo idêntico já existia em outro destino;
- `EXCLUIDO`: lixo técnico, metadado Git ou pasta vazia sem conteúdo científico;
- `EXPANDIDO`: ZIP analisado, com suas entradas registradas separadamente;
- `ARQUIVADO`: configuração antiga preservada apenas para consulta.
