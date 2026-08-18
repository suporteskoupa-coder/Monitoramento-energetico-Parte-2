"""2. O Código Python (O Receptor)
Este código roda no seu computador. Para que ele funcione na vida real, você precisará instalar a biblioteca pyserial no seu terminal (pip install pyserial).

O Python vai abrir a mesma porta onde o Arduino está conectado e ficará lendo cada linha que o Arduino enviar.
"""

import serial
import time

# Configuração da porta serial
# IMPORTANTE: Substitua 'COM3' pela porta onde seu Arduino físico está conectado (ex: 'COM3' no Windows ou '/dev/ttyUSB0' no Linux)
porta_serial = "COM3"
velocidade = 9600

try:
    # Abre a conexão com a porta serial
    arduino = serial.Serial(porta_serial, velocidade, timeout=1)
    print(f"Conectado com sucesso na porta {porta_serial}!")
    time.sleep(2)  # Aguarda 2 segundos para o Arduino reiniciar após a conexão

    while True:
        # Verifica se há dados aguardando na porta serial
        if arduino.in_waiting > 0:
            # Lê a linha de dados até o caractere de nova linha (\n)
            linha_bytes = arduino.readline()

            # Decodifica os bytes para texto (string) e remove espaços ou quebras de linha em branco
            dados_texto = linha_bytes.decode("utf-8").strip()

            if dados_texto:
                print(f"Valor recebido do sensor: {dados_texto}")

except KeyboardInterrupt:
    print("\nSimulação encerrada pelo usuário.")

except Exception as e:
    print(f"Erro ao conectar ou ler a serial: {e}")

finally:
    # Garante que a porta serial será fechada ao encerrar o programa
    if "arduino" in locals() and arduino.is_open:
        arduino.close()
        print("Conexão serial fechada.")
