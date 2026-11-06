#!/usr/bin/env bash
# Compila y ejecuta todos los experimentos del Laboratorio 1 y guarda las
# salidas en bruto en resultados/. Uso, desde Laboratorio1:
#     bash scripts/ejecutar_todo.sh
# Conviene lanzarlo con el portatil enchufado y sin otros programas abiertos.
set -euo pipefail
cd "$(dirname "$0")/.."

CFLAGS="-Wall -Wextra -O2 -std=c11 -fopenmp"
R=resultados
mkdir -p bin $R/ejercicio{1,2,3,4,5,6}

echo "== Compilacion del Ejercicio 1 ($CFLAGS)"
echo "   (los ejercicios 2 a 6 los compila scripts/ejecutar_ej2_en_adelante.sh)"
: > $R/compilacion.txt
for f in src/ejercicio1/*.c; do
    exe=bin/$(basename "$f" .c)
    echo "gcc $CFLAGS $f -o $exe -lm" | tee -a $R/compilacion.txt
    gcc $CFLAGS "$f" -o "$exe" -lm 2>&1 | tee -a $R/compilacion.txt
done
echo "(si no aparece ningun aviso arriba, todo compila sin avisos)" >> $R/compilacion.txt

echo "== Entorno"
{
    echo "Fecha: $(date '+%F %T')"
    lscpu
    echo; free -h
    echo; uname -sr
    echo; gcc --version | head -1
    echo; echo "nproc: $(nproc)"
} > $R/entorno.txt

echo "== Ejercicio 1: tiempos secuenciales (5 ejecuciones por tamano)"
for N in 512 1024 2048 4096; do
    out=$R/ejercicio1/tiempos_$N.txt
    : > "$out"
    for rep in 1 2 3 4 5; do
        echo "REPETICIÓN $rep DE 5" >> "$out"
        bin/p1_ej1 $N 300 1e-6 medida >> "$out"
        echo >> "$out"
    done
done
bin/p1_ej1 64 100000 1e-6 simulacion comprobar > $R/ejercicio1/prueba_tolerancia.txt
bin/p1_ej1 512 300 1e-6 medida comprobar > $R/ejercicio1/prueba_pasos_fijos.txt
bin/p1_ej1 512 300 1e-6 medida comprobar csv > $R/ejercicio1/prueba_pasos_fijos.csv

echo "== Grafica del Ejercicio 1"
"${PYTHON:-python3}" src/ejercicio1/grafica_ej1.py

echo "== Ejercicios 2 a 6 (compilacion, medidas, graficas y registro de salidas)"
bash scripts/ejecutar_ej2_en_adelante.sh
echo "Hecho: $(date '+%F %T')"
