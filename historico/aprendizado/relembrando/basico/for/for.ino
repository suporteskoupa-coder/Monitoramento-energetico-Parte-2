// variável dos pinos dos led
// Array(variaveis) -> portas
int pinLed[9] = {12, 11,10, 9,8,7,6,5,4};

int numeroLed;


void setup() {
  // put your setup code here, to run once:

  int x;

  // Modos das portas
  // Compatível com pinLed[9]
  for (x = 0; x <= 8; x = x + 1) {

      pinMode(pinLed[x], OUTPUT);

  }



}

void loop() {
  // put your main code here, to run repeatedly:


  // Ligando os leds
  for (numeroLed = 0; numeroLed <= 8; numeroLed = numeroLed + 1) {
          digitalWrite(pinLed[numeroLed], HIGH);
            delay(200);
  }

  // Voltar apagando invertido

  for (numeroLed = 8; numeroLed >= 0; numeroLed = numeroLed - 1) {
          digitalWrite(pinLed[numeroLed], LOW;
            delay(200);
  }



}
