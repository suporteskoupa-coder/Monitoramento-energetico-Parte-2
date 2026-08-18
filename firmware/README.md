# Firmware candidato

O código mais recente está em [`src/main.cpp`](src/main.cpp). Ele é o candidato atual, mas
ainda não foi validado como fluxo oficial de ponta a ponta. O repositório não contém uma
configuração reproduzível de compilação (`platformio.ini` ou sketch Arduino completo).

## Funções implementadas

- leitura no GPIO 34 / ADC1;
- 600 amostras por janela e cálculo RMS;
- estimativa de corrente por fator provisório;
- potência aparente estimada com tensão nominal de 127 V;
- diagnóstico de offset, ruído e saturação;
- gravação em `/leituras.csv` no SPIFFS;
- espelhamento no monitor serial.

## Cabeçalho produzido

```csv
timestamp_ms,tempo_relativo,corrente_a,potencia_va,campanha
```

Esse esquema não é compatível com a base histórica processada por
[`dados/scripts/consolidar_base.py`](../dados/scripts/consolidar_base.py), que contém data e
hora civis. A integração precisa ser decidida antes do próximo teste-piloto.

## Parâmetros atuais

| Item | Valor |
|---|---:|
| placa pretendida | ESP32 WROOM/DevKit, modelo exato a confirmar |
| pino analógico | GPIO 34 / ADC1_CH6 |
| resolução | 12 bits |
| serial | 115200 baud |
| intervalo nominal | 1 s |
| fator de corrente | 29,0 A/V, provisório |
| tensão nominal | 127 V |
| corte de corrente | 0,05 A |

## Pendências para uso oficial

- configuração de build, upload e versão do core ESP32;
- confirmação da placa, sensor e montagem físicos;
- timestamp civil e metadados completos da campanha;
- procedimento de transferência do SPIFFS;
- teste integrado comprovando uma linha por segundo;
- fator obtido por calibração com referência independente.

## Documentação

- [estado detalhado](../docs/firmware.md);
- [metodologia](../docs/metodologia.md);
- [metrologia](../docs/metrologia.md);
- [decisões abertas](../docs/decisoes.md);
- [arquitetura](docs/ARCHITECTURE.md);
- [explicação do código](docs/EXPLAIN.md);
- [ajustes anteriores](docs/ADJUSTMENTS.md).

Protótipos anteriores foram preservados em [`../historico/firmware/`](../historico/firmware/)
e não fazem parte do fluxo atual.
