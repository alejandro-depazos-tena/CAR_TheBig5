#!/usr/bin/env bash
# Ejercicio 2 de la Practica 4: genera todo lo que pide el PDF con una orden.
#   1. Compila p4_ej2.c con -O2 -Wall -Wextra (los avisos salen por pantalla).
#   2. Busca el umbral del interbloqueo en modo send: dobla M hasta que se
#      cuelga y despues afina por biseccion hasta el double exacto. Cada
#      ejecucion tiene un plazo maximo (trampa 1); si se agota, se cuenta como
#      colgada y se comprueba que no ha quedado ningun proceso vivo.
#   3. Repite los dos tamanos de la frontera para ver que el umbral es estable,
#      y comprueba que los tres arreglos NO se cuelgan justo en el umbral.
#   4. Compara los tres arreglos con un millon de double (8 MB), RONDAS veces.
#   5. Genera las tablas y la grafica de barras.
#
# En PracticaT4/resultados/ejercicio2/ deja cinco ficheros:
#   umbral.txt            cada orden lanzada en la busqueda y lo que paso
#   umbral.csv            la misma busqueda en columnas
#   comparacion.csv       todas las medidas de los tres arreglos
#   tabla_ej2.md          tabla del barrido y tabla de la comparacion
#   grafica_arreglos.png  grafica de barras con los tres tiempos
#
# Uso, desde cualquier directorio (en WSL, con Open MPI):
#     bash PracticaT4/src/ejercicio2/ejecutar_ej2.sh          # 10 rondas, plazo 10 s
#     bash PracticaT4/src/ejercicio2/ejecutar_ej2.sh 20 5     # 20 rondas, plazo 5 s
# SOBRESCRIBE esos cinco ficheros. Tarda unos tres minutos: cada ejecucion que
# se cuelga consume el plazo entero.
set -euo pipefail
cd "$(dirname "$0")/../.."                # carpeta PracticaT4

RONDAS=${1:-10}
PLAZO=${2:-10}                            # segundos antes de matar una ejecucion
M_INI=64                                  # primer tamano del barrido (doubles)
M_MAX=4194304                             # 32 MB: si no se cuelga antes, se para
M_COMP=1048576                            # un millon de double = 8 MB
CFLAGS="-O2 -Wall -Wextra"
R=resultados/ejercicio2
MPIRUN="mpirun --use-hwthread-cpus"       # igual que en el ejercicio 1
BIN=bin/p4_ej2

mkdir -p bin "$R"

echo "== 1. Compilacion"
ORDEN="mpicc $CFLAGS -o $BIN src/ejercicio2/p4_ej2.c"
echo "\$ $ORDEN"
if ! AVISOS=$($ORDEN 2>&1); then
    echo "$AVISOS"
    echo "ERROR: p4_ej2.c no compila" >&2
    exit 1
fi
if [ -n "$AVISOS" ]; then echo "$AVISOS"; else echo "(sin ningun aviso con -Wall -Wextra)"; fi

# Lanza una ejecucion con plazo. Escribe en ULTIMO la linea del programa y en
# ULTIMO_US los microsegundos; devuelve 0 si termina bien, 1 si se cuelga.
# Cualquier otro fallo (o un DATO MAL) para el script.
lanzar() {
    local m=$1 modo=$2 salida rc=0
    salida=$(timeout -k 3 "$PLAZO" $MPIRUN -np 2 "$BIN" "$m" "$modo" 2>&1) || rc=$?
    if [ "$rc" -eq 124 ] || [ "$rc" -eq 137 ]; then
        # trampa 1: que no quede ningun proceso colgado comiendose la maquina
        sleep 1
        if pgrep -x p4_ej2 > /dev/null; then
            pkill -9 -x p4_ej2 || true
            echo "    (quedaban procesos vivos: matados con pkill)" >&2
        fi
        ULTIMO="m = $m doubles ($((m * 8)) bytes): SE CUELGA (plazo $PLAZO s)"
        ULTIMO_US=""
        return 1
    fi
    ULTIMO=$(echo "$salida" | grep -E "^$modo +m =" || true)
    if [ "$rc" -ne 0 ] || [ -z "$ULTIMO" ]; then
        echo "$salida" >&2
        echo "ERROR: la ejecucion con m = $m ($modo) fallo con codigo $rc" >&2
        exit 1
    fi
    if echo "$ULTIMO" | grep -q "DATO MAL"; then
        echo "$ULTIMO" >&2
        echo "ERROR: $modo recibe un dato que no es el del vecino" >&2
        exit 1
    fi
    ULTIMO_US=$(echo "$ULTIMO" | awk '{print $(NF-5)}')
    return 0
}

TXT="$R/umbral.txt"
CSV="$R/umbral.csv"
echo "fase,modo,m_doubles,bytes,resultado,us_por_intercambio" > "$CSV"
{
    echo "Busqueda del umbral del interbloqueo (modo send), plazo maximo $PLAZO s por ejecucion."
    echo "Orden de cada prueba: timeout -k 3 $PLAZO $MPIRUN -np 2 $BIN <m> send"
    echo
} > "$TXT"

probar() {                                # probar <fase> <m> <modo>
    local fase=$1 m=$2 modo=$3 res
    if lanzar "$m" "$modo"; then res=ok; else res=cuelga; fi
    echo "$fase,$modo,$m,$((m * 8)),$res,$ULTIMO_US" >> "$CSV"
    printf '  %-10s %s\n' "[$fase]" "$ULTIMO" | tee -a "$TXT"
    [ "$res" = ok ]
}

echo "== 2a. Doblando M en modo send desde $M_INI doubles" | tee -a "$TXT"
LO=0; HI=0; m=$M_INI
while [ "$m" -le "$M_MAX" ]; do
    if probar doblar "$m" send; then LO=$m; m=$((m * 2)); else HI=$m; break; fi
done

if [ "$HI" -eq 0 ]; then
    echo "No se ha colgado hasta $M_MAX doubles: esta implementacion copia" \
         "mensajes muy grandes. No hay umbral que afinar." | tee -a "$TXT"
else
    if [ "$LO" -eq 0 ]; then
        echo "Se cuelga ya con $M_INI doubles: baja M_INI en el script." | tee -a "$TXT"
        exit 1
    fi
    echo "== 2b. Biseccion entre $LO (pasa) y $HI (se cuelga)" | tee -a "$TXT"
    while [ $((HI - LO)) -gt 1 ]; do
        MID=$(((LO + HI) / 2))
        if probar biseccion "$MID" send; then LO=$MID; else HI=$MID; fi
    done
    echo "== 3a. Repetir la frontera: $LO debe pasar siempre y $HI colgarse siempre" | tee -a "$TXT"
    for k in 1 2 3; do probar repeticion "$LO" send || true; done
    for k in 1 2; do probar repeticion "$HI" send || true; done
    echo "== 3b. Los tres arreglos con m = $HI, donde send se cuelga" | tee -a "$TXT"
    for modo in sendrecv orden noblo; do
        probar arreglos "$HI" "$modo" || { echo "ERROR: $modo se cuelga" >&2; exit 1; }
    done
    {
        echo
        echo "UMBRAL: con $LO doubles ($((LO * 8)) bytes) funciona;"
        echo "        con $HI doubles ($((HI * 8)) bytes) se cuelga."
    } | tee -a "$TXT"
fi

echo "== 4. Los tres arreglos con m = $M_COMP doubles (8 MB), $RONDAS rondas"
# El orden de los tres arreglos rota en cada ronda para que ninguno salga
# siempre el primero (calentamiento, turbo).
COMP="$R/comparacion.csv"
echo "ronda,modo,m_doubles,us_por_intercambio" > "$COMP"
MODOS=(sendrecv orden noblo)
for ronda in $(seq 1 "$RONDAS"); do
    for k in 0 1 2; do
        modo=${MODOS[$(((ronda + k) % 3))]}
        lanzar "$M_COMP" "$modo" || { echo "ERROR: $modo se cuelga con 8 MB" >&2; exit 1; }
        echo "$ronda,$modo,$M_COMP,$ULTIMO_US" >> "$COMP"
        printf '  ronda %2d  %s\n' "$ronda" "$ULTIMO"
    done
done

echo "== 5. Tablas y grafica"
"${PYTHON:-python3}" src/ejercicio2/grafica_ej2.py --resultados "$R"
