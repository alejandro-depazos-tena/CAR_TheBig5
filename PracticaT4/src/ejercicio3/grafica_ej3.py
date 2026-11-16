"""Genera la tabla y la grafica del ejercicio 3 a partir de tiempos.csv."""

import argparse
import csv
import statistics
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def coma(valor, decimales):
    return f"{valor:.{decimales}f}".replace(".", ",")


def leer(ruta):
    datos = defaultdict(lambda: {"mano": [], "reduce": []})
    with ruta.open(newline="", encoding="utf-8") as fichero:
        for fila in csv.DictReader(fichero):
            p = int(fila["procesos"])
            datos[p]["mano"].append(float(fila["t_mano_us"]))
            datos[p]["reduce"].append(float(fila["t_reduce_us"]))
    if sorted(datos) != [2, 4, 8, 12, 24]:
        raise ValueError("La tabla debe contener P = 2, 4, 8, 12 y 24.")
    return dict(sorted(datos.items()))


def escribir_tabla(datos, carpeta):
    lineas = [
        "Tiempos por recogida, medidos con `MPI_Wtime` dentro del programa. "
        f"Minimo de {len(datos[2]['mano'])} repeticiones por fila; entre parentesis, la mediana.",
        "",
        "| Procesos | A mano (us) | MPI_Reduce (us) | A mano / MPI_Reduce |",
        "|---:|---:|---:|---:|",
    ]
    for p, valores in datos.items():
        mano = valores["mano"]
        reduce = valores["reduce"]
        t_mano = min(mano)
        t_reduce = min(reduce)
        lineas.append(
            f"| {p} | {coma(t_mano, 1)} ({coma(statistics.median(mano), 1)}) "
            f"| {coma(t_reduce, 1)} ({coma(statistics.median(reduce), 1)}) "
            f"| {coma(t_mano / t_reduce, 2)}x |"
        )
    lineas += [
        "",
        "Cada ejecución hace 2.000 recogidas consecutivas de un `double`; "
        "la tabla muestra el tiempo medio por recogida.",
    ]
    (carpeta / "tabla_ej3.md").write_text("\n".join(lineas) + "\n", encoding="utf-8")


def dibujar(datos, carpeta):
    procesos = list(datos)
    mano = [min(datos[p]["mano"]) for p in procesos]
    reduce = [min(datos[p]["reduce"]) for p in procesos]
    plt.figure(figsize=(8, 5), dpi=150)
    plt.plot(procesos, mano, "o-", label="a mano: MPI_Send/MPI_Recv")
    plt.plot(procesos, reduce, "o-", label="MPI_Reduce")
    plt.xscale("log", base=2)
    plt.yscale("log")
    plt.xticks(procesos, [str(p) for p in procesos])
    plt.xlabel("numero de procesos")
    plt.ylabel("microsegundos por recogida (escala logaritmica)")
    plt.title("Ejercicio 3: recogida manual frente a MPI_Reduce")
    plt.grid(True, which="both", alpha=0.25)
    plt.legend(frameon=False)
    plt.tight_layout()
    plt.savefig(carpeta / "grafica_ej3.png")
    plt.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--resultados", type=Path, default=Path("resultados/ejercicio3"))
    args = parser.parse_args()
    datos = leer(args.resultados / "tiempos.csv")
    escribir_tabla(datos, args.resultados)
    dibujar(datos, args.resultados)
    print(f"Escritos {args.resultados / 'tabla_ej3.md'} y "
          f"{args.resultados / 'grafica_ej3.png'}")


if __name__ == "__main__":
    main()
