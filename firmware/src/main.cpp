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
const float FATOR_CALIB     = 29.0;   // Fator de amostragem padrão (sujeito à calibração)[cite: 1]

// Controle de tempo e amostragem rigorosa a 1 Hz
const int NUM_AMOSTRAS      = 600;    // Quantidade de amostras por leitura (~3 ciclos em 60Hz)
const int ATRASO_MICROS     = 100;    // Intervalo entre amostras (us)
const unsigned long DELAY_GRAVACAO = 1000; // Salva exatamente 1 registro por segundo (1 Hz)

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

  // Garante a gravação rigorosa de 1 linha por segundo sem bloquear a CPU
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
