## Búsqueda del umbral (modo send)

| Fase | Modo | M (doubles) | Bytes | Resultado | µs por intercambio |
|---|---|---:|---:|---|---:|
| doblando | send | 64 | 512 | funciona | 4,7 |
| doblando | send | 128 | 1 024 | funciona | 2,2 |
| doblando | send | 256 | 2 048 | funciona | 2,6 |
| doblando | send | 512 | 4 096 | **se cuelga** | — |
| bisección | send | 384 | 3 072 | funciona | 1,6 |
| bisección | send | 448 | 3 584 | funciona | 4,0 |
| bisección | send | 480 | 3 840 | funciona | 3,2 |
| bisección | send | 496 | 3 968 | funciona | 1,9 |
| bisección | send | 504 | 4 032 | **se cuelga** | — |
| bisección | send | 500 | 4 000 | funciona | 4,3 |
| bisección | send | 502 | 4 016 | funciona | 2,0 |
| bisección | send | 503 | 4 024 | **se cuelga** | — |
| repetición | send | 502 | 4 016 | funciona | 1,9 |
| repetición | send | 502 | 4 016 | funciona | 1,8 |
| repetición | send | 502 | 4 016 | funciona | 6,3 |
| repetición | send | 503 | 4 024 | **se cuelga** | — |
| repetición | send | 503 | 4 024 | **se cuelga** | — |
| arreglo en el umbral | sendrecv | 503 | 4 024 | funciona | 4,2 |
| arreglo en el umbral | orden | 503 | 4 024 | funciona | 3,6 |
| arreglo en el umbral | noblo | 503 | 4 024 | funciona | 2,3 |

**Umbral:** funciona con 502 doubles (4 016 bytes) y se cuelga con 503 doubles (4 024 bytes).

## Los tres arreglos con 1 048 576 doubles (8 MB)

Tiempo por intercambio (media de 20 dentro del programa). Mínimo de 10 ejecuciones por arreglo; entre paréntesis, la mediana.

| Arreglo | µs por intercambio | Respecto al más rápido | Ancho de banda (GB/s) |
|---|---:|---:|---:|
| MPI_Sendrecv | 1542,7 (1798,6) | 1,00× | 10,88 |
| ordenar los envíos | 2460,0 (2593,2) | 1,59× | 6,82 |
| Isend + Irecv + Waitall | 1569,1 (1719,4) | 1,02× | 10,69 |

Ancho de banda = 2 × 8 MB (los dos sentidos) / tiempo mínimo.
