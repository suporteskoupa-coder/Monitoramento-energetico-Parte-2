# Estado do Firmware

## Fonte atual

O arquivo [`../firmware/src/main.cpp`](../firmware/src/main.cpp) é o candidato
atual a firmware oficial. Ele substitui conceitualmente o antigo sketch de validação descrito
em versões anteriores deste documento.

> **Status:** código disponível, mas ainda sem configuração de build no repositório e sem
> evidência de uma execução integrada que produza o formato consumido pelo pipeline Python.

Os arquivos em [`../historico/firmware/bancada/`](../historico/firmware/bancada/) são
protótipos preservados e não devem ser usados como versão oficial sem nova decisão.

## Fluxo implementado

```text
setup
  -> configura ADC1 no GPIO 34
  -> monta SPIFFS
  -> cria /leituras.csv se necessário
  -> executa diagnóstico de offset e ruído

loop
  -> inicia uma janela nominal a cada 1 s
  -> coleta 600 amostras
  -> calcula média, mínimo, máximo e RMS AC
  -> estima corrente
  -> estima potência aparente
  -> grava CSV e espelha na serial
```

## Parâmetros encontrados no código

| Parâmetro | Valor | Observação |
|---|---:|---|
| GPIO do sinal | 34 | ADC1; montagem física ainda precisa ser confirmada |
| resolução | 12 bits | 0 a 4095 |
| amostras por janela | 600 | valor do código atual |
| atraso solicitado | 100 µs | o tempo real também inclui `analogRead()` |
| intervalo de gravação | 1.000 ms | intervalo nominal controlado por `millis()` |
| Vref usada no cálculo | 3,3 V | hipótese de conversão, não calibração do ADC |
| fator de corrente | 29,0 A/V | provisório, sem validação experimental registrada |
| tensão nominal | 127 V | usada para estimar potência aparente |
| corte de corrente | 0,05 A | precisa ser confrontado com o piso de ruído |

## Cálculo RMS

O firmware calcula a variância das leituras para retirar o offset DC:

```text
variância = E[x²] - E[x]²
RMS_AC = sqrt(variância)
```

Depois converte a contagem para tensão por uma regra linear e aplica o fator A/V. A abordagem
remove a média da janela, mas não corrige sozinha a resposta do ADC nem substitui a calibração
do conjunto.

## CSV produzido pelo código atual

Cabeçalho presente em `main.cpp`:

```csv
timestamp_ms,tempo_relativo,corrente_a,potencia_va,campanha
```

Limitações:

- `timestamp_ms` e `tempo_relativo` não são data e hora civis;
- `tempo_relativo` volta a `00:00:00` após 24 horas;
- o campo `ID_EQUIPAMENTO` é exibido na inicialização, mas não é gravado no CSV;
- local, versão do firmware e commit não aparecem em cada registro;
- o script `dados/scripts/consolidar_base.py` espera outro cabeçalho.

## Diagnósticos e falhas

O firmware verifica offset, ruído e saturação, mas as falhas apenas geram mensagens. A coleta
não é bloqueada automaticamente. Uma falha na montagem do SPIFFS também não impede o `loop`
de tentar continuar.

Antes de validar o firmware oficial, a equipe deve definir:

- quais falhas bloqueiam a campanha;
- como registrar o estado de qualidade no CSV;
- como evitar perda ou formatação indevida do armazenamento;
- como limitar ou rotacionar o arquivo;
- como transferir o CSV do ESP32;
- como obter timestamp real.

## Pendências para declarar versão oficial

1. escolher SPIFFS, captura serial ou combinação dos dois;
2. adicionar configuração reproduzível de compilação;
3. fixar placa e versão do core ESP32;
4. unificar o esquema do CSV com o pipeline de dados;
5. confirmar GPIO e montagem física;
6. substituir o fator provisório após calibração;
7. executar teste-piloto e guardar código, saída serial e CSV;
8. registrar versão e commit no controle de campanha.

As decisões abertas estão em [`decisoes.md`](decisoes.md), e os critérios do teste-piloto
estão em [`metodologia.md`](metodologia.md).
