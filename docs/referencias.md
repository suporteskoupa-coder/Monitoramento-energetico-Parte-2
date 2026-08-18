# Referências Técnicas e Científicas

## Critério de seleção

Foram priorizados documentos dos fabricantes, documentação oficial das ferramentas e fontes
metrológicas institucionais. O modelo exato do SCT-013 deve ser confirmado antes de escolher
o datasheet definitivo para cálculos e calibração.

## ESP32 e conversão analógico-digital

1. ESPRESSIF SYSTEMS. **ESP32-WROOM-32 Datasheet**. Versão 3.6. Disponível em:
   <https://documentation.espressif.com/esp32-wroom-32_datasheet_en.pdf>.
   Acesso em: 18 ago. 2026.

2. ESPRESSIF SYSTEMS. **Analog to Digital Converter (ADC) — ESP32**. ESP-IDF Programming
   Guide. Disponível em:
   <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/adc/index.html>.
   Acesso em: 18 ago. 2026.

3. ESPRESSIF SYSTEMS. **Analog to Digital Converter (ADC) Calibration Driver — ESP32**.
   ESP-IDF Programming Guide. Disponível em:
   <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc/adc_calibration.html>.
   Acesso em: 18 ago. 2026.

4. ESPRESSIF SYSTEMS. **Arduino-ESP32 ADC API**. Disponível em:
   <https://github.com/espressif/arduino-esp32/blob/master/docs/en/api/adc.rst>.
   Acesso em: 18 ago. 2026.

## Sensor de corrente

5. YHDC. **SCT013 Series Split Core Current Transformer — Datasheet**. Cópia do documento do
   fabricante disponibilizada pela Seeed Studio em:
   <https://files.seeedstudio.com/wiki/AC_Current_Sensor/Datasheet_of_SCT013.pdf>.
   Acesso em: 18 ago. 2026.

> Confirmar se a unidade física é SCT-013-000, SCT-013-030 ou outra variante. As variantes
> possuem saídas e requisitos de burden diferentes; não usar um fator de outro modelo.

## Processamento e rastreabilidade dos dados

6. PANDAS DEVELOPMENT TEAM. **pandas.DataFrame.groupby**. Disponível em:
   <https://pandas.pydata.org/docs/reference/api/pandas.DataFrame.groupby.html>.
   Acesso em: 18 ago. 2026.

7. PANDAS DEVELOPMENT TEAM. **pandas.read_csv**. Disponível em:
   <https://pandas.pydata.org/docs/reference/api/pandas.read_csv.html>.
   Acesso em: 18 ago. 2026.

## Metrologia

8. JOINT COMMITTEE FOR GUIDES IN METROLOGY. **International Vocabulary of Metrology — Basic
   and General Concepts and Associated Terms (VIM)**. Publicações do JCGM. Disponível em:
   <https://www.bipm.org/en/committees/jc/jcgm/publications>.
   Acesso em: 18 ago. 2026.

## Referências ainda necessárias

- artigo ou livro sobre cálculo RMS e amostragem de sinais de 60 Hz;
- referência científica sobre monitoramento não invasivo de cargas;
- trabalhos de séries temporais, clusterização e detecção de anomalias, somente quando essa
  etapa for metodologicamente liberada;
- manual e certificado do alicate amperímetro que será usado na calibração;
- datasheet definitivo da variante SCT-013 encontrada na bancada.

Novas fontes devem indicar autor, título, versão ou ano, endereço e data de acesso. Tutoriais
podem apoiar a aprendizagem, mas não devem substituir datasheets e documentação primária em
decisões de projeto.
