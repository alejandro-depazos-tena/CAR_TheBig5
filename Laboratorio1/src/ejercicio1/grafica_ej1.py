#!/usr/bin/env python3
"""Gráfica del ejercicio 1: coste por celda y paso de la versión secuencial.

Uso (desde Laboratorio1):  python3 src/ejercicio1/grafica_ej1.py
Lee resultados/ejercicio1/tiempos_<N>.txt (una salida del programa por
repetición) y guarda resultados/ejercicio1/grafica_coste_normalizado_ej1.png.
Requiere matplotlib.
"""

import re
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

RESULTADOS = Path(__file__).resolve().parents[2] / "resultados" / "ejercicio1"
L3_MB = 12  # lscpu en entorno.txt: L3 de 12 MiB (i7-1355U)


def leer_tiempos():
    """Devuelve {N: [tiempos]} y el número de pasos, leídos de los .txt."""
    tiempos, pasos = {}, set()
    for ruta in sorted(RESULTADOS.glob("tiempos_*.txt")):
        texto = ruta.read_text(encoding="utf-8-sig")
        n = int(re.search(r"Tamaño de la malla:\s+(\d+)", texto).group(1))
        tiempos[n] = [float(t) for t in
                      re.findall(r"Tiempo de ejecución:\s+([0-9.]+) s", texto)]
        pasos.update(int(p) for p in
                     re.findall(r"Pasos ejecutados:\s+(\d+)", texto))
    if not tiempos or len(pasos) != 1:
        raise SystemExit("Faltan tiempos o hay ficheros con distinto número de pasos.")
    return tiempos, pasos.pop()


def coma(valor, decimales=2):
    return f"{valor:.{decimales}f}".replace(".", ",")


def main():
    tiempos, pasos = leer_tiempos()
    tamanos = sorted(tiempos)
    # T / (N² · pasos) en nanosegundos, con el mínimo y el máximo medidos.
    ns = {n: [t / (n * n * pasos) * 1e9 for t in tiempos[n]] for n in tamanos}
    minimos = [min(ns[n]) for n in tamanos]
    maximos = [max(ns[n]) for n in tamanos]
    reps = {len(tiempos[n]) for n in tamanos}

    fig, ax = plt.subplots(figsize=(8, 5))
    x = range(len(tamanos))
    barras = ax.bar(x, minimos, width=0.58, color="#386AA6", zorder=3,
                    label="mínimo (criterio usado)")
    # Bigote hasta el máximo: deja a la vista la variabilidad de 2048 y 4096.
    ax.errorbar(x, minimos, yerr=[[0] * len(tamanos),
                                  [M - m for m, M in zip(minimos, maximos)]],
                fmt="none", ecolor="#444444", capsize=6, zorder=4,
                label="hasta el máximo de las repeticiones")

    etiquetas = []
    for n in tamanos:
        mb = 2 * n * n * 8 / 2**20
        cabe = "cabe en L3" if mb <= L3_MB else "no cabe en L3"
        etiquetas.append(f"{n} × {n}\n{mb:.0f} MB · {cabe}")
    ax.set_xticks(list(x))
    ax.set_xticklabels(etiquetas)
    ax.set_xlabel("Tamaño de la malla (memoria de los dos buffers)", labelpad=10)
    ax.set_ylabel("Tiempo por celda y paso (ns)", labelpad=10)
    ax.set_title("Coste por celda y paso de la versión secuencial", pad=16)
    ax.set_ylim(0, max(maximos) * 1.15)
    ax.yaxis.set_major_formatter(
        FuncFormatter(lambda valor, posicion: f"{valor:g}".replace(".", ","))
    )
    ax.grid(axis="y", color="#DDDDDD", linewidth=0.7, zorder=0)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    for barra, coste in zip(barras, minimos):
        ax.text(barra.get_x() + barra.get_width() / 2, coste / 2,
                coma(coste), ha="center", va="center", fontsize=10,
                color="white", fontweight="bold")
    ax.legend(loc="upper left", frameon=False, fontsize=9)

    n_reps = reps.pop() if len(reps) == 1 else "varias"
    fig.text(0.5, 0.02,
             f"{pasos} pasos · {n_reps} repeticiones por tamaño · "
             f"T / (N² × pasos) · L3 = {L3_MB} MiB",
             ha="center", fontsize=9, color="#444444")
    fig.tight_layout(rect=(0, 0.05, 1, 1))
    destino = RESULTADOS / "grafica_coste_normalizado_ej1.png"
    fig.savefig(destino, dpi=300, facecolor="white")
    plt.close(fig)

    for n, m, M in zip(tamanos, minimos, maximos):
        print(f"N={n}: mínimo={min(tiempos[n]):.9f} s; "
              f"coste={coma(m, 3)} ns (máx. {coma(M, 3)} ns)")
    print(f"\nGráfica guardada en: {destino}")


if __name__ == "__main__":
    main()
