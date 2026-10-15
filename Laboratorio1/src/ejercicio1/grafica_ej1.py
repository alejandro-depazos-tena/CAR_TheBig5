#!/usr/bin/env python3
"""Gráfica del ejercicio 1 con los tiempos medidos en WSL2.

Uso: python3 resultados/ejercicio1/grafica_ej1.py
Requiere matplotlib. Guarda el PNG junto a este script.
"""

from pathlib import Path

try:
    import matplotlib
    matplotlib.use("Agg")  # Permite ejecutarlo en WSL sin abrir ventanas.
    import matplotlib.pyplot as plt
    from matplotlib.ticker import FuncFormatter
except ImportError:
    raise SystemExit(
        "Falta matplotlib. En Ubuntu puedes instalarlo con:\n"
        "sudo apt install python3-matplotlib"
    )


# Cinco repeticiones por tamaño; tiempos en segundos.
TIEMPOS = {
    512: [0.136066251, 0.136912214, 0.137844957, 0.138205972, 0.136412712],
    1024: [0.652555053, 0.633790492, 0.634163802, 0.623602062, 0.622786105],
    2048: [2.797206726, 3.630152077, 3.055007552, 3.244937489, 2.983358555],
    4096: [24.998528768, 25.042492832, 22.967773202, 18.406898008, 18.691549068],
}
PASOS = 300


def main():
    tamanos = sorted(TIEMPOS)
    minimos = [min(TIEMPOS[n]) for n in tamanos]
    # T / (N² * pasos), convertido de segundos a nanosegundos.
    costes_ns = [t / (n * n * PASOS) * 1e9
                 for n, t in zip(tamanos, minimos)]

    fig, ax = plt.subplots(figsize=(8, 4.8))
    barras = ax.bar(range(len(tamanos)), costes_ns, width=0.58,
                    color="#386AA6", zorder=3)
    ax.set_xticks(range(len(tamanos)))
    ax.set_xticklabels([f"{n} × {n}" for n in tamanos])
    ax.set_xlabel("Tamaño de la malla", labelpad=10)
    ax.set_ylabel("Tiempo por celda y paso (ns)", labelpad=10)
    ax.set_title("Coste por celda y paso de la versión secuencial", pad=16)
    ax.set_ylim(0, max(costes_ns) * 1.2)
    ax.yaxis.set_major_formatter(
        FuncFormatter(lambda valor, posicion: f"{valor:g}".replace(".", ","))
    )
    ax.grid(axis="y", color="#DDDDDD", linewidth=0.7, zorder=0)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    for barra, coste in zip(barras, costes_ns):
        ax.text(barra.get_x() + barra.get_width() / 2,
                barra.get_height() + 0.06,
                f"{coste:.5f}".replace(".", ","),
                ha="center", va="bottom", fontsize=10)

    fig.text(0.5, 0.025,
             "300 pasos · Mínimo de cinco repeticiones · T / (N² × pasos)",
             ha="center", fontsize=9, color="#444444")
    fig.tight_layout(rect=(0, 0.065, 1, 1))
    destino = Path(__file__).resolve().parent / "grafica_coste_normalizado_ej1.png"
    fig.savefig(destino, dpi=300, facecolor="white")
    plt.close(fig)

    for n, tiempo, coste in zip(tamanos, minimos, costes_ns):
        print(f"N={n}: mínimo={tiempo:.9f} s; coste={coste:.5f} ns")
    print(f"\nGráfica guardada en: {destino}")


if __name__ == "__main__":
    main()
