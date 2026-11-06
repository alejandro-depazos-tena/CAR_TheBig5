#!/usr/bin/env bash
# Compila y ejecuta todos los experimentos del Laboratorio 1 y guarda las
# salidas en bruto en resultados/. Uso, desde Laboratorio1:
#     bash scripts/ejecutar_todo.sh
# Conviene lanzarlo con el portatil enchufado y sin otros programas abiertos.
set -euo pipefail
cd "$(dirname "$0")/.."

echo "== Ejercicio 1 (compilacion, entorno, tiempos, pruebas y grafica)"
bash scripts/ejecutar_ej1.sh

echo "== Ejercicios 2 a 6 (compilacion, medidas, graficas y registro de salidas)"
bash scripts/ejecutar_ej2_en_adelante.sh
echo "Hecho: $(date '+%F %T')"
