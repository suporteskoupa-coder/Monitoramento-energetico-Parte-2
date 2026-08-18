void setup () {
  // pinmode -> informa se um dos pinos vai enviar corrente elétrica ao circuito ou se vai receber corrente elétrica do circuito 
  
  // pino que estamos usando na instrução e estado
  // Vai enviar
  pinMode(10, OUTPUT) ;

} 

void loop () {

  // digitalWrite liga e desliga pino do arduino 

  digitalWrite(10, HIGH);
  // Pausa

  delay(10000);
  digitalWrite(10, LOW);

   delay(10000);

}