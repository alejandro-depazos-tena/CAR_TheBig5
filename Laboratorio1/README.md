# Laboratorio 1 · TermoHPC I — Simulación térmica paralela con OpenMP

```text
Laboratorio1/
├── src/
│   ├── ejercicio1/p1_ej1.c       # Versión secuencial de referencia (+ grafica_ej1.py)
│   ├── ejercicio2/p1_ej2.c       # Paralelización, verificación y fallo in situ
│   ├── ejercicio3/p1_ej3.c       # Escalabilidad fuerte, techo de cálculo y coste de región
│   ├── ejercicio4/p1_ej4.c       # Escalabilidad débil (N = N1·√p)
│   ├── ejercicio5/p1_ej5.c       # Planificaciones (schedule(runtime))
│   ├── ejercicio6/p1_ej6.c       # Variantes de optimización, verificadas
│   ├── ejercicio6/p1_stream.c    # Techo de ancho de banda (STREAM triad)
│   └── graficas_lab1.py          # Gráficas de los ejercicios 3 a 6
├── scripts/ejecutar_todo.sh      # Compila, ejecuta todos los barridos y genera las gráficas
├── resultados/                   # Salidas en bruto (CSV/TXT), entorno y gráficas
└── docs/                         # Resumen de resultados para la memoria
```

## Reproducir todas las tablas

Desde `Laboratorio1`:

```console
bash scripts/ejecutar_todo.sh
```

Compila con `gcc -Wall -Wextra -O2 -std=c11 -fopenmp` (en `bin/`), guarda el
entorno en `resultados/entorno.txt` y ejecuta todos los experimentos.
Las gráficas necesitan matplotlib (`sudo apt install python3-matplotlib`).

Cada programa también se puede lanzar por separado; los parámetros van por
línea de órdenes (ver la cabecera de cada `.c`).

Los datos de `resultados/ejercicio1/i7-1355U_2026-10-14/` son de la primera
medida en otra máquina y no se mezclan con el resto.
