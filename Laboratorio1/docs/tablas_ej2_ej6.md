# Laboratorio 1 · Tablas de los ejercicios 2 a 6

Generado automáticamente por `src/analisis_lab1.py markdown` a partir de los CSV de `resultados/`. No editar a mano: se regenera con `scripts/ejecutar_ej2_en_adelante.sh`.


### Ejercicio 2 · verificación numérica (tolerancia 1e-9, declarada antes de medir)


#### N=2048, 300 pasos (fichero verificacion_2048.csv)

Jacobi OpenMP: 8/8 recuentos de hilos pasan la verificación; 8/8 salen idénticos bit a bit; hilos reales = pedidos en 8/8. Mayor diferencia celda a celda: 0.000e+00; mayor diferencia relativa del checksum: 0.000e+00; residuo idéntico al secuencial en 8/8.

Fallo provocado (in situ): rechazado en 16/16 ejecuciones; 4 checksums distintos entre los 8 recuentos de hilos; las dos ejecuciones con los mismos hilos coinciden en 5/8.

| hilos | checksum in situ (1) | checksum in situ (2) | 1 = 2 | dif. rel. vs secuencial | max_diff | cambia vs p anterior |
|---:|---:|---:|---:|---:|---:|---:|
| 1 | 2757088.6260519819 | 2757088.6260519819 | sí | 4.5805e-01 | 1.709e+01 | - |
| 2 | 2757088.6260519819 | 2757088.6260519819 | sí | 4.5805e-01 | 1.709e+01 | no |
| 4 | 2757088.6260519819 | 2757088.6260519819 | sí | 4.5805e-01 | 1.709e+01 | no |
| 8 | 2757088.6260519819 | 2757088.6260519819 | sí | 4.5805e-01 | 1.709e+01 | no |
| 12 | 2757088.6260519819 | 2757088.6260519819 | sí | 4.5805e-01 | 1.709e+01 | no |
| 16 | 2757088.6260519517 | 2757088.6260519428 | no | 4.5805e-01 | 1.709e+01 | sí |
| 20 | 2757088.6249166471 | 2757088.6248872746 | no | 4.5805e-01 | 1.709e+01 | sí |
| 24 | 2757088.5351495906 | 2757088.5446903012 | no | 4.5805e-01 | 1.709e+01 | sí |


#### N=256, 300 pasos (fichero verificacion_256.csv)

Jacobi OpenMP: 8/8 recuentos de hilos pasan la verificación; 8/8 salen idénticos bit a bit; hilos reales = pedidos en 8/8. Mayor diferencia celda a celda: 0.000e+00; mayor diferencia relativa del checksum: 0.000e+00; residuo idéntico al secuencial en 8/8.

Fallo provocado (in situ): rechazado en 16/16 ejecuciones; 8 checksums distintos entre los 8 recuentos de hilos; las dos ejecuciones con los mismos hilos coinciden en 3/8.

| hilos | checksum in situ (1) | checksum in situ (2) | 1 = 2 | dif. rel. vs secuencial | max_diff | cambia vs p anterior |
|---:|---:|---:|---:|---:|---:|---:|
| 1 | 326159.9175717709 | 326159.9175717709 | sí | 4.3551e-01 | 1.709e+01 | - |
| 2 | 326159.91757166851 | 326159.91757166851 | sí | 4.3551e-01 | 1.709e+01 | sí |
| 4 | 326158.94022618054 | 326158.91335287696 | no | 4.3551e-01 | 1.709e+01 | sí |
| 8 | 326061.18153463252 | 326061.18153463252 | sí | 4.3508e-01 | 1.709e+01 | sí |
| 12 | 325717.67117864446 | 325757.41343242401 | no | 4.3356e-01 | 1.706e+01 | sí |
| 16 | 325619.74589581066 | 325678.13661242684 | no | 4.3313e-01 | 1.705e+01 | sí |
| 20 | 325541.86367609765 | 325496.58197262208 | no | 4.3279e-01 | 1.700e+01 | sí |
| 24 | 325546.17134625604 | 325353.48056008614 | no | 4.3281e-01 | 1.701e+01 | sí |


### Ejercicio 3 · escalabilidad fuerte


#### N=2048, 300 pasos, mínimo de 5 repeticiones (fichero fuerte_2048.csv)

T1 = 1.9677 s (versión secuencial sin pragmas en el mismo proceso, mínimo de las rondas; la peor ronda dio 2.3433 s). Binario compilado SIN -fopenmp (p1_seq): mínimo 1.7101 s, mediana 1.8650 s, máximo 2.0918 s (dispersión máx/mín 22.3%); la T1 de la tabla difiere de su mínimo un +15.1 %.

Prueba de humo (V.I): T(1 hilo)/T1 = 1.016.

| hilos | reales | T (s) | peor/mejor | S abs | S rel | E abs | techo cálc. | S/techo | Karp-Flatt | ms/paso | paso vacío (µs) | vacío/paso | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1 | 1.9990 | 1.10 | 0.984 | 1.000 | 98.4 % | 1.00 | 98.4 % | - | 6.6635 | 0.30 | 0.0 % | sí |
| 2 | 2 | 1.3464 | 1.08 | 1.462 | 1.485 | 73.1 % | 1.93 | 75.8 % | 0.368 | 4.4880 | 0.55 | 0.0 % | sí |
| 4 | 4 | 1.2160 | 1.04 | 1.618 | 1.644 | 40.5 % | 3.38 | 47.9 % | 0.491 | 4.0533 | 0.93 | 0.0 % | sí |
| 8 | 8 | 0.9640 | 1.10 | 2.041 | 2.074 | 25.5 % | 6.81 | 30.0 % | 0.417 | 3.2133 | 1.16 | 0.0 % | sí |
| 12 | 12 | 1.4193 | 1.28 | 1.386 | 1.409 | 11.6 % | 8.36 | 16.6 % | 0.696 | 4.7309 | 1.90 | 0.0 % | sí |
| 16 | 16 | 1.2070 | 1.13 | 1.630 | 1.656 | 10.2 % | 8.80 | 18.5 % | 0.588 | 4.0232 | 459.84 | 11.4 % | sí |
| 20 | 20 | 1.1552 | 1.10 | 1.703 | 1.730 | 8.5 % | 8.19 | 20.8 % | 0.565 | 3.8505 | 528.60 | 13.7 % | sí |
| 24 | 24 | 1.1588 | 1.07 | 1.698 | 1.725 | 7.1 % | 8.61 | 19.7 % | 0.571 | 3.8628 | 544.11 | 14.1 % | sí |

Speedup absoluto máximo: 2.041 con 8 hilos. Hilos lógicos de la máquina: 12 (por encima hay sobresuscripción).

Amdahl: fracción serie ajustada por mínimos cuadrados con 2, 4, 8 hilos (menos que las 12 CPU lógicas): f = 0.436 (límite 1/f = 2.30). Compárese con la columna Karp-Flatt: si Amdahl describiera la curva, sería constante.

| hilos | S medido | S Amdahl con f ajustada |
|---:|---:|---:|
| 1 | 0.984 | 1.000 |
| 2 | 1.462 | 1.393 |
| 4 | 1.618 | 1.734 |
| 8 | 2.041 | 1.976 |
| 12 | 1.386 | 2.072 |
| 16 | 1.630 | 2.124 |
| 20 | 1.703 | 2.156 |
| 24 | 1.698 | 2.178 |

Techo de ancho de banda: la secuencial mueve 10.21 GB/s (16 B/celda) o 15.32 GB/s (24 B/celda, con la lectura previa de la línea escrita). STREAM: 1 hilo 18.23/24.31 GB/s, máximo 23.73/31.63 GB/s (modelos 24/32 B) con 8 hilos. Speedup máximo que permite la memoria si el programa estuviera limitado solo por ella: 31.63/15.32 = 2.06 (modelos con lectura previa) o 2.32 (modelos sin ella).

| hilos | GB/s (16 B) | % STREAM 24 B | GB/s (24 B) | % STREAM 32 B |
|---:|---:|---:|---:|---:|
| 1 | 10.05 | 42.4 % | 15.08 | 47.7 % |
| 2 | 14.92 | 62.9 % | 22.39 | 70.8 % |
| 4 | 16.52 | 69.6 % | 24.79 | 78.4 % |
| 8 | 20.84 | 87.8 % | 31.27 | 98.8 % |
| 12 | 14.16 | 59.7 % | 21.24 | 67.1 % |
| 16 | 16.65 | 70.2 % | 24.97 | 79.0 % |
| 20 | 17.39 | 73.3 % | 26.09 | 82.5 % |
| 24 | 17.34 | 73.1 % | 26.01 | 82.2 % |


#### N=256, 19200 pasos, mínimo de 5 repeticiones (fichero fuerte_256.csv)

T1 = 1.0686 s (versión secuencial sin pragmas en el mismo proceso, mínimo de las rondas; la peor ronda dio 1.4398 s). Binario compilado SIN -fopenmp (p1_seq): mínimo 1.1045 s, mediana 1.3165 s, máximo 1.5331 s (dispersión máx/mín 38.8%); la T1 de la tabla difiere de su mínimo un -3.2 %.

Prueba de humo (V.I): T(1 hilo)/T1 = 1.016.

| hilos | reales | T (s) | peor/mejor | S abs | S rel | E abs | techo cálc. | S/techo | Karp-Flatt | ms/paso | paso vacío (µs) | vacío/paso | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1 | 1.0854 | 1.48 | 0.985 | 1.000 | 98.5 % | 1.00 | 98.5 % | - | 0.0565 | 0.14 | 0.2 % | sí |
| 2 | 2 | 0.5771 | 1.89 | 1.852 | 1.881 | 92.6 % | 1.90 | 97.7 % | 0.080 | 0.0301 | 0.37 | 1.2 % | sí |
| 4 | 4 | 0.3483 | 2.17 | 3.069 | 3.117 | 76.7 % | 3.52 | 87.1 % | 0.101 | 0.0181 | 0.70 | 3.9 % | sí |
| 8 | 8 | 0.2337 | 1.75 | 4.573 | 4.645 | 57.2 % | 6.10 | 75.0 % | 0.107 | 0.0122 | 1.06 | 8.7 % | sí |
| 12 | 12 | 30.6380 | 1.55 | 0.035 | 0.035 | 0.3 % | 7.32 | 0.5 % | 31.167 | 1.5957 | 10.38 | 0.7 % | sí |
| 16 | 16 | 8.4575 | 1.17 | 0.126 | 0.128 | 0.8 % | 6.12 | 2.1 % | 8.372 | 0.4405 | 437.87 | 99.4 % | sí |
| 20 | 20 | 9.8284 | 1.23 | 0.109 | 0.110 | 0.5 % | 6.58 | 1.7 % | 9.631 | 0.5119 | 510.46 | 99.7 % | sí |
| 24 | 24 | 10.8725 | 1.29 | 0.098 | 0.100 | 0.4 % | 6.79 | 1.4 % | 10.572 | 0.5663 | 581.76 | 102.7 % | sí |

Speedup absoluto máximo: 4.573 con 8 hilos. Hilos lógicos de la máquina: 12 (por encima hay sobresuscripción).

Amdahl: fracción serie ajustada por mínimos cuadrados con 2, 4, 8 hilos (menos que las 12 CPU lógicas): f = 0.101 (límite 1/f = 9.93). Compárese con la columna Karp-Flatt: si Amdahl describiera la curva, sería constante.

| hilos | S medido | S Amdahl con f ajustada |
|---:|---:|---:|
| 1 | 0.985 | 1.000 |
| 2 | 1.852 | 1.817 |
| 4 | 3.069 | 3.072 |
| 8 | 4.573 | 4.693 |
| 12 | 0.035 | 5.694 |
| 16 | 0.126 | 6.374 |
| 20 | 0.109 | 6.866 |
| 24 | 0.098 | 7.239 |


### Ejercicio 4 · escalabilidad débil

N1 = 1024, 300 pasos, mínimo de 5 repeticiones. N(p) = round(N1·√p). Eficiencia débil = T(1 hilo, N1)/T(p hilos, N(p)).

| hilos | reales | N | celdas/hilo | MB 2 buffers | T (s) | peor/mejor | E débil | T mín. memoria (s) | T / T mín. | T sec. mismo N (s) | ns/celda·paso sec. | S abs mismo N | max_diff | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1 | 1024 | 1048576 | 16.8 | 0.3826 | 2.23 | 100.0 % | 0.2378 | 1.61 | 0.4388 | 1.395 | 1.147 | 0.000e+00 | sí |
| 2 | 2 | 1448 | 1048352 | 33.5 | 0.4810 | 2.03 | 79.5 % | 0.4760 | 1.01 | 0.7919 | 1.259 | 1.646 | 0.000e+00 | sí |
| 4 | 4 | 2048 | 1048576 | 67.1 | 1.5082 | 1.63 | 25.4 % | 0.9529 | 1.58 | 1.5925 | 1.266 | 1.056 | 0.000e+00 | sí |
| 8 | 8 | 2896 | 1048352 | 134.2 | 1.9411 | 1.02 | 19.7 % | 1.9065 | 1.02 | 2.9185 | 1.160 | 1.504 | 0.000e+00 | sí |
| 12 | 12 | 3547 | 1048434 | 201.3 | 3.3873 | 1.12 | 11.3 % | 2.8607 | 1.18 | 4.4258 | 1.173 | 1.307 | 0.000e+00 | sí |
| 16 | 16 | 4096 | 1048576 | 268.4 | 4.2154 | 1.24 | 9.1 % | 3.8153 | 1.10 | 9.7290 | 1.933 | 2.308 | 0.000e+00 | sí |
| 20 | 20 | 4579 | 1048362 | 335.5 | 5.1249 | 1.04 | 7.5 % | 4.7686 | 1.07 | 7.4841 | 1.190 | 1.460 | 0.000e+00 | sí |
| 24 | 24 | 5017 | 1048762 | 402.7 | 6.1877 | 1.02 | 6.2 % | 5.7250 | 1.08 | 9.7999 | 1.298 | 1.584 | 0.000e+00 | sí |

T mín. memoria = 24 B × (N-2)² × pasos / 31.63 GB/s (máximo de STREAM con el mismo criterio de bytes reales): lo que tardaría el paso si solo lo limitara el ancho de banda de la máquina.

Coste secuencial por celda y paso para N >= 2048 (dos buffers de 64 MB o más, fuera de cualquier caché de esta clase de máquina): mín 1.160, máx 1.933 ns (variación 66.6 %); N = 1024: 1.395 ns.

Celdas por hilo tras redondear N: de 1048352 a 1048762 (desviación máxima 0.039 %).


### Ejercicio 5 · planificaciones


#### configuración pedida: N=2048, 300 pasos, 12 hilos (reales 12), mínimo de 5 rondas (fichero planificaciones.csv)

T1 = 1.6456 s (peor ronda 2.1478 s).

| planificación | T (s) | peor/mejor | S abs | T / T(static) | puesto | max_diff | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|
| static | 1.6467 | 1.08 | 0.999 | 1.000 | 8 | 0.000e+00 | sí |
| static,1 | 1.7768 | 1.07 | 0.926 | 1.079 | 10 | 0.000e+00 | sí |
| static,4 | 1.6690 | 1.09 | 0.986 | 1.014 | 9 | 0.000e+00 | sí |
| static,16 | 1.6017 | 1.43 | 1.027 | 0.973 | 7 | 0.000e+00 | sí |
| static,64 | 1.5174 | 1.89 | 1.085 | 0.922 | 5 | 0.000e+00 | sí |
| dynamic,1 | 1.5282 | 1.52 | 1.077 | 0.928 | 6 | 0.000e+00 | sí |
| dynamic,4 | 1.4975 | 1.35 | 1.099 | 0.909 | 3 | 0.000e+00 | sí |
| dynamic,16 | 1.4385 | 1.17 | 1.144 | 0.874 | 1 | 0.000e+00 | sí |
| dynamic,64 | 1.5112 | 1.23 | 1.089 | 0.918 | 4 | 0.000e+00 | sí |
| guided | 1.4963 | 1.16 | 1.100 | 0.909 | 2 | 0.000e+00 | sí |

Más rápida: dynamic,16 (1.4385 s, 1.145 veces más rápida que static sin trozo).

Separación de efectos, mismo trozo: T(static,c)/T(static) mide lo que cuesta entrelazar trozos de c filas sin gestión dinámica; T(dynamic,c)/T(static,c) mide lo que añade (o quita) repartir esos mismos trozos bajo demanda.

| trozo c (filas) | T(static,c)/T(static) | T(dynamic,c)/T(static,c) |
|---:|---:|---:|
| 1 | 1.079 | 0.860 |
| 4 | 1.014 | 0.897 |
| 16 | 0.973 | 0.898 |
| 64 | 0.922 | 0.996 |


#### complementaria, 8 hilos: N=2048, 300 pasos, 8 hilos (reales 8), mínimo de 5 rondas (fichero planificaciones_8hilos.csv)

T1 = 1.7147 s (peor ronda 2.0734 s).

| planificación | T (s) | peor/mejor | S abs | T / T(static) | puesto | max_diff | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|
| static | 1.1228 | 1.03 | 1.527 | 1.000 | 9 | 0.000e+00 | sí |
| static,1 | 1.1370 | 1.05 | 1.508 | 1.013 | 10 | 0.000e+00 | sí |
| static,4 | 1.1004 | 1.16 | 1.558 | 0.980 | 7 | 0.000e+00 | sí |
| static,16 | 1.0684 | 1.06 | 1.605 | 0.952 | 5 | 0.000e+00 | sí |
| static,64 | 1.1015 | 1.06 | 1.557 | 0.981 | 8 | 0.000e+00 | sí |
| dynamic,1 | 1.0814 | 1.07 | 1.586 | 0.963 | 6 | 0.000e+00 | sí |
| dynamic,4 | 1.0061 | 1.06 | 1.704 | 0.896 | 2 | 0.000e+00 | sí |
| dynamic,16 | 1.0222 | 1.04 | 1.677 | 0.910 | 3 | 0.000e+00 | sí |
| dynamic,64 | 1.0469 | 1.05 | 1.638 | 0.932 | 4 | 0.000e+00 | sí |
| guided | 0.9813 | 1.06 | 1.748 | 0.874 | 1 | 0.000e+00 | sí |

Más rápida: guided (0.9813 s, 1.144 veces más rápida que static sin trozo).

Separación de efectos, mismo trozo: T(static,c)/T(static) mide lo que cuesta entrelazar trozos de c filas sin gestión dinámica; T(dynamic,c)/T(static,c) mide lo que añade (o quita) repartir esos mismos trozos bajo demanda.

| trozo c (filas) | T(static,c)/T(static) | T(dynamic,c)/T(static,c) |
|---:|---:|---:|
| 1 | 1.013 | 0.951 |
| 4 | 0.980 | 0.914 |
| 16 | 0.952 | 0.957 |
| 64 | 0.981 | 0.950 |


### Ejercicio 6 · optimización guiada por medición

Techo de ancho de banda (STREAM triad): máximo 23.73 GB/s (24 B) / 31.63 GB/s (32 B, con lectura previa) con 8 hilos; 1 hilo: 18.23 / 24.31 GB/s.


#### sin afinidad: N=2048, 300 pasos, 12 hilos (reales 12), mínimo de 5 rondas (fichero variantes.csv)

T1 = 1.5850 s (peor ronda 2.5237 s).

Evidencia E2 desequilibrio static: reloj_por_paso_s=7.6430e-03 trabajo_por_paso_s min=2.1694e-03 media=2.6538e-03 max=3.0548e-03 max/min=1.41 fraccion_sin_trabajar=65.3%

Evidencia E2 trabajo de cada hilo static (ms/paso): 2.892 2.313 2.206 2.830 2.838 2.433 3.055 2.813 2.169 2.782 2.737 2.776

Evidencia E2 desequilibrio dynamic4: reloj_por_paso_s=5.6901e-03 trabajo_por_paso_s min=3.0778e-03 media=3.1308e-03 max=3.2094e-03 max/min=1.04 fraccion_sin_trabajar=45.0%

Evidencia E2 trabajo de cada hilo dynamic4 (ms/paso): 3.133 3.113 3.124 3.108 3.078 3.188 3.124 3.157 3.159 3.096 3.209 3.079

Evidencia E1 paso_vacio_s=2.9480e-06 s_por_paso_base=4.7684e-03 fraccion_del_paso=0.06%

| variante | parte de | T (s) | peor/mejor | S abs | mejora vs anterior | mejora vs base | GB/s 16 B | GB/s real | % STREAM real | max_diff | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| base | - | 1.4305 | 1.67 | 1.108 | 1.000 | 1.000 | 14.05 | 24 B: 21.07 | 66.6 % | 0.000e+00 | sí |
| it1_region | base | 1.4854 | 2.18 | 1.067 | 0.963 | 0.963 | 13.53 | 24 B: 20.29 | 64.1 % | 0.000e+00 | sí |
| it2_dynamic | base | 1.4352 | 1.92 | 1.104 | 0.997 | 0.997 | 14.00 | 24 B: 21.00 | 66.4 % | 0.000e+00 | sí |
| it3_simd | it2_dynamic | 1.5658 | 1.39 | 1.012 | 0.917 | 0.914 | 12.83 | 24 B: 19.25 | 60.9 % | 0.000e+00 | sí |
| it4_control | it3_simd | 1.4768 | 1.55 | 1.073 | 1.060 | 0.969 | 13.61 | 24 B: 20.41 | 64.5 % | 0.000e+00 | sí |
| it4_nt | it4_control | 1.1537 | 1.09 | 1.374 | 1.280 | 1.240 | 17.42 | 16 B: 17.42 | 55.1 % | 0.000e+00 | sí |
| alt_collapse2 | base | 1.4582 | 1.21 | 1.087 | 0.981 | 0.981 | 13.78 | 24 B: 20.67 | 65.3 % | 0.000e+00 | sí |
| alt_residuo10 | base | 1.4596 | 1.20 | 1.086 | 0.980 | 0.980 | 13.77 | 24 B: 20.65 | 65.3 % | 0.000e+00 | sí |

GB/s real: bytes que la variante mueve de verdad por celda (24 B con escritura normal, que lee la línea antes de escribirla; 16 B con escritura no temporal, que no la lee). % STREAM real: frente al máximo de STREAM contado también con sus bytes reales (modelo de 32 B, que incluye esa misma lectura previa).


#### OMP_PROC_BIND=close OMP_PLACES=cores: N=2048, 300 pasos, 12 hilos (reales 12), mínimo de 5 rondas (fichero variantes_afinidad_close.csv)

T1 = 1.5408 s (peor ronda 2.0311 s).

Evidencia E2 desequilibrio static: reloj_por_paso_s=4.8642e-03 trabajo_por_paso_s min=2.1258e-03 media=2.6379e-03 max=2.9317e-03 max/min=1.38 fraccion_sin_trabajar=45.8%

Evidencia E2 trabajo de cada hilo static (ms/paso): 2.266 2.203 2.866 2.851 2.814 2.832 2.862 2.897 2.932 2.871 2.137 2.126

Evidencia E2 desequilibrio dynamic4: reloj_por_paso_s=4.8852e-03 trabajo_por_paso_s min=2.9945e-03 media=3.0358e-03 max=3.0968e-03 max/min=1.03 fraccion_sin_trabajar=37.9%

Evidencia E2 trabajo de cada hilo dynamic4 (ms/paso): 3.039 3.023 3.083 3.097 2.994 3.034 3.089 3.008 3.009 2.998 3.019 3.035

Evidencia E1 paso_vacio_s=4.2337e-04 s_por_paso_base=4.8640e-03 fraccion_del_paso=8.70%

| variante | parte de | T (s) | peor/mejor | S abs | mejora vs anterior | mejora vs base | GB/s 16 B | GB/s real | % STREAM real | max_diff | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| base | - | 1.4592 | 1.30 | 1.056 | 1.000 | 1.000 | 13.77 | 24 B: 20.66 | 65.3 % | 0.000e+00 | sí |
| it1_region | base | 1.4764 | 1.41 | 1.044 | 0.988 | 0.988 | 13.61 | 24 B: 20.41 | 64.5 % | 0.000e+00 | sí |
| it2_dynamic | base | 1.4989 | 1.20 | 1.028 | 0.974 | 0.974 | 13.41 | 24 B: 20.11 | 63.6 % | 0.000e+00 | sí |
| it3_simd | it2_dynamic | 1.5286 | 1.03 | 1.008 | 0.981 | 0.955 | 13.14 | 24 B: 19.72 | 62.3 % | 0.000e+00 | sí |
| it4_control | it3_simd | 1.4341 | 1.17 | 1.074 | 1.066 | 1.018 | 14.01 | 24 B: 21.02 | 66.5 % | 0.000e+00 | sí |
| it4_nt | it4_control | 1.1371 | 1.17 | 1.355 | 1.261 | 1.283 | 17.67 | 16 B: 17.67 | 55.9 % | 0.000e+00 | sí |
| alt_collapse2 | base | 1.5036 | 1.05 | 1.025 | 0.971 | 0.971 | 13.36 | 24 B: 20.05 | 63.4 % | 0.000e+00 | sí |
| alt_residuo10 | base | 1.4362 | 1.04 | 1.073 | 1.016 | 1.016 | 13.99 | 24 B: 20.99 | 66.4 % | 0.000e+00 | sí |

GB/s real: bytes que la variante mueve de verdad por celda (24 B con escritura normal, que lee la línea antes de escribirla; 16 B con escritura no temporal, que no la lee). % STREAM real: frente al máximo de STREAM contado también con sus bytes reales (modelo de 32 B, que incluye esa misma lectura previa).


#### OMP_PROC_BIND=spread OMP_PLACES=threads: N=2048, 300 pasos, 12 hilos (reales 12), mínimo de 5 rondas (fichero variantes_afinidad_spread.csv)

T1 = 1.7000 s (peor ronda 1.8384 s).

Evidencia E2 desequilibrio static: reloj_por_paso_s=8.4577e-03 trabajo_por_paso_s min=1.7951e-03 media=2.6418e-03 max=3.4839e-03 max/min=1.94 fraccion_sin_trabajar=68.8%

Evidencia E2 trabajo de cada hilo static (ms/paso): 1.795 1.995 2.214 2.833 2.660 3.265 2.758 3.484 3.273 2.805 2.589 2.030

Evidencia E2 desequilibrio dynamic4: reloj_por_paso_s=5.7958e-03 trabajo_por_paso_s min=3.0771e-03 media=3.1497e-03 max=3.2417e-03 max/min=1.05 fraccion_sin_trabajar=45.7%

Evidencia E2 trabajo de cada hilo dynamic4 (ms/paso): 3.107 3.176 3.150 3.077 3.171 3.182 3.148 3.133 3.242 3.210 3.096 3.104

Evidencia E1 paso_vacio_s=5.9539e-04 s_por_paso_base=5.4263e-03 fraccion_del_paso=10.97%

| variante | parte de | T (s) | peor/mejor | S abs | mejora vs anterior | mejora vs base | GB/s 16 B | GB/s real | % STREAM real | max_diff | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| base | - | 1.6279 | 1.23 | 1.044 | 1.000 | 1.000 | 12.34 | 24 B: 18.51 | 58.5 % | 0.000e+00 | sí |
| it1_region | base | 1.6073 | 1.11 | 1.058 | 1.013 | 1.013 | 12.50 | 24 B: 18.75 | 59.3 % | 0.000e+00 | sí |
| it2_dynamic | base | 1.4355 | 1.04 | 1.184 | 1.134 | 1.134 | 14.00 | 24 B: 21.00 | 66.4 % | 0.000e+00 | sí |
| it3_simd | it2_dynamic | 1.5144 | 1.06 | 1.123 | 0.948 | 1.075 | 13.27 | 24 B: 19.90 | 62.9 % | 0.000e+00 | sí |
| it4_control | it3_simd | 1.4433 | 1.30 | 1.178 | 1.049 | 1.128 | 13.92 | 24 B: 20.88 | 66.0 % | 0.000e+00 | sí |
| it4_nt | it4_control | 1.1454 | 1.97 | 1.484 | 1.260 | 1.421 | 17.54 | 16 B: 17.54 | 55.5 % | 0.000e+00 | sí |
| alt_collapse2 | base | 1.6625 | 1.31 | 1.022 | 0.979 | 0.979 | 12.09 | 24 B: 18.13 | 57.3 % | 0.000e+00 | sí |
| alt_residuo10 | base | 1.5660 | 1.30 | 1.086 | 1.040 | 1.040 | 12.83 | 24 B: 19.25 | 60.9 % | 0.000e+00 | sí |

GB/s real: bytes que la variante mueve de verdad por celda (24 B con escritura normal, que lee la línea antes de escribirla; 16 B con escritura no temporal, que no la lee). % STREAM real: frente al máximo de STREAM contado también con sus bytes reales (modelo de 32 B, que incluye esa misma lectura previa).


#### complementaria, 8 hilos, sin afinidad: N=2048, 300 pasos, 8 hilos (reales 8), mínimo de 5 rondas (fichero variantes_8hilos.csv)

T1 = 1.6700 s (peor ronda 1.7976 s).

Evidencia E2 desequilibrio static: reloj_por_paso_s=3.9976e-03 trabajo_por_paso_s min=2.8494e-03 media=3.1077e-03 max=3.3248e-03 max/min=1.17 fraccion_sin_trabajar=22.3%

Evidencia E2 trabajo de cada hilo static (ms/paso): 3.120 2.913 3.270 3.104 2.849 3.325 3.046 3.233

Evidencia E2 desequilibrio dynamic4: reloj_por_paso_s=3.5213e-03 trabajo_por_paso_s min=3.4524e-03 media=3.4554e-03 max=3.4591e-03 max/min=1.00 fraccion_sin_trabajar=1.9%

Evidencia E2 trabajo de cada hilo dynamic4 (ms/paso): 3.455 3.459 3.452 3.456 3.457 3.454 3.454 3.456

Evidencia E1 paso_vacio_s=1.1415e-06 s_por_paso_base=3.3051e-03 fraccion_del_paso=0.03%

| variante | parte de | T (s) | peor/mejor | S abs | mejora vs anterior | mejora vs base | GB/s 16 B | GB/s real | % STREAM real | max_diff | verif. |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| base | - | 0.9915 | 1.11 | 1.684 | 1.000 | 1.000 | 20.26 | 24 B: 30.40 | 96.1 % | 0.000e+00 | sí |
| it1_region | base | 0.9733 | 1.15 | 1.716 | 1.019 | 1.019 | 20.64 | 24 B: 30.97 | 97.9 % | 0.000e+00 | sí |
| it2_dynamic | base | 0.9441 | 1.13 | 1.769 | 1.050 | 1.050 | 21.28 | 24 B: 31.93 | 100.9 % | 0.000e+00 | sí |
| it3_simd | it2_dynamic | 1.0169 | 1.08 | 1.642 | 0.928 | 0.975 | 19.76 | 24 B: 29.64 | 93.7 % | 0.000e+00 | sí |
| it4_control | it3_simd | 0.9138 | 1.14 | 1.828 | 1.113 | 1.085 | 21.99 | 24 B: 32.98 | 104.3 % | 0.000e+00 | sí |
| it4_nt | it4_control | 0.6366 | 1.07 | 2.623 | 1.435 | 1.558 | 31.56 | 16 B: 31.56 | 99.8 % | 0.000e+00 | sí |
| alt_collapse2 | base | 1.1018 | 1.04 | 1.516 | 0.900 | 0.900 | 18.24 | 24 B: 27.35 | 86.5 % | 0.000e+00 | sí |
| alt_residuo10 | base | 1.0176 | 1.19 | 1.641 | 0.974 | 0.974 | 19.75 | 24 B: 29.62 | 93.6 % | 0.000e+00 | sí |

GB/s real: bytes que la variante mueve de verdad por celda (24 B con escritura normal, que lee la línea antes de escribirla; 16 B con escritura no temporal, que no la lee). % STREAM real: frente al máximo de STREAM contado también con sus bytes reales (modelo de 32 B, que incluye esa misma lectura previa).


#### Afinidad: la misma tabla con OMP_PROC_BIND / OMP_PLACES (tiempos en s)

| variante | sin afinidad | OMP_PROC_BIND=close OMP_PLACES=cores | OMP_PROC_BIND=spread OMP_PLACES=threads | mejora OMP_PROC_BIND=close | mejora OMP_PROC_BIND=spread |
|---:|---:|---:|---:|---:|---:|
| base | 1.4305 | 1.4592 | 1.6279 | 0.980 | 0.879 |
| it1_region | 1.4854 | 1.4764 | 1.6073 | 1.006 | 0.924 |
| it2_dynamic | 1.4352 | 1.4989 | 1.4355 | 0.957 | 1.000 |
| it3_simd | 1.5658 | 1.5286 | 1.5144 | 1.024 | 1.034 |
| it4_control | 1.4768 | 1.4341 | 1.4433 | 1.030 | 1.023 |
| it4_nt | 1.1537 | 1.1371 | 1.1454 | 1.015 | 1.007 |
| alt_collapse2 | 1.4582 | 1.5036 | 1.6625 | 0.970 | 0.877 |
| alt_residuo10 | 1.4596 | 1.4362 | 1.5660 | 1.016 | 0.932 |


### Resumen de validaciones (calculado de los CSV)

| comprobación | estado | detalle |
|---:|---:|---:|
| Ej2 Jacobi N=2048 | PASA | 8/8 recuentos correctos |
| Ej2 in situ rechazado N=2048 | PASA | 16/16 rechazadas |
| Ej2 in situ depende de los hilos N=2048 | PASA | 4 checksums distintos |
| Ej2 Jacobi N=256 | PASA | 8/8 recuentos correctos |
| Ej2 in situ rechazado N=256 | PASA | 16/16 rechazadas |
| Ej2 in situ depende de los hilos N=256 | PASA | 8 checksums distintos |
| Ej3 fuerte N=2048 | PASA | checksum y residuo = secuencial; hilos reales = pedidos |
| Ej3 fuerte N=256 | PASA | checksum y residuo = secuencial; hilos reales = pedidos |
| Ej4 débil | PASA | cada N verificada contra la secuencial con la misma N |
| Ej5 planificaciones.csv | PASA | todas dan el campo secuencial |
| Ej5 planificaciones_8hilos.csv | PASA | todas dan el campo secuencial |
| Ej6 variantes.csv | PASA | 8/8 variantes verificadas |
| Ej6 variantes_afinidad_close.csv | PASA | 8/8 variantes verificadas |
| Ej6 variantes_afinidad_spread.csv | PASA | 8/8 variantes verificadas |
| Ej6 variantes_8hilos.csv | PASA | 8/8 variantes verificadas |
| STREAM | PASA | a[i] = 1 + 3·2 = 7 en todas las filas |

TODAS LAS VERIFICACIONES PASAN

