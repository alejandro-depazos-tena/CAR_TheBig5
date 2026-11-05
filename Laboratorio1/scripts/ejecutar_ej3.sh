#!/usr/bin/env bash
# Ejercicio 3: escalabilidad fuerte, malla grande y malla pequena EN LA MISMA
# SESION, y regenera las graficas. Uso, desde Laboratorio1:
#     bash scripts/ejecutar_ej3.sh
# Portatil enchufado, modo alto rendimiento y sin otros programas abiertos.
# Escribe primero en .tmp y solo sustituye el CSV si la ejecucion termina:
# si se corta, los resultados anteriores no se pierden.
set -euo pipefail
cd "$(dirname "$0")/.."

CFLAGS="-Wall -Wextra -O2 -std=c11 -fopenmp"
R=resultados/ejercicio3
mkdir -p bin "$R"

echo "== Compilacion"
gcc $CFLAGS src/ejercicio3/p1_ej3.c -o bin/p1_ej3 -lm

echo "== Entorno"
{
    echo "Fecha: $(date '+%F %T')"
    lscpu | grep -E "Model name|^CPU\(s\)|Thread|Core|L1d|L2|L3"
    free -h | head -2
    gcc --version | head -1
} > "$R/entorno_ej3.txt"

mide() {  # mide N pasos fichero
    echo "== N=$1, $2 pasos -> $3"
    bin/p1_ej3 "$1" "$2" 4 > "$3.tmp"
    mv "$3.tmp" "$3"
}
mide 2048 300   "$R/fuerte_2048.csv"
# 256^2 * 19200 = 2048^2 * 300: mismo trabajo total (celdas-paso)
mide 256  19200 "$R/fuerte_256.csv"

echo "== Graficas"
"${PYTHON:-python3}" src/graficas_lab1.py
echo "Hecho: $(date '+%F %T')"
