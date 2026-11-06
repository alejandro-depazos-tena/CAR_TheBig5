#!/usr/bin/env bash
# Laboratorio 1 · Ejercicios 2 a 6. El Ejercicio 1 ya estaba completado y este
# guion NO lo toca: ni su codigo, ni su binario, ni sus resultados.
#
# Compila y ejecuta TODOS los experimentos desde el Ejercicio 2 en una sola
# sesion (misma maquina, mismo estado) y guarda:
#   resultados/ejercicioN/*.csv              salidas en bruto de cada programa
#   resultados/ejercicioN/*.png              graficas (src/graficas_lab1.py)
#   resultados/ejercicio6/vectorizacion.txt  informe de gcc (evidencia E3)
#   resultados/salidas_ejercicios_2_en_adelante.txt
#       TODAS las salidas de consola, literales, con la orden que las produce,
#       organizadas por ejercicio y por requisito del enunciado.
#   docs/tablas_ej2_ej6.md                   tablas derivadas de los CSV
#
# Uso (desde cualquier directorio; las rutas son relativas a Laboratorio1):
#     bash Laboratorio1/scripts/ejecutar_ej2_en_adelante.sh [repeticiones=5]
# Dura unos 40 minutos. Portatil ENCHUFADO, modo de energia de maximo
# rendimiento y sin usar el equipo mientras mide.
set -euo pipefail
cd "$(dirname "$0")/.."

REPS=${1:-5}
CFLAGS="-Wall -Wextra -O2 -std=c11 -fopenmp"
CFLAGS_SEQ="-Wall -Wextra -O2 -std=c11"   # T1 sin -fopenmp (apartado V.B)
PY=${PYTHON:-python3}
R=resultados
LOG=$R/salidas_ejercicios_2_en_adelante.txt
INICIO=$SECONDS
mkdir -p bin docs $R/ejercicio2 $R/ejercicio3 $R/ejercicio4 $R/ejercicio5 $R/ejercicio6
: > "$LOG"

log() { printf '%s\n' "$*" | tee -a "$LOG"; }
seccion() {
    log ""
    log "########################################################################"
    log "# $*"
    log "########################################################################"
}
apartado() { log ""; log "=== $* ==="; }

# muestra ARGUMENTOS...: la orden tal como se escribiria en la terminal
# (entrecomilla los argumentos con espacios o simbolos del shell).
muestra() {
    local s="" a
    for a in "$@"; do
        case "$a" in
            *[[:space:]\|\>\<\&\;\*]*) s+=" '$a'" ;;
            *) s+=" $a" ;;
        esac
    done
    printf '%s' "${s# }"
}

# ejecuta DESTINO ORDEN...
# Escribe la orden en el registro, la ejecuta y copia literalmente su stdout
# (que, si DESTINO no es '-', se guarda ademas en ese fichero) y su stderr.
# Si la orden falla, el guion se detiene sin sustituir el fichero anterior.
ejecuta() {
    local destino=$1 err rc t0
    shift
    err=$(mktemp)
    if [ "$destino" = "-" ]; then
        log "\$ $(muestra "$@")"
    else
        log "\$ $(muestra "$@") > $destino"
    fi
    t0=$SECONDS
    set +e
    if [ "$destino" = "-" ]; then
        "$@" 2> "$err" | tee -a "$LOG"; rc=${PIPESTATUS[0]}
    else
        "$@" 2> "$err" | tee "$destino.tmp" | tee -a "$LOG"; rc=${PIPESTATUS[0]}
    fi
    set -e
    if [ -s "$err" ]; then
        log "[stderr]"
        tee -a "$LOG" < "$err"
    fi
    rm -f "$err"
    log "[codigo de salida: $rc · $((SECONDS - t0)) s]"
    if [ "$rc" -ne 0 ]; then
        log "ERROR: la orden anterior ha fallado; se detiene la ejecucion."
        rm -f "${destino}.tmp"
        exit 1
    fi
    if [ "$destino" != "-" ]; then mv "$destino.tmp" "$destino"; fi
}

AVISOS=0
compila() {   # compila ARGUMENTOS_DE_GCC...
    local salida
    log "\$ gcc $*"
    if ! salida=$(gcc "$@" 2>&1); then
        log "$salida"
        log "ERROR de compilacion; se detiene la ejecucion."
        exit 1
    fi
    if [ -n "$salida" ]; then log "$salida"; AVISOS=1; else log "(sin avisos)"; fi
}

variables_omp() { env | grep '^OMP_' || echo "(ninguna variable OMP_* fijada)"; }

# Solo bajo WSL: procesador real, alimentacion y modo de energia del portatil.
# Todo se recoge primero y se escribe despues en UTF-8 (powercfg escribe en la
# pagina de codigos de la consola de Windows, que no es UTF-8).
info_windows() {
    local ps
    ps='$cpu = Get-CimInstance Win32_Processor | Format-List Name,NumberOfCores,NumberOfLogicalProcessors,MaxClockSpeed | Out-String; '
    ps+='$bat = Get-CimInstance Win32_Battery | Format-List BatteryStatus,EstimatedChargeRemaining | Out-String; '
    ps+='$plan = powercfg /getactivescheme; '
    ps+='$k = Get-ItemProperty "HKLM:\SYSTEM\CurrentControlSet\Control\Power\User\PowerSchemes"; '
    ps+='$modo = switch ($k.ActiveOverlayAcPowerScheme) { "ded574b5-45a0-4f42-8737-46345c09c238" { "Maximo rendimiento" } "961cc777-2547-4f9d-8174-7d86181b8a7a" { "Maxima eficiencia energetica" } default { "Equilibrado (predeterminado)" } }; '
    ps+='[Console]::OutputEncoding = [Text.Encoding]::UTF8; '
    ps+='$cpu; $bat; $plan; "Modo de energia con corriente (overlay AC): $modo"'
    if ! powershell.exe -NoProfile -Command "$ps" | tr -d '\r' | sed '/^[[:space:]]*$/d'; then
        echo "(no se pudo consultar el anfitrion Windows)"
    fi
    echo "(BatteryStatus: 2 = conectado a la corriente, 1 = descargando en bateria)"
}

informe_vectorizacion() {   # evidencia E3 del Ejercicio 6
    echo "# Lineas de los bucles interiores de src/ejercicio6/p1_ej6.c:"
    grep -n 'for (size_t j' src/ejercicio6/p1_ej6.c
    echo "# gcc $CFLAGS -fopt-info-vec-optimized (bucles y bloques vectorizados):"
    gcc $CFLAGS -fopt-info-vec-optimized src/ejercicio6/p1_ej6.c -o /dev/null -lm 2>&1
    echo "# gcc $CFLAGS -fopt-info-vec-missed (solo los bucles NO vectorizados):"
    gcc $CFLAGS -fopt-info-vec-missed src/ejercicio6/p1_ej6.c -o /dev/null -lm 2>&1 \
        | grep "couldn't vectorize loop" | sort -t: -k2,2n -u
}

log "LABORATORIO 1 · TermoHPC I — SALIDAS DE CONSOLA DESDE EL EJERCICIO 2"
log "Generado por scripts/ejecutar_ej2_en_adelante.sh (repeticiones=$REPS) el $(date '+%F %T %z')."
log "El Ejercicio 1 ya estaba completado (codigo en src/ejercicio1/, resultados en"
log "resultados/ejercicio1/) y no se incluye ni se vuelve a ejecutar aqui."
log "Convenciones: '\$ orden' es la orden ejecutada desde Laboratorio1/; debajo va su"
log "salida literal (stdout y, tras [stderr], la salida de error). '[codigo de salida]'"
log "da el codigo y los segundos que tardo. Cada apartado indica el requisito del"
log "enunciado al que responde. Criterio de todas las tablas de tiempos: MINIMO de"
log "las repeticiones; la inicializacion queda fuera del cronometro."

# --------------------------------------------------------------------------
seccion "0. Entorno de la sesion (maquina, sistema, compilador)"
ejecuta - date '+%F %T %z'
ejecuta - uname -srvmo
ejecuta - grep PRETTY_NAME /etc/os-release
ejecuta - lscpu
ejecuta - lscpu -e
ejecuta - free -h
ejecuta - nproc
ejecuta - gcc --version
ejecuta - "$PY" --version
ejecuta - variables_omp
if grep -qi microsoft /proc/version && command -v powershell.exe > /dev/null; then
    apartado "Anfitrion Windows (WSL2): procesador real y alimentacion"
    ejecuta - info_windows
fi

# --------------------------------------------------------------------------
seccion "1. Compilacion (Ejercicios 2 a 6, flags del apartado IV.A)"
compila $CFLAGS src/ejercicio2/p1_ej2.c -o bin/p1_ej2 -lm
compila $CFLAGS src/ejercicio3/p1_ej3.c -o bin/p1_ej3 -lm
compila $CFLAGS_SEQ src/ejercicio3/p1_seq.c -o bin/p1_seq -lm
compila $CFLAGS src/ejercicio4/p1_ej4.c -o bin/p1_ej4 -lm
compila $CFLAGS src/ejercicio5/p1_ej5.c -o bin/p1_ej5 -lm
compila $CFLAGS src/ejercicio6/p1_ej6.c -o bin/p1_ej6 -lm
compila $CFLAGS src/ejercicio6/p1_stream.c -o bin/p1_stream -lm
if [ "$AVISOS" -eq 0 ]; then
    log "RESULTADO: todo compila sin errores y sin avisos con -Wall -Wextra."
else
    log "RESULTADO: hay avisos de compilacion (ver arriba)."
fi
ejecuta - sh -c "ldd bin/p1_seq | grep -c gomp || true"
log "(0 = bin/p1_seq no enlaza libgomp: es la secuencial compilada sin -fopenmp)"
log "Repeticiones: $REPS por punto, en RONDAS INTERCALADAS dentro de cada programa"
log "(cada ronda mide una vez todas las configuraciones de la tabla, para que una"
log "racha lenta de la maquina no caiga sobre todas las repeticiones de un punto)."

# --------------------------------------------------------------------------
seccion "EJERCICIO 2 · Paralelizacion con OpenMP y verificacion numerica"
apartado "Ej2 [PROG] Tabla de verificacion con 1, 2, 4, 8, 12, 16, 20 y 24 hilos (N=2048, 300 pasos): diferencia maxima celda a celda y checksum frente a la secuencial; columna hilos_reales = comprobacion obligatoria del apartado IV.D. Filas tipo=in_situ: [PROG] fallo provocado (un solo buffer), dos ejecuciones por recuento de hilos"
ejecuta $R/ejercicio2/verificacion_2048.csv bin/p1_ej2 2048 300
apartado "Ej2 [PROG] La misma tabla con N=256, 300 pasos: con malla pequena el calor llega a las fronteras entre franjas y el fallo in situ cambia ya desde pocos hilos"
ejecuta $R/ejercicio2/verificacion_256.csv bin/p1_ej2 256 300
apartado "Ej2 · Resumen de la verificacion calculado de los dos CSV anteriores"
ejecuta - "$PY" src/analisis_lab1.py ej2

# --------------------------------------------------------------------------
seccion "EJERCICIO 3 · Escalabilidad fuerte"
apartado "Ej3/Ej6 [PROG] Techo de ancho de banda de la maquina: STREAM triad, 3 x 40e6 double (960 MB), 10 repeticiones (apartado V.G; lo usan el Ej3 y el Ej6)"
ejecuta $R/ejercicio6/stream.csv bin/p1_stream 40000000 10
apartado "Ej3 [PROG] T1 para el speedup ABSOLUTO: secuencial compilada SIN -fopenmp, N=2048, 300 pasos, 10 repeticiones (cada repeticion, minimo, mediana y maximo = dispersion de la maquina)"
ejecuta $R/ejercicio3/t1_sin_openmp_2048.csv bin/p1_seq 2048 300 10
apartado "Ej3 [PROG] T1 sin -fopenmp para la malla pequena: N=256, 19200 pasos (256^2 x 19200 = 2048^2 x 300 celdas-paso)"
ejecuta $R/ejercicio3/t1_sin_openmp_256.csv bin/p1_seq 256 19200 10
apartado "Ej3 [PROG] Barrido 1..24 hilos, N=2048, 300 pasos, $REPS repeticiones: tiempo, speedup absoluto (y relativo, etiquetado), eficiencia, hilos reales, [PROG] techo de calculo medido (apartado IV.C) y coste de un paso vacio (para el [RAZ] de la malla pequena)"
ejecuta $R/ejercicio3/fuerte_2048.csv bin/p1_ej3 2048 300 "$REPS"
apartado "Ej3 [PROG] Malla pequena: N=256, 19200 pasos (mismo trabajo total), $REPS repeticiones"
ejecuta $R/ejercicio3/fuerte_256.csv bin/p1_ej3 256 19200 "$REPS"
apartado "Ej3 · Tablas derivadas de los CSV: speedup absoluto/relativo, % del techo, Karp-Flatt, Amdahl, coste de paso vacio frente a tiempo por paso, ancho de banda frente a STREAM"
ejecuta - "$PY" src/analisis_lab1.py ej3

# --------------------------------------------------------------------------
seccion "EJERCICIO 4 · Escalabilidad debil"
apartado "Ej4 [PROG] N(p) = round(1024 * sqrt(p)), 300 pasos, $REPS repeticiones; cada N verificada contra la secuencial con la misma N"
ejecuta $R/ejercicio4/debil.csv bin/p1_ej4 1024 300 "$REPS"
apartado "Ej4 · Tablas derivadas: eficiencia debil, N exactos tras redondear, coste por celda frente al tamano de la malla"
ejecuta - "$PY" src/analisis_lab1.py ej4

# --------------------------------------------------------------------------
seccion "EJERCICIO 5 · Comparacion de planificaciones"
apartado "Ej5 [PROG] 12 hilos, N=2048, 300 pasos, $REPS repeticiones: static sin trozo, static 1/4/16/64, dynamic 1/4/16/64 y guided; [PROG] cada una verificada contra la secuencial"
ejecuta $R/ejercicio5/planificaciones.csv bin/p1_ej5 2048 300 12 "$REPS"
apartado "Ej5 · Medida complementaria (no pedida): las mismas planificaciones con 8 hilos. En esta maquina 12 hilos son TODAS las CPU logicas que ve WSL2, y el anfitrion Windows compite por ellas; con 8 queda margen"
ejecuta $R/ejercicio5/planificaciones_8hilos.csv bin/p1_ej5 2048 300 8 "$REPS"
apartado "Ej5 · Tablas derivadas: orden, coste relativo y separacion de los efectos de entrelazar y de repartir bajo demanda"
ejecuta - "$PY" src/analisis_lab1.py ej5

# --------------------------------------------------------------------------
seccion "EJERCICIO 6 · Optimizacion guiada por medicion"
apartado "Ej6 · Evidencia E3: informe de vectorizacion de gcc (el bucle interior de la base no se vectoriza; el de it3_simd si)"
ejecuta $R/ejercicio6/vectorizacion.txt informe_vectorizacion
apartado "Ej6 [PROG] Punto de partida, evidencias E1/E2, iteraciones 1-4, control y otras hipotesis; 12 hilos, N=2048, 300 pasos, $REPS repeticiones; cada variante verificada"
ejecuta $R/ejercicio6/variantes.csv bin/p1_ej6 2048 300 12 "$REPS"
apartado "Ej6 · Afinidad (candidato del enunciado): el mismo binario con OMP_PROC_BIND=close OMP_PLACES=cores"
ejecuta $R/ejercicio6/variantes_afinidad_close.csv env OMP_PROC_BIND=close OMP_PLACES=cores bin/p1_ej6 2048 300 12 "$REPS"
apartado "Ej6 · Afinidad: OMP_PROC_BIND=spread OMP_PLACES=threads"
ejecuta $R/ejercicio6/variantes_afinidad_spread.csv env OMP_PROC_BIND=spread OMP_PLACES=threads bin/p1_ej6 2048 300 12 "$REPS"
apartado "Ej6 · Medida complementaria (no pedida): las mismas variantes con 8 hilos y sin afinidad (12 hilos ocupan todas las CPU logicas de WSL2)"
ejecuta $R/ejercicio6/variantes_8hilos.csv bin/p1_ej6 2048 300 8 "$REPS"
apartado "Ej6 · Tablas derivadas: cadena de iteraciones, mejora de cada una sobre la anterior, ancho de banda conseguido frente a STREAM y efecto de la afinidad"
ejecuta - "$PY" src/analisis_lab1.py ej6

# --------------------------------------------------------------------------
seccion "Graficas y tablas para la memoria"
ejecuta - "$PY" src/graficas_lab1.py
ejecuta - sh -c "ls -l $R/ejercicio3/*.png $R/ejercicio4/*.png $R/ejercicio5/*.png $R/ejercicio6/*.png"
log "\$ $PY src/analisis_lab1.py markdown > docs/tablas_ej2_ej6.md"
"$PY" src/analisis_lab1.py markdown > docs/tablas_ej2_ej6.md
log "(generado docs/tablas_ej2_ej6.md: las mismas tablas derivadas, en Markdown)"

# --------------------------------------------------------------------------
seccion "Resumen final de validaciones"
ejecuta - "$PY" src/analisis_lab1.py resumen
log ""
log "Fin: $(date '+%F %T %z'). Duracion total: $((SECONDS - INICIO)) s."
