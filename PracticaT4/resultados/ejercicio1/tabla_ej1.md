Tiempos de la suma (solo la funcion `trozo`, medida con MPI_Wtime). Minimo de 30 ejecuciones por fila; entre parentesis, la mediana.

| Procesos | T rango 0 (s) | T mas lento (s) | Speedup rango 0 | Eficiencia | Speedup mas lento | Eficiencia |
|---:|---:|---:|---:|---:|---:|---:|
| 1 | 0,2260 (0,2414) | 0,2260 (0,2414) | 1,00 | 100,0 % | 1,00 | 100,0 % |
| 2 | 0,1187 (0,1301) | 0,1187 (0,1301) | 1,90 | 95,2 % | 1,90 | 95,2 % |
| 4 | 0,0654 (0,0898) | 0,0944 (0,1040) | 3,46 | 86,4 % | 2,39 | 59,9 % |
| 8 | 0,0334 (0,0718) | 0,0711 (0,0811) | 6,77 | 84,6 % | 3,18 | 39,7 % |
| 12 | 0,0237 (0,0628) | 0,0610 (0,0711) | 9,54 | 79,5 % | 3,70 | 30,9 % |

Speedup = T(1 proceso) / T(P procesos), con los minimos. Eficiencia = speedup / P.
