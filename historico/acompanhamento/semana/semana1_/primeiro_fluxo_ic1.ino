void setup() {
  // Inicializa a comunicação serial a uma velocidade de 9600 bits por segundo (baud rate)
  Serial.begin(9600); 
}

void loop() {
  // Lê o valor analógico no pino A0 (do potenciômetro)
  int valor = analogRead(A0); 
  
  // Envia o valor via serial seguido de uma quebra de linha (\n)
  Serial.println(valor); 
  
  // Aguarda 1000 milissegundos (1 segundo) antes de ler novamente
  delay(1000); 
}