# Laboratorio 1 · Ejercicios 2 a 6: qué se ha medido, dónde está la evidencia y qué dicen los datos

Este documento acompaña a las salidas en bruto. No sustituye a la memoria: recoge
las condiciones de medida, dónde está la evidencia de cada requisito del enunciado
y los números clave. Todos los números salen de los CSV de `resultados/` y de
`resultados/salidas_ejercicios_2_en_adelante.txt`. Las tablas completas, generadas
desde los CSV sin intervención manual, están en `docs/tablas_ej2_ej6.md`.

El código del Ejercicio 1 (`src/ejercicio1/`) no ha cambiado. Sus resultados
(`resultados/ejercicio1/`) se volvieron a medir en esta misma máquina el
2026-11-06 con `scripts/ejecutar_ej1.sh`, para que todo el laboratorio esté medido
en el mismo equipo. La medida anterior, hecha en otra máquina, está archivada en
`resultados/ultra5-225U_2026-10-15/`.

## 1. Cómo se reproduce

```console
bash Laboratorio1/scripts/ejecutar_ej2_en_adelante.sh      # 5 repeticiones, ~25-40 min
```

Una sola orden compila (avisos activados), mide los ejercicios 2 a 6 en la misma
sesión, verifica cada variante, genera las gráficas y las tablas, y deja **todas**
las salidas de consola con su orden exacta en
`resultados/salidas_ejercicios_2_en_adelante.txt`, organizadas por ejercicio y por
requisito del enunciado. Si se cambia solo el análisis,
`python3 src/analisis_lab1.py refrescar-registro` recalcula las tablas derivadas
del registro sobre los mismos CSV (y lo deja anotado al final del registro).

## 2. Condiciones de medida (declararlas en la memoria)

| Aspecto | Valor |
|---|---|
| Máquina | Portátil con Intel Core i7-1355U (13.ª gen.): 10 núcleos físicos (2 P con dos hilos + 8 E), 12 hilos lógicos, L3 de 12 MB, CPU de 15 W |
| Entorno | Ubuntu 24.04 sobre WSL2 (Windows 11): Linux ve 12 vCPU con una topología virtual 6×2 que **no** corresponde a la real |
| Memoria vista por WSL2 | 7,6 GiB |
| Compilador | gcc 13.3.0, `-Wall -Wextra -O2 -std=c11 -fopenmp` (sin avisos) |
| Alimentación | Conectado a la corriente; plan «Equilibrado» (el modo de energía «Máximo rendimiento» no constaba activo en el registro de Windows) |
| Fecha | 2026-11-06, una sola sesión (de 17:34 a 17:59, 1500 s) |
| Repeticiones | 5 por punto, en **rondas intercaladas**: cada ronda mide una vez todas las configuraciones de la tabla |
| Criterio | **Mínimo** de las rondas; la columna `tiempo_peor_s` da el máximo (dispersión de cada punto) |
| Cronómetro | `omp_get_wtime()`; inicialización, reserva y checksum fuera del cronómetro |
| T1 (speedup absoluto) | Versión secuencial sin pragmas, medida en cada ronda del mismo programa. Además, `p1_seq` la mide compilada **sin `-fopenmp`** |
| Speedup relativo | Columna `speedup_rel` del Ej. 3 (frente a la versión paralela con 1 hilo), etiquetada aparte |

Por qué rondas intercaladas: en una primera ejecución completa (con las
repeticiones seguidas) la T1 de un hilo osciló entre ≈1,55 s y ≈2,3 s según el
momento, y algunos puntos quedaron afectados por completo. Este portátil cambia de
frecuencia durante minutos (límite de potencia de 15 W, núcleos P y E). Con las
repeticiones intercaladas, cada punto tiene una medida en cada ronda. Esa primera
ejecución se descartó (además, su registro tenía un fallo de codificación UTF-8) y
no se conserva. La dispersión de la máquina se mide de forma explícita: 10
ejecuciones idénticas de la secuencial con N=2048 van de 1,710 s a 2,092 s
(22,3 %; `t1_sin_openmp_2048.csv`).

Consecuencias del entorno que hay que tener presentes al leer cualquier tabla:

- **12 hilos = todas las CPU lógicas que ve WSL2**, y Windows sigue necesitando
  CPU. Si en mitad de un paso Windows le quita una vCPU a la máquina virtual, la
  barrera espera a ese hilo. Por eso 12 hilos suele salir *peor* que 8. Los
  ejercicios 5 y 6 se han medido además con 8 hilos (medida complementaria,
  marcada como tal).
- **16, 20 y 24 hilos sobresuscriben** las 12 vCPU: hay más hilos que CPU.
- **La afinidad** (`OMP_PROC_BIND`/`OMP_PLACES`) fija hilos a vCPU, pero Windows no
  fija las vCPU a núcleos físicos.

## 3. Mapa requisito → evidencia

`[§X]` remite al apartado del registro `resultados/salidas_ejercicios_2_en_adelante.txt`
cuyo título empieza por esa etiqueta.

| Ej. | Requisito del enunciado | Evidencia |
|---|---|---|
| 2 | [PROG] `parallel for` en el bucle de filas, `default(none)`, `reduction(max:r)` | `src/ejercicio2/p1_ej2.c`, función `step_omp` (comentario con el papel de cada variable) |
| 2 | [PROG] Función de verificación: norma del máximo + checksum | `p1_ej2.c`: `difference`, `checksum`, `checks` (bordes y rango [0,100]) |
| 2 | [PROG] Tabla de verificación con 1, 2, 4, 8, 12, 16, 20 y 24 hilos | `resultados/ejercicio2/verificacion_2048.csv` (y `_256`) · [§ Ej2 [PROG] Tabla…] |
| 2 | [PROG] Fallo provocado in situ: rechazado y dependiente de los hilos | filas `in_situ` de los dos CSV (dos ejecuciones por recuento) · [§ Ej2 · Resumen] |
| 3 | [PROG] Barrido 1–24 hilos, N=2048, 300 pasos, `omp_set_dynamic(0)` + hilos reales | `resultados/ejercicio3/fuerte_2048.csv` (columna `hilos_reales`) |
| 3 | [PROG] Tiempo, speedup absoluto (y relativo etiquetado), eficiencia, condiciones | mismo CSV (línea `#`) · [§ Ej3 · Tablas derivadas] |
| 3 | [PROG] T1 de la secuencial compilada sin `-fopenmp` | `t1_sin_openmp_2048.csv`, `t1_sin_openmp_256.csv` (`ldd` confirma que no enlaza libgomp, [§ 1. Compilación]) |
| 3 | [PROG] Techo de cálculo propio (tipo IV.C) en la gráfica, con la diagonal | columnas `tiempo_techo_s`/`techo_speedup`; `resultados/ejercicio3/grafica_fuerte.png` |
| 3 | [PROG] Malla pequeña con trabajo comparable (256² × 19200 = 2048² × 300) | `fuerte_256.csv`; las dos curvas en `grafica_fuerte.png` y `grafica_eficiencia.png` |
| 3 | [RAZ] Tiempo por paso frente al coste de una barrera; Amdahl | columnas `s_por_paso` y `region_vacia_s`; Karp-Flatt y ajuste de Amdahl en [§ Ej3 · Tablas derivadas] |
| 4 | [PROG] N(p)=round(1024·√p), N exactos, tabla, gráfica con la plana ideal, eficiencia débil | `resultados/ejercicio4/debil.csv`, `grafica_debil.png` · [§ Ej4] |
| 4 | Verificación de cada N contra la secuencial con la misma N | columnas `max_diff`, `dif_checksum_rel`, `correcto` de `debil.csv` |
| 4 | [RAZ] Efecto de la memoria (hasta 402,7 MB) | columnas `ns_celda_paso_seq` y `T mín. memoria` en [§ Ej4 · Tablas derivadas] |
| 5 | [PROG] 12 hilos: static, static 1/4/16/64, dynamic 1/4/16/64, guided; tabla y gráfica de barras | `resultados/ejercicio5/planificaciones.csv`, `grafica_planificaciones.png` |
| 5 | [PROG] Todas dan el mismo campo | columnas `max_diff`/`correcto` (verificación en todas las rondas) |
| 5 | Complementaria con 8 hilos | `planificaciones_8hilos.csv` (barras rayadas de la gráfica) |
| 6 | [PROG] Punto de partida (Ej. 2) con tiempo y speedup | fila `base` de `resultados/ejercicio6/variantes.csv` |
| 6 | [PROG] Iteraciones 1-4 con evidencia previa, cambio único, nueva medida y verificación | `src/ejercicio6/p1_ej6.c` (cabecera y una función por variante); líneas `# E1`/`# E2` y columnas `parte_de`, `mejora_vs_anterior`, `correcto` de `variantes.csv`; `vectorizacion.txt` (evidencia E3) |
| 6 | [PROG] Tabla resumen con mejora respecto de la anterior | `variantes.csv` · [§ Ej6 · Tablas derivadas]; `grafica_variantes.png` |
| 6 | [RAZ] Ancho de banda conseguido frente al de la máquina | `resultados/ejercicio6/stream.csv`, columnas `GB_s_*` · `grafica_ancho_banda.png` |
| 6 | Afinidad (candidato del enunciado) | `variantes_afinidad_close.csv`, `variantes_afinidad_spread.csv` |
| — | Resumen de todas las verificaciones | [§ Resumen final de validaciones]: todas PASAN |

## 4. Resultados clave

### Ejercicio 2 · verificación (tolerancia 1e-9, declarada en el código antes de medir)

- Jacobi OpenMP con N=2048 y N=256 (300 pasos): **8/8 recuentos de hilos dan el
  campo idéntico bit a bit** al secuencial (diferencia máxima 0). El checksum
  (1890947.6669302983 con N=2048) y el residuo coinciden exactamente, y los hilos
  reales coinciden con los pedidos en los 8 casos. El residuo coincide aunque es
  una reducción porque el máximo no depende del orden en que se combinan los
  parciales. La suma sí dependería, pero el checksum se calcula en secuencial.
- Fallo provocado (un solo buffer): **rechazado en 16/16 ejecuciones**, con
  diferencia máxima de 17,09 grados frente al secuencial.
  - Con N=256 el resultado cambia ya con 2 hilos.
  - Con N=2048 los checksums de 1 a 12 hilos coinciden y cambian a partir de 16.
    Con 16 hilos la primera frontera entre franjas está en la fila ≈128, la
    misma altura a la que N=256 ya cambia con 2 hilos. Lo coherente con estos
    datos es que, con 12 hilos o menos, las fronteras caen en filas donde el
    campo vale tan poco que la diferencia no llega a mover el checksum.
  - Con los mismos hilos, las dos ejecuciones coinciden en 5/8 (N=2048) y 3/8
    (N=256) de los casos. La firma principal es la dependencia del número de
    hilos, pero con muchos hilos aparece además la carrera: el resultado depende
    de qué hilo llega antes a la fila de la frontera.

### Ejercicio 3 · escalabilidad fuerte (5 rondas, mínimo)

| hilos | 1 | 2 | 4 | 8 | 12 | 16 | 20 | 24 |
|---|---|---|---|---|---|---|---|---|
| N=2048: T (s) | 1,999 | 1,346 | 1,216 | **0,964** | 1,419 | 1,207 | 1,155 | 1,159 |
| N=2048: S abs | 0,98 | 1,46 | 1,62 | **2,04** | 1,39 | 1,63 | 1,70 | 1,70 |
| techo de cálculo medido | 1,00 | 1,93 | 3,38 | 6,81 | 8,36 | 8,80 | 8,19 | 8,61 |
| N=256 (19200 pasos): S abs | 0,98 | 1,85 | 3,07 | **4,57** | 0,035 | 0,13 | 0,11 | 0,10 |

- T1 (N=2048) = 1,968 s; prueba de humo T(1 hilo)/T1 = 1,016. El binario sin
  `-fopenmp` dio un mínimo de 1,710 s (mediana 1,865 s). La diferencia está dentro
  de la dispersión de la máquina, pero mueve los speedups absolutos: con esa T1,
  S(8) sería 1,77. Hay que declararlo.
- **La cifra que no depende de T1**: con 8 hilos el programa mueve 31,3 GB/s
  (modelo de 24 B/celda), el **98,8 %** de lo que da STREAM en esta máquina
  (31,6 GB/s con el modelo equivalente de 32 B). El techo de speedup que permite la
  memoria es 2,06 (STREAM / ancho de banda de la secuencial), y S(8) = 2,04.
- STREAM: 1 hilo ya consigue 24,3 de 31,6 GB/s (77 %); con 4 hilos está saturado.
- Karp-Flatt no es constante (0,37; 0,49; 0,42 con 2, 4 y 8 hilos; 0,70 con 12).
  El ajuste de Amdahl con 2, 4 y 8 hilos da f = 0,44 (límite 2,30), pero no
  describe la curva: no hay una «parte serie» de ese tamaño; lo que se agota es el
  ancho de banda.
- Malla pequeña: N=256 (dos buffers de 1 MB) cabe en caché y escala hasta 4,57 con
  8 hilos, más que la grande. Con 12 o más hilos se hunde.
  - Con 16–24 hilos, el tiempo por paso (0,44–0,57 ms) ≈ el coste de un paso
    vacío (0,44–0,58 ms): el paso es solo sincronización.
  - Con 12 hilos el paso cuesta 1,6 ms, aunque el paso vacío mínimo fue de 10 µs.
    Es el efecto de ocupar las 12 vCPU, y fue sistemático en las 5 rondas
    (30,6–47,6 s por ejecución).

### Ejercicio 4 · escalabilidad débil (N exactos: 1024, 1448, 2048, 2896, 3547, 4096, 4579, 5017)

- Eficiencia débil: 100 %, 79,5 %, 25,4 %, 19,7 %, 11,3 %, 9,1 %, 7,5 % y 6,2 %.
  Por el redondeo de N, las celdas por hilo se desvían como mucho un 0,039 %.
- Con p ≥ 2 el tiempo medido está entre **1,01 y 1,18 veces el mínimo que impone
  el ancho de banda** (24 B por celda / 31,6 GB/s); la excepción es p = 4, con
  1,58. El trabajo total crece con p, pero el bus no: el
  tiempo crece casi en proporción a los bytes totales (línea marrón de
  `grafica_debil.png`).
- Memoria (hasta 402,7 MB con N=5017): el coste secuencial por celda y paso está
  entre 1,16 y 1,40 ns para todas las N, sin tendencia con el tamaño, salvo
  **N=4096 (1,93 ns)**, que en esta sesión salió lento en las 5 rondas
  (9,7–13,4 s). No es un efecto del tamaño total: N=4579 y N=5017, mayores,
  vuelven a 1,19 y 1,30 ns. La causa no queda establecida con los datos
  disponibles:
  - La medida del Ejercicio 1 del 2026-10-14 en esta máquina
    (`resultados/ejercicio1/i7-1355U_2026-10-14/`) también fue más lenta con
    N=4096: 3,66 ns frente a 2,22 ns con N=2048.
  - La repetición del Ejercicio 1 del 2026-11-06 (`resultados/ejercicio1/`) no
    lo muestra: 2,16 ns frente a 2,22 ns. Ese programa calcula el paso y el
    residuo en dos recorridos.
  - Si la memoria lo comenta, debe presentarlo como una observación abierta.

### Ejercicio 5 · planificaciones (N=2048, 300 pasos)

| planificación | static | static,1 | static,4 | static,16 | static,64 | dynamic,1 | dynamic,4 | dynamic,16 | dynamic,64 | guided |
|---|---|---|---|---|---|---|---|---|---|---|
| 12 hilos, T (s) | 1,647 | 1,777 | 1,669 | 1,602 | 1,517 | 1,528 | 1,497 | **1,439** | 1,511 | 1,496 |
| 8 hilos, T (s) | 1,123 | 1,137 | 1,100 | 1,068 | 1,102 | 1,081 | 1,006 | 1,022 | 1,047 | **0,981** |

- Las 20 configuraciones dan el campo secuencial (diferencia máxima 0).
- Con 12 hilos todas las planificaciones quedan cerca de S ≈ 1 y la dispersión
  llega a 1,89 (peor/mejor). Con 8 hilos la dispersión es ≤ 1,16 y el orden es
  más fiable: guided y dynamic,4 son ≈10–13 % más rápidas que static sin trozo.
- Separación de los dos efectos con el mismo trozo de 1 fila:
  - T(static,1)/T(static) = 1,08 con 12 hilos y 1,01 con 8: entrelazar filas
    cuesta poco.
  - T(dynamic,1)/T(static,1) = 0,86 con 12 hilos y 0,95 con 8: repartir bajo
    demanda no solo no cuesta, compensa.
  - Una fila tiene 2046 celdas (16 KB). Con 8 hilos cada hilo tarda ≈13 µs por
    fila, mucho más que pedir un trozo, y recorre una zona contigua de memoria.
  - En esta máquina, dynamic con trozos pequeños **no** sale especialmente mal,
    en contra de lo que anticipa el enunciado para la suya. La memoria tiene que
    contarlo con estos datos.

### Ejercicio 6 · optimización (N=2048, 300 pasos, 12 hilos; mejora = T(anterior)/T)

| variante | parte de | cambio único | T (s) | mejora vs anterior | ¿confirma? |
|---|---|---|---|---|---|
| base | — | versión del Ej. 2 | 1,431 | — | — |
| it1_region | base | una sola región paralela | 1,485 | 0,96 | no |
| it2_dynamic | base | `schedule(dynamic,4)` | 1,435 | 1,00 (12 h) / 1,05 (8 h) | no con 12 h; leve con 8 h |
| it3_simd | it2 | `#pragma omp simd` | 1,566 | 0,92 | no (empeora) |
| it4_control | it3 | lo mismo con SSE2 a mano, escritura normal | 1,477 | 1,06 | control |
| it4_nt | it4_control | escritura **no temporal** | **1,154** | **1,28** | **sí** |
| alt_collapse2 | base | `collapse(2)` | 1,458 | 0,98 | no |
| alt_residuo10 | base | residuo cada 10 pasos | 1,460 | 0,98 | no |

- Evidencias medidas con la base (12 hilos):
  - E1: un paso vacío cuesta 2,9 µs, el 0,06 % de un paso. Ya anticipaba que la
    región única no podía ganar nada, y además añade una barrera por paso (la del
    `single`).
  - E2 (static): el trabajo por hilo va de 2,17 a 3,05 ms, pero el paso dura
    7,64 ms; en media, un hilo pasa el 65 % del paso sin trabajar.
  - E2 (dynamic,4): iguala el trabajo (1,04) y reduce el paso a 5,69 ms, pero
    sigue un 45 % sin trabajar. La mayor parte de la espera no es desequilibrio
    entre hilos sino hilos que no están corriendo (las 12 vCPU ocupadas). Con 8
    hilos, dynamic baja la fracción sin trabajar del 22 % al 1,9 %.
  - E3: gcc no vectoriza el bucle interior de la base (`couldn't vectorize loop`,
    línea 120 de `p1_ej6.c`), y con `omp simd` sí (`loop vectorized`, línea 189).
    Vectorizar no mejora: el programa espera a la memoria, no a la aritmética.
  - E4: la base usa el 66,6 % del ancho de banda real de STREAM con 12 hilos y el
    96,1 % con 8.
- Iteración 4, la que funciona: con escritura normal el procesador lee cada línea
  antes de escribirla (24 B/celda); la no temporal la escribe sin leerla
  (16 B/celda).
  - Mejora 1,28 con 12 hilos y 1,26 con cada una de las dos afinidades.
  - Con 8 hilos mejora 1,44 (0,637 s, S = 2,62) y mueve 31,6 GB/s reales, el
    99,8 % del máximo de STREAM.
  - Es la única optimización que reduce tráfico de memoria, y es la única que
    funciona.
- Afinidad: `close/cores` y `spread/threads` no dan un efecto consistente. Frente
  a la ejecución sin afinidad, las relaciones van de 0,88 a 1,03 según la variante
  (en la base, 0,98 con close y 0,88 con spread), dentro de la dispersión medida.
  Bajo WSL2 los lugares son vCPU, no núcleos físicos.
- Decisión que hay que declarar: la cadena it2→it3→it4 se fijó antes de la medida
  definitiva, porque en la exploración `dynamic` ganaba un 17 % con 12 hilos. En
  la medida definitiva su efecto con 12 hilos quedó dentro de la dispersión
  (1,00; 0,97 con close; 1,13 con spread). Las iteraciones 3 y 4 se comparan
  siempre con su variante de partida, así que esa decisión no contamina su mejora.

## 5. Lo que queda para la memoria del grupo

Los [RAZ] del enunciado no están redactados aquí. Hay que razonarlos con estos
datos y en la defensa:

- Elección del bucle de filas y de cada cláusula de datos.
- Barreras implícitas.
- Tolerancia de 1e-9.
- Lectura por tramos de la curva frente al techo.
- Malla pequeña frente a grande.
- Límites de Amdahl.
- La raíz cuadrada de la escalabilidad débil y la comparación débil/fuerte.
- Elección de planificación y cuándo cambiaría.
- El argumento del ancho de banda.

El enunciado exige además que la memoria sea elaboración propia del grupo y que el
uso de herramientas de IA sea transparente.
