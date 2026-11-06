# Laboratorio 1 · TermoHPC I — Simulación térmica paralela con OpenMP

```text
Laboratorio1/
├── src/
│   ├── ejercicio1/p1_ej1.c       # Versión secuencial de referencia (+ grafica_ej1.py)
│   ├── ejercicio2/p1_ej2.c       # Paralelización, verificación y fallo in situ
│   ├── ejercicio3/p1_ej3.c       # Escalabilidad fuerte, techo de cálculo y coste de región
│   ├── ejercicio3/p1_seq.c       # T1 secuencial compilada SIN -fopenmp (speedup absoluto)
│   ├── ejercicio4/p1_ej4.c       # Escalabilidad débil (N = N1·√p), verificada en cada N
│   ├── ejercicio5/p1_ej5.c       # Planificaciones (schedule(runtime))
│   ├── ejercicio6/p1_ej6.c       # Evidencias + iteraciones de optimización, verificadas
│   ├── ejercicio6/p1_stream.c    # Techo de ancho de banda (STREAM triad)
│   ├── graficas_lab1.py          # Gráficas de los ejercicios 3 a 6
│   └── analisis_lab1.py          # Tablas derivadas de los CSV (texto y Markdown)
├── scripts/
│   ├── ejecutar_ej2_en_adelante.sh  # Ejercicios 2 a 6: compila, mide, grafica y registra
│   ├── ejecutar_ej3.sh              # Solo el Ejercicio 3 (las dos mallas en la misma sesión)
│   └── ejecutar_todo.sh             # Ejercicio 1 + llamada al guion de los ejercicios 2 a 6
├── bin/                          # Ejecutables
├── resultados/
│   ├── ejercicio1/               # Ejercicio 1
│   ├── ejercicio2 … ejercicio6/  # CSV en bruto y gráficas de los ejercicios 2 a 6
│   ├── salidas_ejercicios_2_en_adelante.txt  # TODAS las salidas de consola desde el Ej. 2
│   ├── ultra5-225U_2026-10-15/   # Medidas anteriores de los Ej. 2–6 en otra máquina (archivo)
│   └── descartadas_2026-10-15/   # Medidas descartadas (CPU lenta)
└── docs/
    ├── ejercicios_2_a_6.md       # Condiciones, mapa requisito → evidencia y resultados clave
    └── tablas_ej2_ej6.md         # Tablas derivadas, generadas desde los CSV
```

## Ejercicios 2 a 6

Desde la raíz del repositorio (o desde cualquier otro directorio):

```console
bash Laboratorio1/scripts/ejecutar_ej2_en_adelante.sh        # 5 repeticiones por punto
bash Laboratorio1/scripts/ejecutar_ej2_en_adelante.sh 4      # u otro número
```

El guion:

- Compila en `bin/` con `gcc -Wall -Wextra -O2 -std=c11 -fopenmp` (y `p1_seq.c`
  sin `-fopenmp`).
- Ejecuta en una sola sesión todos los experimentos de los ejercicios 2 a 6, con
  las repeticiones en rondas intercaladas.
- Verifica cada variante frente a la versión secuencial.
- Genera las gráficas y `docs/tablas_ej2_ej6.md`.
- Deja **todas** las salidas de consola, con la orden exacta que las produce, en
  `resultados/salidas_ejercicios_2_en_adelante.txt`.
- Termina con un resumen de validaciones y se detiene si alguna orden falla.

Dura entre 25 y 40 minutos. Conviene tener el portátil enchufado, el modo de
energía en máximo rendimiento y no usar el equipo mientras mide. Las gráficas
necesitan matplotlib (`sudo apt install python3-matplotlib`).

Cada programa también se puede lanzar por separado; los parámetros van por línea
de órdenes (ver la cabecera de cada `.c`). Todas las tablas de tiempos usan el
**mínimo** de las repeticiones, con la inicialización fuera del cronómetro.
`docs/ejercicios_2_a_6.md` explica las condiciones de medida, dónde está la
evidencia de cada requisito del enunciado y los resultados clave.

## Ejercicio 1 y `ejecutar_todo.sh`

`bash scripts/ejecutar_todo.sh` repite también el Ejercicio 1 (sobrescribe sus
resultados y `resultados/entorno.txt`) y después llama al guion de los ejercicios
2 a 6. Los datos de `resultados/ejercicio1/i7-1355U_2026-10-14/` son de la primera
medida en otra máquina y no se mezclan con el resto.

## Máquinas

- Ejercicio 1 (`resultados/ejercicio1/`, entorno en `resultados/entorno.txt`):
  Intel Core Ultra 5 225U. Primera medida en Intel Core i7-1355U en
  `resultados/ejercicio1/i7-1355U_2026-10-14/`.
- Ejercicios 2 a 6 (`resultados/ejercicio2` … `ejercicio6`): Intel Core i7-1355U
  (10 núcleos, 12 hilos lógicos) con Ubuntu 24.04 sobre WSL2, medidos el
  2026-11-06. El entorno completo está al principio de
  `resultados/salidas_ejercicios_2_en_adelante.txt`.
- Las medidas anteriores de los ejercicios 2 a 6 (Intel Core Ultra 5 225U,
  2026-10-15) se conservan en `resultados/ultra5-225U_2026-10-15/`; su `LEEME.md`
  explica por qué no se mezclan.
