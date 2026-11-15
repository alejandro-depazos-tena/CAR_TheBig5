#!/usr/bin/env bash
# Ejercicio 1 de la Practica 4: genera todo lo que pide el PDF con una orden.
#   1. Compila p4_ej1.c con -O2 -Wall -Wextra (los avisos salen por pantalla).
#   2. Lanza tres veces con 4 procesos para estudiar el orden de la salida.
#   3. Mide con 1, 2, 4, 8 y 12 procesos, REPS veces cada uno.
#   4. Comprueba la suma con repartos que no salen exactos (3, 7 y 12 procesos).
#   5. Genera la tabla de tiempos y la grafica de speedup.
#
# En PracticaT4/resultados/ejercicio1/ deja solo cuatro ficheros:
#   orden_4procesos.txt   las tres ejecuciones con 4 procesos (paso 2)
#   tiempos.csv           todas las medidas, de las que salen tabla y grafica
#   tabla_ej1.md          la tabla de tiempos, speedup y eficiencia
#   grafica_speedup.png   la grafica de speedup con la diagonal ideal
# Lo demas (compilacion, comprobacion de la suma) solo sale por pantalla.
#
# Uso, desde cualquier directorio:
#     bash PracticaT4/src/ejercicio1/ejecutar_ej1.sh          # 20 repeticiones
#     bash PracticaT4/src/ejercicio1/ejecutar_ej1.sh 30       # 30 repeticiones
# SOBRESCRIBE esos cuatro ficheros. Con 20 repeticiones tarda unos dos minutos.
# Conviene lanzarlo con el portatil enchufado y sin otros programas abiertos.
set -euo pipefail
cd "$(dirname "$0")/../.."                # carpeta PracticaT4

REPS=${1:-20}
PROCESOS="1 2 4 8 12"
CFLAGS="-O2 -Wall -Wextra"
R=resultados/ejercicio1
# WSL 2 presenta la CPU como 6 nucleos con 2 hilos cada uno, y Open MPI da por
# defecto un hueco por nucleo: sin esta opcion rechaza -np 8 y -np 12.
MPIRUN="mpirun --use-hwthread-cpus"

mkdir -p bin "$R"

echo "== 1. Compilacion"
ORDEN="mpicc $CFLAGS -o bin/p4_ej1 src/ejercicio1/p4_ej1.c -lm"
echo "\$ $ORDEN"
if ! AVISOS=$($ORDEN 2>&1); then
    echo "$AVISOS"
    echo "ERROR: p4_ej1.c no compila" >&2
    exit 1
fi
if [ -n "$AVISOS" ]; then echo "$AVISOS"; else echo "(sin ningun aviso con -Wall -Wextra)"; fi

echo "== 2. Orden de la salida: tres ejecuciones con 4 procesos"
: > "$R/orden_4procesos.txt"
for k in 1 2 3; do
    {
        echo "\$ $MPIRUN -np 4 bin/p4_ej1        # ejecucion $k de 3"
        $MPIRUN -np 4 bin/p4_ej1
        echo
    } | tee -a "$R/orden_4procesos.txt"
done

echo "== 3. Tiempos: $REPS rondas, y en cada ronda 1, 2, 4, 8 y 12 procesos"
# Se alternan los numeros de procesos dentro de cada ronda para que el turbo o
# el calentamiento del portatil no favorezcan a ninguna configuracion.
CSV="$R/tiempos.csv"
echo "procesos,repeticion,t_rango0_s,t_max_s,rango_lento,suma" > "$CSV"
for rep in $(seq 1 "$REPS"); do
    for p in $PROCESOS; do
        SALIDA=$($MPIRUN -np "$p" bin/p4_ej1)
        L1=$(echo "$SALIDA" | grep '^suma con')
        L2=$(echo "$SALIDA" | grep 'suma completa')
        T0=$(echo "$L1" | awk '{print $(NF-1)}')
        TMAX=$(echo "$L2" | awk '{print $(NF-1)}')
        LENTO=$(echo "$L2" | awk '{print $(NF-2)}' | tr -d ':')
        SUMA=$(echo "$L2" | awk '{print $4}')
        echo "$p,$rep,$T0,$TMAX,$LENTO,$SUMA" >> "$CSV"
        printf '  ronda %2d  P = %2d  rango 0 %s s  mas lento %s s\n' "$rep" "$p" "$T0" "$TMAX"
    done
done

echo "== 4. Comprobacion de la suma con repartos exactos y no exactos"
echo "N = 200000000 sumandos; N % P sumandos de mas van a los primeros rangos."
for p in 1 2 3 4 7 8 12; do
    printf 'P = %2d  (N %% P = %d)  ' "$p" "$((200000000 % p))"
    $MPIRUN -np "$p" bin/p4_ej1 | grep 'suma completa' | sed 's/^ *//'
done

echo "== 5. Tabla y grafica"
"${PYTHON:-python3}" src/ejercicio1/grafica_ej1.py --resultados "$R"
