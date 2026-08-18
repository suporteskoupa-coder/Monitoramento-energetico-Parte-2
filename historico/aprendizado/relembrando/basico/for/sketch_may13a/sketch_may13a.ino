int valorX;
int valorY;
int valorZ;


// Comunicação Serial , cabo Usb, porta 0 e 1 exclusiva
// É uma forma de debugar

void setup() {
  // put your setup code here, to run once:
  // Velocidade da comunicação
  Serial.begin(9600); // também colocar a mesma no monitor


  valorX = 10;
  valorY = 20;
  valorZ = ;

  Serial.print("valorY=");
  Serial.print(valorY);

  Serial.print("  valorY=");
  Serial.print(valorY);


  Serial.print("  valorZ=")
  Serial.print(valorZ);

  /* No Arduino, BIN refere-se ao formato de arquivo binário (.bin), que é o código compilado e pronto para ser executado diretamente pelo microcontrolador. Ele transforma seu código .ino (texto) em uma linguagem de máquina (0s e 1s) que o chip entende, permitindo carregar o firmware sem a necessidade do código-fonte original. */

}

void loop() {
  // put your main code here, to run repeatedly:

}
