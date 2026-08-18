# Informações Pendentes

Este arquivo lista somente informações que não puderam ser confirmadas pelos artefatos atuais.
Nenhum campo abaixo deve ser preenchido por estimativa.

## Hardware — responsável principal: Pedro

- modelo exato e faixa do SCT-013, com foto legível da identificação;
- modelo exato da placa ESP32;
- confirmação do GPIO usado fisicamente;
- valores medidos, tolerâncias e identificação de R1 e R2;
- presença e valor do resistor de burden, se aplicável;
- valor e polaridade do capacitor;
- tensão medida no ponto médio na montagem que será usada;
- esquema final fiel à bancada;
- modelo do multímetro e da carga de teste;
- fotos finais da montagem e do grampo no condutor.

## Firmware e aquisição — responsável principal: Mel

- declaração de qual arquivo será o firmware oficial;
- escolha entre SPIFFS, captura serial ou uso combinado;
- configuração reproduzível de build e upload;
- versão do core ESP32 e identificação da placa;
- estratégia para timestamp real;
- cabeçalho canônico do CSV;
- método de transferência do arquivo do ESP32;
- política para falhas de offset, ruído, saturação e memória;
- evidência de teste com uma linha por segundo;
- versão ou commit associado ao próximo teste.

## Dados e campanhas — responsável principal: Nathália

- decisão entre primeiro registro e média na Campanha 01;
- confirmação do identificador oficial da Campanha 01;
- equipamento que foi monitorado na Campanha 01;
- firmware ou commit usado na Campanha 01;
- causas das quatro interrupções, somente se houver evidência;
- esquema final do CSV aceito pelo pipeline;
- formato e local do relatório automático de qualidade;
- protocolo e identificador do próximo teste-piloto.

## Metrologia — orientador e equipe

- disponibilidade, modelo, faixa e condição de calibração do alicate amperímetro;
- faixa de corrente de interesse;
- erro máximo admissível;
- critério de repetibilidade;
- limiar mínimo de detecção;
- duração de estabilização por ponto;
- quantidade e níveis das cargas de calibração;
- critério para aprovar a campanha definitiva.

## Governança e planejamento — equipe e orientador

- licença do repositório;
- prazo institucional do relatório parcial;
- data da próxima reunião;
- datas previstas do teste-piloto e da campanha definitiva;
- aprovação da pergunta de pesquisa, hipótese e escopo;
- referências científicas exigidas pelo orientador e padrão bibliográfico adotado.

## Como fechar uma pendência

Cada item deve ser substituído por informação acompanhada de evidência: foto, medição,
datasheet, commit, CSV, ata ou decisão registrada. Depois do fechamento, atualizar também os
documentos afetados e [`decisoes.md`](decisoes.md), quando aplicável.
