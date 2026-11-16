#!/usr/bin/env bash
# Ejecucion del ejercicio 2 pensada para hacer capturas para la memoria: antes
# de cada orden imprime el prompt y la orden, como si se tecleara.
#     bash PracticaT4/src/ejercicio2/captura_ej2.sh            # compilacion y ejecucion
#     bash PracticaT4/src/ejercicio2/captura_ej2.sh barrido    # busqueda del umbral
# Si todavia no existe resultados/ejercicio2/umbral.csv, primero lanza
# ejecutar_ej2.sh (unos tres minutos) para saber cual es el umbral exacto.
cd "$(dirname "$0")/../.."                # carpeta PracticaT4
PLAZO=10
VERDE='\033[1;32m'; AZUL='\033[1;34m'; NORMAL='\033[0m'
PROMPT="${VERDE}${USER}@$(hostname)${NORMAL}:${AZUL}~/CAR_TheBig5/PracticaT4${NORMAL}\$ "

teclear() { printf "%b%s\n" "$PROMPT" "$1"; }   # solo imprime la orden
orden() { teclear "$*"; "$@"; }                 # la imprime y la ejecuta

if ! command -v mpicc > /dev/null; then
    echo "No encuentro mpicc en esta WSL: instala Open MPI con"
    echo "    sudo apt install openmpi-bin libopenmpi-dev"
    exit 1
fi
if [ ! -f resultados/ejercicio2/umbral.csv ]; then
    echo "Buscando el umbral exacto con ejecutar_ej2.sh (unos tres minutos)..."
    bash src/ejercicio2/ejecutar_ej2.sh > /tmp/ejecutar_ej2.log 2>&1 \
        || { tail -20 /tmp/ejecutar_ej2.log; exit 1; }
fi
LO=$(awk -F, '$2=="send" && $5=="ok"{print $3}' resultados/ejercicio2/umbral.csv | sort -n | tail -1)
HI=$(awk -F, '$2=="send" && $5=="cuelga"{print $3}' resultados/ejercicio2/umbral.csv | sort -n | head -1)
clear

if [ "${1:-}" = barrido ]; then
    orden cat resultados/ejercicio2/umbral.txt
    exit 0
fi

orden mpicc --showme:version
teclear "ompi_info --param btl sm --level 9 | grep -o 'btl_sm_eager_limit\" (current value: \"[0-9]*\"'"
ompi_info --param btl sm --level 9 | grep -o 'btl_sm_eager_limit" (current value: "[0-9]*"'
orden mpicc -O2 -Wall -Wextra -o bin/p4_ej2 src/ejercicio2/p4_ej2.c
echo
orden mpirun -np 2 bin/p4_ej2 "$LO" send
teclear "timeout -k 3 $PLAZO mpirun -np 2 bin/p4_ej2 $HI send"
rc=0; timeout -k 3 "$PLAZO" mpirun -np 2 bin/p4_ej2 "$HI" send || rc=$?
[ "$rc" -eq 124 ] && echo "  -> SE CUELGA: timeout lo mata a los $PLAZO s (codigo de salida 124)"
teclear "pgrep -x p4_ej2 || echo 'ningun proceso colgado'"
sleep 1; pgrep -x p4_ej2 || echo 'ningun proceso colgado'
echo
for modo in sendrecv orden noblo; do
    orden mpirun -np 2 bin/p4_ej2 "$HI" "$modo"
done
echo
for modo in sendrecv orden noblo; do
    orden mpirun -np 2 bin/p4_ej2 1048576 "$modo"
done
echo
orden mpirun --mca btl_sm_eager_limit 8192 -np 2 bin/p4_ej2 "$HI" send
