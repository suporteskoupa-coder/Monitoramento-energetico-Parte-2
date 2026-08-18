#include <Arduino.h>
#include <time.h>


// Identificacao desta unidade (vai pro banco)
const char* EQUIPAMENTO = "Computador - 800w";
const char* LOCALIZACAO = "Residencia - Sala";
const char* UNIDADE     = "A";

// ============================================================
//  CALIBRACAO -- AJUSTE COM CARGA CONHECIDA NA BANCADA
// ============================================================
const float AMPS_POR_VOLT = 30.0;   // SCT-013-030: 1V = 30A
const float TENSAO_REDE   = 127.0;  // 127 ou 220, conforme sua rede
const float LIMIAR_RUIDO  = 0.030;  // abaixo de 30mV RMS = considera zero

float somaCorrente = 0;
int   contadorAmostras = 0;

// ============================================================
//  PARAMETROS
// ============================================================
const int PINO_ADC = 4;         // GPIO 4 = ADC10

const int   NUM_AMOSTRAS_TESTE = 500;   // Amostras por teste
const float VREF               = 3.3;   // Tensao de referencia
const int   ADC_MAX            = 4095;  // 12 bits

// Faixas de aceitacao (o que consideramos "OK")
const int   OFFSET_ESPERADO       = 2048;  // Meio da escala = 1,65V
const int   OFFSET_TOLERANCIA     = 250;   // +/- desse valor
const int   RUIDO_MAXIMO_OK       = 50;    // Variacao aceitavel

// ============================================================
//  ESTRUTURAS
// ============================================================
// Agrupa o resultado da amostragem, incluindo agora o valor RMS
struct AmostragemStats {
  int   minimo;
  int   maximo;
  int   variacao;   // maximo - minimo
  float media;
  float rmsTensao;  // Valor RMS convertido para Volts
};

// ============================================================
//  PROTOTIPOS
// ============================================================
void teste1_offset();
void teste2_ruido();
void teste3_deteccaoSinal();

AmostragemStats amostrarADC(int pino, int numAmostras, int atrasoMicros = 200);
float adcParaTensao(float valorADC);
void  separador(char c);
void imprimirLeitura(float correnteA, float potenciaVA);
String timestampISO8601();

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(2000);  // Espera Serial estabilizar

  // Configura ADC pra usar 12 bits e faixa ate 3,3V
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println();
  separador('=');
  Serial.println(" TESTE DE VALIDACAO - Circuito de Offset SCT-013");
  separador('=');
  Serial.println();
  Serial.println("Aguardando 3 segundos para estabilizacao...");
  Serial.println("(o capacitor precisa carregar completamente)");
  delay(3000);

  // Executa os 3 testes em sequencia
  teste1_offset();
  teste2_ruido();
  teste3_deteccaoSinal();

  Serial.println();
  separador('=');
  Serial.println(" TESTES CONCLUIDOS");
  separador('=');
  Serial.println("Se todos passaram: circuito OK, pode medir corrente.");
  Serial.println("Se algum falhou: veja a mensagem de diagnostico.");
  Serial.println(); 
  Serial.println("Reinicie o ESP32 para rodar os testes novamente.");
}

// ============================================================
//  LOOP - Monitoramento limpo com RMS
// ============================================================
void loop() {
    // Coleta um pacote de amostras e calcula o RMS para estabilizar a leitura ao vivo
    AmostragemStats monitor = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);

    // Converte RMS (Volts) para corrente (Amperes)
    float corrente = monitor.rmsTensao * AMPS_POR_VOLT;
    if (monitor.rmsTensao < LIMIAR_RUIDO) corrente = 0.0;  // descarta ruido de fundo

    somaCorrente += corrente;
    contadorAmostras++;
 
    float correnteMedia = somaCorrente / contadorAmostras;
    float potencia      = correnteMedia * TENSAO_REDE;   // potencia aparente (VA)

    imprimirLeitura(correnteMedia, potencia);
  
}

// ============================================================
//  FUNCOES AUXILIARES
// ============================================================

// Converte uma leitura ADC (0-4095) para tensao em Volts
float adcParaTensao(float valorADC) {
  return (valorADC / (float)ADC_MAX) * VREF;
}

void imprimirLeitura(float correnteA, float potenciaVA) {
  String ts = timestampISO8601();

  // Sem NTP sincronizado, mostra tempo desde o boot como referencia
  bool relogioOk = (ts.length() > 0);
  if (!relogioOk) {
    ts = "T+" + String(millis() / 1000) + "s (sem NTP)";
  }
  
  Serial.println();
  separador('-');
  Serial.printf("Equipamento : %s\n",      EQUIPAMENTO);
  Serial.printf("Timestamp   : %s\n",      ts.c_str());
  Serial.printf("Corrente    : %.3f A\n",  correnteA);
  Serial.printf("Potencia    : %.1f VA\n", potenciaVA);
  separador('-');
  Serial.println();
}

// Le N amostras do pino aplicando o calculo RMS para isolar o sinal AC
AmostragemStats amostrarADC(int pino, int numAmostras, int atrasoMicros) {
  int   minimo = ADC_MAX;
  int   maximo = 0;
  double soma = 0;
  double somaQuadrados = 0;

  for (int i = 0; i < numAmostras; i++) {
    int leitura = analogRead(pino);
    if (leitura < minimo) minimo = leitura;
    if (leitura > maximo) maximo = leitura;
    
    soma += leitura;
    somaQuadrados += ((double)leitura * leitura); // Necessario para o calculo RMS
    
    delayMicroseconds(atrasoMicros);
  }

  double media = soma / numAmostras;
  double mediaQuadrados = somaQuadrados / numAmostras;

  // Formula da variancia: remove o Offset DC matematicamente
  double variancia = mediaQuadrados - (media * media);
  if (variancia < 0) variancia = 0;

  double rmsADC = sqrt(variancia);

  AmostragemStats stats;
  stats.minimo    = minimo;
  stats.maximo    = maximo;
  stats.variacao  = maximo - minimo;
  stats.media     = media;
  stats.rmsTensao = adcParaTensao(rmsADC); // Converte o RMS do ADC para Volts

  return stats;
}

// Imprime uma linha separadora de 50 caracteres
void separador(char c) {
  for (int i = 0; i < 50; i++) Serial.print(c);
  Serial.println();
}


// ============================================================
//  TESTE 1 - OFFSET DC
// ============================================================
void teste1_offset() {
  separador('-');
  Serial.println(" TESTE 1: OFFSET DC");
  separador('-');
  Serial.println("Medindo o ponto medio do divisor de tensao...");
  Serial.println();

  AmostragemStats stats = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);
  float diferenca = stats.media - OFFSET_ESPERADO;

  Serial.printf("Offset medido: %.1f (%.3fV)\n", stats.media, adcParaTensao(stats.media));
  Serial.printf("Offset esperado: %d (%.3fV)\n", OFFSET_ESPERADO, adcParaTensao(OFFSET_ESPERADO));
  Serial.printf("Diferenca: %.1f unidades ADC\n", diferenca);
  Serial.println();

  // Diagnostico
  if (abs(diferenca) <= OFFSET_TOLERANCIA) {
    Serial.println(">>> RESULTADO: PASSOU");
    Serial.println("    O divisor esta gerando ~1,65V como esperado.");
  } else if (stats.media > OFFSET_ESPERADO + OFFSET_TOLERANCIA) {
    Serial.println(">>> RESULTADO: FALHOU - Offset ALTO demais");
    Serial.println("    POSSIVEIS CAUSAS:");
    Serial.println("    1. Voce alimentou o divisor com 5V em vez de 3,3V");
    Serial.println("    2. R2 (resistor inferior) esta maior que R1");
    Serial.println("    3. Mau contato em R2");
    Serial.println("    ACAO: Verifique se o trilho + esta em 3,3V");
    Serial.println("          e meça R1 e R2 isoladamente com multimetro");
  } else {
    Serial.println(">>> RESULTADO: FALHOU - Offset BAIXO demais");
    Serial.println("    POSSIVEIS CAUSAS:");
    Serial.println("    1. R1 (resistor superior) esta maior que R2");
    Serial.println("    2. Mau contato em R1");
    Serial.println("    3. Capacitor invertido puxando o offset pra baixo");
    Serial.println("    ACAO: Meca cada resistor isoladamente e");
    Serial.println("          verifique polaridade do capacitor");
  }

  Serial.println();
  delay(3000);
}

// ============================================================
//  TESTE 2 - RUIDO
// ============================================================
void teste2_ruido() {
  separador('-');
  Serial.println(" TESTE 2: NIVEL DE RUIDO");
  separador('-');
  Serial.println("Medindo variacao das leituras...");
  Serial.println();

  AmostragemStats stats = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);
  float variacaoTensaoMV = adcParaTensao(stats.variacao) * 1000;

  Serial.printf("Minimo lido: %d\n", stats.minimo);
  Serial.printf("Maximo lido: %d\n", stats.maximo);
  Serial.printf("Variacao (pico-a-pico): %d unidades (%.1f mV)\n", stats.variacao, variacaoTensaoMV);
  Serial.println();

  // Diagnostico
  if (stats.variacao <= RUIDO_MAXIMO_OK) {
    Serial.println(">>> RESULTADO: PASSOU");
    Serial.println("    Ruido esta em nivel aceitavel.");
    Serial.println("    O capacitor esta filtrando bem.");
  } else if (stats.variacao <= 200) {
    Serial.println(">>> RESULTADO: ALERTA - Ruido moderado");
    Serial.println("    O circuito funciona mas nao esta ideal.");
    Serial.println("    POSSIVEIS CAUSAS:");
    Serial.println("    1. Capacitor com valor menor que 10uF");
    Serial.println("    2. Interferencia eletromagnetica proxima");
    Serial.println("    3. Fonte USB do ESP32 muito ruidosa");
    Serial.println("    ACAO: teste em outro cabo/porta USB.");
  } else {
    Serial.println(">>> RESULTADO: FALHOU - Ruido excessivo");
    Serial.println("    O sinal esta muito instavel para medir.");
    Serial.println("    POSSIVEIS CAUSAS:");
    Serial.println("    1. Capacitor NAO conectado ou invertido");
    Serial.println("    2. Capacitor com defeito");
    Serial.println("    3. Fio muito longo entre divisor e ADC");
    Serial.println("    ACAO: verifique a polaridade do capacitor");
    Serial.println("          (perna longa = positivo, no ponto medio)");
  }

  Serial.println();
  delay(1000);
}

// ============================================================
//  TESTE 3 - DETECCAO DE SINAL AC
// ============================================================
void teste3_deteccaoSinal() {
  separador('-');
  Serial.println(" TESTE 3: DETECCAO DE SINAL DO SENSOR (COM RMS)");
  separador('-');
  Serial.println("Este teste mostra se o sensor esta captando algo.");
  Serial.println();
  Serial.println("VOCE VAI VER 10 MEDICOES:");
  Serial.println("  - Sem carga no fio pinçado: valor RMS baixo");
  Serial.println("  - Com carga: valor RMS aumenta proporcionalmente");
  Serial.println();
  Serial.println("Medicao | Variacao | Sinal AC (RMS)");
  Serial.println("--------+----------+----------------");

  for (int m = 0; m < 10; m++) {
    AmostragemStats stats = amostrarADC(PINO_ADC, NUM_AMOSTRAS_TESTE);
    float rmsMV = stats.rmsTensao * 1000; // Converte para mV para exibicao

    Serial.printf("   %d    |   %d    |   %.1f mV\n",
                  m + 1, stats.variacao, rmsMV);

    delay(500);
  }

  Serial.println();
  Serial.println(">>> COMO INTERPRETAR:");
  Serial.println("    - RMS < 30 mV: sem sinal significativo (ou sem corrente)");
  Serial.println("    - RMS 30-150 mV: corrente baixa");
  Serial.println("    - RMS > 150 mV: corrente moderada/alta");
  Serial.println();
  delay(1000);
}

// Retorna ISO 8601 em UTC, ou string vazia se o relogio nao sincronizou
String timestampISO8601() {
  struct tm t;
  // timeout 0 = so consulta o relogio e retorna. Sem isso, o default de 5s
  // travaria o loop a cada chamada enquanto o NTP nao estiver sincronizado.
  if (!getLocalTime(&t, 0)) return "";

  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &t);
  return String(buffer);
}