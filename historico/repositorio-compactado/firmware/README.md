# Firmware

Código embarcado que roda no ESP32.

| Sketch | Descrição |
|--------|-----------|
| [`monitoramento_energetico/`](monitoramento_energetico/) | Sketch principal: valida o circuito de condicionamento e exibe o sinal do sensor ao vivo |

A explicação detalhada do que o código faz está em
[`docs/firmware.md`](../docs/firmware.md).

---

## Ambiente de desenvolvimento

| Item | Valor |
|------|-------|
| IDE | Arduino IDE 2.x (ou PlatformIO) |
| Core | `esp32` by Espressif Systems — versão utilizada: `<preencher>` |
| Placa selecionada | ESP32 Dev Module (WROOM) |
| Velocidade do monitor serial | **115200 baud** |
| Bibliotecas externas | nenhuma (só a API padrão do Arduino/ESP32) |

## Como gravar

1. Instale o suporte a ESP32 no Arduino IDE
   (*Preferências → URLs adicionais* → `https://espressif.github.io/arduino-esp32/package_esp32_index.json`,
   depois *Gerenciador de placas* → `esp32`).
2. Abra `monitoramento_energetico/monitoramento_energetico.ino`.
3. Selecione a placa **ESP32 Dev Module** e a porta serial correspondente.
4. Clique em *Upload*. Se der erro de conexão, segure o botão **BOOT** durante o início
   da gravação.
5. Abra o **Monitor Serial em 115200 baud**. Os testes começam ~5 s após o boot.

> **Antes de gravar:** confira as pendências listadas na
> [seção 8 de `docs/firmware.md`](../docs/firmware.md#8-limitações-conhecidas-e-pendências).
> A linha 1 do sketch contém um `cpp` solto que impede a compilação.

## Convenção da pasta do sketch

O Arduino IDE exige que a pasta tenha **o mesmo nome do arquivo `.ino`**. Por isso o
caminho é `monitoramento_energetico/monitoramento_energetico.ino`. Arquivos `.h`/`.cpp`
adicionais colocados nessa mesma pasta são compilados junto automaticamente.

## Histórico

O sketch vivia antes em `Semana 7/Código/`. Essa pasta foi removida na reorganização do
repositório por conter uma cópia idêntica deste mesmo arquivo — o histórico continua
acessível pelo Git.
