void setup() {
  // Portas digitais

  //  Entrada de corrente elétrica
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);

  // Entrada
  pinMode(7, INPUT_PULLUP);

  // Usar resistor do próprio arduino pra evitar o uso de resistores

  // em vez de ligar na porta 5v, vamos no terra, ele puxa a porta pra cima, ai quando apertar vai estar no estado inverso
  // O ESTADO dos leds ficarão invertidos

}

void loop() {
  // Para o programa entender o estado do botão

  int estadoBotao;
  // Verifica o estado da porta
  estadoBotao = digitalRead(7);

  // Led vai ficar ligado caso o botão esteja ligado e o contrário tbm

  // Ao apertar o botão vc controla o estado dos leds

  digitalWrite(10, estadoBotao); 
  digitalWrite(9, !estadoBotao);


}
