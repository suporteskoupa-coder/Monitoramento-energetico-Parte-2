# Montagem

Passo a passo para reproduzir o protótipo. Tempo estimado: 30–40 min.

## Antes de começar

- [ ] Conferir o modelo do sensor impresso no corpo do SCT-013 e registrar em
      [`lista-de-componentes.md`](lista-de-componentes.md)
- [ ] Medir R1 e R2 isoladamente com multímetro e anotar os valores reais
- [ ] Identificar a perna longa (positivo) do capacitor eletrolítico
- [ ] Ter o Arduino IDE com suporte a ESP32 já instalado

## Etapa 1 — Divisor de tensão

1. Encaixe o ESP32 na protoboard, deixando linhas livres dos dois lados.
2. Ligue `3V3` do ESP32 ao trilho **+** da protoboard e `GND` ao trilho **−**.
3. Monte `R1` do trilho **+** até uma linha livre — essa linha será o **ponto médio**.
4. Monte `R2` do **ponto médio** até o trilho **−**.

## Etapa 2 — Capacitor de estabilização

5. Encaixe o capacitor de 10 µF com a **perna longa (+) no ponto médio** e a perna curta
   (marcada com a faixa clara) no trilho **−**.

> ⚠️ Capacitor eletrolítico invertido pode inchar, vazar ou estourar. Confira a
> polaridade duas vezes antes de energizar.

## Etapa 3 — Sinal para o ADC

6. Ligue o **ponto médio** ao **GPIO 34**, conforme o candidato atual a firmware, com um
   jumper curto. Antes de energizar, confirme a pinagem na placa física e no código usado.

## Etapa 4 — Primeira verificação (ainda sem o sensor)

7. Alimente o ESP32 pela USB.
8. Com o multímetro, meça entre o ponto médio e o GND: deve dar **~1,65 V**.
9. Se o valor estiver fora, **pare e corrija** antes de continuar — não adianta ligar o
   sensor em um circuito de offset errado.

## Etapa 5 — Sensor

10. **Desligue o disjuntor** do circuito onde o sensor será instalado.
11. Abra o grampo do SCT-013 e feche-o em volta de **um único condutor** — a fase **ou**
    o neutro do equipamento a ser monitorado.

> ⚠️ Abraçar fase e neutro juntos faz os campos se cancelarem: a leitura dá **zero**.
> Este é o erro de montagem mais comum.

12. Ligue os dois terminais do plugue P2 do sensor: um ao **ponto médio**, outro ao
    **GND**. Ver [`esquema-de-ligacao.md`](esquema-de-ligacao.md).
13. Religue o disjuntor.

## Etapa 6 — Validação por software

14. Prepare e grave o firmware candidato (ver
    [`firmware/README.md`](../firmware/README.md)) e abra o Monitor
    Serial em **115200 baud**.
15. Observe os diagnósticos disponíveis no código usado:

| Verificação | O que observar |
|-------|---------------------|
| Offset DC | valor próximo do ponto médio medido com multímetro |
| Ruído | manter a carga desligada e registrar a variação pico a pico |
| Resposta à carga | ligar e desligar a carga durante um teste-piloto curto e observar o CSV/serial |

> O candidato atual apenas informa falhas; ele ainda não bloqueia automaticamente uma
> campanha inválida. Não continuar se offset, ruído, saturação ou armazenamento falharem.

16. Registre a saída completa do serial em
    [`experimentos/`](../experimentos/) e fotografe a montagem em [`evidencias/fotos/`](evidencias/fotos/).

## Registro de evidências

Cada montagem deve gerar, no mínimo:

- [ ] Foto geral da protoboard montada
- [ ] Foto em detalhe do divisor + capacitor (mostrando a polaridade)
- [ ] Foto do grampo posicionado no condutor
- [ ] Captura ou arquivo `.txt` com a saída completa dos 3 testes
- [ ] Leitura do multímetro no ponto médio

Arquivos em [`evidencias/fotos/`](evidencias/fotos/) e no diretório do experimento correspondente.
