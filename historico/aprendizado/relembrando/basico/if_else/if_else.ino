// --- Definição dos Pinos ---
// Nomeamos os pinos para que o código seja fácil de ler e manter.
// O "int" reserva um espaço na memória para o número da porta digital do Arduino.
int pinVermelho = 10;          // Semáforo carros
int pinAmarelo = 9;            // Semáforo carros
int pinVerde = 8;             // Semáforo carros
int pinBotao = 7;             // Botão de interação
int pinPedestreVermelho = 3;  // Semáforo pedestres
int pinPedestreVerde = 2;     // Semáforo pedestres

// --- Variáveis de Controle de Estado ---
int faseSemaforo;         // Armazena em qual estágio o semáforo está (1, 2, 3 ou 4)
int estadoBotao;          // Lê se o botão está apertado agora (HIGH ou LOW)
int estadoAnteriorBotao;  // Guarda o estado anterior para detectar a "subida" do clique

// --- Variáveis de Temporização Auxiliar ---
// Usadas para criar o efeito de piscar sem travar o código com delay()
int tempoPisca;           // Contador que incrementa a cada ciclo do loop
int estadoPisca;          // Alterna entre HIGH e LOW para o LED piscar

void setup() {
  // Configura os pinos dos LEDs como SAÍDA (enviam energia)
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAmarelo, OUTPUT);
  pinMode(pinVermelho, OUTPUT);
  pinMode(pinPedestreVerde, OUTPUT);
  pinMode(pinPedestreVermelho, OUTPUT);

  // Configura o pino do botão como ENTRADA (recebe sinal do mundo externo)
  pinMode(pinBotao, INPUT);

  // Inicialização do sistema
  faseSemaforo = 1;                   // Começa no Verde para carros
  /* O que o digitalRead faz? Ele lê o pino do botão e retorna se ele está pressionado (HIGH/1) ou solto (LOW/0). */
  estadoAnteriorBotao = digitalRead(pinBotao); // Lê o estado inicial para evitar disparos falsos
  tempoPisca = 0;
  // inicializando para nao conter valores lixo 
  /* você garante que, no exato momento em que o semáforo entrar na Fase 4 (alerta), o LED comece aceso. Se começasse em LOW, haveria um pequeno atraso (o tempo de contagem) antes de você ver a primeira luz. */
  estadoPisca = HIGH;
}

void loop() {
  // 1. LEITURA E LÓGICA DO BOTÃO (Edge Detection)
  // O loop roda milhares de vezes por segundo. Precisamos saber se o botão ACABOU de ser apertado.
  estadoBotao = digitalRead(pinBotao);

  // Verificamos se o botão está HIGH AGORA e estava LOW ANTES.
  // Isso garante que a fase só mude UMA VEZ por clique, mesmo que você segure o botão.
  if (estadoBotao == HIGH && estadoAnteriorBotao == LOW) {
    if (faseSemaforo < 4) {           // Se não chegou na última fase (4), avança
      faseSemaforo = faseSemaforo + 1;
    } else {                          // Se já estava na fase 4, volta para a 1
      faseSemaforo = 1;
    }
  }
  // Atualiza o estado anterior para a próxima volta do loop.
  // Sem isso, o "estadoAnterior" seria sempre o do setup.
  estadoAnteriorBotao = estadoBotao;

  // 2. EXECUÇÃO DAS FASES (Controle das Luzes)

  // FASE 1: Carros passam (Verde), Pedestres esperam (Vermelho)
  if (faseSemaforo == 1) {
    digitalWrite(pinVerde, HIGH);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, LOW);
    digitalWrite(pinPedestreVerde, LOW);
    digitalWrite(pinPedestreVermelho, HIGH);
  }

  // FASE 2: Transição para carros (Amarelo), Pedestres ainda esperam
  if (faseSemaforo == 2) {
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, HIGH);
    digitalWrite(pinVermelho, LOW);
    digitalWrite(pinPedestreVerde, LOW);
    digitalWrite(pinPedestreVermelho, HIGH);
  }

  // FASE 3: Carros param totalmente (Vermelho), Pedestres atravessam (Verde)
  if (faseSemaforo == 3) {
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, HIGH);
    digitalWrite(pinPedestreVerde, HIGH);
    digitalWrite(pinPedestreVermelho, LOW);
  }

  // FASE 4: Alerta de fechamento para pedestres.
  // Carros continuam no Vermelho por segurança.
  if (faseSemaforo == 4) {
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, HIGH);
    digitalWrite(pinPedestreVerde, LOW);

    // O Arduino conta cada volta do loop até chegar em 400.
    tempoPisca = tempoPisca + 1; 

    if (tempoPisca == 400) {
      // Se estava ligado vai desligar
      // Se estava desligado vai ligar
      // Mudança de estado 
      estadoPisca = !estadoPisca; // O símbolo '!' inverte o valor (HIGH -> LOW)
      // se chegou a 400 volta a zero para recomeçar a contagem do próximo pisca
      tempoPisca = 0; 
    }

    digitalWrite(pinPedestreVerde, LOW);
    // O LED vermelho do pedestre obedece ao estado da variável que fica alternando.
    digitalWrite(pinPedestreVermelho, estadoPisca);
  }

  // O delay aqui define a velocidade de "batida" do coração do código. 
  // Um valor baixo permite que o botão seja detectado instantaneamente.
  delay(1); 
}


/* Imagine que o Arduino é uma pessoa muito rápida lendo uma lista de instruções de cima a baixo. O loop é essa lista sendo lida repetidamente.

Aqui está o porquê desse delay(1) ser chamado de "batida do coração":

1. A velocidade do Arduino
O chip do Arduino funciona a 16 MHz. Sem nenhum delay, ele conseguiria ler todo o seu código milhares de vezes por segundo. Se ele corre demais, ele pode ler o botão sendo apertado 500 vezes antes mesmo de você conseguir tirar o dedo de lá, o que causaria instabilidade (o semáforo pularia fases rápido demais).

2. O delay(1) como um "respiro"
Ao colocar delay(1), você está dizendo: "Arduino, leia tudo, execute as luzes, verifique o botão e, ao chegar no fim, descanse por 1 milissegundo antes de começar de novo".

Por que não usar um delay(500) ou mais? Se você colocar um descanso de 1 segundo, e o usuário der um clique rápido no botão (que dura uns 200ms), o Arduino pode estar "dormindo" bem na hora do clique e ignorar o comando.

A "Instantaneidade": Com delay(1), o Arduino verifica o botão 1.000 vezes por segundo. Para um humano, isso é instantâneo. O botão nunca vai falhar.

3. A relação com o tempoPisca == 400
O valor que você escolheu para o contador de pisca (400) depende diretamente desse delay.

Se o loop roda a cada 1ms (aproximadamente, por causa do delay), e você espera chegar a 400, o LED vai piscar a cada 400ms (quase meio segundo).

Se você mudasse o delay(1) para delay(10), o seu contador de 400 levaria 4.000ms (4 segundos) para piscar o LED, deixando o semáforo lentíssimo.

Em resumo:
O delay(1) serve para cadenciar o código. Ele é longo o suficiente para estabilizar a leitura dos componentes eletrônicos, mas curto o suficiente para que o Arduino responda ao toque do seu dedo no exato milissegundo em que ele acontece.

É como se você estivesse ajustando a velocidade de um filme: se for rápido demais, você perde os detalhes; se for devagar demais, a imagem trava. O 1 é o equilíbrio perfeito para este projeto. */