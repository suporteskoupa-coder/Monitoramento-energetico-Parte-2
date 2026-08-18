# Resumo das Alterações no Firmware (main.cpp)

* Troca de pino para ADC1: Migrou do GPIO 4 para o GPIO 34, liberando o canal e prevenindo conflitos com o Wi-Fi.

* Correção conceitual da potência: Nomenclatura ajustada para Potência Aparente (VA), calculada corretamente por Volts x Amperes sem assumir fator de potência unitário.

*  Agendamento nominal em 1 Hz: O loop() tenta iniciar uma gravação a cada 1 segundo utilizando millis(), sem bloquear o loop principal entre janelas.

*  Salvamento autônomo e rastreável: Cria o arquivo /leituras.csv com cabeçalho contendo timestamp_ms, tempo relativo, corrente, potência e metadados da campanha.

*  Filtros e Diagnóstico: Mantém os testes automáticos de offset DC e ruído pico-a-pico no setup(), além do cut-off para correntes residuais baixas.
