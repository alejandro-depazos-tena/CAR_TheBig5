"""Genera la tabla y la gráfica del ejercicio 4."""

import argparse
import csv
import statistics
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def coma(valor, decimales):
    return f"{valor:.{decimales}f}".replace(".", ",")


def leer(ruta):
    with ruta.open(newline="", encoding="utf-8") as fichero:
        filas = list(csv.DictReader(fichero))
    if len(filas) != 11 or any(f["resultado"] != "correcto" for f in filas):
        raise ValueError("Faltan tamaños o hay datos incorrectos.")
    return filas


def escribir_tabla(filas, carpeta):
    lineas = [
        "Medidas de ping-pong. El tiempo se cronometra con `MPI_Wtime` sobre "
        "todo el bucle y se divide entre sus repeticiones.",
        "",
        "| Bytes | Repeticiones | Ida y vuelta (us) | Una dirección (us) | MB/s |",
        "|---:|---:|---:|---:|---:|",
    ]
    for f in filas:
        lineas.append(
            f"| {int(f['bytes']):,} | {f['repeticiones']} | "
            f"{coma(float(f['ida_vuelta_us']), 3)} | "
            f"{coma(float(f['una_direccion_us']), 3)} | "
            f"{coma(float(f['mb_s']), 1)} |".replace(",", " ")
        )
    lineas.append("")
    lineas.append("Cada fila corresponde a una ejecución; no se han promediado ejecuciones externas.")
    (carpeta / "tabla_ej4.md").write_text("\n".join(lineas) + "\n", encoding="utf-8")


def dibujar(filas, carpeta):
    x = [int(f["bytes"]) for f in filas]
    y = [float(f["mb_s"]) for f in filas]
    plt.figure(figsize=(8, 5), dpi=150)
    plt.plot(x, y, "o-", color="#2a78d6")
    plt.xscale("log", base=4)
    plt.yscale("log")
    plt.xlabel("tamaño del mensaje (bytes)")
    plt.ylabel("ancho de banda (MB/s)")
    plt.title("Ejercicio 4: ancho de banda del ping-pong")
    plt.grid(True, which="both", alpha=0.25)
    plt.tight_layout()
    plt.savefig(carpeta / "grafica_ej4.png")
    plt.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--resultados", type=Path, default=Path("resultados/ejercicio4"))
    args = parser.parse_args()
    filas = leer(args.resultados / "tiempos.csv")
    escribir_tabla(filas, args.resultados)
    dibujar(filas, args.resultados)
    latencia = float(filas[0]["una_direccion_us"])
    mejor = max(filas, key=lambda f: float(f["mb_s"]))
    print(f"Latencia: {latencia:.3f} us")
    print(f"Mejor ancho de banda: {float(mejor['mb_s']):.1f} MB/s "
          f"con {int(mejor['bytes'])} bytes")


if __name__ == "__main__":
    main()
