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

echo "== Compilacion ($CFLAGS)"
: > $R/compilacion.txt
for f in src/ejercicio*/*.c; do
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

echo "== Ejercicio 2: verificacion con 8 recuentos de hilos"
bin/p1_ej2 2048 300 > $R/ejercicio2/verificacion_2048.csv || true
bin/p1_ej2 256 300 > $R/ejercicio2/verificacion_256.csv || true

echo "== Ejercicio 3: escalabilidad fuerte (malla grande y pequena)"
bin/p1_ej3 2048 300 4 > $R/ejercicio3/fuerte_2048.csv
# 256^2 * 19200 pasos = 2048^2 * 300 pasos: mismo trabajo total
bin/p1_ej3 256 19200 4 > $R/ejercicio3/fuerte_256.csv

echo "== Ejercicio 4: escalabilidad debil"
bin/p1_ej4 1024 300 4 > $R/ejercicio4/debil.csv

echo "== Ejercicio 5: planificaciones (12 hilos)"
bin/p1_ej5 2048 300 12 4 > $R/ejercicio5/planificaciones.csv

echo "== Ejercicio 6: techo de memoria (STREAM triad) y variantes"
bin/p1_stream 40000000 10 > $R/ejercicio6/stream.csv
bin/p1_ej6 2048 300 12 4 > $R/ejercicio6/variantes.csv
OMP_PROC_BIND=close OMP_PLACES=cores bin/p1_ej6 2048 300 12 4 \
    > $R/ejercicio6/variantes_afinidad_close.csv
OMP_PROC_BIND=spread OMP_PLACES=threads bin/p1_ej6 2048 300 12 4 \
    > $R/ejercicio6/variantes_afinidad_spread.csv
# Vectorizacion del bucle interior (candidato de la lista del enunciado)
gcc $CFLAGS -fopt-info-vec-optimized src/ejercicio6/p1_ej6.c -o /dev/null -lm \
    > $R/ejercicio6/vectorizacion.txt 2>&1 || true

echo "== Graficas"
"${PYTHON:-python3}" src/ejercicio1/grafica_ej1.py
"${PYTHON:-python3}" src/graficas_lab1.py
echo "Hecho: $(date '+%F %T')"
