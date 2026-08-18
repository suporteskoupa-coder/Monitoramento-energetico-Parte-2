#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <time.h>

// ============================================================
// IDENTIFICAÇÃO E METADADOS
// ============================================================
const char* EQUIPAMENTO     = "Computador_800W";
const char* LOCALIZACAO     = "Residencia_Sala";
const char* ID_CAMPANHA     = "CAMPANHA_PILOTO_01";
const char* NOME_ARQUIVO    = "/leituras.csv"; // Arquivo CSV salvo na flash

// ============================================================
// CALIBRAÇÃO E CONFIGURAÇÕES DE HARDWARE
// ============================================================
// PINO MIGRADO: GPIO 34 pertence ao ADC1 (evita conflito com Wi-Fi)
const int PINO_ADC          = 34;     
const float AMPS_POR_VOLT   = 30.0;   // SCT-013-030: 1V = 30A
const float TENSAO_REDE     = 127.0;  // Tensão nominal em Volts
const float LIMIAR_RUIDO    = 0.030;  // Abaixo de 30mV RMS = considera 0.0A

const int NUM_AMOSTRAS_TESTE= 500;    // Amostras por leitura (~3 ciclos em 60Hz)
const float VREF            = 3.3;    // Tensão de referência
const int ADC_MAX           = 4095;   // 12 bits
const unsigned long DELAY_1HZ = 1000; // Intervalo exato de 1 Hz (1 registro por segundo)

// Limites para testes de hardware
const int OFFSET_ESPERADO   = 2048;   // Meio da escala = 1,65V
const int OFFSET_TOLERANCIA = 250;    
const int RUIDO_MAXIMO_OK   = 50;     

// ============================================================
// ESTRUTURAS
// ============================================================
struct AmostragemStats {
  int minimo;
  int maximo;
  int variacao;
  float media;
  float rmsTensao;
};

// ============================================================
// PROTÓTIPOS DAS FUNÇÕES
// ============================================================
void iniciarSPIFFS();
void teste1_offset();
void teste2_ruido();
void teste3_deteccaoSinal();

AmostragemStats amostrarADC(int pino, int numAmostras, int atrasoMicros = 200);
float adcParaTensao(float valorADC);
void separador(char c);
void gravarEImprimirLeitura(unsigned long tempoMs, float correnteA, float potenciaVA);
String timestampISO8601();

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(2000);

  // Configuração do ADC1
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println();
  separador('=');
  Serial.println(" INICIALIZANDO MONITOR DE ENERGIA ");
  separador('=');

  // 1. Inicializa o sistema de arquivos interno
  iniciarSPIFFS();

  Serial.println("\nAguardando 3 segundos para estabilização do circuito...");
  delay(3000);

  // 2. Executa os testes de integridade do hardware
  teste1_offset();
  teste2_ruido();
  teste3_deteccaoSinal();

  Serial.println();
  separador('=');
  Serial.println(" TESTES CONCLUIDOS - INICIANDO GRAVAÇÃO CONTINUA (1 Hz)");
  separador('=');
  Serial.println("Formato CSV: timestamp_ms,tempo_relativo,equipamento,corrente_a,potencia_va");
}

// ============================================================
// LOOP PRINCIPAL (COLETA E GRAVAÇÃO CONTINUA A 1 Hz)
// ============================================================
void loop() {
  static unsigned long ultimoTempo = 0;
  unsigned long tempoAtual = millis();

  // Controle de tempo rigoroso para 1 Hz
  if (tempoAtual - ultimoTempo >= DELAY_1HZ) {
    ultimoTempo = tempoAtual;

    // Coleta amostras do ADC
    AmostragemStats monitor = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);

    // Converte RMS (Volts) para corrente (A)
    float corrente = monitor.rmsTensao * AMPS_POR_VOLT;
    if (monitor.rmsTensao < LIMIAR_RUIDO) {
      corrente = 0.0; // Elimina ruído de fundo
    }

    // Cálculo da Potência Aparente (VA)
    float potenciaVA = corrente * TENSAO_REDE;

    // Grava no SPIFFS e imprime no terminal
    gravarEImprimirLeitura(tempoAtual, corrente, potenciaVA);
  }
}

// ============================================================
// GERENCIAMENTO DE MEMÓRIA (SPIFFS)
// ============================================================
void iniciarSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("[ERRO CRÍTICO] Falha ao montar o sistema de arquivos SPIFFS!");
    return;
  }

  // Cria o arquivo CSV com cabeçalho caso não exista
  if (!SPIFFS.exists(NOME_ARQUIVO)) {
    File arq = SPIFFS.open(NOME_ARQUIVO, FILE_WRITE);
    if (arq) {
      arq.println("timestamp_ms,tempo_relativo,equipamento,corrente_a,potencia_va");
      arq.close();
      Serial.println("[MEMÓRIA] Arquivo leituras.csv criado com sucesso.");
    } else {
      Serial.println("[ERRO] Falha ao criar o arquivo CSV.");
    }
  } else {
    Serial.println("[MEMÓRIA] Arquivo leituras.csv localizado na flash.");
  }
}

void gravarEImprimirLeitura(unsigned long tempoMs, float correnteA, float potenciaVA) {
  String ts = timestampISO8601();

  // Converte tempo rodando em tempo relativo HH:MM:SS
  unsigned long segsTotais = tempoMs / 1000;
  int h = (segsTotais / 3600) % 24;
  int m = (segsTotais / 60) % 60;
  int s = segsTotais % 60;

  char tempoRelativo[16];
  snprintf(tempoRelativo, sizeof(tempoRelativo), "%02d:%02d:%02d", h, m, s);

  if (ts.length() == 0) {
    ts = String(tempoRelativo);
  }

  // Monta a linha formatada em CSV
  char linhaCSV[128];
  snprintf(linhaCSV, sizeof(linhaCSV), "%lu,%s,%s,%.3f,%.1f",
           tempoMs, ts.c_str(), EQUIPAMENTO, correnteA, potenciaVA);

  // 1. Grava no arquivo interno da memória Flash
  File arq = SPIFFS.open(NOME_ARQUIVO, FILE_APPEND);
  if (arq) {
    arq.println(linhaCSV);
    arq.close();
  } else {
    Serial.println("[ERRO] Falha ao abrir leituras.csv para escrita.");
  }

  // 2. Exibe os dados formatados no Serial Monitor
  Serial.print("GRAVADO -> ");
  Serial.println(linhaCSV);
}

// ============================================================
// FUNÇÕES AUXILIARES E AMOSTRAGEM ADC
// ============================================================
float adcParaTensao(float valorADC) {
  return (valorADC / (float)ADC_MAX) * VREF;
}

AmostragemStats amostrarADC(int pino, int numAmostras, int atrasoMicros) {
  int minimo = ADC_MAX;
  int maximo = 0;
  double soma = 0;
  double somaQuadrados = 0;

  for (int i = 0; i < numAmostras; i++) {
    int leitura = analogRead(pino);
    if (leitura < minimo) minimo = leitura;
    if (leitura > maximo) maximo = leitura;

    soma += leitura;
    somaQuadrados += ((double)leitura * leitura);

    delayMicroseconds(atrasoMicros);
  }

  double media = soma / numAmostras;
  double mediaQuadrados = somaQuadrados / numAmostras;

  // Remoção matemática do Offset DC (variância)
  double variancia = mediaQuadrados - (media * media);
  if (variancia < 0) variancia = 0;

  AmostragemStats stats;
  stats.minimo    = minimo;
  stats.maximo    = maximo;
  stats.variacao  = maximo - minimo;
  stats.media     = media;
  stats.rmsTensao = adcParaTensao(sqrt(variancia));

  return stats;
}

void separador(char c) {
  for (int i = 0; i < 50; i++) Serial.print(c);
  Serial.println();
}

String timestampISO8601() {
  struct tm t;
  if (!getLocalTime(&t, 0)) return "";

  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &t);
  return String(buffer);
}

// ============================================================
// TESTES DE HARDWARE
// ============================================================
void teste1_offset() {
  separador('-');
  Serial.println(" TESTE 1: OFFSET DC");
  separador('-');
  
  AmostragemStats stats = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);
  float diferenca = stats.media - OFFSET_ESPERADO;

  Serial.printf("Offset medido no GPIO %d: %.1f (%.3fV)\n", PINO_ADC, stats.media, adcParaTensao(stats.media));
  
  if (abs(diferenca) <= OFFSET_TOLERANCIA) {
    Serial.println(">>> RESULTADO: PASSOU");
  } else {
    Serial.println(">>> RESULTADO: FALHOU - Verifique o divisor de tensão.");
  }
  delay(1000);
}

void teste2_ruido() {
  separador('-');
  Serial.println(" TESTE 2: NÍVEL DE RUÍDO");
  separador('-');

  AmostragemStats stats = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);
  Serial.printf("Variação pico-a-pico: %d unidades\n", stats.variacao);

  if (stats.variacao <= RUIDO_MAXIMO_OK) {
    Serial.println(">>> RESULTADO: PASSOU");
  } else {
    Serial.println(">>> RESULTADO: ALERTA/FALHA - Ruído elevado.");
  }
  delay(1000);
}

void teste3_deteccaoSinal() {
  separador('-');
  Serial.println(" TESTE 3: DETECÇÃO DE SINAL AC");
  separador('-');

  for (int m = 0; m < 5; m++) {
    AmostragemStats stats = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);
    Serial.printf("Amostra %d | Ruído: %d | Sinal RMS: %.1f mV\n", m + 1, stats.variacao, stats.rmsTensao * 1000);
    delay(300);
  }
  delay(1000);
}