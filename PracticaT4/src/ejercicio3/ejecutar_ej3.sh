#!/usr/bin/env bash
# Compila, mide y genera la tabla y la grafica del ejercicio 3.
set -euo pipefail

cd "$(dirname "$0")/../.."

REPS=${1:-20}
PROCESOS="2 4 8 12 24"
CFLAGS="-O2 -Wall -Wextra"
BIN=bin/p4_ej3
R=resultados/ejercicio3

mkdir -p bin "$R"

{
    echo "Compilacion del ejercicio 3"
    echo "\$ mpicc $CFLAGS -o $BIN src/ejercicio3/p4_ej3.c"
    if ! AVISOS=$(mpicc $CFLAGS -o "$BIN" src/ejercicio3/p4_ej3.c 2>&1); then
        echo "$AVISOS"
        exit 1
    fi
    if [ -n "$AVISOS" ]; then
        echo "$AVISOS"
    else
        echo "(sin ningun aviso con -Wall -Wextra)"
    fi
} | tee "$R/compilacion.txt"

MPIRUN="mpiexec"
CSV="$R/tiempos.csv"
SALIDAS="$R/salidas.txt"
echo "procesos,repeticion,t_mano_us,t_reduce_us,ratio,suma" > "$CSV"
: > "$SALIDAS"

echo "Medidas: $REPS repeticiones por numero de procesos" | tee "$SALIDAS"
for rep in $(seq 1 "$REPS"); do
    for p in $PROCESOS; do
        salida=$($MPIRUN -n "$p" "$BIN")
        printf '%s\n' "$salida" >> "$SALIDAS"
        linea=$(printf '%s\n' "$salida" | grep 'procesos')
        t_mano=$(printf '%s\n' "$linea" | awk '{print $5}')
        t_reduce=$(printf '%s\n' "$linea" | awk '{print $8}')
        ratio=$(printf '%s\n' "$linea" | awk '{print $10}' | tr -d 'x')
        suma=$(printf '%s\n' "$linea" | awk '{print $11" "$12}')
        if [ "$suma" != "suma correcta" ]; then
            echo "ERROR: resultado incorrecto para P=$p, repeticion=$rep" >&2
            exit 1
        fi
        echo "$p,$rep,$t_mano,$t_reduce,$ratio,$suma" >> "$CSV"
        printf '  ronda %2d  P = %2d  a mano %s us  MPI_Reduce %s us  %sx\n' \
            "$rep" "$p" "$t_mano" "$t_reduce" "$ratio"
    done
done

python3 src/ejercicio3/grafica_ej3.py --resultados "$R"
