"""Tabla y grafica de speedup del ejercicio 1 (Practica 4).

Lee resultados/ejercicio1/tiempos.csv, que escribe ejecutar_ej1.sh, y genera en
la misma carpeta:
    tabla_ej1.md           minimo y mediana de cada configuracion, speedup y
                           eficiencia, lista para pegar (coma decimal)
    grafica_speedup.png    speedup frente a procesos, con la diagonal ideal
Ademas muestra por pantalla la comprobacion de la suma y que pasaria con un
reparto que no tuviera en cuenta el resto (no lo guarda en ningun fichero).

Desde PracticaT4:
    python3 src/ejercicio1/grafica_ej1.py
"""

import argparse
import csv
import math
import statistics
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")  # guarda la imagen sin necesitar ventana
import matplotlib.pyplot as plt

N = 200_000_000  # sumandos, igual que en p4_ej1.c

# Colores: dos series categoricas fijas, diagonal y textos en grises neutros.
AZUL = "#2a78d6"      # serie 1: tiempo del rango 0
NARANJA = "#eb6834"   # serie 2: tiempo del rango mas lento
GRIS_IDEAL = "#8a8984"
TEXTO = "#0b0b0b"
TEXTO_2 = "#52514e"
REJILLA = "#e4e3df"


def coma(valor, decimales):
    return f"{valor:.{decimales}f}".replace(".", ",")


def leer_tiempos(ruta):
    datos = defaultdict(lambda: {"t0": [], "tmax": [], "suma": []})
    with ruta.open(newline="", encoding="utf-8") as fichero:
        for fila in csv.DictReader(fichero):
            p = int(fila["procesos"])
            datos[p]["t0"].append(float(fila["t_rango0_s"]))
            datos[p]["tmax"].append(float(fila["t_max_s"]))
            datos[p]["suma"].append(fila["suma"])
    if 1 not in datos:
        raise ValueError("Falta la medida con 1 proceso, que es la referencia del speedup.")
    return dict(sorted(datos.items()))


def resumir(datos):
    t1_0 = min(datos[1]["t0"])
    t1_max = min(datos[1]["tmax"])
    filas = []
    for p, d in datos.items():
        t0_min, tmax_min = min(d["t0"]), min(d["tmax"])
        s0, smax = t1_0 / t0_min, t1_max / tmax_min
        filas.append({
            "procesos": p,
            "repeticiones": len(d["t0"]),
            "t_rango0_min_s": t0_min,
            "t_rango0_mediana_s": statistics.median(d["t0"]),
            "t_max_min_s": tmax_min,
            "t_max_mediana_s": statistics.median(d["tmax"]),
            "speedup_rango0": s0,
            "eficiencia_rango0": s0 / p,
            "speedup_mas_lento": smax,
            "eficiencia_mas_lento": smax / p,
        })
    return filas


def escribir_tabla(filas, carpeta):
    reps = filas[0]["repeticiones"]
    lineas = [
        f"Tiempos de la suma (solo la funcion `trozo`, medida con MPI_Wtime). "
        f"Minimo de {reps} ejecuciones por fila; entre parentesis, la mediana.",
        "",
        "| Procesos | T rango 0 (s) | T mas lento (s) | Speedup rango 0 | Eficiencia | "
        "Speedup mas lento | Eficiencia |",
        "|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for f in filas:
        lineas.append(
            f"| {f['procesos']} "
            f"| {coma(f['t_rango0_min_s'], 4)} ({coma(f['t_rango0_mediana_s'], 4)}) "
            f"| {coma(f['t_max_min_s'], 4)} ({coma(f['t_max_mediana_s'], 4)}) "
            f"| {coma(f['speedup_rango0'], 2)} | {coma(100 * f['eficiencia_rango0'], 1)} % "
            f"| {coma(f['speedup_mas_lento'], 2)} | {coma(100 * f['eficiencia_mas_lento'], 1)} % |"
        )
    lineas += ["", "Speedup = T(1 proceso) / T(P procesos), con los minimos. "
               "Eficiencia = speedup / P."]
    (carpeta / "tabla_ej1.md").write_text("\n".join(lineas) + "\n", encoding="utf-8")


def analizar_reparto(datos):
    lineas = ["1) La suma no cambia con el numero de procesos", ""]
    referencia = float(datos[1]["suma"][0])
    for p, d in datos.items():
        distintas = sorted(set(d["suma"]))
        valor = float(distintas[0])
        relativa = abs(valor - referencia) / referencia
        lineas.append(
            f"  P = {p:2d}: {len(d['suma'])} ejecuciones, {len(distintas)} valor(es) distinto(s); "
            f"suma = {valor:.15e}  ({valor:e})  diferencia relativa con P = 1: {relativa:.1e}")
    lineas += [
        "",
        "  Con cada P la suma es identica en todas las ejecuciones: el rango 0 recibe",
        "  en orden fijo de rango. Entre distintos P solo cambian los ultimos digitos:",
        "  cada rango suma su trozo y luego se suman los parciales, y la suma en coma",
        "  flotante no es asociativa. No falta ni sobra ningun sumando.",
        "",
        "2) Que pasaria con un reparto que no tuviera en cuenta el resto",
        "",
        "  a) Todos se llevan N / P sumandos y nadie coge los N % P que sobran:",
    ]
    for p in (3, 7, 12):
        base, resto = divmod(N, p)
        perdidos = math.fsum(math.sqrt(i + 0.5) for i in range(p * base, N))
        mala = referencia - perdidos
        lineas += [
            f"     P = {p:2d}: se pierden los {resto} ultimos sumandos (i = {p * base} ... {N - 1}),",
            f"             que valen {perdidos:.1f} en total.",
            f"             suma correcta  {referencia:.15e}  ->  impresa con %e: {referencia:e}",
            f"             suma sin resto {mala:.15e}  ->  impresa con %e: {mala:e}",
            f"             error relativo {perdidos / referencia:.1e}",
        ]
    lineas += [
        "     Con %e (7 cifras) el resultado erroneo se imprime IGUAL que el correcto:",
        "     por eso p4_ej1.c imprime ademas la suma con todas sus cifras.",
        "",
        "  b) Todos se llevan N / P y el ultimo se queda con todo el resto:",
    ]
    for n, p in ((N, 12), (100, 24), (1000, 24)):
        base, resto = divmod(n, p)
        media = n / p
        lineas.append(
            f"     N = {n:>9}, P = {p:2d}: el ultimo hace {base + resto} y los demas {base}; "
            f"el mas cargado hace {(base + resto) / media:.2f} veces la media "
            f"(con el reparto del enunciado: {math.ceil(n / p) / media:.2f}).")
    lineas += [
        "     Con N = 2*10^8 el desequilibrio es despreciable; con pocos sumandos por",
        "     proceso el programa entero iria al ritmo del ultimo.",
    ]
    print("\n".join(lineas) + "\n")


def dibujar(filas, carpeta):
    procesos = [f["procesos"] for f in filas]
    s0 = [f["speedup_rango0"] for f in filas]
    smax = [f["speedup_mas_lento"] for f in filas]
    reps = filas[0]["repeticiones"]
    pmax = max(procesos)

    plt.rcParams.update({"font.family": "DejaVu Sans", "font.size": 11,
                         "text.color": TEXTO, "axes.labelcolor": TEXTO_2,
                         "xtick.color": TEXTO_2, "ytick.color": TEXTO_2})
    fig, ax = plt.subplots(figsize=(9.5, 6.2))
    fig.subplots_adjust(left=0.085, right=0.76, bottom=0.21, top=0.80)

    ax.plot([1, pmax], [1, pmax], color=GRIS_IDEAL, lw=1.5, ls=(0, (5, 4)), zorder=2)
    ax.axvline(10, color=REJILLA, lw=1, zorder=1)
    ax.text(10.1, 0.35, "10 núcleos\nfísicos", color=TEXTO_2, fontsize=9, va="bottom")

    for valores, color, nombre in ((smax, NARANJA, "rango más lento"),
                                   (s0, AZUL, "rango 0")):
        ax.plot(procesos, valores, color=color, lw=2, marker="o", ms=7,
                markeredgecolor="white", markeredgewidth=1.5,
                solid_capstyle="round", solid_joinstyle="round", zorder=3, label=nombre)

    # Etiquetas directas solo al final de cada linea, con el texto en gris.
    fin = [(pmax, s0[-1], "rango 0", AZUL), (pmax, smax[-1], "rango más lento", NARANJA),
           (pmax, pmax, "ideal (S = P)", GRIS_IDEAL)]
    fin.sort(key=lambda e: e[1])
    ultimo_y = -10.0
    for x, y, nombre, color in fin:
        y_texto = max(y, ultimo_y + 0.75)  # separa las etiquetas si se juntan
        ultimo_y = y_texto
        ax.annotate(f"{nombre}  {coma(y, 2)}×" if nombre != "ideal (S = P)" else nombre,
                    xy=(x, y), xytext=(x + 0.45, y_texto), color=TEXTO, fontsize=10,
                    va="center", annotation_clip=False,
                    arrowprops=dict(arrowstyle="-", color=color, lw=1) if abs(y_texto - y) > 0.05 else None)

    ax.set_xticks(procesos)
    ax.set_xlim(0.5, pmax + 0.3)
    ax.set_ylim(0, pmax + 0.5)
    ax.set_yticks(range(0, pmax + 1, 2))
    ax.set_xlabel("número de procesos P", labelpad=8)
    ax.set_ylabel("speedup  T(1) / T(P)", labelpad=8)
    ax.grid(axis="y", color=REJILLA, lw=1)
    ax.set_axisbelow(True)
    for lado in ("top", "right"):
        ax.spines[lado].set_visible(False)
    for lado in ("left", "bottom"):
        ax.spines[lado].set_color(REJILLA)
    manejadores, nombres = ax.get_legend_handles_labels()
    ax.legend(manejadores[::-1], nombres[::-1], loc="lower left", bbox_to_anchor=(0, 1.02),
              ncol=2, frameon=False, handlelength=2.2)

    fig.suptitle("Ejercicio 1 · Speedup de la suma repartida con MPI",
                 x=0.085, ha="left", y=0.96, fontsize=15, fontweight="bold")
    fig.text(0.085, 0.895, "Intel Core i7-1355U (2 P + 8 E, 12 hilos) · WSL 2 · Open MPI 4.1.6",
             color=TEXTO_2)
    fig.text(0.085, 0.03,
             f"Mínimo de {reps} ejecuciones por punto. «Rango 0»: tiempo de cálculo del rango 0, "
             "el que imprime el programa.\n«Rango más lento»: el mayor tiempo de cálculo entre "
             "todos los rangos, que es lo que tarda el programa.",
             fontsize=9, color=TEXTO_2)

    destino = carpeta / "grafica_speedup.png"
    fig.savefig(destino, dpi=200, facecolor="white")
    plt.close(fig)
    return destino


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--resultados", type=Path, default=Path("resultados/ejercicio1"))
    carpeta = parser.parse_args().resultados
    try:
        datos = leer_tiempos(carpeta / "tiempos.csv")
    except (OSError, ValueError, KeyError) as error:
        parser.exit(1, f"Error: {error}\n")

    filas = resumir(datos)
    escribir_tabla(filas, carpeta)
    print((carpeta / "tabla_ej1.md").read_text(encoding="utf-8"))
    analizar_reparto(datos)
    print(f"Grafica guardada en: {dibujar(filas, carpeta).resolve()}")


if __name__ == "__main__":
    main()
