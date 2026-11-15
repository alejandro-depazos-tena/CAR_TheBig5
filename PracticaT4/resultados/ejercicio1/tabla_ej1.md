Tiempos de la suma (solo la funcion `trozo`, medida con MPI_Wtime). Minimo de 20 ejecuciones por fila; entre parentesis, la mediana.

| Procesos | T rango 0 (s) | T mas lento (s) | Speedup rango 0 | Eficiencia | Speedup mas lento | Eficiencia |
|---:|---:|---:|---:|---:|---:|---:|
| 1 | 0,2454 (0,2529) | 0,2454 (0,2529) | 1,00 | 100,0 % | 1,00 | 100,0 % |
| 2 | 0,1296 (0,1327) | 0,1301 (0,1347) | 1,89 | 94,7 % | 1,89 | 94,3 % |
| 4 | 0,0711 (0,0993) | 0,1055 (0,1162) | 3,45 | 86,3 % | 2,33 | 58,2 % |
| 8 | 0,0350 (0,0764) | 0,0791 (0,0825) | 7,01 | 87,6 % | 3,10 | 38,8 % |
| 12 | 0,0367 (0,0578) | 0,0662 (0,0765) | 6,69 | 55,7 % | 3,71 | 30,9 % |

Speedup = T(1 proceso) / T(P procesos), con los minimos. Eficiencia = speedup / P.
