#!/usr/bin/env bash
# Compila y ejecuta el ping-pong del ejercicio 4.
set -euo pipefail

cd "$(dirname "$0")/../.."
BIN=bin/p4_ej4
R=resultados/ejercicio4
mkdir -p bin "$R"

echo "\$ mpicc -O2 -Wall -Wextra -o $BIN src/ejercicio4/p4_ej4.c"
mpicc -O2 -Wall -Wextra -o "$BIN" src/ejercicio4/p4_ej4.c
echo "(compilacion sin avisos)"

echo "bytes,repeticiones,ida_vuelta_us,una_direccion_us,mb_s,resultado" > "$R/tiempos.csv"
mpiexec -n 2 "$BIN" | tee "$R/medidas.txt" | while IFS= read -r linea; do
    echo "$linea" >> "$R/tiempos.csv"
done

python3 src/ejercicio4/grafica_ej4.py --resultados "$R"
