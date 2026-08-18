# Hardware

Documentação da parte física do protótipo: componentes, esquema de ligação, montagem e
princípio de funcionamento.

| Documento | Conteúdo |
|-----------|----------|
| [`lista-de-componentes.md`](lista-de-componentes.md) | Lista de materiais (BOM) com especificações e custos |
| [`esquema-de-ligacao.md`](esquema-de-ligacao.md) | Diagrama do circuito e mapa de pinos |
| [`montagem.md`](montagem.md) | Passo a passo da montagem e procedimento de segurança |
| [`esquematicos/`](esquematicos/) | Arquivos de esquemático (Fritzing, KiCad, PDF) |
| [`fotos/`](fotos/) | Fotos da montagem física |

---

## Princípio de funcionamento

```
   Condutor de                                    ┌──────────────┐
   fase da carga                                  │    ESP32     │
        ║                                         │              │
     ╭──╫──╮   sinal AC        ┌──────────────┐   │              │
     │ SCT │  ~±V, centrado    │ Divisor      │   │              │
     │ 013 ├───── em 0 V ──────┤ resistivo    ├───┤ GPIO 4 (ADC) │
     ╰─────╯                   │ + capacitor  │   │              │
    (grampo não                │ = offset     │   │              │
     invasivo)                 │   de 1,65 V  │   │  3,3 V ──────┼──┐
                               └──────┬───────┘   │  GND  ───────┼──┤
                                      └───────────┴──────────────┘  │
                                                                    ▼
                                                      alimenta o divisor
```

1. **Sensor.** O SCT-013 é um transformador de corrente do tipo grampo: ele se fecha em
   volta de **um único condutor** (fase **ou** neutro, nunca os dois) e induz no
   secundário um sinal proporcional à corrente que passa ali. Como é indutivo, **não há
   contato elétrico com a rede** — daí "não invasivo".

2. **Problema.** O sinal induzido é alternado e oscila em torno de zero, com semiciclos
   negativos. O ADC do ESP32 só lê de 0 V a 3,3 V.

3. **Condicionamento.** Um divisor resistivo cria um ponto médio em **1,65 V**
   (metade de 3,3 V). O sinal do sensor passa a oscilar em torno desse ponto, ficando
   inteiramente dentro da faixa que o ADC consegue ler. O capacitor eletrolítico de
   10 µF entre o ponto médio e o GND funciona como reservatório, mantendo esse ponto
   estável quando o sinal varia rapidamente.

4. **Leitura.** O firmware amostra o pino e separa novamente as duas componentes: a
   média das amostras é o offset DC (1,65 V), e o desvio em torno dela é o sinal AC
   real. Ver [`docs/firmware.md`](../docs/firmware.md).

## Segurança

> ⚠️ O sensor é não invasivo, mas a montagem é feita **perto da rede elétrica**.

- O grampo abraça **apenas um condutor**. Abraçando fase e neutro juntos, os campos se
  cancelam e a leitura dá **zero**.
- Nunca abra o grampo do SCT-013 com corrente passando pelo condutor **se o sensor não
  tiver resistor de burden interno** — um TC com secundário aberto sob carga desenvolve
  tensões perigosas nos terminais. O SCT-013-030 (saída em tensão) já tem burden
  interno; o SCT-013-000 (saída em corrente) **não tem** e exige o resistor externo.
- Nenhuma parte do circuito de baixa tensão deve tocar condutores energizados.
- Trabalhe com o disjuntor do circuito desligado ao posicionar o grampo.
