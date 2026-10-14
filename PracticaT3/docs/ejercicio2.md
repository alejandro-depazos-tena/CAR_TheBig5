# Práctica 3 · Ejercicio 2

## Un bucle desequilibrado y las planificaciones

[`p3_ej2.c`](../src/ejercicio2/p3_ej2.c) compara un bucle triangular en el que
la iteración `i` cuesta `i` sumas. Se prueban `static`, `static, 1`,
`dynamic, 1`, `dynamic, 64` y `guided`, y se mide la mejor de cinco
ejecuciones.

La función `reparto` vuelve a ejecutar el bucle con `schedule(runtime)`.
Cada hilo acumula el coste `i` de las iteraciones que recibe en su propia
casilla. Al terminar calcula:

- el factor `carga máxima / carga media`;
- cuántos hilos no han recibido trabajo.

Por eso `static` puede repartir el mismo número de iteraciones, pero no el
mismo trabajo: las iteraciones finales son mucho más caras. Las planificaciones
por bloques pequeños mezclan iteraciones baratas y caras y suelen equilibrar
mejor la carga, aunque introducen más coste de planificación.

## Compilación

Ejecuta desde `PracticaT3`:

```console
gcc -O2 -Wall -Wextra -fopenmp -Iinclude -o p3_ej2 src/ejercicio2/p3_ej2.c
./p3_ej2
```

En Windows con MinGW, el ejecutable será `p3_ej2.exe` y se ejecutará como
`p3_ej2.exe`.
