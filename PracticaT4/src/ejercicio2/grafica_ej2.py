"""Tablas y grafica de barras del ejercicio 2 (Practica 4).

Lee los dos CSV que escribe ejecutar_ej2.sh en resultados/ejercicio2/:
    umbral.csv        la busqueda del umbral del interbloqueo
    comparacion.csv   las medidas de los tres arreglos con 8 MB
y genera en la misma carpeta:
    tabla_ej2.md          tabla del barrido y tabla de la comparacion, listas
                          para pegar (coma decimal)
    grafica_arreglos.png  grafica de barras con el tiempo de cada arreglo

Desde PracticaT4:
    python3 src/ejercicio2/grafica_ej2.py
"""

import argparse
import csv
import statistics
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")  # guarda la imagen sin necesitar ventana
import matplotlib.pyplot as plt

MODOS = ["sendrecv", "orden", "noblo"]
NOMBRES = {
    "sendrecv": "MPI_Sendrecv",
    "orden": "ordenar los envíos",
    "noblo": "Isend + Irecv + Waitall",
}
FASES = {
    "doblar": "doblando",
    "biseccion": "bisección",
    "repeticion": "repetición",
    "arreglos": "arreglo en el umbral",
}

# Mismos colores que la grafica del ejercicio 1.
AZUL = "#2a78d6"
NARANJA = "#eb6834"
TEXTO = "#0b0b0b"
TEXTO_2 = "#52514e"
REJILLA = "#e4e3df"


def coma(valor, decimales):
    return f"{valor:.{decimales}f}".replace(".", ",")


def miles(entero):
    return f"{entero:,}".replace(",", " ")


def leer_umbral(ruta):
    with ruta.open(newline="", encoding="utf-8") as fichero:
        return list(csv.DictReader(fichero))


def leer_comparacion(ruta):
    datos = defaultdict(list)
    with ruta.open(newline="", encoding="utf-8") as fichero:
        for fila in csv.DictReader(fichero):
            datos[fila["modo"]].append(float(fila["us_por_intercambio"]))
    faltan = [m for m in MODOS if m not in datos]
    if faltan:
        raise ValueError(f"Faltan medidas de: {', '.join(faltan)}")
    return datos


def frontera(barrido):
    """Mayor tamano que paso y menor que se colgo, en modo send."""
    send = [f for f in barrido if f["modo"] == "send"]
    ok = [int(f["m_doubles"]) for f in send if f["resultado"] == "ok"]
    mal = [int(f["m_doubles"]) for f in send if f["resultado"] == "cuelga"]
    return (max(ok) if ok else None), (min(mal) if mal else None)


def escribir_tabla(barrido, comp, carpeta):
    lo, hi = frontera(barrido)
    lineas = ["## Búsqueda del umbral (modo send)", ""]
    lineas += [
        "| Fase | Modo | M (doubles) | Bytes | Resultado | µs por intercambio |",
        "|---|---|---:|---:|---|---:|",
    ]
    for f in barrido:
        us = coma(float(f["us_por_intercambio"]), 1) if f["us_por_intercambio"] else "—"
        res = "funciona" if f["resultado"] == "ok" else "**se cuelga**"
        lineas.append(
            f"| {FASES.get(f['fase'], f['fase'])} | {f['modo']} | {miles(int(f['m_doubles']))} "
            f"| {miles(int(f['bytes']))} | {res} | {us} |"
        )
    lineas.append("")
    if lo is not None and hi is not None:
        lineas.append(
            f"**Umbral:** funciona con {miles(lo)} doubles ({miles(lo * 8)} bytes) y se cuelga "
            f"con {miles(hi)} doubles ({miles(hi * 8)} bytes)."
        )
    else:
        lineas.append("**Umbral:** no se ha encontrado en el rango probado.")

    rondas = len(comp[MODOS[0]])
    mejor = min(min(v) for v in comp.values())
    lineas += [
        "",
        f"## Los tres arreglos con 1 048 576 doubles (8 MB)",
        "",
        f"Tiempo por intercambio (media de 20 dentro del programa). Mínimo de {rondas} "
        f"ejecuciones por arreglo; entre paréntesis, la mediana.",
        "",
        "| Arreglo | µs por intercambio | Respecto al más rápido | Ancho de banda (GB/s) |",
        "|---|---:|---:|---:|",
    ]
    for modo in MODOS:
        v = comp[modo]
        tmin = min(v)
        # en cada intercambio viajan 8 MB en cada sentido
        gbs = 2 * 8 * 1024 * 1024 / (tmin * 1e-6) / 1e9
        lineas.append(
            f"| {NOMBRES[modo]} | {coma(tmin, 1)} ({coma(statistics.median(v), 1)}) "
            f"| {coma(tmin / mejor, 2)}× | {coma(gbs, 2)} |"
        )
    lineas += [
        "",
        "Ancho de banda = 2 × 8 MB (los dos sentidos) / tiempo mínimo.",
        "",
    ]
    (carpeta / "tabla_ej2.md").write_text("\n".join(lineas), encoding="utf-8")


def dibujar(comp, carpeta):
    fig, ax = plt.subplots(figsize=(8, 4.8), dpi=150)
    x = range(len(MODOS))
    minimos = [min(comp[m]) for m in MODOS]
    barras = ax.bar(x, minimos, width=0.55, color=AZUL, zorder=2, label="mínimo")
    for i, modo in enumerate(MODOS):
        ax.scatter([i] * len(comp[modo]), comp[modo], s=14, color=NARANJA, alpha=0.75,
                   zorder=3, label="cada ejecución" if i == 0 else None)
    for barra, valor in zip(barras, minimos):
        ax.annotate(coma(valor, 1), (barra.get_x() + barra.get_width() / 2, valor),
                    xytext=(0, -14), textcoords="offset points", ha="center",
                    color="white", fontsize=10, fontweight="bold")
    ax.set_xticks(list(x), [NOMBRES[m] for m in MODOS], color=TEXTO)
    ax.set_ylabel("microsegundos por intercambio", color=TEXTO_2)
    ax.set_ylim(0, max(max(v) for v in comp.values()) * 1.12)
    ax.set_title("Tres formas correctas de intercambiar 8 MB entre dos procesos",
                 color=TEXTO, fontsize=11, loc="left")
    ax.grid(axis="y", color=REJILLA, zorder=0)
    for lado in ("top", "right"):
        ax.spines[lado].set_visible(False)
    ax.tick_params(colors=TEXTO_2)
    ax.legend(frameon=False, loc="upper right")
    fig.tight_layout()
    fig.savefig(carpeta / "grafica_arreglos.png")
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--resultados", default="resultados/ejercicio2", type=Path)
    args = parser.parse_args()
    carpeta = args.resultados
    barrido = leer_umbral(carpeta / "umbral.csv")
    comp = leer_comparacion(carpeta / "comparacion.csv")
    escribir_tabla(barrido, comp, carpeta)
    dibujar(comp, carpeta)
    lo, hi = frontera(barrido)
    print(f"Umbral: funciona con {lo} doubles y se cuelga con {hi}.")
    for modo in MODOS:
        print(f"  {NOMBRES[modo]:<24} mínimo {coma(min(comp[modo]), 1)} us")
    print(f"Escritos {carpeta / 'tabla_ej2.md'} y {carpeta / 'grafica_arreglos.png'}")


if __name__ == "__main__":
    main()
