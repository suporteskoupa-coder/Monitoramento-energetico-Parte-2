#!/usr/bin/env python3
"""
Le a saida serial do ESP32 e grava as leituras num arquivo CSV.

Formato do CSV: data, hora, corrente, potencia

O script fica escutando a porta serial e procura pelos blocos que o firmware
imprime a cada ciclo de agregacao (funcao imprimirLeitura do main.cpp):

    --------------------------------------------------
    Equipamento : Computador - 800w
    Timestamp   : T+43s (sem NTP)
    Corrente    : 0.123 A
    Potencia    : 15.6 VA
    --------------------------------------------------

A data e a hora vem do relogio do PC, nao do ESP32 -- o ESP nao tem RTC com
bateria e so sabe a hora depois de sincronizar via NTP.

Uso:
    python3 main.py
    python3 main.py --porta /dev/ttyUSB0 --saida /media/pedro/SDCARD/leituras.csv
    python3 main.py --verbose        # mostra tambem as linhas cruas do serial
"""

import argparse
import csv
import os
import re
import sys
import time
from datetime import datetime

try:
    import serial
except ImportError:
    sys.exit("pyserial nao encontrado. Instale com: pip install pyserial")


# ============================================================
#  PADROES DE LEITURA
# ============================================================
# Aceita ponto ou virgula como separador decimal, por seguranca.
RE_EQUIPAMENTO = re.compile(r"Equipamento\s*:\s*(.+?)\s*$")
RE_CORRENTE = re.compile(r"Corrente\s*:\s*([-\d.,]+)\s*A", re.IGNORECASE)
RE_POTENCIA = re.compile(r"Potencia\s*:\s*([-\d.,]+)\s*VA", re.IGNORECASE)

CABECALHO = ["data", "hora", "corrente", "potencia"]


def para_float(texto):
    """Converte '0,123' ou '0.123' em float. Retorna None se nao der."""
    try:
        return float(texto.replace(",", "."))
    except ValueError:
        return None


def abrir_csv(caminho):
    """
    Abre o CSV em modo append e escreve o cabecalho se o arquivo for novo.
    Assim voce pode parar e retomar a coleta sem perder o que ja foi gravado.
    """
    pasta = os.path.dirname(os.path.abspath(caminho))
    os.makedirs(pasta, exist_ok=True)

    precisa_cabecalho = (
        not os.path.exists(caminho) or os.path.getsize(caminho) == 0
    )

    arquivo = open(caminho, "a", newline="", encoding="utf-8")
    escritor = csv.writer(arquivo)

    if precisa_cabecalho:
        escritor.writerow(CABECALHO)
        arquivo.flush()

    return arquivo, escritor


def gravar_linha(arquivo, escritor, corrente, potencia):
    """Grava uma leitura e forca a escrita em disco imediatamente."""
    agora = datetime.now()
    escritor.writerow(
        [
            agora.strftime("%Y-%m-%d"),
            agora.strftime("%H:%M:%S"),
            f"{corrente:.3f}",
            f"{potencia:.1f}",
        ]
    )
    # flush + fsync garantem que o dado sobrevive se o cabo/SD for removido
    arquivo.flush()
    os.fsync(arquivo.fileno())


def coletar(args):
    arquivo, escritor = abrir_csv(args.saida)
    total = 0

    # Acumula os campos de um bloco ate ter corrente E potencia
    pendente = {}

    print(f"Porta   : {args.porta} @ {args.baud}")
    print(f"Saida   : {os.path.abspath(args.saida)}")
    print("Ctrl+C para encerrar.\n")

    try:
        while True:
            try:
                with serial.Serial(args.porta, args.baud, timeout=1) as porta:
                    print(f"[{datetime.now():%H:%M:%S}] Conectado.")

                    while True:
                        bruto = porta.readline()
                        if not bruto:
                            continue  # timeout, so volta a esperar

                        # errors="replace" evita quebrar com lixo no boot do ESP
                        linha = bruto.decode("utf-8", errors="replace").strip()
                        if not linha:
                            continue

                        if args.verbose:
                            print(f"  | {linha}")

                        # Equipamento marca o inicio de um bloco novo
                        m = RE_EQUIPAMENTO.search(linha)
                        if m:
                            pendente = {"equipamento": m.group(1)}
                            continue

                        m = RE_CORRENTE.search(linha)
                        if m:
                            valor = para_float(m.group(1))
                            if valor is not None:
                                pendente["corrente"] = valor
                            continue

                        m = RE_POTENCIA.search(linha)
                        if m:
                            valor = para_float(m.group(1))
                            if valor is not None:
                                pendente["potencia"] = valor

                        # Bloco completo -> grava
                        if "corrente" in pendente and "potencia" in pendente:
                            gravar_linha(
                                arquivo,
                                escritor,
                                pendente["corrente"],
                                pendente["potencia"],
                            )
                            total += 1
                            print(
                                f"[{datetime.now():%H:%M:%S}] "
                                f"#{total:<5} "
                                f"{pendente['corrente']:.3f} A  |  "
                                f"{pendente['potencia']:.1f} VA"
                            )
                            pendente = {}

            except serial.SerialException as erro:
                # ESP desconectado ou porta ocupada -- tenta de novo
                print(f"[{datetime.now():%H:%M:%S}] Serial indisponivel: {erro}")
                print("Nova tentativa em 5s...")
                pendente = {}
                time.sleep(5)

    except KeyboardInterrupt:
        print(f"\nEncerrado. {total} leituras gravadas em {args.saida}")

    finally:
        arquivo.close()


def main():
    parser = argparse.ArgumentParser(
        description="Le a serial do ESP32 e grava corrente/potencia em CSV."
    )
    parser.add_argument(
        "--porta",
        default="/dev/ttyUSB0",
        help="porta serial do ESP32 (padrao: /dev/ttyUSB0)",
    )
    parser.add_argument(
        "--baud",
        type=int,
        default=115200,
        help="velocidade da serial, tem que bater com o Serial.begin (padrao: 115200)",
    )
    parser.add_argument(
        "--saida",
        default="leituras.csv",
        help="caminho do CSV. Para gravar no SD card, aponte para o ponto de "
        "montagem, ex: /media/pedro/SDCARD/leituras.csv",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="mostra tambem as linhas cruas recebidas do serial",
    )

    coletar(parser.parse_args())


if __name__ == "__main__":
    main()
