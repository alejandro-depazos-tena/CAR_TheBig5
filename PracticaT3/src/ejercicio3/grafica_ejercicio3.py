"""Genera la grafica del ejercicio 3 a partir de su salida de terminal.

Desde PracticaT3:
    python3 src/ejercicio3/grafica_ejercicio3.py

Tambien admite rutas personalizadas:
    python3 grafica_ejercicio3.py --entrada salida.txt --salida grafica.png
"""

import argparse
import math
import re
from pathlib import Path

import matplotlib

matplotlib.use("Agg")  # Guarda la imagen sin necesitar una ventana grafica.
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter


def leer_resultados(ruta):
    texto = ruta.read_text(encoding="utf-8-sig")
    cabecera = re.search(
        r"(?m)^\s*(\d+) hilos, (\d+) sumas, mejor de (\d+)\s*$", texto
    )
    if not cabecera:
        raise ValueError("No se encuentra la cabecera con hilos, sumas y repeticiones.")
    if "los ocho acumulados dan lo mismo" not in texto:
        raise ValueError("Falta la comprobacion final de que los acumulados coinciden.")

    partes = re.split(r"(?m)^acumulador de tipo (double|long long)\s*$", texto)
    metodos = ["secuencial", "critical", "atomic", "reduction"]
    datos = {}

    for tipo, bloque in zip(partes[1::2], partes[2::2]):
        if tipo in datos:
            raise ValueError("El archivo contiene varias ejecuciones; usa solo una.")
        tiempos = []
        for metodo in metodos:
            coincidencia = re.search(
                rf"(?m)^\s*{metodo}\s+([0-9]+(?:[.,][0-9]+)?)\s+s\b", bloque
            )
            if not coincidencia:
                raise ValueError(f"Falta el tiempo de {metodo} para {tipo}.")
            tiempo = float(coincidencia.group(1).replace(",", "."))
            if not math.isfinite(tiempo) or tiempo <= 0:
                raise ValueError("La escala logaritmica requiere tiempos positivos.")
            tiempos.append(tiempo)
        datos[tipo] = tiempos

    if set(datos) != {"double", "long long"}:
        raise ValueError("Se necesitan los resultados de double y long long.")
    return datos, tuple(map(int, cabecera.groups()))


def crear_grafica(datos, configuracion, destino):
    hilos, sumas, repeticiones = configuracion
    metodos = ["Secuencial", "critical", "atomic", "reduction"]
    colores = ["#526278", "#d7614b", "#dbab3c", "#258a81"]
    posiciones = [0, 1]
    ancho = 0.18

    plt.rcParams.update({"font.family": "DejaVu Sans", "font.size": 11})
    fig, ax = plt.subplots(figsize=(10, 6))
    fig.subplots_adjust(left=0.12, right=0.98, bottom=0.18, top=0.78)

    for i, (metodo, color) in enumerate(zip(metodos, colores)):
        valores = [datos["double"][i], datos["long long"][i]]
        lugares = [p + (i - 1.5) * ancho for p in posiciones]
        barras = ax.bar(lugares, valores, ancho, label=metodo, color=color, zorder=3)
        for barra, valor in zip(barras, valores):
            ax.text(
                barra.get_x() + barra.get_width() / 2,
                valor * 1.15,
                f"{valor:.4f}".replace(".", ","),
                ha="center", va="bottom", fontsize=10,
            )

    todos = datos["double"] + datos["long long"]
    ax.set_yscale("log")
    inferior = 10 ** math.floor(math.log10(min(todos)))
    superior = 10 ** math.ceil(math.log10(max(todos) * 1.5))
    ax.set_ylim(inferior, superior)
    ax.yaxis.set_major_formatter(
        FuncFormatter(lambda valor, posicion: f"{valor:g}".replace(".", ","))
    )
    ax.set_xticks(posiciones, ["double", "long long"])
    ax.set_xlabel("Tipo de acumulador", labelpad=12)
    ax.set_ylabel("Tiempo (s), escala logarítmica", labelpad=12)
    ax.grid(axis="y", which="major", alpha=0.22, zorder=0)
    for lado in ["top", "right"]:
        ax.spines[lado].set_visible(False)
    ax.legend(loc="lower left", bbox_to_anchor=(0, 1.06), ncol=4, frameon=False)

    fig.suptitle(
        "Ejercicio 3 · Comparación de las formas de acumular",
        x=0.12, ha="left", y=0.97, fontsize=17, fontweight="bold",
    )
    cantidad = f"{sumas:,}".replace(",", " ")
    fig.text(
        0.12, 0.905,
        f"{cantidad} sumas · Versiones paralelas con {hilos} hilos",
        color="#526278",
    )
    fig.text(
        0.12, 0.06,
        f"Mínimo de {repeticiones} repeticiones por versión. Datos leídos de la salida del programa.",
        fontsize=10, color="#526278",
    )

    destino.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(destino, dpi=200, facecolor="white")
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--entrada", type=Path,
        default=Path("resultados/ejercicio3/salida_wsl.txt"),
    )
    parser.add_argument("--salida", type=Path)
    argumentos = parser.parse_args()
    destino = argumentos.salida or argumentos.entrada.with_name("grafica_ejercicio3.png")
    try:
        datos, configuracion = leer_resultados(argumentos.entrada)
        crear_grafica(datos, configuracion, destino)
    except (OSError, ValueError) as error:
        parser.exit(1, f"Error: {error}\n")
    print(f"Grafica guardada en: {destino.resolve()}")


if __name__ == "__main__":
    main()
