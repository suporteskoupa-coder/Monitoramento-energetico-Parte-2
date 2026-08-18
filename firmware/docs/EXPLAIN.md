# Guia Completo do Código - Monitor de Energia com ESP32 e SCT-013

> **Documento auxiliar.** Em caso de divergência, [`main.cpp`](../src/main.cpp) é a fonte do
> comportamento implementado. O código ainda é candidato a firmware oficial e não possui
> evidência de execução integrada no repositório.

## Visão Geral
Este código implementa um monitor de energia elétrica utilizando um ESP32 e um sensor de corrente SCT-013. O sistema realiza amostragens do sinal analógico, calcula valores RMS de corrente e potência, e armazena os dados em um arquivo CSV na memória interna (SPIFFS).

# Firmware Monitor de Energia ESP32 (Aquisição 1 Hz)

Firmware em C++/Arduino para **ESP32** voltado ao monitoramento contínuo de corrente elétrica e potência aparente. O sistema realiza amostragens de alta frequência no conversor analógico-digital (ADC1), extrai a componente RMS desconsiderando o offset DC, aplica filtros de corte e persiste as leituras em um arquivo CSV local via **SPIFFS**.

---

##  Funcionalidades

- **Amostragem em Alta Frequência:** Coleta 600 amostras a cada ciclo de leitura para isolamento preciso do sinal senoidal.
- **Cálculo de RMS via Variância:** Isola o offset de tensão do circuito sem necessidade de filtros analógicos complexos.
- **Armazenamento Local (SPIFFS):** Gravação automática nominal em `/leituras.csv` a intervalos de 1 segundo.
- **Diagnóstico Automático (Boot Self-Test):** Avalia a saúde do circuito checando o offset DC (~1,65V) e o nível de ruído pico a pico antes do início da amostragem.
- **Filtro de Ruído (Cut-off):** Descarta automaticamente correntes residuais inferiores a 50 mA.
- **Compatibilidade Wi-Fi:** Mapeado no pino **GPIO 34 (ADC1)**, garantindo coexistência segura com a pilha de rádio do ESP32.

---

## Metadados do Sistema

| Parâmetro | Valor Padrão | Descrição |
| :--- | :--- | :--- |
| **ID Campanha** | `CAMPANHA_PILOTO_01` | Identificador único do lote de medições |
| **ID Equipamento** | `ESP32_DEV_01` | Código de identificação do hardware |
| **Versão Firmware** | `v1.2.0-adc1` | Versão atual do firmware |
| **Taxa de Gravação** | 1 Hz | 1 registro gravado por segundo |

---

## Estrutura do Arquivo CSV (`/leituras.csv`)

Os dados são armazenados na memória flash do ESP32 no seguinte formato:

```csv
timestamp_ms,tempo_relativo,corrente_a,potencia_va,campanha
1000,00:00:01,0.450,57.2,CAMPANHA_PILOTO_01
2000,00:00:02,0.452,57.4,CAMPANHA_PILOTO_01

```

# Campos:

1. timestamp_ms: Tempo total de execução do ESP32 em milissegundos (millis()).
2. tempo_relativo: Formato HH:MM:SS calculado a partir do boot.
3. corrente_a: Corrente RMS calculada em Amperes (A).
4. potencia_va: Potência aparente calculada em Volt-Amperes (VA).
5. campanha: String com o identificador da campanha ativa.

#  Pinagem e Requisitos de Hardware

* MCU: ESP32 (qualquer variante padrão DevKit).
* Pino de Entrada: GPIO 34 (ADC1_CHANNEL_6).
* Atenuação ADC: ADC_11db (Escala completa de ~0V a 3.3V).
* Sensor Compatível: Transformador de Corrente (ex: SCT-013-000) com circuito de offset DC em Vref / 2 (~1.65V).

#  Estrutura do Código
```bash
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h> // Armazenamento interno da flash do ESP32

// ============================================================
// CONFIGURAÇÕES GERAIS E METADADOS
// ============================================================
const char* ID_CAMPANHA     = "CAMPANHA_PILOTO_01";
const char* ID_EQUIPAMENTO  = "ESP32_DEV_01";
const char* VERSAO_FIRMWARE = "v1.2.0-adc1";

// PINO MIGRADO: GPIO 34 pertence ao ADC1 (compatível com uso futuro de Wi-Fi)
const int PINO_ADC          = 34;     
const float VREF            = 3.3;    // Tensão de referência em Volts
const int ADC_MAX           = 4095;   // Resolução de 12 bits
const float TENSAO_REDE     = 127.0;  // Tensão nominal em Volts
const float FATOR_CALIB     = 29.0;   // Fator provisório; ver ../../docs/metrologia.md

// Controle de tempo e amostragem rigorosa a 1 Hz
const int NUM_AMOSTRAS      = 600;    // Quantidade de amostras por leitura (~3 ciclos em 60Hz)
const int ATRASO_MICROS     = 100;    // Intervalo entre amostras (us)
const unsigned long DELAY_GRAVACAO = 1000; // Intervalo nominal de 1 segundo

// Limites para testes de hardware
const int OFFSET_ESPERADO   = 2048;   // ~1,65V (metade da escala de 3,3V)
const int TOLERANCIA_OFFSET = 300;
const int RUIDO_MAXIMO      = 80;

const char* NOME_ARQUIVO    = "/leituras.csv"; // Arquivo CSV no SPIFFS

// ============================================================
// ESTRUTURAS DE DADOS
// ============================================================
struct LeituraADC {
  int min;
  int max;
  int delta;
  float media;
  float rmsTensao;
};

struct DadosProcessados {
  float corrente;
  float potenciaAparente; // Potência Aparente em VA
  bool erroSaturacao;
};

// ============================================================
// PROTÓTIPOS DAS FUNÇÕES
// ============================================================
void iniciarSistemaMemoria();
void rodarTestesHardware();
LeituraADC capturarSinal(int pino, int quantidade, int delayUs);
DadosProcessados calcularMedicao(const LeituraADC& adc);
void gravarRegistroCSV(unsigned long tempoMs, float corrente, float potenciaAparente);
float converterAdcParaVolts(float valor);

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configuração do conversor analógico/digital (ADC1)
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println("\n--- INICIALIZANDO MONITOR DE ENERGIA (FIRMWARE OFICIAL) ---");
  Serial.printf("Campanha: %s | Equipamento: %s | Firmware: %s\n", ID_CAMPANHA, ID_EQUIPAMENTO, VERSAO_FIRMWARE);

  // Liga o sistema de arquivos da memória interna
  iniciarSistemaMemoria();

  // Executa checagem dos componentes antes de iniciar a coleta
  rodarTestesHardware();

  Serial.println("--- INICIANDO COLETA E GRAVAÇÃO CONTÍNUA (1 Hz) ---");
  Serial.println("Formato: timestamp_ms,tempo_relativo,corrente_a,potencia_va,campanha");
}

// ============================================================
// LOOP PRINCIPAL (AQUISIÇÃO E ARMAZENAMENTO CONTINUO - 1Hz)
// ============================================================
void loop() {
  static unsigned long ultimoTempo = 0;
  unsigned long tempoAtual = millis();

  // Agenda a gravação em intervalo nominal sem usar delay no loop principal
  if (tempoAtual - ultimoTempo >= DELAY_GRAVACAO) {
    ultimoTempo = tempoAtual;

    // 1. Amostragem do hardware no GPIO 34
    LeituraADC adc = capturarSinal(PINO_ADC, NUM_AMOSTRAS, ATRASO_MICROS);

    // 2. Processamento dos dados
    DadosProcessados resultado = calcularMedicao(adc);

    // 3. Tratamento básico de erro no monitor serial
    if (resultado.erroSaturacao) {
      Serial.println("[ALERTA] Entrada do ADC saturou! Verifique se a corrente está muito alta.");
    }

    // 4. Salva no arquivo CSV interno e mostra no Serial Monitor
    gravarRegistroCSV(tempoAtual, resultado.corrente, resultado.potenciaAparente);
  }
}

// ============================================================
// FUNÇÕES DE AMOSTRAGEM E PROCESSAMENTO
// ============================================================

// Lê o pino e calcula o valor RMS isolando o offset DC
LeituraADC capturarSinal(int pino, int quantidade, int delayUs) {
  LeituraADC adc;
  adc.min = ADC_MAX;
  adc.max = 0;

  double soma = 0;
  double somaQuadrados = 0;

  for (int i = 0; i < quantidade; i++) {
    int v = analogRead(pino);

    if (v < adc.min) adc.min = v;
    if (v > adc.max) adc.max = v;

    soma += v;
    somaQuadrados += ((double)v * v);

    delayMicroseconds(delayUs);
  }

  adc.delta = adc.max - adc.min;
  adc.media = soma / quantidade;

  // Cálculo da variância para extrair o valor RMS sem a componente DC
  double variancia = (somaQuadrados / quantidade) - (adc.media * adc.media);
  if (variancia < 0) variancia = 0;

  adc.rmsTensao = converterAdcParaVolts(sqrt(variancia));

  return adc;
}

// Converte a tensão RMS em Corrente (A) e Potência Aparente (VA)
DadosProcessados calcularMedicao(const LeituraADC& adc) {
  DadosProcessados d;
  d.erroSaturacao = (adc.min == 0 || adc.max >= ADC_MAX);

  // Converte a tensão do secundário do sensor em corrente
  d.corrente = adc.rmsTensao * FATOR_CALIB;

  // Filtro de corte (cut-off): descarta ruídos menores que 50mA
  if (d.corrente < 0.05) {
    d.corrente = 0.0;
  }

  // Cálculo de Potência Aparente em VA (Corrente x Tensão Nominal)
  d.potenciaAparente = d.corrente * TENSAO_REDE;

  return d;
}

float converterAdcParaVolts(float valor) {
  return (valor / (float)ADC_MAX) * VREF;
}

// ============================================================
// GRAVAÇÃO DE DADOS (SPIFFS / CSV)
// ============================================================

void iniciarSistemaMemoria() {
  if (!SPIFFS.begin(true)) {
    Serial.println("[ERRO CRÍTICO] Falha ao montar o sistema de arquivos SPIFFS!");
    return;
  }

  // Cria o arquivo CSV com o cabeçalho completo e metadados caso ele não exista
  if (!SPIFFS.exists(NOME_ARQUIVO)) {
    File arq = SPIFFS.open(NOME_ARQUIVO, FILE_WRITE);
    if (arq) {
      arq.println("timestamp_ms,tempo_relativo,corrente_a,potencia_va,campanha");
      arq.close();
      Serial.println("[MEMÓRIA] Arquivo leituras.csv criado com sucesso.");
    } else {
      Serial.println("[ERRO] Não foi possível criar o arquivo CSV.");
    }
  } else {
    Serial.println("[MEMÓRIA] Arquivo leituras.csv localizado.");
  }
}

void gravarRegistroCSV(unsigned long tempoMs, float corrente, float potenciaAparente) {
  // Converte tempo rodando em tempo relativo HH:MM:SS
  unsigned long segsTotais = tempoMs / 1000;
  int h = (segsTotais / 3600) % 24;
  int m = (segsTotais / 60) % 60;
  int s = segsTotais % 60;

  char linhaCSV[128];
  // Formatação com timestamp exato em ms + tempo relativo + dados + ID da campanha
  snprintf(linhaCSV, sizeof(linhaCSV), "%lu,%02d:%02d:%02d,%.3f,%.1f,%s", 
           tempoMs, h, m, s, corrente, potenciaAparente, ID_CAMPANHA);

  // Grava no arquivo da memória flash (modo append)
  File arq = SPIFFS.open(NOME_ARQUIVO, FILE_APPEND);
  if (arq) {
    arq.println(linhaCSV);
    arq.close();
  } else {
    Serial.println("[ERRO] Falha ao abrir o arquivo para gravação.");
  }

  // Espelha o registro no Monitor Serial
  Serial.print("GRAVADO -> ");
  Serial.println(linhaCSV);
}

// ============================================================
// DIAGNÓSTICO E TESTES DE HARDWARE
// ============================================================

void rodarTestesHardware() {
  Serial.println("\n[TESTE] Verificando integridade do circuito...");

  LeituraADC teste = capturarSinal(PINO_ADC, NUM_AMOSTRAS, ATRASO_MICROS);

  Serial.printf("Offset medido no GPIO %d: %.1f (Esperado: %d) -> ", PINO_ADC, teste.media, OFFSET_ESPERADO);
  if (abs(teste.media - OFFSET_ESPERADO) <= TOLERANCIA_OFFSET) {
    Serial.println("OK");
  } else {
    Serial.println("FALHOU (Verifique o divisor de tensão)");
  }

  Serial.printf("Ruído P-P medido: %d unidades -> ", teste.delta);
  if (teste.delta <= RUIDO_MAXIMO) {
    Serial.println("OK");
  } else {
    Serial.println("ALERTA (Ruído elevado)");
  }
  Serial.println();
}
```
