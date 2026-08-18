# Monitoramento Energético de Equipamentos Elétricos

Projeto de Iniciação Científica para aquisição e organização de dados de corrente elétrica
com ESP32 e sensor SCT-013. O objetivo é construir uma base experimental rastreável para
análise de séries temporais, identificação de padrões e, futuramente, detecção de anomalias.

## Situação atual

O repositório contém um protótipo de hardware, um firmware candidato, uma campanha
diagnóstica e um pipeline de consolidação. A Campanha 01 serve para estudar a qualidade da
aquisição; seus valores ainda **não são medições metrologicamente validadas**.

Resultados preservados:

- 101.244 registros brutos entre 04/08/2026 e 05/08/2026;
- 16.709 timestamps distintos e quatro interrupções identificadas;
- firmware com leitura do ADC, cálculo RMS e gravação em SPIFFS;
- documentos de hardware, metodologia, metrologia e governança;
- versões anteriores separadas em `historico/`, sem competir com os arquivos atuais.

## Comece por aqui

| Área | Entrada principal | Conteúdo |
|---|---|---|
| Projeto | [`docs/projeto.md`](docs/projeto.md) | escopo, estado e limites científicos |
| Metodologia | [`docs/metodologia.md`](docs/metodologia.md) | fluxo experimental e critérios de validação |
| Hardware | [`hardware/README.md`](hardware/README.md) | montagem, componentes, esquemas e evidências |
| Firmware | [`firmware/README.md`](firmware/README.md) | código candidato e pendências de integração |
| Dados | [`dados/README.md`](dados/README.md) | bases, esquema e reprodução do processamento |
| Experimentos | [`experimentos/README.md`](experimentos/README.md) | padrão de campanhas e Campanha 01 |
| Pendências | [`docs/pendencias_informacao.md`](docs/pendencias_informacao.md) | informações que precisam ser obtidas com a equipe |
| Migração | [`docs/MIGRACAO.md`](docs/MIGRACAO.md) | origem, destino e critérios da reorganização |

## Fluxo atualmente implementado

```text
SCT-013 -> condicionamento/offset -> ADC1 GPIO 34 -> cálculo RMS
        -> corrente estimada -> potência aparente estimada -> CSV no SPIFFS
```

O código atual está em [`firmware/src/main.cpp`](firmware/src/main.cpp). Ele ainda depende de
configuração externa de compilação e de uma validação ponta a ponta. Códigos anteriores estão
preservados em [`historico/firmware/`](historico/firmware/).

A potência registrada é **potência aparente estimada (VA)**, calculada com tensão nominal
fixa de 127 V. O sistema não mede tensão instantânea, potência ativa em watts ou fator de
potência.

## Campanha 01

| Indicador | Valor verificado |
|---|---:|
| Registros brutos | 101.244 |
| Timestamps distintos | 16.709 |
| Duplicações exatas excedentes | 80.965 |
| Registros excedentes pelo timestamp | 84.535 |
| Interrupções temporais | 4 |

Arquivos canônicos:

- [base bruta](dados/brutos/campanha-01/leituras.csv);
- [script de consolidação](dados/scripts/consolidar_base.py);
- [base consolidada pela média](dados/processados/campanha-01/base_consolidada_media.csv);
- [relatórios e evidências](experimentos/2026-08-04_campanha-01-diagnostica/README.md).

Para reproduzir a consolidação atual, execute a partir da raiz:

```bash
pip install -r requirements.txt
python dados/scripts/consolidar_base.py
```

Existe uma divergência metodológica aberta: o script calcula a média das leituras de um
segundo, enquanto um relatório anterior determina manter o primeiro registro. As duas saídas
foram preservadas e a decisão deve ser registrada em [`docs/decisoes.md`](docs/decisoes.md).

## Organização

```text
monitoramento-energetico-v2/
|-- dados/
|   |-- brutos/campanha-01/       # evidência original; nunca sobrescrever
|   |-- processados/campanha-01/  # resultados derivados
|   `-- scripts/                  # processamento reproduzível
|-- docs/                         # projeto, método e governança
|-- experimentos/                 # uma pasta por campanha
|-- firmware/
|   |-- src/                      # candidato atual
|   `-- docs/                     # arquitetura e explicações
|-- hardware/                     # montagem e evidências físicas
`-- historico/                    # material legado, fora do fluxo oficial
```

Arquivos compactados do repositório anterior foram examinados. Conteúdo único foi extraído e
preservado; cópias idênticas e metadados internos de Git não foram duplicados. O rastreamento
item a item está em [`docs/migracao_manifesto.csv`](docs/migracao_manifesto.csv).

## Equipe e responsabilidades

| Frente | Responsável | Produto esperado |
|---|---|---|
| Hardware e Instrumentação | Pedro Arnaldo Madeira Lopes | montagem confirmada e bancada preparada |
| Firmware e Aquisição | Mel Pereira Guimarães | firmware executável e teste integrado |
| Dados Experimentais | Nathália Barbosa Feitosa | bases oficiais e controle de campanhas |
| Documentação e Apoio Científico | **Fábio da Silva Cardoso Júnior** | navegação, método, limitações, referências e decisões |
| Orientação | Prof. Rogerio Mandelli | validação das decisões e instrumento de referência |

## Antes de uma campanha definitiva

É necessário confirmar um único firmware reproduzível, a montagem física, a identificação do
sensor e do equipamento, a regra de transformação, a calibração com referência independente
e um teste-piloto sem ambiguidades. Veja o [`cronograma`](docs/cronograma.md).

## Segurança e licença

O trabalho ocorre próximo à rede elétrica. A instalação deve ser feita com o circuito
desenergizado, e o grampo deve envolver apenas um condutor. O modelo exato do SCT-013 precisa
ser confirmado antes do uso.

A licença permanece pendente de decisão da equipe e do orientador. Sem um arquivo de licença,
não existe autorização explícita de reutilização, modificação ou redistribuição.
