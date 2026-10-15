#!/usr/bin/env python3
"""Gráficas de los ejercicios 3, 4 y 5 a partir de las salidas CSV.

Uso (desde Laboratorio1), tras guardar las salidas de los programas:
    ./p1_ej3 2048 300   > resultados/ejercicio3/fuerte_2048.csv
    ./p1_ej3 256 19200  > resultados/ejercicio3/fuerte_256.csv
    ./p1_ej4            > resultados/ejercicio4/debil.csv
    ./p1_ej5            > resultados/ejercicio5/planificaciones.csv
    python3 src/graficas_lab1.py

Las líneas que empiezan por '#' se tratan como comentarios. Cada PNG se
guarda junto a su CSV; los ficheros que no existan se omiten.
"""

import csv
from pathlib import Path

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError:
    raise SystemExit("Falta matplotlib: sudo apt install python3-matplotlib")

RES = Path(__file__).resolve().parents[1] / "resultados"


def leer(ruta):
    with open(ruta, encoding="utf-8-sig") as f:
        lineas = [l for l in f if l.strip() and not l.startswith("#")]
    return list(csv.DictReader(lineas))


def comentario(ruta):
    with open(ruta, encoding="utf-8-sig") as f:
        return " ".join(l[1:].strip() for l in f if l.startswith("#"))


def estilo(ax):
    ax.grid(color="#DDDDDD", linewidth=0.7, zorder=0)
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)


def fuerte():
    rutas = sorted((RES / "ejercicio3").glob("fuerte_*.csv"))
    if not rutas:
        return
    fig, ax = plt.subplots(figsize=(8, 5))
    hilos_max = 1
    techo_dibujado = False
    for ruta in rutas:
        filas = leer(ruta)
        h = [int(f["hilos"]) for f in filas]
        hilos_max = max(hilos_max, max(h))
        n, pasos = filas[0]["N"], filas[0]["pasos"]
        ax.plot(h, [float(f["speedup_abs"]) for f in filas], "o-", zorder=3,
                label=f"TermoHPC N={n}, {pasos} pasos")
        if not techo_dibujado:
            ax.plot(h, [float(f["techo_speedup"]) for f in filas], "s--",
                    color="#2E8B57", zorder=3, label="techo medido de la máquina")
            techo_dibujado = True
    ax.plot([1, hilos_max], [1, hilos_max], ":", color="#888888",
            label="ideal (lineal)")
    ax.set_xticks([1, 2, 4, 8, 12, 16, 20, 24])
    ax.set_xlabel("Hilos")
    ax.set_ylabel("Speedup absoluto (T secuencial / T paralelo)")
    ax.set_title("Escalabilidad fuerte frente al techo de la máquina")
    estilo(ax)
    ax.legend(frameon=False)
    fig.text(0.5, 0.01, "Mínimo de las repeticiones · T1 = versión secuencial",
             ha="center", fontsize=9, color="#444444")
    fig.tight_layout(rect=(0, 0.04, 1, 1))
    fig.savefig(RES / "ejercicio3" / "grafica_fuerte.png", dpi=200,
                facecolor="white")
    plt.close(fig)


def debil():
    ruta = RES / "ejercicio4" / "debil.csv"
    if not ruta.exists():
        return
    filas = leer(ruta)
    h = [int(f["hilos"]) for f in filas]
    t = [float(f["tiempo_s"]) for f in filas]
    fig, ax = plt.subplots(figsize=(8, 5))
    ax.plot(h, t, "o-", zorder=3, label="medido")
    ax.axhline(t[0], linestyle=":", color="#888888", label="ideal: plana")
    for hi, ti, f in zip(h, t, filas):
        ax.annotate(f"N={f['N']}", (hi, ti), textcoords="offset points",
                    xytext=(0, 8), ha="center", fontsize=8)
    ax.set_xticks(h)
    ax.set_ylim(0, max(t) * 1.2)
    ax.set_xlabel("Hilos (la malla crece como N1·√p)")
    ax.set_ylabel("Tiempo (s)")
    ax.set_title("Escalabilidad débil")
    estilo(ax)
    ax.legend(frameon=False)
    fig.text(0.5, 0.01, comentario(ruta), ha="center", fontsize=9,
             color="#444444")
    fig.tight_layout(rect=(0, 0.04, 1, 1))
    fig.savefig(ruta.with_name("grafica_debil.png"), dpi=200, facecolor="white")
    plt.close(fig)


def planificaciones():
    ruta = RES / "ejercicio5" / "planificaciones.csv"
    if not ruta.exists():
        return
    filas = leer(ruta)
    nombres = [f["schedule"] + ("" if f["chunk"] == "0" else f", {f['chunk']}")
               for f in filas]
    t = [float(f["tiempo_s"]) for f in filas]
    colores = {"static": "#386AA6", "dynamic": "#D7614B", "guided": "#DBAB3C"}
    fig, ax = plt.subplots(figsize=(9, 5))
    barras = ax.bar(nombres, t, color=[colores[f["schedule"]] for f in filas],
                    zorder=3)
    for b, f, ti in zip(barras, filas, t):
        marca = "" if f["correcto"] == "1" else " ✗"
        ax.text(b.get_x() + b.get_width() / 2, ti, f"{ti:.4g}{marca}".replace(".", ","),
                ha="center", va="bottom", fontsize=8)
    ax.set_ylabel("Tiempo (s)")
    ax.set_title("Comparación de planificaciones")
    ax.tick_params(axis="x", rotation=35)
    estilo(ax)
    ax.grid(axis="x", visible=False)
    fig.text(0.5, 0.01, comentario(ruta), ha="center", fontsize=8,
             color="#444444")
    fig.tight_layout(rect=(0, 0.04, 1, 1))
    fig.savefig(ruta.with_name("grafica_planificaciones.png"), dpi=200,
                facecolor="white")
    plt.close(fig)


def eficiencia():
    rutas = sorted((RES / "ejercicio3").glob("fuerte_*.csv"))
    if not rutas:
        return
    fig, ax = plt.subplots(figsize=(8, 5))
    for i, ruta in enumerate(rutas):
        filas = leer(ruta)
        h = [int(f["hilos"]) for f in filas]
        ax.plot(h, [100 * float(f["eficiencia"]) for f in filas], "o-",
                zorder=3, label=f"TermoHPC N={filas[0]['N']}")
        if i == 0:
            ax.plot(h, [100 * float(f["techo_speedup"]) / int(f["hilos"])
                        for f in filas], "s--", color="#2E8B57", zorder=3,
                    label="eficiencia del techo")
    ax.set_xticks([1, 2, 4, 8, 12, 16, 20, 24])
    ax.set_ylim(0, 110)
    ax.set_xlabel("Hilos")
    ax.set_ylabel("Eficiencia (%)")
    ax.set_title("Eficiencia = speedup / hilos")
    estilo(ax)
    ax.legend(frameon=False)
    fig.tight_layout()
    fig.savefig(RES / "ejercicio3" / "grafica_eficiencia.png", dpi=200,
                facecolor="white")
    plt.close(fig)


def ancho_banda():
    ruta = RES / "ejercicio6" / "stream.csv"
    if not ruta.exists():
        return
    filas = leer(ruta)
    h = [int(f["hilos"]) for f in filas]
    fig, ax = plt.subplots(figsize=(8, 5))
    ax.plot(h, [float(f["GB_s_24B"]) for f in filas], "o-", zorder=3,
            label="STREAM triad (modelo 24 B)")
    ax.plot(h, [float(f["GB_s_32B"]) for f in filas], "o--", zorder=3,
            label="STREAM triad (modelo 32 B)")
    fuerte = RES / "ejercicio3" / "fuerte_2048.csv"
    if fuerte.exists():
        ff = leer(fuerte)
        n = int(ff[0]["N"])
        bytes_paso = 16 * (n - 2) ** 2  # modelo del apartado V.G (cota inferior)
        ax.plot([int(f["hilos"]) for f in ff],
                [bytes_paso / float(f["s_por_paso"]) / 1e9 for f in ff],
                "^-", color="#D7614B", zorder=3,
                label=f"TermoHPC N={n} (modelo 16 B/celda)")
    ax.set_xticks([1, 2, 4, 8, 12, 16, 20, 24])
    ax.set_ylim(bottom=0)
    ax.set_xlabel("Hilos")
    ax.set_ylabel("Ancho de banda (GB/s)")
    ax.set_title("Ancho de banda conseguido frente al de la máquina")
    estilo(ax)
    ax.legend(frameon=False)
    fig.tight_layout()
    fig.savefig(RES / "ejercicio6" / "grafica_ancho_banda.png", dpi=200,
                facecolor="white")
    plt.close(fig)


def variantes():
    rutas = {"sin afinidad": RES / "ejercicio6" / "variantes.csv",
             "close/cores": RES / "ejercicio6" / "variantes_afinidad_close.csv",
             "spread/threads": RES / "ejercicio6" / "variantes_afinidad_spread.csv"}
    datos = {k: leer(r) for k, r in rutas.items() if r.exists()}
    if not datos:
        return
    nombres = [f["variante"] for f in next(iter(datos.values()))]
    fig, ax = plt.subplots(figsize=(9, 5))
    ancho = 0.8 / len(datos)
    for i, (etq, filas) in enumerate(datos.items()):
        xs = [k + (i - (len(datos) - 1) / 2) * ancho for k in range(len(nombres))]
        t = [float(f["tiempo_s"]) for f in filas]
        barras = ax.bar(xs, t, ancho, label=etq, zorder=3)
        for b, ti in zip(barras, t):
            ax.text(b.get_x() + b.get_width() / 2, ti, f"{ti:.3g}".replace(".", ","),
                    ha="center", va="bottom", fontsize=7)
    ax.set_xticks(range(len(nombres)), nombres)
    ax.set_ylabel("Tiempo (s)")
    ax.set_title("Ejercicio 6 · variantes (12 hilos, N=2048, 300 pasos)")
    estilo(ax)
    ax.grid(axis="x", visible=False)
    ax.legend(title="Afinidad", frameon=False)
    fig.tight_layout()
    fig.savefig(RES / "ejercicio6" / "grafica_variantes.png", dpi=200,
                facecolor="white")
    plt.close(fig)


if __name__ == "__main__":
    fuerte()
    eficiencia()
    debil()
    planificaciones()
    ancho_banda()
    variantes()
    print(f"Gráficas guardadas bajo {RES}")
