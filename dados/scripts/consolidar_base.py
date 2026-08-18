"""Consolida a Campanha 01 para uma linha por segundo usando a média."""

from pathlib import Path

import pandas as pd


RAIZ = Path(__file__).resolve().parents[2]
CAMINHO_BRUTO = RAIZ / "dados" / "brutos" / "campanha-01" / "leituras.csv"
CAMINHO_PROCESSADO = (
    RAIZ
    / "dados"
    / "processados"
    / "campanha-01"
    / "base_consolidada_media.csv"
)
COLUNAS_OBRIGATORIAS = {"data", "hora", "corrente A", "potencia VA"}


def consolidar_base() -> None:
    if not CAMINHO_BRUTO.is_file():
        raise FileNotFoundError(f"Base bruta não encontrada: {CAMINHO_BRUTO}")

    print("=== 1. LENDO A BASE BRUTA ===")
    df = pd.read_csv(CAMINHO_BRUTO)
    df.columns = df.columns.str.strip()

    faltantes = COLUNAS_OBRIGATORIAS.difference(df.columns)
    if faltantes:
        raise ValueError(f"Colunas obrigatórias ausentes: {sorted(faltantes)}")

    print(f"Total de registros: {len(df)}")
    duplicadas_exatas = int(df.duplicated().sum())
    segundos_unicos = int((df["data"].astype(str) + " " + df["hora"].astype(str)).nunique())

    print("\n=== 2. DIAGNÓSTICO DE QUALIDADE ===")
    print(f"Linhas exatamente idênticas: {duplicadas_exatas}")
    print(f"Instantes de tempo únicos: {segundos_unicos}")

    print("\n=== 3. CONSOLIDANDO POR MÉDIA ===")
    consolidado = df.groupby(["data", "hora"], as_index=False).agg(
        {"corrente A": "mean", "potencia VA": "mean"}
    )
    consolidado.rename(
        columns={"corrente A": "corrente_a", "potencia VA": "potencia_va"},
        inplace=True,
    )
    consolidado["corrente_a"] = consolidado["corrente_a"].round(3)
    consolidado["potencia_va"] = consolidado["potencia_va"].round(1)
    consolidado["campanha"] = "CAMPANHA_PILOTO_01"
    consolidado = consolidado[
        ["data", "hora", "corrente_a", "potencia_va", "campanha"]
    ]

    CAMINHO_PROCESSADO.parent.mkdir(parents=True, exist_ok=True)
    consolidado.to_csv(CAMINHO_PROCESSADO, index=False)

    print("\n=== 4. RESULTADO ===")
    print(f"Linhas consolidadas: {len(consolidado)}")
    print(f"Arquivo: {CAMINHO_PROCESSADO.relative_to(RAIZ)}")


if __name__ == "__main__":
    consolidar_base()
